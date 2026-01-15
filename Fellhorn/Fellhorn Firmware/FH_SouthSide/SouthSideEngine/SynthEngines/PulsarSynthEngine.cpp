/*
  ==============================================================================

    PulsarSynthEngine.cpp
    Created: 24/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include <stdio.h>

#include "PulsarSynthEngine.h"

#include "SouthSideAudioProcessor.h"

#include "Synthesizer/Basics/PulsarSynth.h"
#include "Synthesizer/Basics/Wavetable_LFO.h"

#include "NerveNet/NerveNetSlaveThread.h"
#include "NerveNet/NerveNetMessageHeader.h"
#include "NerveNet/NerveNetMessage.h"

#include "SynthEngineMessage.h"

namespace CasualNoises
{

//==============================================================================
//          prepareToPlay
//
// this method is called before the first call to processBlock()
//
//  CasualNoises    24/07/2025  First implementation
//==============================================================================
void PulsarSynthEngine::prepareToPlay (
		float sampleRate,
		uint32_t maximumExpectedSamplesPerBlock,
		void* inSynthParams)
{

	// store settings
	mSampleRate 					    = sampleRate;
	mMaximumExpectedSamplesPerBlock     = maximumExpectedSamplesPerBlock;
	sSynthesiserParams*	synthParamsPtr	= (sSynthesiserParams*)inSynthParams;
	float frequency						= synthParamsPtr->frequency;

	mPulsarSynthPtr	  = new PulsarSynth(mSampleRate, frequency);
	mWavetable_LFOPtr = new Wavetable_LFO(mSampleRate, frequency);
/*
	mPulsarSynthPtr->setMorphFactor(0.0f / 100.0f);								// ToDo remove default settings
	mPulsarSynthPtr->setFormant(42.0f / 100.0f);
	mPulsarSynthPtr->setCluster(11.0f / 100.0f);
	mPulsarSynthPtr->setWaveFolder(4 / 100.0f);
*/
}

//==============================================================================
//          releaseResources
//
// This method is called after the last call to processBlock()
//
//  CasualNoises    24/07/2025  First implementation
//==============================================================================
void PulsarSynthEngine::releaseResources()
{
	CN_ReportFault(eErrorCodes::runtimeError);			// Not implemented yet
}

//==============================================================================
//          processNerveNetData
//
// Process incoming NerveNet data
//
//  CasualNoises    24/07/2025  First implementation
//==============================================================================
void PulsarSynthEngine::processNerveNetData(uint32_t threadNo, uint32_t size, uint8_t* ptr)
{
	CasualNoises::NerveNetSlaveThread* threadPtr = gNerveNetSlaveThreadPtr[threadNo];

	// Handle all event
	tNerveNetMessageHeader* headerPtr	  = (tNerveNetMessageHeader*)ptr;
	eSynthEngineMessageType messageType   = (eSynthEngineMessageType)headerPtr->messageTag;
	uint32_t				messageLength = headerPtr->messageLength;
	if (messageLength != size)
		CN_ReportFault(eErrorCodes::NerveNetThread_Error);

	switch (messageType)
	{
	case eSynthEngineMessageType::requestSetupInfo:		// Set-up info request
	{
		tSetupInfoReplyMessageData reply;
		reply.header.messageTag = (uint32_t)eSynthEngineMessageType::setupInfoReply;
		reply.header.messageLength = sizeof(tSetupInfoReplyMessageData);
		sprintf(reply.setupName, "Pulsar");
		threadPtr->sendMessage((void*)&reply, sizeof(tSetupInfoReplyMessageData));
	}
		break;
	case eSynthEngineMessageType::setFrequency:			// Set oscillator frequency
	{
		tSetFrequencyMessage* messagePtr = (tSetFrequencyMessage*)ptr;
		mPulsarSynthPtr->setFrequency(messagePtr->frequency);
	}
		break;
	case eSynthEngineMessageType::potentiometerValue:	// Update potentiometer values
	{
		tPotValueMessage* messagePtr = (tPotValueMessage*)ptr;
		uint32_t potId = messagePtr->potId;
		float potValue = messagePtr->potValue;
																		//	ToDo put code back in
		switch (potId)
		{
		case 0:
			mPulsarSynthPtr->setMorphFactor(potValue / 100.0f);
			break;
		case 2:
			mPulsarSynthPtr->setFormant(potValue / 100.0f);
			break;
		case 3:
			mPulsarSynthPtr->setCluster(potValue / 100.0f);
			break;
		case 1:
			mPulsarSynthPtr->setWaveFolder(potValue / 100.0f);
			break;
		default:
			break;
		}

	}
		break;
	default:
		CN_ReportFault(eErrorCodes::NerveNetThread_Error);
	}

}

//==============================================================================
//          processBlock
//
// This method is called each time a new audio block needs to be processed
//
//  CasualNoises    24/07/2025  First implementation
//==============================================================================
void PulsarSynthEngine::processBlock (AudioBuffer& buffer, AudioBuffer& NN_buffer)
{

	float* lwptr = NN_buffer.getWritePointer(0);
	float* rwptr = NN_buffer.getWritePointer(1);

	uint32_t noSamples = NN_buffer.getNumSamples();

	for (uint32_t i = 0; i < noSamples; ++i)
	{
		float gain = 1.0f;
		float sample_1 = mPulsarSynthPtr->nextSample() * gain;
		*lwptr++ = sample_1;
		float sample_2 = mWavetable_LFOPtr->nextSample() * gain;
		*rwptr++ = sample_2;
	}

}

//==============================================================================
//          setFrequency
//
// Set freuency of all parts
//
//  CasualNoises    28/07/2025  First implementation
//==============================================================================
void PulsarSynthEngine::setFrequency(float frequency)
{
	mPulsarSynthPtr->setFrequency(frequency);
	mWavetable_LFOPtr->setFrequency(frequency);
}

} // namespace CasualNoises

