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

#include <Synthesizer/Basics/PulsarSynth.h>
#include <Synthesizer/Basics/Wavetable_LFO.h>

#include <NerveNet/NerveNetSlaveThread.h>
#include <NerveNet/NerveNetMessageHeader.h>
#include <NerveNet/NerveNetMessage.h>

#include <SynthEngineMessage.h>

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
		threadPtr->sendMessage( (void*) &reply, sizeof ( tSetupInfoReplyMessageData ) );
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
		uint8_t multNo = messagePtr->multiplexerNo;
		uint8_t chanNo = messagePtr->multiplexerChannelNo;
		ePotentioMeterId potId  = ( ePotentioMeterId)( ( multNo << 4	) + chanNo );
		float potValue = (float)messagePtr->potValue / 0x0000ffff;
		switch ( potId )
		{
		case ePotentioMeterId::pot_1:
			mFrequencyOffset = potValue * 5;
			break;
		case ePotentioMeterId::pot_2:
			mFormantGain = potValue;
			break;
		case ePotentioMeterId::pot_3:
			mClusterGain = potValue;
			break;
		case ePotentioMeterId::pot_4:
			mWaveFoldGain = potValue;
			break;
		case ePotentioMeterId::slider_1:
		{
			mTragetFormant = potValue;
			float formant = mTragetFormant + ( mFormant_CV * mFormantGain );
			mPulsarSynthPtr->setFormant ( formant );
		}
			break;
		case ePotentioMeterId::slider_2:
		{
			mTargetCluster = potValue;
			float cluster = mTargetCluster + ( mCluster_CV * mClusterGain );
			mPulsarSynthPtr->setCluster ( cluster );
		}
			break;
		case ePotentioMeterId::slider_3:
		{
			mTargetWaveFold = potValue;
			float waveFold = mTargetWaveFold + ( mWaveFold_CV * mWaveFoldGain );
			mPulsarSynthPtr->setWaveFolder ( waveFold );
		}
			break;
		case ePotentioMeterId::slider_4:
			mPulsarSynthPtr->setMorphFactor ( potValue );
			break;
		default:
			break;
		}
	}
		break;
	case eSynthEngineMessageType::triggerEvent:
		// ToDo handle trigger events
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
void PulsarSynthEngine::processBlock ( AudioBuffer& buffer, AudioBuffer& NN_buffer )
{

	float* lwptr = NN_buffer.getWritePointer(0);
	float* rwptr = NN_buffer.getWritePointer(1);

	uint32_t noSamples = NN_buffer.getNumSamples();

	constexpr double f0 = 8.1757989156;
	float frequency = f0 * std::pow(2.0, mTargetFrequency + mFrequencyOffset);

	for ( uint32_t i = 0; i < noSamples; ++i )
	{
		if ( mCurrentFrequency != ( mTargetFrequency + mFrequencyOffset ) )
		{
			mCurrentFrequency = mTargetFrequency + mFrequencyOffset;
			mPulsarSynthPtr->setFrequency ( frequency );
		}

		float gain = mGain;
		float sample_1 = mPulsarSynthPtr->nextSample () * gain;
		*lwptr++ = sample_1;
		*rwptr++ = sample_1;
	}

}

//==============================================================================
//          applyControlVoltages
//
// Apply control voltage values
//
//  CasualNoises    11/03/2026  First implementation
//==============================================================================
void PulsarSynthEngine::applyControlVoltages ( sControlVoltages* voltages )
{

	mTargetFrequency = voltages->_1V_OCT_1;

	mGain = voltages->CV_Inputs [ CV_INPUT_4 ];
	if ( mGain < 0.0f )
		mGain = 0.0f;

	mFormant_CV = voltages->CV_Inputs [ CV_INPUT_5 ];
	float formant = mTragetFormant + ( mFormant_CV * mFormantGain );
	mPulsarSynthPtr->setFormant ( formant );

	mCluster_CV = voltages->CV_Inputs [ CV_INPUT_6 ];
	float cluster = mTargetCluster + ( mCluster_CV * mClusterGain );
	mPulsarSynthPtr->setCluster ( cluster );

	mWaveFold_CV = voltages->CV_Inputs [ CV_INPUT_7 ];
	float waveFold = mTargetWaveFold + ( mWaveFold_CV * mWaveFoldGain );
	mPulsarSynthPtr->setWaveFolder ( waveFold );

}

} // namespace CasualNoises

