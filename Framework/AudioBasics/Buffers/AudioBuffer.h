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

	  uint32_t		getNumChannels()	const noexcept		{ return NUM_CHANNELS; }
	  uint32_t		getNumSamples()		const noexcept  	{ return AUDIO_BUFFER_SIZE / (NUM_CHANNELS * 2); }
	  uint32_t		getFullBufferSize() const noexcept		{ return AUDIO_BUFFER_SIZE; }

	  void			clearAudioBuffer();

	  const float*	getReadPointer  (int channelNumber) const noexcept;
	  float* 	    getWritePointer (int channelNumber) const noexcept;

	  // Method to be used by the AudioProcessorPlayer
	  sAudioBufferPtrs* getAudioBufferPtrs()
	  {
		  static sAudioBufferPtrs pointers;
		  pointers.audioBuffer1 = mAudioBuffer[0];
		  pointers.audioBuffer2 = mAudioBuffer[1];
		  return &pointers;
	  }

	  // Methods to be used by the AudioProcessor

private:
	  uint16_t			mFullBufferSize 	{ AUDIO_BUFFER_SIZE };
	  const uint16_t	mNoOfChannels 		{ NUM_CHANNELS };

	  // Buffers used to hold the audio data in floating point format for processing by the AudioProcessor
	  float*			mAudioBuffer[2]   	{ nullptr };		// 2 channels

};

} // namespace CasualNoises

