/*
  ==============================================================================

    SubharmonicGenerator.h
    Created: 16/04/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

namespace CasualNoises
{

class SubharmonicGenerator
{
public:

	SubharmonicGenerator ( float sampleRate, float ratio = 0.5f ):
		mSampleRate ( sampleRate ),
		mRatio ( ratio )
	{

	}

	float getRatio ()
	{
		return mRatio;
	}

	void setRatio ( float ratio )
	{
		mRatio = ratio;
	}

    float processSample( float x )
    {

        // Estimate the fundamental frequency and apply a ratio to it
        float rect = std::fabs(x);
        bool zeroCross = ( rect > 0.1f && mLastSample <= 0.1f );
        ++mCounter;
        if ( zeroCross )
        {
            mLastPeriod = mCounter;
            mCounter = 0;
            float freq = 0.0f;
            if ( mLastPeriod > 0 )
            	freq = mSampleRate / ( (float) mLastPeriod * 2 );
            mSubFreq = freq * mRatio;

        }
        mLastSample = rect;

        // Generate sub-harmonic square wave and apply a low-pass filter
        mSubPhase += mSubFreq / mSampleRate;
        if (mSubPhase >= 1.0f)
            mSubPhase -= 1.0f;
        float sub = (mSubPhase < 0.5f) ? 1.0f : -1.0f;
        mLpState = mLpState + mLpCoeff * (sub - mLpState);

        return mLpState;

    }

private:

    float 	mSampleRate 	{ 44100.0f };

    float	mRatio			{ 1.0f };
    int		mLastPeriod		{ 0 };

    // State
    int 	mCounter		{ 0 };
    float 	mLastSample		{ 1.0f };
    float 	mSubPhase		{ 1.0f };
    float 	mSubFreq		{ 50.0f };
    float 	mLpState		{ 1.0f };

    // Low-pass filter coefficient
    float	mLpCoeff 		{ 0.05f };

};
} // namespace CasualNoises
