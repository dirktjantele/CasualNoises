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
	 LFO() = delete;
	~LFO() = default;

	LFO(float sampleRate, float frequency = 440.0f)
	: mSampleRate (sampleRate)
	{
		setFrequency(frequency);
	};

	inline void setFrequency(float frequency) noexcept
	{
		mFrequency	= frequency;
		mStep = (2 * pi * mFrequency) / mSampleRate;
	}

	inline const float getFrequency() const noexcept
	{
		return mFrequency;
	}

	const float nextSample() noexcept
	{
		float sample = sin(mAngle);
		mAngle += mStep;
		if (mAngle > (2 * pi))
			mAngle = 0.0f;
		return sample;
	}

private:
	float	mSampleRate	{ 0.0f };
	float	mFrequency	{ 0.0f };

	float	mAngle		{ 0.0f };
	float  	mStep		{ 0.0f };

};

} // namespace CasualNoises



