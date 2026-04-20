/*
  ==============================================================================

    SaturatedSineWaveSynth.h
    Created: 16/04/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_AUDIO_BUFFER

#pragma once

#include <arm_math.h>
#include "maths.h"

namespace CasualNoises
{

//==============================================================================
//          SaturatedSineWaveSynth
//
//  CasualNoises    16/04/2026  First implementation
//==============================================================================
class SaturatedSineWaveSynth final
{
public:

	  SaturatedSineWaveSynth() = delete;
	 ~SaturatedSineWaveSynth()
	 {
//			 delete mPulsarCurvePtr;
	 };

	 SaturatedSineWaveSynth ( const SaturatedSineWaveSynth& ) = default;
	 SaturatedSineWaveSynth ( SaturatedSineWaveSynth&& ) 	  = default;

	//==============================================================================
	//          SaturatedSineWaveSynth
	//
	//  CasualNoises    16/04/2026  First implementation
	//==============================================================================
	 SaturatedSineWaveSynth ( float sampleRate, float frequency ) :
		 mSampleRate ( sampleRate ),
		 mFrequency ( frequency )
	{
		 setFrequency ( frequency );
/*
		 // Calculate initial pulsar curve
		 mPulsarCurvePtr = new float[mWaveLength];
		 for (uint32_t i = 0; i < mWaveLength; ++i)
			 mPulsarCurvePtr[i] = i;

		 // Sync with CachedWavetable_LFO
		 mPulsarStep = mStep;

		 // Update pulsar curve
		 setFormant ( 0.0f );
*/
	}

	//==============================================================================
	//          nextSample
	//
	//  CasualNoises    16/04/2026  First implementation
	//==============================================================================
	float nextSample ()
	{

		float pure = sin ( mAngle );

//		float saturated = std::tanh ( ( mSaturation + 1.0f ) * pure );
//		float saturated = std::atan ( pi / 2 ) * ( ( mSaturation + 1.0f ) * pure );
		float saturated = std::sin ( ( ( mSaturation * 4 ) + 1.0f ) * ( pure / twoPi ) );

		mAngle += mStep;
		if (mAngle > twoPi)
			mAngle -= twoPi;

		return saturated * 0.8f;

	}

	//==============================================================================
	//          setFrequency
	//
	//  CasualNoises    16/04/2026  First implementation
	//==============================================================================
	void setFrequency ( float frequency )
	{
		mFrequency	= frequency;
		mStep = ( twoPi * mFrequency ) / mSampleRate;
	}

	//==============================================================================
	//          setSaturation
	//
	//  CasualNoises    16/04/2026  First implementation
	//==============================================================================
	void setSaturation ( float saturation )
	{
		mSaturation	= saturation;
	}

private:

	float	mSampleRate		{ 0.0f };
	float	mFrequency		{ 0.0f };

	float	mAngle			{ 0.0f };
	float  	mStep			{ 0.0f };

	float	mSaturation		{ 0.0f };

};

} // namespace CasualNoises

#endif
