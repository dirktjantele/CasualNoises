/*
  ==============================================================================

    AudioProcessorPlayer.h
    Created: 24 jul 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef USE_AUDIO_PROCESSOR_PLAYER

#pragma once

#include "Utilities/ReportFault.h"
#include "AudioProcessors/Processors/AudioProcessor.h"

#include "../../NerveNet/NerveNetMessage.h"

#include "semphr.h"

namespace CasualNoises
{

class AudioBuffer;

class AudioProcessorPlayer final
{
public:
	 ~AudioProcessorPlayer() = default;

	//==============================================================================
	//          getAudioProcessorPlayer
	//
	// This class is implemented using a singleton design pattern
	// getAudioProcessorPlayer() can only be called once to prevent creation of multiple
	// instances of the player class
	//
	//  CasualNoises    24/07/2023  First implementation
	//==============================================================================
	static AudioProcessorPlayer* getAudioProcessorPlayer(
			AudioProcessor* audioProcessorPtr/*,
			AudioBuffer* audioBufferPtr*/)
	{
		if ( ! mIsAllocated )
		{
			mIsAllocated = true;
			mAudioProcessorPtr = audioProcessorPtr;
			return &mAudioProcessorPlayer;
		}
		CN_ReportFault(eErrorCodes::AudioThreadError);
		return nullptr;
	};

	// Set a pointer to the I2S device handle to be used, nullptr = no I2S is used
	inline void sethi2sHandlePtr (I2S_HandleTypeDef* ptr) noexcept { m_hi2sHandlePtr = ptr; }

	// Set pointer to the synthesiser parameters
//	inline void setSynthesiserParamsPtr (void* ptr) noexcept { mSynthesiserParamsPtr = ptr; }

	// Run the player, this method never returns
	void runAudioProcessor(AudioBuffer* audioBufferPtr, void (**nerveNetCallBackPtr)(CasualNoises::sNerveNetData*));

private:

	// Access locker
	static AudioProcessorPlayer		mAudioProcessorPlayer;
	static bool						mIsAllocated;

	// I2S device handle for audio communication with the codec
	static I2S_HandleTypeDef*  		m_hi2sHandlePtr;

	// Pointer to the paarams to be used by the synthesiser
	static void*					mSynthesiserParamsPtr;

	// Pointer to the AudioProcessor to be used, this is platform specific (ex. see M1vi_AudioProcessor)
	static AudioProcessor* 			mAudioProcessorPtr;

	AudioProcessorPlayer() = default;

};

} // namespace CasualNoises

#endif
