/*
  ==============================================================================

    NorthSideAudioProcessor.h
    Created: 10/03/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

//#ifdef debugging

#pragma once

#include "main.h"

#include "AudioProcessors/Processors/AudioProcessor.h"
#include "Threads/ADC_Thread.h"

#include "Utilities/ReportFault.h"

namespace CasualNoises
{

//class NorthSideAudioProcessor
class AbstractEffectEngine;
class NorthSideAudioProcessor : public AudioProcessor, public ADC_DataHandler
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
		static NorthSideAudioProcessor* getNorthSideAudioProcessor()
		{
			if ( ! mIsAllocated )
			{
				mIsAllocated = true;
				return &mNorthSideAudioProcessor;
			}
			CN_ReportFault ( eErrorCodes::runtimeError );
			return nullptr;
		}

		void 	prepareToPlay (float sampleRate, uint32_t maximumExpectedSamplesPerBlock, void* synthParams) 	noexcept override;
		void 	releaseResources () 																			noexcept override;
		void 	processNerveNetData ( uint32_t threadNo, uint32_t size, uint8_t* ptr ) 							noexcept override;
		void 	processBlock ( AudioBuffer* buffer, AudioBuffer* inputBuffer ) 									noexcept override;

		void 	handle_ADC_Data ( uint32_t noOfEntries, uint16_t* adcDataPtr );

private:

		// Access locker
		static NorthSideAudioProcessor	mNorthSideAudioProcessor;
		static bool						mIsAllocated;

		float 							mSampleRate 					{ 0.0f };
		uint32_t						mMaximumExpectedSamplesPerBlock { 0 };

		AbstractEffectEngine*			mEffectEnginePtr				{ nullptr };

};

} // namespace CasualNoises

//#endif
