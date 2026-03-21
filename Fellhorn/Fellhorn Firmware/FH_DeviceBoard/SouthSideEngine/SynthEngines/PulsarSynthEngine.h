/*
  ==============================================================================

    PulsarSynthEngine.h
    Created: 24/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "AbstractSynthEngine.h"
#include "Utilities/ReportFault.h"

namespace CasualNoises
{

class PulsarSynth;
class Wavetable_LFO;
class PulsarSynthEngine : public AbstractSynthEngine
{
public:
	 PulsarSynthEngine() = default;
	~PulsarSynthEngine() = default;

	void 	prepareToPlay (float sampleRate,
						   uint32_t maximumExpectedSamplesPerBlock,
						   void* inSynthParams) noexcept override;
	void 	releaseResources() noexcept override;
	void 	processNerveNetData(uint32_t threadNo, uint32_t size, uint8_t* ptr) noexcept override;
	void 	processBlock (AudioBuffer &buffer, AudioBuffer& NN_buffer) noexcept override;

	void 	applyControlVoltages ( sControlVoltages* voltages ) override;

private:

	float 				mSampleRate 					{ 0.0f };
	uint32_t			mMaximumExpectedSamplesPerBlock { 0 };

	PulsarSynth*		mPulsarSynthPtr					{ nullptr };

	float				mFrequencyOffset				{ 0.0f };			// From P1
	float				mTargetFrequency				{ 220.0f };			// From CV1 - 1V/OCT
	float				mCurrentFrequency				{ 220.0f };

	float				mGain							{ 1.0f };			// From CV4

	float				mFormantGain					{ 1.0f };			// From P2
	float				mTragetFormant					{ 0.0f };			// From Slider 1
	float				mFormant_CV						{ 0.0f };			// From CV5

	float				mClusterGain					{ 1.0f };			// From P3
	float				mTargetCluster					{ 0.0f };			// From Slider 2
	float				mCluster_CV						{ 0.0f };			// From CV6

	float				mWaveFoldGain					{ 1.0f };			// From P4
	float				mTargetWaveFold					{ 0.0f };			// From Slider 3
	float				mWaveFold_CV					{ 0.0f };			// From CV7

};

} // namespace CasualNoises
