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

#include "../../AudioBasics/Processors/WaveFolder.h"

namespace CasualNoises
{

//==============================================================================
//          PulsarSynth
//
//  CasualNoises    21/07/2025  First implementation
//==============================================================================
class PulsarSynth final : public CachedWavetable_LFO
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
	 PulsarSynth(float sampleRate, float frequency)
	: CachedWavetable_LFO(sampleRate, frequency)
	{

		 // Calculate initial pulsar curve
		 mPulsarCurvePtr = new float[mWaveLength];
		 for (uint32_t i = 0; i < mWaveLength; ++i)
			 mPulsarCurvePtr[i] = i;

		 // Sync with CachedWavetable_LFO
		 mPulsarStep = mStep;

		 // Update pulsar curve
		 setFormant(0.0f);

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
			sample = fold(sample, mWaveFold);
		}

		// Increase index, on next cycle update morph amount to avoid clicks
		//      also update pulsar curve
		mPulsarWaveIndex += mPulsarStep;
		if (mPulsarWaveIndex >= mWaveLength)
		{
			setTimeMarker_1();												// ToDo remove this
			mPulsarWaveIndex -= mWaveLength;
			CachedWavetable_LFO::setMorphFactor(mMorph);
			refresh();
			resetTimeMarker_1();											// ToDo remove this
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
	// Set formant value, refresh is postponed until start of next wave cycle
	//
	//  CasualNoises    21/07/2025  First implementation
	//==============================================================================
	void setFormant(float formant) noexcept
	{

		mFormant = cn_limit(formant, 0.0f, 1.0f);
		mFormant = cn_map((float)mFormant, 0.0f, 1.0f, (float)mWaveLength, (float)(mWaveLength / 8.0f));
	}

	//==============================================================================
	//          setCluster
	//
	// Set cluster value, refresh is postponed until start of next wave cycle
	//
	//  CasualNoises    22/07/2025  First implementation
	//==============================================================================
	void setCluster(float cluster) noexcept
	{

		mCluster = cn_limit(cluster, 0.0f, 1.0f);
		mCluster = cn_map((float)mCluster, 0.0f, 1.0f, 1.0f, 5.0f);
	}

	//==============================================================================
	//          setWaveFolder
	//
	// Set wave folder value
	//
	//  CasualNoises    22/07/2025  First implementation
	//==============================================================================
	void setWaveFolder(float fold) noexcept
	{
		mWaveFold = cn_limit(fold, 0.0f, 1.0f);
		mWaveFold = cn_map((float)mWaveFold, 0.0f, 1.0f, 1.0f, 10.0f);
	}

	//==============================================================================
	//          setFrequency
	//
	//  CasualNoises    28/07/2025  First implementation
	//==============================================================================
	virtual void setFrequency(float frequency) noexcept
	{
		CachedWavetable_LFO::setFrequency(frequency);
		mPulsarStep = mStep;
	}

private:

	float 		mMorph				{ 0.0f };

	float*		mPulsarCurvePtr		{ nullptr };

	float		mPulsarWaveIndex	{ 0.0f };
	float		mPulsarStep			{ 0.0f };

	float		mFormant			{ 0.0f };		// mWaveLength -> mWaveLength / 8.0f
	float		mCluster			{ 1.0f };		// 1.0f -> 5.0f
	float		mWaveFold			{ 1.0f };		// 1.0f -> 5.0f

	//==============================================================================
	//          refresh
	//
	// Update pulsar curve
	//
	//  CasualNoises    22/07/2025  First implementation
	//==============================================================================
	void refresh() noexcept
	{

		uint32_t i = 0;
		float fw = mFormant;
		float cw = fw / mCluster;
		for (; i < fw; )
		{
			for  (uint32_t j = 0; (j < cw) && (i < fw); ++j, ++i)
			{
				mPulsarCurvePtr[i] = (j * mWaveLength) / cw;
			}
		}
		for (; i < mWaveLength; ++i)
			mPulsarCurvePtr[i] = -1.0f;

	}

};

} // namespace CasualNoises

#endif

