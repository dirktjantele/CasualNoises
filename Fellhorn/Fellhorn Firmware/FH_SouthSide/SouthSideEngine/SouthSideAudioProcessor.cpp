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

#include "NerveNet/NerveNetSlaveThread.h"
#include "NerveNet/NerveNetMessageHeader.h"
#include "NerveNet/NerveNetMessage.h"

#include "AudioBasics/Buffers/AudioBuffer.h"

#include "Core/Maths/Average.h"
#include "maths.h"

#include "SynthEngineMessage.h"
#include "Synthesizer/Basics/ADSR.h"

#include "string.h"

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

	mADSR_Ptr = new ADSR(sampleRate);
	tADSR_Settings adsrSettings;
	adsrSettings.type 		  = eADSR_Type::oneShot;
	adsrSettings.holdTime1	  = 0.0f;
	adsrSettings.attackTime	  = 0.05f;
	adsrSettings.holdTime2	  = 0.02f;
	adsrSettings.decayTime	  = 0.05f;
	adsrSettings.sustainLevel = 0.1f;
	adsrSettings.releaseTime  = 0.0f;
	adsrSettings.cycleTime	  = 0.1f;
	mADSR_Ptr->updateSettings(&adsrSettings);

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
//          process_ADC_data
//
// Process incoming NerveNet data
//
//  CasualNoises    27/07/2025  First implementation
//==============================================================================
void SouthSideAudioProcessor::process_ADC_data(uint16_t* ptr)
{

	// Update picth
	static Average<uint32_t> pitchAverage(10);
	uint32_t pitch = (uint32_t)ptr[0];
	uint32_t pa = pitchAverage.nextAverage(pitch);
	float voltage = cn_map<float>((float)pa, 0.0f, 63200.0f, 0.0f, 10.0f);
	float frequency = 8.1758 * pow(2.0, voltage);
	setFrequency(frequency * pow(2.0, 4));

	// CV1 -> trigger adsr
	static Average<uint32_t> cv1_Average(10);
	uint32_t cv_1 = (uint32_t)ptr[1];
	uint32_t cv_1a = cv1_Average.nextAverage(cv_1);

	static bool waitTriggerOn 	= true;
	static bool waitTriggerOff	= false;
	if (waitTriggerOn && (cv_1a < 0x4000))
	{
		waitTriggerOn	= false;
		waitTriggerOff	= true;
		mADSR_Ptr->handleTrigger();
	}
	if (waitTriggerOff && (cv_1a > 0x8000))
	{
		waitTriggerOn	= true;
		waitTriggerOff	= false;
	}

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

//	CasualNoises::NerveNetSlaveThread* threadPtr = gNerveNetSlaveThreadPtr[threadNo];

	// Handle all events
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
		case eSynthEngineMessageType::setFrequency:			// Set oscillator frequency
		case eSynthEngineMessageType::potentiometerValue:	// Update potentiometer values
			mPulsarSynthEnginePtr->processNerveNetData(threadNo, messageLength, ptr);
			break;
		case eSynthEngineMessageType::ADC_Value:			// ADC values
			process_ADC_data((uint16_t*)((uint32_t)ptr + sizeof(tNerveNetMessageHeader)));
			break;
		case eSynthEngineMessageType::triggerEvent:
//			mADSR_Ptr->handleTrigger();									ToDo: remove comment
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
	setTimeMarker_4();

	mPulsarSynthEnginePtr->processBlock(buffer, NN_buffer);

	uint32_t numSamples = NN_buffer.getNumSamples();
	const float* rptr0 = NN_buffer.getReadPointer(0);
	const float* rptr1 = NN_buffer.getReadPointer(1);
	float* wptr0 = NN_buffer.getWritePointer(0);
	float* wptr1 = NN_buffer.getWritePointer(1);
	for (uint32_t i = 0; i < numSamples; ++i)
	{
//		float gain = mADSR_Ptr->nextValue();					// ToDo: add adsr back in
		float gain = 1.0f;
		*wptr0++ = *rptr0++ * gain;
		*wptr1++ = *rptr1++ * gain;
	}

	resetTimeMarker_4();
}

//==============================================================================
//          setFrequency
//
// Set frequency of all parts
//
//  CasualNoises    28/07/2025  First implementation
//==============================================================================
void SouthSideAudioProcessor::setFrequency(float frequency)
{
	mPulsarSynthEnginePtr->setFrequency(frequency);
}

} // namespace CasualNoises
