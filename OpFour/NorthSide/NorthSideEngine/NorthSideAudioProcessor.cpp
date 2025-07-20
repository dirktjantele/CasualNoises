/*
  ==============================================================================

    NorthSideAudioProcessor.cpp
    Created: 10/03/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "NorthSideAudioProcessor.h"

#include "SystemConfig.h"

#include "CasualNoises.h"

#include "SynthEngineMessage.h"

namespace CasualNoises
{

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

	mLFO_ptr = new LFO(mSampleRate, 440.0f);

/*
	mSynthesiserParamsPtr			= static_cast<sSynthesiserParams*>(synthParams);
	if (mSynthesiserParamsPtr == nullptr)
		CN_ReportFault(eErrorCodes::runtimeError);

	mWavetable_LFO_1_ptr = new Wavetable_LFO(mSampleRate, mSynthesiserParamsPtr->frequency);
	mWavetable_LFO_1_ptr->setMorphFactor(mSynthesiserParamsPtr->morph);

	mWavetable_LFO_2_ptr = new Wavetable_LFO(mSampleRate, mSynthesiserParamsPtr->frequency);
	mWavetable_LFO_2_ptr->setMorphFactor(mSynthesiserParamsPtr->morph);

	mSmoothedValuePtr = new SmoothedValue(1.0f, mMaximumExpectedSamplesPerBlock * 4);
*/
}

//==============================================================================
//          releaseResources
//
// This method is called after the last call to processBlock()
//
//  CasualNoises    04/12/2024  First implementation
//	CasualNoises    10/03/2025  Adapted for Fellhorn
//==============================================================================
void NorthSideAudioProcessor::releaseResources()
{
	CN_ReportFault(eErrorCodes::runtimeError);			// Not implemented yet
}

//==============================================================================
//          processNerveNetData
//
// Process incoming NerveNet data
//
//  CasualNoises    19/07/2025  First implementation
//==============================================================================
void NorthSideAudioProcessor::processNerveNetData(uint32_t threadNo, uint32_t size, uint8_t* ptr)
{
/*
	tSE_Message* dataPtr = (tSE_Message*)ptr;
	UNUSED(dataPtr);
*/
}

//==============================================================================
//          processBlock
//
// This method is called each time a new audio block needs to be processed
//
//  CasualNoises    04/12/2024  First implementation
//	CasualNoises    10/03/2025  Adapted for Fellhorn
//	CasualNoises    13/07/2025  NerveNet support added
//==============================================================================
void NorthSideAudioProcessor::processBlock (AudioBuffer& buffer, AudioBuffer& NN_buffer)
{

	buffer.copyAudio(NN_buffer);

}

} // namespace CasualNoises



