/*
  ==============================================================================

    OpFourAudioProcessor.cpp
    Created: 09 nov 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "OpFourAudioProcessor.h"

#include "SystemConfig.h"

#include "CasualNoises.h"

namespace CasualNoises
{

// Space for statics
OpFourAudioProcessor	OpFourAudioProcessor::mOpFourAudioProcessor;
bool					OpFourAudioProcessor::mIsAllocated = false;

//==============================================================================
//          prepareToPlay
//
// this method is called before the first call to processBlock()
//
//  CasualNoises    04/12/2024  First implementation
//  CasualNoises    14/12/2024  Passing synthesiser params structure
//==============================================================================
void OpFourAudioProcessor::prepareToPlay (
		float sampleRate,
		uint32_t maximumExpectedSamplesPerBlock,
		void* synthParams)
{

	// store settings
	mSampleRate 					= sampleRate;
	mMaximumExpectedSamplesPerBlock = maximumExpectedSamplesPerBlock;
	mSynthesiserParams 				= static_cast<sSynthesiserParams*>(synthParams);
	if (mSynthesiserParams == nullptr)
		CN_ReportFault(1);

//	mWavetable_LFO_1_ptr = new Wavetable_LFO(mSampleRate, mSynthesiserParams->frequency);
	mWavetable_LFO_1_ptr = new CachedWavetable_LFO(mSampleRate, mSynthesiserParams->frequency);
	mWavetable_LFO_1_ptr->setMorphFactor(mSynthesiserParams->morph);

//	mWavetable_LFO_2_ptr = new Wavetable_LFO(mSampleRate, mSynthesiserParams->frequency);
	mWavetable_LFO_2_ptr = new CachedWavetable_LFO(mSampleRate, mSynthesiserParams->frequency);
	mWavetable_LFO_2_ptr->setMorphFactor(mSynthesiserParams->morph);

	mSmoothedValuePtr = new SmoothedValue(1.0f, mMaximumExpectedSamplesPerBlock * 4);

}

//==============================================================================
//          releaseResources
//
// This method is called after the last call to processBlock()
//
//  CasualNoises    04/12/2024  First implementation
//==============================================================================
void OpFourAudioProcessor::releaseResources()
{
	CN_ReportFault(1);			// Not implemented yet
}

//==============================================================================
//          processBlock
//
// This method is called each time a new audio block needs to be processed
//
//  CasualNoises    04/12/2024  First implementation
//==============================================================================
void OpFourAudioProcessor::processBlock (AudioBuffer& buffer/*, MidiBuffer &midiMessages*/)
{

	buffer.clearAudioBuffer();

	uint32_t noChannels = buffer.getNumChannels();
	uint32_t noSamples  = buffer.getNumSamples();
	assert(noChannels == 2);

	float* lwptr = buffer.getWritePointer(0);
	float* rwptr = buffer.getWritePointer(1);

	mWavetable_LFO_1_ptr->setMorphFactor(mSynthesiserParams->morph);

	for (uint32_t i = 0; i < noSamples; ++i)
	{
		float gain = mSmoothedValuePtr->getNextSmoothedValue();
		float sample_1 = mWavetable_LFO_1_ptr->nextSample() * gain;
		*lwptr++ = sample_1;
		float sample_2 = mWavetable_LFO_2_ptr->nextSample() * gain;
		*rwptr++ = sample_2;
		if ( ! mSmoothedValuePtr->isSmoothing())
			mSmoothedValuePtr->reset();
	}
//	mSmoothedValuePtr->applyGain(buffer);
//	if ( ! mSmoothedValuePtr->isSmoothing())
//		mSmoothedValuePtr->reset();

}

} // namespace CasualNoises


