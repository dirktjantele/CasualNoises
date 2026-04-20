/*
  ==============================================================================

    NorthSideAudioProcessor.cpp
    Created: 10/03/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "NorthSideAudioProcessor.h"

#include "SystemConfig.h"

#include "EffectEngines/Echo.h"

#include "SynthEngineMessage.h"

#include "TLV_Definitions.h"
#include "Threads/TLV_DriverThread.h"

#include "Core/Maths/MathsFunctions.h"

#include "CommonUtilities.h"

#include "YellowPages.h"

#include "Utilities/ReportFault.h"

namespace CasualNoises
{

AbstractEffectEngine* gAbstractEffectEnginePtr = nullptr;

// A counter that increments every time processBlock() is called
// This is used to measure time very accurate, ex sample freq = 41.666 Hz & block size = 256
// Counter increments every +/- 6,144 ms, a 32bit counter overflows after more than 305 days
uint32_t	gProcessBlockCallCount	= 0;

// Space for globals
sControlVoltages gControlVoltages;

// Space for statics
NorthSideAudioProcessor	NorthSideAudioProcessor::mNorthSideAudioProcessor;
bool					NorthSideAudioProcessor::mIsAllocated = false;


//==============================================================================
//          prepareToPlay ()
//
// This method is called before the first call to processBlock() only once
//
//  CasualNoises    04/12/2024  First implementation
//  CasualNoises    14/12/2024  Passing synthesizer params structure
//	CasualNoises    10/03/2025  Adapted for Fellhorn
//  CasualNoises    20/04/2026  mAveragesPtrs added
//==============================================================================
void NorthSideAudioProcessor::prepareToPlay (
		float sampleRate,
		uint32_t maximumExpectedSamplesPerBlock )
{

	// Used to calculate average ADC data
	for ( uint32_t i = 0; i < TOTAL_NUM_CV_INPUTS; ++i )
	{
		mAveragesPtrs [ i ] = new Average <float> ( 10 );
	}

	// store settings
	mSampleRate 					= sampleRate;
	mMaximumExpectedSamplesPerBlock = maximumExpectedSamplesPerBlock;

	// Load initial calibration values
	loadCalibrationValues ();

	mEffectEnginePtr = new Echo;															// ToDo create an effect according to ...
	mEffectEnginePtr->prepareToPlay ( sampleRate, maximumExpectedSamplesPerBlock );
	gAbstractEffectEnginePtr = mEffectEnginePtr;

}

//==============================================================================
//          releaseResources
//
// This method is called after the last call to processBlock()
//
//  CasualNoises    04/12/2024  First implementation
//	CasualNoises    13/03/2026  Adapted for Fellhorn
//  CasualNoises    20/04/2026  mAveragesPtrs added
//==============================================================================
void NorthSideAudioProcessor::releaseResources()
{
	// Delete average calculators
	for ( uint32_t i = 0; i < TOTAL_NUM_CV_INPUTS; ++i )
	{
		if ( mAveragesPtrs [ i ] != nullptr ) delete mAveragesPtrs [ i ];
	}

	mEffectEnginePtr->releaseResources ();
}

//==============================================================================
//          processNerveNetData
//
// Process incoming NerveNet data
//
//  CasualNoises    21/01/2026  First implementation
//==============================================================================
void NorthSideAudioProcessor::processNerveNetData ( uint32_t threadNo, uint32_t size, uint8_t* ptr )
{
	// This is handled by the DeviceBoardConnection
}

//==============================================================================
//          processBlock
//
// This method is called each time a new audio block needs to be processed
//
//  CasualNoises    04/12/2024  First implementation
//	CasualNoises    13/07/2025  NerveNet support added
//	CasualNoises    13/03/2026  Adapted for Fellhorn
//==============================================================================
void NorthSideAudioProcessor::processBlock (
		AudioBuffer* buffer, 					// Audio from the codec and audio for the codec
		AudioBuffer* inputBuffer ) noexcept		// Audio from NerveNet
{

	buffer->copyAudio( *inputBuffer );			// Bypass effects
	return;

	gProcessBlockCallCount += 1;
	Echo* ptr = dynamic_cast< Echo* > ( mEffectEnginePtr );			// ToDo update for multiple effect classes
	if ( ptr == nullptr )
		CN_ReportFault ( eErrorCodes::AudioThreadError );
	else
		ptr->processBlock ( buffer, inputBuffer );

}

//==============================================================================
//          handle_ADC_Data ()
//
// Handle new adc data:
//	1 V/OCT on CV1 & CV2
//	CV1 - CV7
//
//	CasualNoises    27/07/2025  NerveNet support added
//	CasualNoises    20/04/2026  mAveragesPtrs added
//==============================================================================
void NorthSideAudioProcessor::handle_ADC_Data ( uint32_t noOfEntries, uint16_t* adcDataPtr ) noexcept
{
	setTimeMarker_1 ();

	// Get average values
	for ( uint32_t i = 0; i < TOTAL_NUM_CV_INPUTS; ++i )
	{
		adcDataPtr [ i ] = mAveragesPtrs [ i ]->nextAverage( adcDataPtr [ i ] );
	}

	// Save unnormalized values
	gControlVoltages._1V_OCT_1	= static_cast<float>( ( int16_t ) adcDataPtr[0] );
	gControlVoltages._1V_OCT_2	= static_cast<float>( ( int16_t ) adcDataPtr[1] );
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
		controlVoltages.CV_Inputs [ i ] = normalizeCV_Input ( adcDataPtr[i + 2], i, mCV_CalibrationValuesLoaded, mCV_CalibrationValues );
	}

	// Apply voltages
	if ( mEffectEnginePtr != nullptr )
		mEffectEnginePtr->applyControlVoltages ( &controlVoltages );

	resetTimeMarker_1 ();
}

//==============================================================================
//          loadCalibrationValues ()
//
//	CasualNoises    20/04/2026  NerveNet support added
//==============================================================================
void NorthSideAudioProcessor::loadCalibrationValues ( bool reload ) noexcept
{

	if ( ( ! m1V_OctCalibrationValuesLoaded ) || reload )
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

	if ( ( ! mCV_CalibrationValuesLoaded ) || reload )
	{
		uint32_t length = readTLV_TagBytes ( gYellowPages.gTLV_DriverThreadQueueHandle,
											 (uint32_t)eTLV_Tag::CV_CalibrationValues,
											 sizeof ( tCV_CalibrationValues ),
											 ( uint32_t* ) &mCV_CalibrationValues );
		if ( length == 0 )
		{
			for ( uint32_t i = 0; i <  TOTAL_NUM_CV_INPUTS; ++ i )
			{
				mCV_CalibrationValues.openInputValues 	 [ i ] =  0.0f;
				mCV_CalibrationValues.min5V_InputValues  [ i ] = -1.0f;
				mCV_CalibrationValues.plus5V_InputValues [ i ] =  1.0f;
			}
		}
		mCV_CalibrationValuesLoaded = true;
	}

}

} // namespace CasualNoises

