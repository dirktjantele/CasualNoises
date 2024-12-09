/*
  ==============================================================================

    AudioProcessorPlayer.h
    Created: 24 jul 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef USE_AUDIO_PROCESSOR

#pragma once

#include "Utilities/ReportFault.h"
#include "AudioProcessors/Processors/AudioProcessor.h"

#include "semphr.h"

namespace CasualNoises
{

class AudioBuffer;
class AudioProcessorPlayer
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
		CN_ReportFault(1);
		return nullptr;
	};

	// Set a pointer to the I2S device handle to be used, nullptr = no I2S is used
	void sethi2sHandlePtr(I2S_HandleTypeDef* ptr) { m_hi2sHandlePtr = ptr; }

	// Run the player, this method never returns
	void runAudioProcessor(/*AudioProcessor* audioProcessor,*/ AudioBuffer* audioBufferPtr);

private:

	// Access locker
	static AudioProcessorPlayer		mAudioProcessorPlayer;
	static bool						mIsAllocated;

	// I2S device handle for audio communication with the codec
	static I2S_HandleTypeDef*  		m_hi2sHandlePtr;

	// Pointer to the AudioProcessor to be used, this is platform specific (ex. see M1vi_AudioProcessor)
	static AudioProcessor* 			mAudioProcessorPtr;

	AudioProcessorPlayer() = default;

};

} // namespace CasualNoises

#endif
