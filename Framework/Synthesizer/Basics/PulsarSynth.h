/*
  ==============================================================================

    PulsarSynth.h
    Created: 21/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef USE_AUDIO_BUFFER

#pragma once

#include "Wavetable_LFO.h"

namespace CasualNoises
{

//==============================================================================
//          PulsarSynth
//
//  CasualNoises    21/07/2025  First implementation
//==============================================================================
class PulsarSynth : public CachedWavetable_LFO
{
public:

	  PulsarSynth() = delete;
	 ~PulsarSynth()
	 {
		 if (mPulsarCurvePtr != nullptr)
			 delete mPulsarCurvePtr;
	 };

	 PulsarSynth ( const PulsarSynth& ) = default;
	 PulsarSynth ( PulsarSynth&& ) 		= default;

	//==============================================================================
	//          PulsarSynth
	//
	//  CasualNoises    06/12/2024  First implementation
	//==============================================================================
	 PulsarSynth(float sampleRate, float frequency = 440.0f)
	: CachedWavetable_LFO(sampleRate, frequency)
	{

		 // Calculate initial pulsar curve
		 mPulsarCurvePtr = new float[mWaveLength];
		 for (uint32_t i = 0; i < mWaveLength; ++i)
			 mPulsarCurvePtr[i] = i;

		 setFormant(0.0f);

		 mPulsarStep = mStep;

	};

	//==============================================================================
	//          nextSample
	//
	//  CasualNoises    21/07/2025  First implementation
	//==============================================================================
	float nextSample() noexcept override
	{

		// Calculate next sample
		float sample = 0.0f;
		int32_t idx = static_cast<int32_t>(mPulsarWaveIndex);
		if (mPulsarCurvePtr[idx] >= 0.0f)
		{
			CachedWavetable_LFO::mWaveIndex = mPulsarCurvePtr[idx];
			sample = CachedWavetable_LFO::nextSample();
		}

		// Increase index, on next cycle update morph amount to avoid clicks
		mPulsarWaveIndex += mPulsarStep;
		if (mPulsarWaveIndex >= mWaveLength)
		{
			mPulsarWaveIndex -= mWaveLength;
			CachedWavetable_LFO::setMorphFactor(mMorph);
		}

		return sample;

	}

	//==============================================================================
	//          setMorphFactor
	//
	// factor:	0.0f -> 1.0f
	//
	//  CasualNoises    21/07/2025  First implementation
	//==============================================================================
	void setMorphFactor(float morph) noexcept
	{
		if (morph != mMorph)
		{
			// Only use part of the wave table
			mMorph = cn_map(morph, 0.0f, 1.0f, 0.0f, (float)(1.0f / 8.0f) * 3.0f);
		}
	}

	//==============================================================================
	//          setFormant
	//
	// Set formant value by updating the pulsar curve
	//
	//  CasualNoises    21/07/2025  First implementation
	//==============================================================================
	void setFormant(float formant)
	{

		mFormant = cn_limit(formant, 0.0f, 1.0f);
		mFormant = cn_map((float)mFormant, 0.0f, 1.0f, (float)mWaveLength, (float)(mWaveLength / 8.0f));

		float i = 0;
		float c = 1.5f;
		for (; i < mFormant; ++i)
			mPulsarCurvePtr[static_cast<uint32_t>(i)] = (i * mWaveLength) / mFormant;
		for (; i < mWaveLength; ++i)
			mPulsarCurvePtr[static_cast<uint32_t>(i)] = -1.0f;

	}

private:

	float 		mMorph				{ 0.0f };

	float*		mPulsarCurvePtr		{ nullptr };

	float		mPulsarWaveIndex	{ 0.0f };
	float		mPulsarStep			{ 0.0f };

	float		mFormant			{ 0.0f };

};

} // namespace CasualNoises

#endif

