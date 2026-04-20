/*
  ==============================================================================

    SaturatedSineWaveEngine.cpp
    Created: 16/04/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include <stdio.h>

#include "SaturatedSineWaveEngine.h"

#include "SouthSideAudioProcessor.h"
#include "AudioBasics/Buffers/AudioBuffer.h"

#include "Synthesizer/Basics/SaturatedSineWaveSynth.h"

#include "DSP/SubharmonicGenerator.h"

#include "NerveNet/NerveNetSlaveThread.h"
#include "NerveNet/NerveNetMessage.h"

#include "SynthEngineMessage.h"

#include "Core/Maths/MathsFunctions.h"

namespace CasualNoises
{

//==============================================================================
//          ~SaturatedSineWaveEngine()
//
//  CasualNoises    16/04/2026  First implementation
//==============================================================================
SaturatedSineWaveEngine::~SaturatedSineWaveEngine()
{
	if ( mSaturatedSineWaveSynthPtr != nullptr ) delete mSaturatedSineWaveSynthPtr;
}

//==============================================================================
//          prepareToPlay
//
// this method is called before the first call to processBlock()
//
//  CasualNoises    16/04/2026  First implementation
//==============================================================================
void SaturatedSineWaveEngine::prepareToPlay (
		float sampleRate,
		uint32_t maximumExpectedSamplesPerBlock )
{

	// store settings
	mSampleRate 					    = sampleRate;
	mMaximumExpectedSamplesPerBlock     = maximumExpectedSamplesPerBlock;
	float frequency 					= 440.0f;														// ToDo implement presets in flash

	mSaturatedSineWaveSynthPtr	 		= new SaturatedSineWaveSynth ( mSampleRate, frequency );
	mSubharmonicGeneratorPtr1			= new SubharmonicGenerator ( mSampleRate, 0.5f );
	mSubharmonicGeneratorPtr2			= new SubharmonicGenerator ( mSampleRate, 0.25f );
	mSubharmonicGeneratorPtr3			= new SubharmonicGenerator ( mSampleRate, 0.125f );
	mSubharmonicGeneratorPtr4			= new SubharmonicGenerator ( mSampleRate, 0.0625f );

}

//==============================================================================
//          releaseResources
//
// This method is called after the last call to processBlock()
//
//  CasualNoises    16/04/2026  First implementation
//==============================================================================
void SaturatedSineWaveEngine::releaseResources()											// ToDo save current state to flash
{
	if ( mSaturatedSineWaveSynthPtr != nullptr )	delete mSaturatedSineWaveSynthPtr;
	if ( mSubharmonicGeneratorPtr1	!= nullptr )	delete mSubharmonicGeneratorPtr1;
	if ( mSubharmonicGeneratorPtr2	!= nullptr )	delete mSubharmonicGeneratorPtr2;
	if ( mSubharmonicGeneratorPtr3	!= nullptr )	delete mSubharmonicGeneratorPtr3;
	if ( mSubharmonicGeneratorPtr4	!= nullptr )	delete mSubharmonicGeneratorPtr4;
}

//==============================================================================
//          processNerveNetData
//
// Process incoming NerveNet data
//
//  CasualNoises    16/04/2026  First implementation
//==============================================================================
void SaturatedSineWaveEngine::processNerveNetData(
		uint32_t threadNo,
		uint32_t size,
		uint8_t* ptr )
{
	CasualNoises::NerveNetSlaveThread* threadPtr = gNerveNetSlaveThreadPtr [ threadNo ];

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
		tRequestSetupInfoReplyData reply;
		reply.header.sourceID 		= eNerveNetSourceId::FellhornNorthSide;
		reply.header.destinationID	= eNerveNetSourceId::FellhornSouthSide;
		reply.header.messageTag 	= (uint32_t)eSynthEngineMessageType::requestSetupInfo;
		reply.header.messageLength 	= sizeof ( tRequestSetupInfoReplyData );
		reply.version				= 0x00010001;				// #1.1
		threadPtr->sendMessage( ( tNerveNetMessageHeader* ) &reply, reply.header.messageLength );
	}
		break;
	case eSynthEngineMessageType::setFrequency:			// Set oscillator frequency
	{
		tSetFrequencyMessage* messagePtr = (tSetFrequencyMessage*)ptr;
		mSaturatedSineWaveSynthPtr->setFrequency(messagePtr->frequency);
	}
		break;
	case eSynthEngineMessageType::potentiometerValue:	// Update potentiometer values
	{
		tPotValueMessage* messagePtr = (tPotValueMessage*)ptr;
		uint8_t multiplexerNo 		 = messagePtr->multiplexerNo;
		uint8_t multiplexerChannelNo = messagePtr->multiplexerChannelNo;
		ePotentioMeterId potId  = ( ePotentioMeterId )( ( multiplexerNo << 4	) + multiplexerChannelNo );
		float potValue = (float)messagePtr->potValue / 0x0000ffff;
		switch ( potId )
		{
		case ePotentioMeterId::pot_1:
			mFrequencyOffset = potValue * 8;
			mSaturatedSineWaveSynthPtr->setFrequency( mFrequencyOffset * 110.0f );
			break;
		case ePotentioMeterId::pot_2:
			break;
		case ePotentioMeterId::pot_3:
			break;
		case ePotentioMeterId::pot_4:
			break;
		case ePotentioMeterId::slider_1:
			{
				float saturation = jmap ( potValue, 0.0f,1.0f, 4.5f, 7.0f );
				mSaturatedSineWaveSynthPtr->setSaturation( saturation );
			}
			break;
		case ePotentioMeterId::slider_2:
			break;
		case ePotentioMeterId::slider_3:
			break;
		case ePotentioMeterId::slider_4:
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
//  CasualNoises    16/04/2026  First implementation
//==============================================================================
void SaturatedSineWaveEngine::processBlock ( AudioBuffer* /*buffer*/, AudioBuffer* NN_buffer )
{

	float* lwptr = NN_buffer->getWritePointer(0);
	float* rwptr = NN_buffer->getWritePointer(1);

	uint32_t noSamples = NN_buffer->getNumSamples();

	for ( uint32_t i = 0; i < noSamples; ++i )
	{
//		float gain = mGain;															// ToDo restore this line
		float gain = 1.0f;
		float sample_1 = mSaturatedSineWaveSynthPtr->nextSample () * gain;
		float value = sample_1;
		value += mSubharmonicGeneratorPtr1->processSample ( sample_1 ) * 0.01f;
		value += mSubharmonicGeneratorPtr2->processSample ( sample_1 ) * 0.005f;
		value += mSubharmonicGeneratorPtr3->processSample ( sample_1 ) * 0.005f;
		value += mSubharmonicGeneratorPtr4->processSample ( sample_1 ) * 0.005f;
		*lwptr++ = value;
		*rwptr++ = value;
	}

}

//==============================================================================
//          applyControlVoltages
//
// Apply control voltage values
//
//  CasualNoises    16/04/2026  First implementation
//==============================================================================
void SaturatedSineWaveEngine::applyControlVoltages ( sControlVoltages* voltages )
{
/*
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
*/
}

} // namespace CasualNoises

