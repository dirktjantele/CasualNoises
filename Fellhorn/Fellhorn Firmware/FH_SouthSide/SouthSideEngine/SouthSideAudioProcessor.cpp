/*
  ==============================================================================

    SouthSideAudioProcessor.cpp
    Created: 13/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "SystemConfig.h"

#include "SouthSideAudioProcessor.h"

#include "SynthEngines/PulsarSynthEngine.h"
#include "SynthEngines/SaturatedSineWaveEngine.h"
#include "SynthEngines/DrawBarOrganEngine.h"

#include "NerveNet/NerveNetSlaveThread.h"

#include "AudioBasics/Buffers/AudioBuffer.h"

#include "Core/Maths/Average.h"
#include "Core/Maths/MathsFunctions.h"
#include "maths.h"

#include "SynthEngineMessage.h"

#include "TLV_Definitions.h"
#include "Threads/TLV_DriverThread.h"

#include "YellowPages.h"

#include "CommonUtilities.h"

#include "string.h"

// Pointer used to send reply's on NerveNet requests
extern CasualNoises::NerveNetSlaveThread gNerveNetSlaveThreadObjectPtr;

namespace CasualNoises
{

// Space for globals
static sControlVoltages gControlVoltages;

// Space for statics
SouthSideAudioProcessor	SouthSideAudioProcessor::mSouthSideAudioProcessor;
bool					SouthSideAudioProcessor::mIsAllocated = false;

//==============================================================================
//          prepareToPlay ()
//
// this method is called before the first call to processBlock()
//
//  CasualNoises    13/07/2025  First implementation
//==============================================================================
void SouthSideAudioProcessor::prepareToPlay (
		float sampleRate,
		uint32_t maximumExpectedSamplesPerBlock )
{

	for ( uint32_t i = 0; i < TOTAL_NUM_CV_INPUTS; ++i )
	{
		mAveragesPtrs [ i ] = new Average <float> ( 10 );
	}

	mAbstractSynthEnginePtr = new PulsarSynthEngine;								// ToDo create the right kind of engine here
//	mAbstractSynthEnginePtr = new SaturatedSineWaveEngine;							// ToDo create the right kind of engine here
//	mAbstractSynthEnginePtr = new DrawBarOrganEngine;								// ToDo create the right kind of engine here

	mAbstractSynthEnginePtr->prepareToPlay ( sampleRate, maximumExpectedSamplesPerBlock );

}

//==============================================================================
//          releaseResources
//
// This method is called after the last call to processBlock()
//
//  CasualNoises    13/07/2025  First implementation
//==============================================================================
void SouthSideAudioProcessor::releaseResources()
{

	for ( uint32_t i = 0; i < TOTAL_NUM_CV_INPUTS; ++i )
	{
		if ( mAveragesPtrs [ i ] != nullptr ) delete mAveragesPtrs [ i ];
	}

	mAbstractSynthEnginePtr->releaseResources();

}

//==============================================================================
//          processBlock
//
// This method is called each time a new audio block needs to be processed
//
//  CasualNoises    13/07/2025  First implementation
//==============================================================================
void SouthSideAudioProcessor::processBlock ( AudioBuffer* buffer, AudioBuffer* NN_buffer )
{

	mAbstractSynthEnginePtr->processBlock ( buffer, NN_buffer );

}

//==============================================================================
//          processNerveNetData
//
// Process incoming NerveNet data
//
//  CasualNoises    20/07/2025  First implementation
//==============================================================================
void SouthSideAudioProcessor::processNerveNetData (
		uint32_t threadNo,
		uint32_t size,
		uint8_t* ptr )
{

	// Handle all events
	while ( size > 0 )
	{

		tNerveNetMessageHeader* headerPtr	  = (tNerveNetMessageHeader*) ptr;
		eSynthEngineMessageType messageType   = (eSynthEngineMessageType) headerPtr->messageTag;
		uint32_t				messageLength = headerPtr->messageLength;

		switch ( messageType )
		{
		case eSynthEngineMessageType::initSynthEngine:
			initSynthEngine ();
			break;
		case eSynthEngineMessageType::triggerEvent:			// Trigger events
			break;
		case eSynthEngineMessageType::setFrequency:			// Set oscillator frequency
		case eSynthEngineMessageType::potentiometerValue:	// Update potentiometer values
			mAbstractSynthEnginePtr->processNerveNetData ( threadNo, messageLength, ptr );
			break;
		case eSynthEngineMessageType::requestSetupInfo:		// Set-up info request
			handleRequestSetupInfo ( threadNo );
			break;
		case eSynthEngineMessageType::ADC_DataRequest:		// CV inputs data request
			handleADC_DataRequest ( threadNo );
			break;
		case eSynthEngineMessageType::ADC_CalibrationData:	// Calibration data to be saved in flash
			handleADC_CalibrationData ( headerPtr );
			break;
		case eSynthEngineMessageType::_1V_OCT_CalibrationData:
			handle_1V_OCT_CalibrationData ( headerPtr );
			break;
		default:
			CN_ReportFault(eErrorCodes::NerveNetThread_Error);
		}

		size -= messageLength;
		ptr  += messageLength;

	}

}

//==============================================================================
//          initSynthEngine ()
//
//	CasualNoises    18/04/2026  First implementation
//==============================================================================
void SouthSideAudioProcessor::initSynthEngine () noexcept
{

	// Load calibration values
	if ( ! m1V_OctCalibrationValuesLoaded )
	{
		uint32_t length = readTLV_TagBytes ( gYellowPages.gTLV_DriverThreadQueueHandle,
											 (uint32_t)eTLV_Tag::_1V_OCT_CalibrationValues,
											 sizeof ( t1V_OctCalibrationValues ),
											 ( uint32_t* ) &m1V_OctCalibrationValues );
		if ( length == 0 )
		{
			for ( uint32_t i = 0; i <  cTotalNoOfNotes; ++ i )
			{
				m1V_OctCalibrationValues [ i ] = jmap ( ( float ) i, 0.0f, (float) ( cTotalNoOfNotes - 1 ), 0.0f, 65535.0f );
			}
		}
		m1V_OctCalibrationValuesLoaded = true;
	}

}

//==============================================================================
//          handle_ADC_Data ()
//
//	CasualNoises    02/02/2026  First implementation
//	CasualNoises    03/04/2026  Save unnormalized data
//==============================================================================
void SouthSideAudioProcessor::handle_ADC_Data ( uint32_t noOfEntries, uint16_t* adcDataPtr )
{
	assert ( noOfEntries == TOTAL_NUM_CV_INPUTS );

	// Get average values
	for ( uint32_t i = 0; i < TOTAL_NUM_CV_INPUTS; ++i )
	{
		adcDataPtr [ i ] = mAveragesPtrs [ i ]->nextAverage( adcDataPtr [ i ] );
	}

	// Save unnormalized values
	gControlVoltages._1V_OCT_1	= adcDataPtr[0];
	gControlVoltages._1V_OCT_2	= adcDataPtr[1];
	for (uint32_t i = 0; i < NUM_CV_INPUTS; ++i)
	{
		float f = 1.0f - ( static_cast<float>( adcDataPtr[i + 2] ) / 32767.5f );
		gControlVoltages.CV_Inputs [ i ] = f;
	}

	// Normalize voltages against the calibration results
	sControlVoltages controlVoltages;
	controlVoltages._1V_OCT_1 = normalize1V_OCT ( adcDataPtr [ 0 ], m1V_OctCalibrationValuesLoaded, m1V_OctCalibrationValues );
	controlVoltages._1V_OCT_2 = normalize1V_OCT ( adcDataPtr [ 1 ], m1V_OctCalibrationValuesLoaded, m1V_OctCalibrationValues );
	for (uint32_t i = 0; i < NUM_CV_INPUTS; ++i)
	{
		controlVoltages.CV_Inputs [ i ] = normalizeCV_Input ( adcDataPtr[i + 2] );
	}

	// Apply the normalized values on the current engine
	mAbstractSynthEnginePtr->applyControlVoltages ( &controlVoltages );

}

//==============================================================================
//          handleRequestSetupInfo ()
//
//	CasualNoises    05/04/2026  First implementation
//==============================================================================
void SouthSideAudioProcessor::handleRequestSetupInfo ( uint32_t threadNo ) const noexcept
{
	tRequestSetupInfoReplyData reply;
	reply.header.sourceID 		= eNerveNetSourceId::FellhornSouthSide;
	reply.header.destinationID	= eNerveNetSourceId::FellhornDeviceBoard;
	reply.header.messageTag 	= (uint32_t)eSynthEngineMessageType::requestSetupInfo;
	reply.header.messageLength 	= sizeof ( tRequestSetupInfoReplyData );
	reply.version				= 0x00000001;				// #0.1
	CasualNoises::NerveNetSlaveThread* threadPtr = gNerveNetSlaveThreadPtr [threadNo] ;
	threadPtr->sendMessage( ( tNerveNetMessageHeader* ) &reply, sizeof ( tRequestSetupInfoReplyData ) );
}

//==============================================================================
//          handleADC_DataRequest ()
//
//	CasualNoises    03/04/2026  First implementation
//==============================================================================
void SouthSideAudioProcessor::handleADC_DataRequest ( uint32_t threadNo ) const noexcept
{
	tRequestADC_ReplyData reply;
	reply.header.sourceID		= eNerveNetSourceId::FellhornSouthSide;
	reply.header.destinationID	= eNerveNetSourceId::FellhornDeviceBoard;
	reply.header.messageTag		= ( uint32_t ) eSynthEngineMessageType::ADC_DataReply;
	reply.header.messageLength	= sizeof ( tRequestADC_ReplyData );
	reply.data [ 0 ] = gControlVoltages._1V_OCT_1;
	reply.data [ 1 ] = gControlVoltages._1V_OCT_2;
	for ( uint32_t i = 2; i < TOTAL_NUM_CV_INPUTS; ++i)
		reply.data [ i ] = gControlVoltages.CV_Inputs [ i - 2 ];
	CasualNoises::NerveNetSlaveThread* threadPtr = gNerveNetSlaveThreadPtr [threadNo] ;
	threadPtr->sendMessage( ( tNerveNetMessageHeader* ) &reply, sizeof ( tRequestADC_ReplyData ) );
}

//==============================================================================
//          handleADC_CalibrationData ()
//
//	CasualNoises    13/04/2026  First implementation
//==============================================================================
void SouthSideAudioProcessor::handleADC_CalibrationData ( tNerveNetMessageHeader* headerPtr )
{

	// Get calibration values
	tCV_InputCalibrationSettings* settingsPtr = ( tCV_InputCalibrationSettings* ) headerPtr;
	tCV_CalibrationValues values;
	float* openValuesPtr    = settingsPtr->openInputValues;
	float* min5V_ValuesPtr  = settingsPtr->min5V_InputValues;
	float* plus5V_ValuesPtr = settingsPtr->plus5V_InputValues;
	for ( uint32_t i = 0; i < NUM_CV_INPUTS; ++i )
	{
		values.openInputValues    [ i ] = openValuesPtr [ i ];
		values.min5V_InputValues  [ i ] = min5V_ValuesPtr [ i ];
		values.plus5V_InputValues [ i ] = plus5V_ValuesPtr [ i ];
	}

	// Save calibration values to the flash memory
	updateTLV_Bytes ( gYellowPages.gTLV_DriverThreadQueueHandle,
			(uint32_t)eTLV_Tag::CV_CalibrationValues, sizeof ( tCV_CalibrationValues ), (uint32_t*) &values );

}

//==============================================================================
//          handle_1V_OCT_CalibrationData ()
//
//	CasualNoises    15/04/2026  First implementation
//==============================================================================
void SouthSideAudioProcessor::handle_1V_OCT_CalibrationData ( tNerveNetMessageHeader* headerPtr )
{

	// Get calibration values
	t1V_OctCalibrationSettings* settingsPtr = ( t1V_OctCalibrationSettings* ) headerPtr;
	float* valuesPtr = settingsPtr->calibrationValues;

	// Save calibration values to the flash memory
	updateTLV_Bytes ( gYellowPages.gTLV_DriverThreadQueueHandle,
			(uint32_t)eTLV_Tag::_1V_OCT_CalibrationValues, sizeof ( t1V_OctCalibrationValues ), (uint32_t*) valuesPtr );

	// Save calibration values local
	memcpy ( &m1V_OctCalibrationValues, valuesPtr, sizeof ( t1V_OctCalibrationValues ) );

}

} // namespace CasualNoises
