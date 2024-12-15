/*
  ==============================================================================

    OpFourAudioProcessor.h
    Created: 09 nov 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "main.h"

#include "CasualNoises.h"

namespace CasualNoises
{

struct sSynthesiserParams
{
	float 		frequency;
	float		morph;
};

//class OpFourAudioProcessor;
class OpFourAudioProcessor : public AudioProcessor
{
public:
	 OpFourAudioProcessor() = default;
	~OpFourAudioProcessor() = default;

		//==============================================================================
		//          getOpFourAudioProcessor
		//
		// This class is implemented using a singleton design pattern
		// getOpFourAudioProcessor() can only be called once to prevent creation of multiple
		// instances of the audio processor class
		//
		//  CasualNoises    26/07/2023  First implementation for M1vi
		//  CasualNoises    10/11/2024  First implementation adapted for OpFour
		//==============================================================================
		// Prevent multiple users accessing this object, only the AudioThread should use it
		static void* getOpFourAudioProcessor()
		{
			if ( ! mIsAllocated )
			{
				mIsAllocated = true;
				return (void*)&mOpFourAudioProcessor;
			}
			CN_ReportFault(1);
			return nullptr;
		}

		void 	prepareToPlay (float sampleRate, uint32_t maximumExpectedSamplesPerBlock, void* synthParams) override;
		void 	releaseResources() override;
		void 	processBlock (AudioBuffer &buffer/*, MidiBuffer &midiMessages*/) override;

private:
		// Access locker
		static OpFourAudioProcessor		mOpFourAudioProcessor;
		static bool						mIsAllocated;

		float 							mSampleRate 					{ 0.0f };
		uint32_t						mMaximumExpectedSamplesPerBlock { 0 };

		sSynthesiserParams*				mSynthesiserParams				{ nullptr };

		Wavetable_LFO*					mWavetable_LFO_1_ptr			{ nullptr };
		Wavetable_LFO*					mWavetable_LFO_2_ptr			{ nullptr };

		SmoothedValue*					mSmoothedValuePtr				{ nullptr };

};

} // namespace CasualNoises
