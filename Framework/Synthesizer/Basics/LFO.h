/*
  ==============================================================================

    LFO.h
    Created: 1 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include <arm_math.h>
#include "maths.h"

namespace CasualNoises
{

class LFO
{
public:
	 LFO () = delete;
	 LFO ( const LFO& ) = default;
	 LFO ( LFO&& ) = default;
	~LFO () = default;

	//==============================================================================
	//          LFO()
	//
	//  CasualNoises    14/12/2024  First implementation
	//==============================================================================
	LFO(float sampleRate, float frequency = 440.0f)
	: mSampleRate (sampleRate)
	{
		setFrequency(frequency);
	};

	//==============================================================================
	//          setFrequency()
	//
	//  CasualNoises    14/12/2024  First implementation
	//==============================================================================
	inline void setFrequency(float frequency) noexcept
	{
		mFrequency	= frequency;
		mStep = (2 * pi * mFrequency) / mSampleRate;
	}

	//==============================================================================
	//          getFrequency()
	//
	//  CasualNoises    14/12/2024  First implementation
	//==============================================================================
	inline const float getFrequency() const noexcept
	{
		return mFrequency;
	}

	//==============================================================================
	//          nextSample()
	//
	//  CasualNoises    14/12/2024  First implementation
	//==============================================================================
	const float nextSample() noexcept
	{
		float sample = sin(mAngle);
		mAngle += mStep;
		if (mAngle > (2 * pi))
			mAngle = 0.0f;
		return sample;
	}

	//==============================================================================
	//          reset()
	//
	//  CasualNoises    14/12/2024  First implementation
	//==============================================================================
	void reset() noexcept
	{
		mAngle = 0.0f;
	}

private:
	float	mSampleRate	{ 0.0f };
	float	mFrequency	{ 0.0f };

	float	mAngle		{ 0.0f };
	float  	mStep		{ 0.0f };

};

} // namespace CasualNoises



