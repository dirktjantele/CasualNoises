/*
  ==============================================================================

    DrawBarOrganEngine.cpp
    Created: 16/04/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include <stdio.h>

#include "DrawBarOrganEngine.h"

#include "SouthSideAudioProcessor.h"
#include "AudioBasics/Buffers/AudioBuffer.h"

#include "Synthesizer/Basics/LFO.h"

#include "DSP/SubharmonicGenerator.h"

#include "NerveNet/NerveNetSlaveThread.h"
#include "NerveNet/NerveNetMessage.h"

#include "SynthEngineMessage.h"

#include "Core/Maths/MathsFunctions.h"

namespace CasualNoises
{

//==============================================================================
//          ~DrawBarOrganEngine()
//
//  CasualNoises    16/04/2026  First implementation
//==============================================================================
DrawBarOrganEngine::~DrawBarOrganEngine()
{
	if ( mLFO_Ptr != nullptr ) delete mLFO_Ptr;
}

//==============================================================================
//          getRatio
//
// this method is called before the first call to processBlock()
//
//  CasualNoises    16/04/2026  First implementation
//==============================================================================
inline float DrawBarOrganEngine::getRatio ( float interval ) const noexcept
{
	return std::pow ( 2.0f, interval / 12.0f );
}

//==============================================================================
//          prepareToPlay
//
// this method is called before the first call to processBlock()
//
//  CasualNoises    16/04/2026  First implementation
//==============================================================================
void DrawBarOrganEngine::prepareToPlay (
		float sampleRate,
		uint32_t maximumExpectedSamplesPerBlock )
{

	mSampleRate 					    = sampleRate;
	mMaximumExpectedSamplesPerBlock     = maximumExpectedSamplesPerBlock;
	float frequency 					= 440.0f;														// ToDo implement presets in flash
	mLFO_Ptr					 		= new LFO ( mSampleRate, frequency );
	mRatio1 = getRatio (   4.0f );
	mRatio2 = getRatio (   7.0f );
	mRatio3 = getRatio (  11.0f );
	mRatio4 = getRatio (  12.0f );
	mRatio5 = getRatio (  -4.0f );
	mRatio6 = getRatio ( -12.0f );
	mSubharmonicGeneratorPtr1			= new SubharmonicGenerator ( mSampleRate, mRatio1 );
	mSubharmonicGeneratorPtr2			= new SubharmonicGenerator ( mSampleRate, mRatio2 );
	mSubharmonicGeneratorPtr3			= new SubharmonicGenerator ( mSampleRate, mRatio3 );
	mSubharmonicGeneratorPtr4			= new SubharmonicGenerator ( mSampleRate, mRatio4 );
	mSubharmonicGeneratorPtr5			= new SubharmonicGenerator ( mSampleRate, mRatio5 );
	mSubharmonicGeneratorPtr6			= new SubharmonicGenerator ( mSampleRate, mRatio6 );

	mDetuneLFO_Ptr						= new LFO ( mSampleRate, 0.0f );

}

//==============================================================================
//          releaseResources
//
// This method is called after the last call to processBlock()
//
//  CasualNoises    16/04/2026  First implementation
//==============================================================================
void DrawBarOrganEngine::releaseResources()											// ToDo save current state to flash
{
	if ( mLFO_Ptr					!= nullptr )	delete mLFO_Ptr;
	if ( mDetuneLFO_Ptr 			!= nullptr )	delete mDetuneLFO_Ptr;
	if ( mSubharmonicGeneratorPtr1	!= nullptr )	delete mSubharmonicGeneratorPtr1;
	if ( mSubharmonicGeneratorPtr2	!= nullptr )	delete mSubharmonicGeneratorPtr2;
	if ( mSubharmonicGeneratorPtr3	!= nullptr )	delete mSubharmonicGeneratorPtr3;
	if ( mSubharmonicGeneratorPtr4	!= nullptr )	delete mSubharmonicGeneratorPtr4;
	if ( mSubharmonicGeneratorPtr5	!= nullptr )	delete mSubharmonicGeneratorPtr5;
	if ( mSubharmonicGeneratorPtr6	!= nullptr )	delete mSubharmonicGeneratorPtr6;
}

//==============================================================================
//          updateFrequency
//
//  CasualNoises    17/04/2026  First implementation
//==============================================================================
inline void DrawBarOrganEngine::updateFrequency () noexcept
{
	mFrequency = mTargetFrequency + mFrequencyCourse + mFrequencyFine;
	constexpr double f0 = 8.1757989156;
	float frequency = f0 * std::pow ( 2.0, mFrequency );
	mLFO_Ptr->setFrequency ( frequency );


}

//==============================================================================
//          processNerveNetData
//
// Process incoming NerveNet data
//
//  CasualNoises    16/04/2026  First implementation
//==============================================================================
void DrawBarOrganEngine::processNerveNetData(
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
				mFrequencyCourse = potValue * 8;
				updateFrequency ();
				break;
			case ePotentioMeterId::pot_2:
				mFrequencyFine = potValue / 4.0f;
				updateFrequency ();
				break;
			case ePotentioMeterId::pot_3:
				mGainOffset = potValue;
				break;
			case ePotentioMeterId::pot_4:
				mSaturation = potValue;
				break;
			case ePotentioMeterId::slider_1:
				mGain1 = potValue;
				break;
			case ePotentioMeterId::slider_2:
				mGain2 = potValue;
				break;
			case ePotentioMeterId::slider_3:
				mGain3 = potValue;
				break;
			case ePotentioMeterId::slider_4:
				mGain4 = potValue;
				break;
			case ePotentioMeterId::slider_5:
				mGain5 = potValue;
				break;
			case ePotentioMeterId::slider_6:
				mGain6 = potValue;
				break;
			case ePotentioMeterId::slider_7:
				mDetuneFrequency = potValue * 1000.0f;
				mDetuneLFO_Ptr->setFrequency ( mDetuneFrequency );
				break;
			case ePotentioMeterId::slider_8:
				mDetune = potValue / 10;
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
//          applyControlVoltages
//
// Apply control voltage values
//
//  CasualNoises    16/04/2026  First implementation
//==============================================================================
void DrawBarOrganEngine::applyControlVoltages ( sControlVoltages* voltages )
{

	mTargetFrequency = voltages->_1V_OCT_1;
	updateFrequency ();

/*
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

//==============================================================================
//          saturate
//
//  CasualNoises    16/04/2026  First implementation
//==============================================================================
inline float DrawBarOrganEngine::saturate ( float value, float amount )  const noexcept
{
	float res = sqrt ( std::fabs( value ) );
	if ( value < 0.0f )
		res *= -1;
	return ( ( value * ( 1.0f - amount ) ) + ( res * amount ) );
}

//==============================================================================
//          processBlock
//
// This method is called each time a new audio block needs to be processed
//
//  CasualNoises    16/04/2026  First implementation
//==============================================================================
void DrawBarOrganEngine::processBlock ( AudioBuffer* /*buffer*/, AudioBuffer* NN_buffer )
{

	float* lwptr = NN_buffer->getWritePointer(0);
	float* rwptr = NN_buffer->getWritePointer(1);

	uint32_t noSamples = NN_buffer->getNumSamples();

	float detune = mDetuneLFO_Ptr->nextSample () * mDetune;
	mSubharmonicGeneratorPtr1->setRatio ( mRatio1 + detune );
	mSubharmonicGeneratorPtr2->setRatio ( mRatio2 + detune );
	mSubharmonicGeneratorPtr3->setRatio ( mRatio3 + detune );
	mSubharmonicGeneratorPtr4->setRatio ( mRatio4 + detune );
	mSubharmonicGeneratorPtr5->setRatio ( mRatio5 + detune );
	mSubharmonicGeneratorPtr6->setRatio ( mRatio6 + detune );

	for ( uint32_t i = 0; i < noSamples; ++i )
	{
		float gain = mGainOffset;
		float endGain = ( 1.0f + mGain1 + mGain2 + mGain3 + mGain4  + mGain5  + mGain6 ) * 1.2f;
		float root = mLFO_Ptr->nextSample ();
		float value = ( root * ( 1.0f / endGain ) ) * gain;
		value += mSubharmonicGeneratorPtr1->processSample ( root ) * ( ( mGain1 * gain ) / endGain );
		value += mSubharmonicGeneratorPtr2->processSample ( root ) * ( ( mGain2 * gain ) / endGain );
		value += mSubharmonicGeneratorPtr3->processSample ( root ) * ( ( mGain3 * gain ) / endGain );
		value += mSubharmonicGeneratorPtr4->processSample ( root ) * ( ( mGain4 * gain ) / endGain );
		value += mSubharmonicGeneratorPtr5->processSample ( root ) * ( ( mGain5 * gain ) / endGain );
		value += mSubharmonicGeneratorPtr6->processSample ( root ) * ( ( mGain6 * gain ) / endGain );
		value = saturate( value, mSaturation );
		*lwptr++ = value;
		*rwptr++ = value;
	}

}

} // namespace CasualNoises
