/*
  ==============================================================================

    SouthSideAudioProcessor.h
    Created: 13/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "AudioProcessors/Processors/AudioProcessor.h"
#include "Utilities/ReportFault.h"

namespace CasualNoises
{

typedef struct
{
	float 		frequency;
} sSynthesiserParams;

class PulsarSynthEngine;
class SouthSideAudioProcessor : public AudioProcessor
{
public:
	 SouthSideAudioProcessor() = default;
	~SouthSideAudioProcessor() = default;

	//==============================================================================
	//          getSouthSideAudioProcessor
	//
	// This class is implemented using a singleton design pattern
	// getSouthSideAudioProcessor() can only be called once to prevent creation of multiple
	// instances of the audio processor class
	//
	//  CasualNoises    13/07/2025  First implementation for Fellhorn
	//==============================================================================
	// Prevent multiple users accessing this object, only the AudioThread should use it
	static AudioProcessor* getSouthSideAudioProcessor()
	{
		if ( ! mIsAllocated )
		{
			mIsAllocated = true;
			return &mSouthSideAudioProcessor;
		}
		CN_ReportFault(eErrorCodes::runtimeError);
		return nullptr;
	}

	void 	prepareToPlay (float sampleRate,
						   uint32_t maximumExpectedSamplesPerBlock,
						   void* inSynthParams) override;
	void 	releaseResources() override;
	void 	processNerveNetData(uint32_t threadNo, uint32_t size, uint8_t* ptr) override;
	void 	processBlock (AudioBuffer &buffer, AudioBuffer& NN_buffer) override;


private:

	// Access locker
	static SouthSideAudioProcessor	mSouthSideAudioProcessor;
	static bool						mIsAllocated;

	PulsarSynthEngine*				mPulsarSynthEnginePtr			{ nullptr };

};

} // namespace CasualNoises
