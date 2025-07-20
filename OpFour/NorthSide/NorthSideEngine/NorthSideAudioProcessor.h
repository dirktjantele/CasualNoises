/*
  ==============================================================================

    NorthSideAudioProcessor.h
    Created: 10/03/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "main.h"

#include "CasualNoises.h"

namespace CasualNoises
{

typedef struct
{
	float 		frequency;
} sSynthesiserParams;

//class NorthSideAudioProcessor
class NorthSideAudioProcessor : public AudioProcessor
{
public:
	 NorthSideAudioProcessor() = default;
	~NorthSideAudioProcessor() = default;

		//==============================================================================
		//          getNorthSideAudioProcessor
		//
		// This class is implemented using a singleton design pattern
		// getNorthSideAudioProcessor() can only be called once to prevent creation of multiple
		// instances of the audio processor class
		//
		//  CasualNoises    26/07/2023  First implementation for M1vi
		//  CasualNoises    10/11/2024  First implementation adapted for OpFour
		//  CasualNoises    10/03/2025  Adapted for Fellhorn
		//==============================================================================
		// Prevent multiple users accessing this object, only the AudioThread should use it
		static AudioProcessor* getNorthSideAudioProcessor()
		{
			if ( ! mIsAllocated )
			{
				mIsAllocated = true;
				return &mNorthSideAudioProcessor;
			}
			CN_ReportFault(eErrorCodes::runtimeError);
			return nullptr;
		}

		void 	prepareToPlay (float sampleRate, uint32_t maximumExpectedSamplesPerBlock, void* synthParams) override;
		void 	releaseResources() override;
		void 	processNerveNetData(uint32_t threadNo, uint32_t size, uint8_t* ptr) override;
		void 	processBlock (AudioBuffer &buffer, AudioBuffer& NN_buffer) override;

private:

		// Access locker
		static NorthSideAudioProcessor	mNorthSideAudioProcessor;
		static bool						mIsAllocated;

		float 							mSampleRate 					{ 0.0f };
		uint32_t						mMaximumExpectedSamplesPerBlock { 0 };

		sSynthesiserParams*				mSynthesiserParamsPtr			{ nullptr };

		LFO*							mLFO_ptr						{ nullptr };

		Wavetable_LFO*					mWavetable_LFO_1_ptr			{ nullptr };
		Wavetable_LFO*					mWavetable_LFO_2_ptr			{ nullptr };

		SmoothedValue*					mSmoothedValuePtr				{ nullptr };

};

} // namespace CasualNoises
