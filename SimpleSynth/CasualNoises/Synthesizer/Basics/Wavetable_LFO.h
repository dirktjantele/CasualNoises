/*
  ==============================================================================

    Wavetable_LFO.h
    Created: Dec 5, 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include <arm_math.h>
#include "maths.h"

namespace CasualNoises
{

class Wavetable_LFO
{
public:
	 Wavetable_LFO() = delete;

	//==============================================================================
	//          Wavetable_LFO
	//
	// 	Calculate basic waves that fill up the table
	//					mWavetable[0] = sine wave
	//					mWavetable[1] = triangle wave
	//					mWavetable[2] = sawtooth wave
	//					mWavetable[3] = square wave
	//					mWavetable[4] = sine wave
	//					mWavetable[5] = sawtooth wave
	//					mWavetable[6] = triangle wave
	//					mWavetable[7] = square wave
	//					mWavetable[8] = sine wave
	//
	//  CasualNoises    06/12/2024  First implementation
	//==============================================================================
	Wavetable_LFO(float sampleRate, float frequency = 440.0f)
	: mSampleRate (sampleRate)
	{
		setFrequency(frequency);

		// Allocate memory for the wave table and it's waves
		mActualWavePtr = new float[mWaveLength];
		mWavetable = new float*[mWavetableCount];
		for (uint32_t i = 0; i < mNoOfWavesToCalculate; ++i)
		{
			mWavetable[i] = new float[mWaveLength];
		}
		mWavetable[4] = mWavetable[0];
		mWavetable[5] = mWavetable[2];
		mWavetable[6] = mWavetable[1];
		mWavetable[7] = mWavetable[3];
		mWavetable[8] = mWavetable[0];

		// Calculate sine wave, this is also the default wave
		float angle = 0.0f;
		float step  = (2 * pi) / mWaveLength;
		for (uint32_t i = 0; i < mWaveLength; ++i)
		{
			mWavetable[0][i] = sin(angle);
			mActualWavePtr[i] = mWavetable[0][i];
			angle += step;
		}

		// Calculate triangle wave
		step = 1.0f / static_cast<float>(mWaveLength / 4);
		uint32_t i = 0;
		for (float v = 0.0f; i < (mWaveLength / 4); ++i, v += step)
		{
			mWavetable[1][i] = v;
		}
		for (float v = 1.0f; i < (mWaveLength / 4) + (mWaveLength / 2); ++i, v -= step)
		{
			mWavetable[1][i] = v;
		}
		for (float v = -1.0f; i < mWaveLength; ++i, v += step)
		{
			mWavetable[1][i] = v;
		}

		// Calculate sawtooth wave
		step = 2.0f / static_cast<float>(mWaveLength);
		i = 0;
		for (float v = -1.0f; i < mWaveLength; ++i, v += step)
		{
			mWavetable[2][i] = v;
		}

		// Calculate square wave
		i = 0;
		for ( ; i < (mWaveLength / 2); ++i)
		{
			mWavetable[3][i] = 1.0f;
		}
		for ( ; i < mWaveLength; ++i)
		{
			mWavetable[3][i] = -1.0f;
		}

	};

	//==============================================================================
	//          ~Wavetable_LFO
	//
	//  CasualNoises    06/12/2024  First implementation
	//==============================================================================
	~Wavetable_LFO()
	{
		for (int i = 0; i < mWavetableCount - 1; ++i)
		{
			delete mWavetable[i];
		}
		delete[] mWavetable;
	}

	//==============================================================================
	//          setFrequency
	//
	//  CasualNoises    06/12/2024  First implementation
	//==============================================================================
	inline void setFrequency(float frequency) noexcept
	{
		mFrequency	= frequency;
		mStep = (mFrequency / mSampleRate) * mWaveLength;
	}

	//==============================================================================
	//          getFrequency
	//
	//  CasualNoises    06/12/2024  First implementation
	//==============================================================================
	inline const float getFrequency() const noexcept
	{
		return mFrequency;
	}

	//==============================================================================
	//          nextSample
	//
	//  CasualNoises    06/12/2024  First implementation
	//==============================================================================
	const float nextSample() noexcept
	{
		float nextWaveIndex = mWaveIndex + mStep;
		if (nextWaveIndex > mWaveLength)
			nextWaveIndex -= mWaveLength;

		int integerPart1 = static_cast<int>(mWaveIndex);
		int integerPart2 = static_cast<int>(nextWaveIndex);
		float fraction = mWaveIndex - static_cast<float>(integerPart1);

		float sample = (mActualWavePtr[integerPart1] * (1.0f - fraction)) + (mActualWavePtr[integerPart1] * fraction);

		mWaveIndex = nextWaveIndex;

		return sample;
	}

private:
	float				mSampleRate				{ 0.0f };
	float				mFrequency				{ 0.0f };

	const uint32_t		mNoOfWavesToCalculate	{ 4 };
	const uint32_t		mWavetableCount			{ 9 };
	const uint32_t		mWaveLength				{ 1024 };

	float** 			mWavetable	 			{ nullptr };
	float*				mActualWavePtr			{ nullptr };

	float  				mStep					{ 0.0f };
	float			 	mWaveIndex				{ 0.0f };
	float 				mTableNumber			{ 0.0f };			// 0.0 <= mTableNumber < mWavetableCount

};

} // namespace CasualNoises

