/*
  ==============================================================================

    Echo.h
    Created: 13/03/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "AbstractEffectEngine.h"

#include "AudioBasics/Buffers/AudioBuffer.h"

#include "maths.h"

namespace CasualNoises
{

//#ifdef debugging

class Echo : public AbstractEffectEngine
{
public:
	 Echo() = default;
	~Echo() = default;

	//==============================================================================
	//          prepareToPlay()
	//
	// Clear delay buffer
	//
	//  CasualNoises    13/03/2026  First implementation
	//==============================================================================
	void prepareToPlay ( float sampleRate, uint32_t maximumExpectedSamplesPerBlock, void* /*synthParams*/ ) noexcept
	{

		// Save parameters
		mSampleRate 					= sampleRate;
		mMaximumExpectedSamplesPerBlock = maximumExpectedSamplesPerBlock;

		// Allocate echo buffer in RAM_D2 for 2 channel float audio data
		mMaxDelayBufferSampleCnt = RAM_D2_SIZE / 4 / 2;
		mMaxDelayTime = mMaxDelayBufferSampleCnt / mSampleRate;

		// Clear delay buffer
		static __attribute__((section(".RAM_D2"), aligned(32)))
		float buffer [ 1 ];
		mDelayBufferPtr1 = buffer;
		mDelayBufferPtr2 = mDelayBufferPtr1 + ( mMaxDelayBufferSampleCnt / 2 );
		for ( uint32_t i = 0; i < mMaxDelayBufferSampleCnt * 2; ++i)
		{
			buffer [ i ] = 0.0f;
		}

	};

	//==============================================================================
	//          releaseResources()
	//
	//  CasualNoises    13/03/2026  First implementation
	//==============================================================================
	void releaseResources () noexcept
	{
		// Nothing to release here
	};

	//==============================================================================
	//          processNerveNetData()
	//
	//  CasualNoises    13/03/2026  First implementation
	//==============================================================================
	void processNerveNetData ( uint32_t threadNo, uint32_t size, uint8_t* ptr ) noexcept
	{

	};

	//==============================================================================
	//          applyPotentiometerValue()
	//
	//  CasualNoises    14/03/2026  First implementation
	//==============================================================================
	void updateDelayTime ( ) noexcept
	{
		float max = mMaxDelayBufferSampleCnt;
		float offset = cn_map ( mDelayTimeOffset, -1.0f,  1.0f, 10.0f, max);
		float target = mTargetDelayTime + ( offset * mDelayTimeGain );

		target = mTargetDelayTime;					// ToDo improve delay time CV control

		mDelayBufferSampleCnt = cn_map ( target, 0.0f,  65535.0f, 10.0f, max);
	}

	//==============================================================================
	//          applyPotentiometerValue()
	//
	//  CasualNoises    14/03/2026  First implementation
	//==============================================================================
	void applyPotentiometerValue ( ePotentioMeterId potId, float value )
	{
		switch ( potId )
		{
			case ePotentioMeterId::slider_5:	// Delay time
				{
					mTargetDelayTime = value;
					updateDelayTime ();
				}
				break;
			case ePotentioMeterId::slider_6:	// Feedback
				{
					mFeedback = cn_map ( value , 0.0f, 65535.0f, 0.0f, 0.99f);
				}
				break;
			case ePotentioMeterId::slider_7:	// Wet/Dry
				{
					mWetDry = cn_map ( value , 0.0f, 65535.0f, 0.0f, 1.0f);
				}
			case ePotentioMeterId::slider_8:	// Delay time cv gain
				{
					mDelayTimeGain = cn_map ( value , 0.0f, 65535.0f, 0.0f, 1.0f);
					updateDelayTime ();
				}
			break;
			default:
				break;
		}
	};

	//==============================================================================
	//          processBlock()
	//
	//  CasualNoises    13/03/2026  First implementation
	//==============================================================================
	void processBlock ( AudioBuffer* outBuffer, AudioBuffer* inBuffer ) noexcept
	{
/*
		// Simple all pass
		const float* lrptr = outBuffer->getReadPointer ( 0 );
		const float* rrptr = outBuffer->getReadPointer ( 1 );
		float* lwptr = outBuffer->getWritePointer( 0 );
		float* rwptr = outBuffer->getWritePointer( 1 );
		uint32_t noSamples = inBuffer->getNumSamples();

		float foo = 0.0f;
		uint32_t i = 0;
		for ( ; i < noSamples; ++i )
        {
			foo = *lrptr;
        	*lwptr++ = *lrptr++;
        	*rwptr++ = *rrptr++;
        }
*/


		assert ( inBuffer->getNumChannels() == 2 );
		const float* lrptr = inBuffer->getReadPointer ( 0 );						// ToDo: replace outBuffer with inBuffer
		const float* rrptr = inBuffer->getReadPointer ( 1 );
		float* lwptr = outBuffer->getWritePointer( 0 );
		float* rwptr = outBuffer->getWritePointer( 1 );
		uint32_t noSamples = inBuffer->getNumSamples();
        for (uint32_t i = 0; i < noSamples; ++i)
        {
        	float lin = *lrptr++;
        	float rin = *rrptr++;

        	float delayed1 = mDelayBufferPtr1 [ mReadIndex1 ];
        	float delayed2 = mDelayBufferPtr2 [ mReadIndex2 ];

        	mDelayBufferPtr1 [ mWriteIndex1 ] = lin + ( delayed1 * mFeedback);
        	mDelayBufferPtr2 [ mWriteIndex2 ] = rin + ( delayed2 * mFeedback);

        	*lwptr++ = (lin * (1.0f - mWetDry)) + (delayed1 * mWetDry);
        	*rwptr++ = (rin * (1.0f - mWetDry)) + (delayed2 * mWetDry);

        	incrementIndices();

        }

	};

	//==============================================================================
	//          applyControlVoltages
	//
	//  CasualNoises    13/03/2026  First implementation
	//==============================================================================
	void applyControlVoltages ( sControlVoltages* voltages )
	{
		mDelayTimeOffset = voltages->CV_Inputs [ CV_INPUT_2 ];
		updateDelayTime ();
	};

