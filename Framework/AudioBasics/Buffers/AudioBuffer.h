/*
  ==============================================================================

    AudioBuffer.h
    Created: 24 jul 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

//#include "cmsis_os.h"

#include "SystemConfig.h"
#include "AudioUtils/Players/AudioProcessorPlayer.h"

namespace CasualNoises
{

typedef struct
{
	float*		audioBuffer1;
	float*		audioBuffer2;
} sAudioBufferPtrs;

class AudioBuffer
{
public:
	  AudioBuffer();
	 ~AudioBuffer();

	  inline uint32_t		getNumChannels()	const noexcept		{ return NUM_CHANNELS; }
	  inline uint32_t		getNumSamples()		const noexcept  	{ return NUM_SAMPLES; }
	  inline uint32_t		getFullBufferSize() const noexcept		{ return FULL_AUDIO_BUFFER_SIZE; }

	  inline void			clearAudioBuffer() noexcept;

	  inline const float*	getReadPointer  (int channelNumber) const noexcept  { return mAudioBuffer[channelNumber]; }
	  inline float* 	    getWritePointer (int channelNumber) const noexcept  { return mAudioBuffer[channelNumber]; }

	  // Method to be used by the AudioProcessorPlayer
	  inline sAudioBufferPtrs* getAudioBufferPtrs()
	  {
		  static sAudioBufferPtrs pointers;
		  pointers.audioBuffer1 = mAudioBuffer[0];
		  pointers.audioBuffer2 = mAudioBuffer[1];
		  return &pointers;
	  }

	  // Methods to be used by the AudioProcessor

private:

	  // Buffers used to hold the audio data in floating point format for processing by the AudioProcessor
	  float*			mAudioBuffer[2]   	{ nullptr };		// 2 channels

};

} // namespace CasualNoises
