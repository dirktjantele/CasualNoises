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

#include "Utilities/ReportFault.h"

namespace CasualNoises
{

AbstractEffectEngine* gAbstractEffectEnginePtr = nullptr;

// A counter that increments every time processBlock() is called
// This is used to measure time very accurate, ex sample freq = 41.666 Hz & block size = 256
// Counter increments every +/- 6,144 ms, a 32bit counter overflows after more than 305 days
uint32_t	gProcessBlockCallCount	= 0;

// Space for statics
NorthSideAudioProcessor	NorthSideAudioProcessor::mNorthSideAudioProcessor;
bool					NorthSideAudioProcessor::mIsAllocated = false;

//==============================================================================
//          prepareToPlay
//
// this method is called before the first call to processBlock()
//
//  CasualNoises    04/12/2024  First implementation
//  CasualNoises    14/12/2024  Passing synthesiser params structure
//	CasualNoises    10/03/2025  Adapted for Fellhorn
//==============================================================================
void NorthSideAudioProcessor::prepareToPlay (
		float sampleRate,
		uint32_t maximumExpectedSamplesPerBlock,
		void* synthParams)
{

	// store settings
	mSampleRate 					= sampleRate;
	mMaximumExpectedSamplesPerBlock = maximumExpectedSamplesPerBlock;

	mEffectEnginePtr = new Echo;			// ToDo create an effect according to ...
	mEffectEnginePtr->prepareToPlay ( sampleRate, maximumExpectedSamplesPerBlock, synthParams );
	gAbstractEffectEnginePtr = mEffectEnginePtr;

}

//==============================================================================
//          releaseResources
//
// This method is called after the last call to processBlock()
//
//  CasualNoises    04/12/2024  First implementation
//	CasualNoises    13/03/2026  Adapted for Fellhorn
//==============================================================================
void NorthSideAudioProcessor::releaseResources()
{
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
	// Not used on the north side firmware
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

	gProcessBlockCallCount += 1;
	Echo* ptr = dynamic_cast< Echo* > ( mEffectEnginePtr );			// ToDo update for multiple effect classes
	if ( ptr == nullptr )
		CN_ReportFault ( eErrorCodes::AudioThreadError );
	else
		ptr->processBlock ( buffer, inputBuffer );

}

//==============================================================================
//          handle_ADC_Data
//
// Handle new adc data:
//	1 V/OCT
//	CV1 - CV3
//
//	CasualNoises    27/07/2025  NerveNet support added
//==============================================================================
void NorthSideAudioProcessor::handle_ADC_Data ( uint32_t noOfEntries, uint16_t* adcDataPtr )
{
	mEffectEnginePtr->applyControlVoltages ( noOfEntries, adcDataPtr );
}

} // namespace CasualNoises