private:

	float 					mSampleRate 						{ 0.0f };
	uint32_t				mMaximumExpectedSamplesPerBlock 	{ 0 };

    float 					mFeedback							{ 0.5f };
    float 					mWetDry								{ 0.5f };

    float					mTargetDelayTime					{ 0.0f };		// From potentiometer
    float 					mDelayTimeOffset					{ 0.0f };		// From CV 2
    float					mDelayTimeGain						{ 0.0f };		// From potentiometer

	uint32_t				mMaxDelayBufferSampleCnt			{ 0 };
	uint32_t				mDelayBufferSampleCnt				{ 5 };
	float					mMaxDelayTime						{ 0.0f };
	float*					mDelayBufferPtr1					{ nullptr };
	float*					mDelayBufferPtr2					{ nullptr };

	uint32_t			 	mReadIndex1 						{  0 };
	uint32_t			 	mReadIndex2 						{  0 };

	uint32_t			 	mWriteIndex1 						{  0 };
	uint32_t			 	mWriteIndex2 						{  0 };

	//==============================================================================
	//          incrementIndices
	//
	//  CasualNoises    13/03/2026  First implementation
	//==============================================================================
   void incrementIndices ()
    {

       	mReadIndex1  = (mReadIndex1 + 1) % mDelayBufferSampleCnt;
       	mReadIndex2  = (mReadIndex2 + 1) % mDelayBufferSampleCnt;

    	mWriteIndex1 = ( mWriteIndex1 + 1 ) % mDelayBufferSampleCnt;
    	mWriteIndex2 = ( mWriteIndex2 + 1 ) % mDelayBufferSampleCnt;

    }


};

//#endif

} // namespace CasualNoises
