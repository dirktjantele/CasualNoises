/*
  ==============================================================================

    SouthSideAudioProcessor.cpp
    Created: 13/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "SouthSideAudioProcessor.h"

#include "SystemConfig.h"

#include "CasualNoises.h"

#include "SynthEngineMessage.h"

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
		void* synthParams)
{

	// store settings
	mSampleRate 					= sampleRate;
	mMaximumExpectedSamplesPerBlock = maximumExpectedSamplesPerBlock;

	mLFO_ptr = new LFO(mSampleRate, 440.0f);

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
	CN_ReportFault(eErrorCodes::runtimeError);			// Not implemented yet
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
	CasualNoises::NerveNetSlaveThread* threadPtr = gNerveNetSlaveThreadPtr[threadNo];

	while (size > 0)
	{

		tNerveNetMessageHeader* headerPtr	  = (tNerveNetMessageHeader*)ptr;
		eSynthEngineMessageType messageType   = (eSynthEngineMessageType)headerPtr->messageTag;
		uint32_t				messageLength = headerPtr->messageLength;

		switch (messageType)
		{
		case eSynthEngineMessageType::initSynthEngine:		// Ignore initSynthEngine messages
			break;
		case eSynthEngineMessageType::requestSetupInfo:		// Set-up info request
		{
			tSetupInfoReplyMessageData reply;
			reply.header.messageTag = (uint32_t)eSynthEngineMessageType::setupInfoReply;
			reply.header.messageLength = sizeof(tSetupInfoReplyMessageData);
			sprintf(reply.setupName, "Pulsar");
			threadPtr->sendMessage((void*)&reply, sizeof(tSetupInfoReplyMessageData));
		}
			break;
		case eSynthEngineMessageType::potentiometerValue:	// Update potentiometer values
		{
			tPotValueMessage* messagePtr = (tPotValueMessage*)ptr;
			uint32_t potId = messagePtr->potId;
			float potValue = messagePtr->potValue;
			mPotionMeterValues[potId] = potValue;
		}
			break;
		default:
			CN_ReportFault(eErrorCodes::NerveNetThread_Error);
		}

		size -= messageLength;
		ptr  += messageLength;

	}
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

//	NN_buffer.clearAudioBuffer();

	float* lwptr = NN_buffer.getWritePointer(0);
	float* rwptr = NN_buffer.getWritePointer(1);

	uint32_t noSamples = NN_buffer.getNumSamples();

	for (uint32_t i = 0; i < noSamples; ++i)
	{
		float gain = 1.0f;
		float sample_1 = mLFO_ptr->nextSample() * gain;
		*lwptr++ = sample_1;
		*rwptr++ = sample_1;
	}

}


} // namespace CasualNoises
