/*
  ==============================================================================

    DeviceBoardConnection.cpp

    Handle NerveNet slave messages

    Created: 21/01/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_NERVENET_THREAD

#include "main.h"

#include "DeviceBoardConnection.h"

#include "NorthSideAudioProcessor.h"

#include "EffectEngines/AbstractEffectEngine.h"

#include "NerveNet/NerveNetSlaveThread.h"
#include "NerveNet/NerveNetMasterThread.h"
#include "NerveNet/NerveNetMessage.h"

#include "Threads/TLV_DriverThread.h"
#include "TLV_Definitions.h"

#include "YellowPages.h"

#include "Utilities/ReportFault.h"

namespace CasualNoises
{

//==============================================================================
//          DeviceBoardConnection()
//
//  CasualNoises    20/04/2026  First implementation
//==============================================================================
DeviceBoardConnection::DeviceBoardConnection ( NorthSideAudioProcessor* audioProcessorPtr ) :
		mAudioProcessorPtr ( audioProcessorPtr )
{
}

//==============================================================================
//          processNerveNetData()
//
//  CasualNoises    21/01/2026  First implementation
//==============================================================================
void DeviceBoardConnection::processNerveNetData ( uint32_t threadNo, uint32_t size, uint8_t* ptr )
{

	// NerveNet should be up and running right now
	NerveNetSlaveThread* nerveNetThreadPtr = gNerveNetSlaveThreadPtr [ threadNo ];
	if ( nerveNetThreadPtr == nullptr )
		CN_ReportFault ( eErrorCodes::NerveNetThread_Error );

	// Dispatch all incoming messages
	while ( size > 0 )
	{

		// Handle event on this side
		tInitMessage* messagePtr = ( tInitMessage* ) ptr;
		if ( ( messagePtr->header.destinationID == eNerveNetSourceId::FellhornNorthSide ) ||
			 ( messagePtr->header.destinationID == eNerveNetSourceId::FellhornBothSides ) )
		{

			eSynthEngineMessageType type = ( eSynthEngineMessageType ) messagePtr->header.messageTag;
			switch ( type )
			{
				case eSynthEngineMessageType::initSynthEngine:
					break;
				case eSynthEngineMessageType::requestSetupInfo:
					handleRequestSetupInfo ( nerveNetThreadPtr );
					break;
				case eSynthEngineMessageType::ADC_DataRequest:
					handleADC_DataRequest ( nerveNetThreadPtr );
					break;
				case eSynthEngineMessageType::potentiometerValue:
					break;
				case eSynthEngineMessageType::ADC_CalibrationData:
					handleADC_CalibrationData ( messagePtr );
					break;
				case eSynthEngineMessageType::_1V_OCT_CalibrationData:
					handle_1V_OCT_CalibrationData ( messagePtr );
					break;
				default:
					CN_ReportFault ( eErrorCodes::NerveNetThread_Error );
			}

			// Let the NorthSide engine handle this event
			extern AbstractEffectEngine* gAbstractEffectEnginePtr;
			if ( gAbstractEffectEnginePtr != nullptr )
			{
				gAbstractEffectEnginePtr->processNerveNetMessage ( messagePtr );
			}

		}

		// Forward event to other mcu's
		if ( messagePtr->header.destinationID != eNerveNetSourceId::FellhornNorthSide )
		{
			messagePtr->header.destinationID = eNerveNetSourceId::FellhornSouthSide;
			gNerveNetMasterThreadPtr [ 0 ]->sendMessage( messagePtr, messagePtr->header.messageLength, true);
		}

		// Goto next event
		size -= messagePtr->header.messageLength;
		ptr  += messagePtr->header.messageLength;

	}

}

//==============================================================================
//          handleRequestSetupInfo()
//
//  CasualNoises    21/01/2026  First implementation
//==============================================================================
void DeviceBoardConnection::handleRequestSetupInfo ( NerveNetSlaveThread* nerveNetThreadPtr ) const noexcept
{
	tRequestSetupInfoReplyData reply;
	reply.header.sourceID 		= eNerveNetSourceId::FellhornNorthSide;
	reply.header.destinationID	= eNerveNetSourceId::FellhornDeviceBoard;
	reply.header.messageTag 	= (uint32_t)eSynthEngineMessageType::requestSetupInfo;
	reply.header.messageLength 	= sizeof ( tRequestSetupInfoReplyData );
	reply.version				= 0x00010001;				// #1.1
	nerveNetThreadPtr->sendMessage( ( tNerveNetMessageHeader* ) &reply, reply.header.messageLength );
}

//==============================================================================
//          handleADC_DataRequest ()
//
//	CasualNoises    05/04/2026  First implementation
//==============================================================================
extern sControlVoltages gControlVoltages;
void DeviceBoardConnection::handleADC_DataRequest ( NerveNetSlaveThread* nerveNetThreadPtr ) const noexcept
{
	tRequestADC_ReplyData reply;
	reply.header.sourceID		= eNerveNetSourceId::FellhornNorthSide;
	reply.header.destinationID	= eNerveNetSourceId::FellhornDeviceBoard;
	reply.header.messageTag		= ( uint32_t ) eSynthEngineMessageType::ADC_DataReply;
	reply.header.messageLength	= sizeof ( tRequestADC_ReplyData );
	reply.data [ 0 ] = gControlVoltages._1V_OCT_1;
	reply.data [ 1 ] = gControlVoltages._1V_OCT_2;
	for ( uint32_t i = 2; i < TOTAL_NUM_CV_INPUTS; ++i)
		reply.data [ i ] = gControlVoltages.CV_Inputs [ i - 2 ];
	nerveNetThreadPtr->sendMessage( ( tNerveNetMessageHeader* ) &reply, reply.header.messageLength );
}

//==============================================================================
//          handleADC_CalibrationData()
//
//  CasualNoises    12/04/2026  First implementation
//==============================================================================
void DeviceBoardConnection::handleADC_CalibrationData ( tInitMessage* messagePtr ) const noexcept
{

	// Get calibration values
	tCV_InputCalibrationSettings* settingsPtr = ( tCV_InputCalibrationSettings* ) messagePtr;
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

	// Update calibration values
	mAudioProcessorPtr->loadCalibrationValues( true );

}

//==============================================================================
//          handle_1V_OCT_CalibrationData()
//
//  CasualNoises    15/04/2026  First implementation
//==============================================================================
void DeviceBoardConnection::handle_1V_OCT_CalibrationData ( tInitMessage* messagePtr ) const noexcept
{

	// Get calibration values
	t1V_OctCalibrationSettings* settingsPtr = ( t1V_OctCalibrationSettings* ) messagePtr;
	float* valuesPtr = settingsPtr->calibrationValues;

	// Save calibration values to the flash memory
	updateTLV_Bytes ( gYellowPages.gTLV_DriverThreadQueueHandle,
			(uint32_t)eTLV_Tag::_1V_OCT_CalibrationValues, sizeof ( t1V_OctCalibrationValues ), (uint32_t*) valuesPtr );

	// Update calibration values
	mAudioProcessorPtr->loadCalibrationValues( true );

}

} // namespace CasualNoises

#endif
