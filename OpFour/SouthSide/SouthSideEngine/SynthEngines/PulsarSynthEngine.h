/*
  ==============================================================================

    PulsarSynthEngine.h
    Created: 24/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "AudioProcessors/Processors/AudioProcessor.h"
#include "Utilities/ReportFault.h"

namespace CasualNoises
{

class PulsarSynth;
class Wavetable_LFO;
class PulsarSynthEngine : public AudioProcessor
{
public:
	 PulsarSynthEngine() = default;
	~PulsarSynthEngine() = default;

	void 	prepareToPlay (float sampleRate,
						   uint32_t maximumExpectedSamplesPerBlock,
						   void* inSynthParams) override;
	void 	releaseResources() override;
	void 	processNerveNetData(uint32_t threadNo, uint32_t size, uint8_t* ptr) override;
	void 	processBlock (AudioBuffer &buffer, AudioBuffer& NN_buffer) override;

private:

	float 							mSampleRate 					{ 0.0f };
	uint32_t						mMaximumExpectedSamplesPerBlock { 0 };

	PulsarSynth*					mPulsarSynthPtr					{ nullptr };
	Wavetable_LFO*					mWavetable_LFOPtr				{ nullptr };

};

} // namespace CasualNoises
