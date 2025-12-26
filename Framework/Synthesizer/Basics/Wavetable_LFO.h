/*
  ==============================================================================

    Wavetable_LFO.h
    Created: 05/12/2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef USE_AUDIO_BUFFER

#pragma once

#include <arm_math.h>
#include "maths.h"

#include "AudioBasics/Buffers/AudioBuffer.h"

namespace CasualNoises
{

//==============================================================================
//          Wavetable_LFO
//
//  CasualNoises    06/12/2024  First implementation
//==============================================================================
class Wavetable_LFO final
{
public:

	 Wavetable_LFO() = delete;

	 Wavetable_LFO ( const Wavetable_LFO& ) = default;
	 Wavetable_LFO ( Wavetable_LFO&& ) 		= default;

	//==============================================================================
	//          Wavetable_LFO
	//
	//  CasualNoises    06/12/2024  First implementation
	//==============================================================================
	Wavetable_LFO(float sampleRate, float frequency)
	: mWaveIndex ( 0.0f ),
	  mSampleRate ( sampleRate ),
	  mFrequency ( frequency )
	{
		setFrequency(mFrequency);
		setMorphFactor(0.0f);
		createWaveTable();
	};

	//==============================================================================
	//          ~Wavetable_LFO
	//
	//  CasualNoises    06/12/2024  First implementation
	//==============================================================================
	~Wavetable_LFO()
	{
		deleteWaveTable();
	}

	//==============================================================================
	//          setFrequency
	//
	//  CasualNoises    06/12/2024  First implementation
	//==============================================================================
	virtual void setFrequency(float frequency) noexcept
	{
		mFrequency	= frequency;
		mStep 		= (mFrequency / mSampleRate) * mWaveLength;
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
	//          setMorphFactor
	//
	// factor:	0.0f -> 1.0f
	//
	//  CasualNoises    14/12/2024  First implementation
	//==============================================================================
	virtual void setMorphFactor(float morph) noexcept
	{

		if (morph != mMorphAmount)
		{

			// Limit morph amount
			mMorphAmount = morph;
			mMorphAmount = cn_limit(mMorphAmount, 0.0f, 1.0f);

			// Get wave index into the wave table
			float indx = mMorphAmount * (mWavetableCount - 1);
			mIndexTable1 = static_cast<uint32_t>(std::floor(indx));
			mIndexTable2 = mIndexTable1 + 1;
			if (mIndexTable2 >= mWavetableCount)
			{
				mIndexTable2 = 0;
			}

			// Calculate scaling for both waves
			float fac = fmod(indx, 1.0f);
			mScale1 = 1.0f - fac;
			mScale2 = fac;

		}

	}

	//==============================================================================
	//          getMorphFactor
	//
	//  CasualNoises    14/12/2024  First implementation
	//==============================================================================
	inline float getMorphFactor() const noexcept
	{
		return mMorphAmount;
	}

	//==============================================================================
	//          nextSample
	//
	//  CasualNoises    06/12/2024  First implementation
	//==============================================================================
	virtual float nextSample() noexcept
	{
		float nextWaveIndex = mWaveIndex + mStep;
		if (nextWaveIndex >= mWaveLength)
			nextWaveIndex -= mWaveLength;

		uint32_t integerPart1 = static_cast<uint32_t>(mWaveIndex);
		int integerPart2 = static_cast<int>(nextWaveIndex);
		float fraction = mWaveIndex - static_cast<float>(integerPart1);

		float sample1 = (mWavetable[mIndexTable1][integerPart1] * (1.0f - fraction)) + (mWavetable[mIndexTable1][integerPart2] * fraction);
		float p1 = sample1 * mScale1;
		float sample2 = (mWavetable[mIndexTable2][integerPart1] * (1.0f - fraction)) + (mWavetable[mIndexTable2][integerPart2] * fraction);
		float p2 = sample2 * mScale2;
		float sample = p1 + p2;

		mWaveIndex = nextWaveIndex;

		return sample;
	}

	//==============================================================================
	//          reset()
	//
	//  CasualNoises    14/12/2024  First implementation
	//==============================================================================
	void reset() noexcept
	{
		mWaveIndex = 0.0f;
	}

	//==============================================================================
	//          fillAudioBuffer()
	//
	// Fill an audio buffer with one cycle of the lfo
	//
	//  CasualNoises    28/02/2025  First implementation
	//==============================================================================
	void fillAudioBuffer(AudioBuffer& audioBuffer) noexcept
	{

		// Save current state
		float curSampleRate = mSampleRate;
		float curFrequency  = mFrequency;
		float curStep		= mStep;
		float waveIndex     = mWaveIndex;

		mSampleRate = (float)audioBuffer.getNumSamples();
		setFrequency(1.0f);
		float* wptr1 = audioBuffer.getWritePointer(0);
		float* wptr2 = nullptr;
		if (audioBuffer.getNumChannels() > 1)
			wptr2 = audioBuffer.getWritePointer(1);
		for (uint32_t i = 0; i < mSampleRate; ++i)
		{
			float sample = Wavetable_LFO::nextSample();
			*wptr1++ = sample;
			if (wptr2 != nullptr)
				*wptr2++ = sample;
		}

		// Restore lfo state
		mSampleRate = curSampleRate;
		mFrequency  = curFrequency;
		mStep		= curStep;
		mWaveIndex  = waveIndex;

	}

protected:

	// Calculated from the target frequency
	float			 	mWaveIndex				{ 0.0f };
	float  				mStep					{ 0.0f };

	// No of samples in a single wave
	const uint32_t		mWaveLength				{ 1024 };

	const uint32_t		mNoOfWavesToCalculate	{ 4 };
	const uint32_t		mWavetableCount			{ 9 };

private:

	//==============================================================================
	//          createWaveTable()
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
	//  CasualNoises    22/07/2024  Saw wave starts at 0.0f now to avoid the need for
	//									normalisation of the morphed result
	//==============================================================================
	void createWaveTable()
	{

		// Do we already have a wavetable?
		if (mWavetable != nullptr)
			return;

		// Allocate memory for the wave table and it's waves
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
		for (float v = 0.0f; i < (mWaveLength / 2); ++i, v += step)
		{
			mWavetable[2][i] = v;
		}
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

	}

	//==============================================================================
	//          deleteWaveTable()
	//
	//  CasualNoises    06/12/2024  First implementation
	//==============================================================================
	void deleteWaveTable()
	{
		for (uint32_t i = 0; i < mWavetableCount - 1; ++i)
		{
			delete mWavetable[i];
		}
		delete[] mWavetable;
		mWavetable = nullptr;
	}

	float				mSampleRate				{ 0.0f };
	float				mFrequency				{ 0.0f };
	float				mMorphAmount			{ -1.0f };			// 0.0f <= mMorphAmount <= 1.0f

	static float** 		mWavetable;

	// Calculated from morph factor
	uint32_t			mIndexTable1			{ 0 };
	uint32_t			mIndexTable2			{ 0 };
	float 				mScale1					{ 0.0f };
	float 				mScale2					{ 0.0f };

};

class CachedWavetable_LFO : public Wavetable_LFO
{
public:

	 CachedWavetable_LFO() = delete;
	~CachedWavetable_LFO()
	{
		if (mWavetablePtr != nullptr)
			delete mWavetablePtr;
	};

	CachedWavetable_LFO ( const CachedWavetable_LFO& ) = default;
	CachedWavetable_LFO ( CachedWavetable_LFO&& ) 	   = default;

	//==============================================================================
	//          CachedWavetable_LFO
	//
	//  CasualNoises    21/07/2025  First implementation
	//==============================================================================
	CachedWavetable_LFO(float sampleRate, float frequency)
	: Wavetable_LFO(sampleRate, frequency)
	{
		mWavetablePtr = new float[mWaveLength];
		reset();
		AudioBuffer audioBuffer = AudioBuffer(mWaveLength, 1);
		fillAudioBuffer(audioBuffer);
		const float* rptr = audioBuffer.getReadPointer(0);
		for (uint32_t i = 0; i < mWaveLength; ++i)
		{
			mWavetablePtr[i] = *rptr++;
		}
		setMorphFactor(0.0f);
	};

	//==============================================================================
	//          setMorphFactor
	//
	// factor:	0.0f -> 1.0f
	//
	//  CasualNoises    21/07/2025  First implementation
	//==============================================================================
	// note: takes around 1.6msec on an 550MHz STM32H723
	virtual void setMorphFactor(float morph) noexcept override
	{

		if (morph != getMorphFactor())
		{

			// Update wave table morpfh factor
			Wavetable_LFO::setMorphFactor(morph);

			// Fill wave table buffer
			float max = -1.0f;
			float min =  1.0f;
			AudioBuffer audioBuffer = AudioBuffer(mWaveLength, 1);
			fillAudioBuffer(audioBuffer);
			const float* rptr = audioBuffer.getReadPointer(0);
			for (uint32_t i = 0; i < mWaveLength; ++i)
			{
				float sample = *rptr++;
				if (sample > max)
					max = sample;
				if (sample < min)
					min = sample;
				mWavetablePtr[i] = sample;
			}

		}

	}

	//==============================================================================
	//          nextSample
	//
	//  CasualNoises    21/07/2025  First implementation
	//==============================================================================
	float nextSample() noexcept override
	{

		float nextWaveIndex = mWaveIndex + mStep;
		if (nextWaveIndex >= mWaveLength)
			nextWaveIndex -= mWaveLength;

		float sample = mWavetablePtr[static_cast<uint32_t>(mWaveIndex)];

		mWaveIndex = nextWaveIndex;

		return sample;

	}


private:

	float* 	mWavetablePtr		{ nullptr };

};

} // namespace CasualNoises

#endif
