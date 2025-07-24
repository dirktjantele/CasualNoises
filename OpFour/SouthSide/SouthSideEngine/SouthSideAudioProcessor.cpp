/*
  ==============================================================================

    SouthSideAudioProcessor.cpp
    Created: 13/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

//#include <stdio.h>

#include "SystemConfig.h"

#include "SouthSideAudioProcessor.h"

#include "SynthEngines/PulsarSynthEngine.h"

namespace CasualNoises
{

// Space for statics
SouthSideAudioProcessor	SouthSideAudioProcessor::mSouthSideAudioProcessor;
bool					SouthSideAudioProcessor::mIsAllocated = false;

//==============================================================================
//          prepareToPlay
//
// this method is called before the first call to processBlock()
//
//  CasualNoises    13/07/2025  First implementation
//==============================================================================
void SouthSideAudioProcessor::prepareToPlay (
		float sampleRate,
		uint32_t maximumExpectedSamplesPerBlock,
		void* inSynthParams)
{

	mPulsarSynthEnginePtr = new PulsarSynthEngine;
	mPulsarSynthEnginePtr->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock, inSynthParams);

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
	mPulsarSynthEnginePtr->releaseResources();
}

//==============================================================================
//          processNerveNetData
//
// Process incoming NerveNet data
//
//  CasualNoises    20/07/2025  First implementation
//==============================================================================
void SouthSideAudioProcessor::processNerveNetData(uint32_t threadNo, uint32_t size, uint8_t* ptr)
{

	mPulsarSynthEnginePtr->processNerveNetData(threadNo, size, ptr);

}

//==============================================================================
//          processBlock
//
// This method is called each time a new audio block needs to be processed
//
//  CasualNoises    13/07/2025  First implementation
//==============================================================================
void SouthSideAudioProcessor::processBlock (AudioBuffer& buffer, AudioBuffer& NN_buffer)
{

	mPulsarSynthEnginePtr->processBlock(buffer, NN_buffer);

}


} // namespace CasualNoises
