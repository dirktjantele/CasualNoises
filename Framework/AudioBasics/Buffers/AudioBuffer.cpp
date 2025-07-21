/*
  ==============================================================================

    AudioBuffer.cpp
    Created: 24 jul 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef USE_AUDIO_BUFFER

#include "AudioBuffer.h"

#include "Utilities/ReportFault.h"
#include "maths.h"

namespace CasualNoises
{

//==============================================================================
//          createBuffer()
//
//  CasualNoises    28/02/2025  First implementation
//==============================================================================
inline __attribute__((optimize("Ofast"),always_inline))
void AudioBuffer::createBuffer()
{

	 // Buffers used to hold the float representation of the audio data for processing
	 for (uint8_t i = 0; i < mNumChannels; ++i)
	 {
		 mAudioBuffer[i] = (float*)pvPortMalloc(mNumSamples * sizeof(float));
		 if (mAudioBuffer[i] == nullptr) CN_ReportFault(eErrorCodes::audioBufferError);
	 }

	 // Clear audio data buffers
	 clearAudioBuffer();

}

//==============================================================================
//          AudioBuffer()     /       ~AudioBuffer()
//
// bufferSize: since double buffering is used, this should be large enough to hold
//				two half buffers
//			   each sample in the buffer is a stereo sample and requires two 32 bit words,
//			   so a bufferSize of 1024 will result in a 256 samples/channel float buffer for
//				processing by the AudioProcessor during AudioProcessor::processBlock
//
//  CasualNoises    26/07/2023  First implementation
//==============================================================================
AudioBuffer::AudioBuffer()
: mNumSamples ( NUM_SAMPLES ),
  mNumChannels ( NUM_CHANNELS )
{
	createBuffer();
}

AudioBuffer::AudioBuffer(uint32_t numSamples, uint32_t numChannels)
: mNumSamples ( numSamples ),
  mNumChannels ( numChannels )
{
	createBuffer();
}

AudioBuffer::~AudioBuffer()
{
	 for (uint8_t i = 0; i < mNumChannels; ++i)
	 {
		 if (mAudioBuffer[i] != nullptr)
		 {
			 vPortFree((void *)mAudioBuffer[i]);
			 mAudioBuffer[i] = nullptr;
		 }
	 }
}

//==============================================================================
//          clearAudioBuffer()
//
// Clear the audio output buffer (does not clear the raw audio buffers)
//
//  CasualNoises    26/07/2023  First implementation
//==============================================================================
void AudioBuffer::clearAudioBuffer()
 {
	int32_t noOfSamples = getNumSamples();

	for (uint32_t i = 0; i < getNumChannels(); ++i)
	{
		float* wptr = getWritePointer(i);
		for (int32_t j = noOfSamples - 1; j >= 0; --j)
		{
			*wptr++ = 0.0f;
		}
	}

 }

//==============================================================================
//          copyAudio()
//
// Copy audio from another buffer (does not affect the raw audio buffers)
//
//  CasualNoises    13/07/2025  First implementation
//==============================================================================
void AudioBuffer::copyAudio(AudioBuffer& inBuffer)
{
	assert(getNumChannels() == 2);

	int32_t noOfSamples = getNumSamples();

	const float* rptr1 = inBuffer.getReadPointer(0);
	const float* rptr2 = inBuffer.getReadPointer(1);
	float* wptr1 = getWritePointer(0);
	float* wptr2 = getWritePointer(1);

	for (int32_t j = noOfSamples - 1; j >= 0; --j)
	{
		*wptr1++ = *rptr1++;
		*wptr2++ = *rptr2++;
	}

}

//==============================================================================
//          importAudio()
//
// Import audio data from an array of floats into this AudioBuffer
// The array should have the right amount of samples (mNumSamples * mNumChannels)
// Samples in the array are pairs of right/left audio samples
//
//  CasualNoises    13/07/2025  First implementation
//==============================================================================
void AudioBuffer::importAudio(float* ptr)
{
	assert(getNumChannels() == 2);

	int32_t noOfSamples = getNumSamples();

	float* wptr1 = getWritePointer(0);
	float* wptr2 = getWritePointer(1);
	for (int32_t j = noOfSamples - 1; j >= 0; --j)
	{
		*wptr1++ = *ptr++;
		*wptr2++ = *ptr++;
	}

}

//==============================================================================
//          exportAudio()
//
// Copy audio from another buffer (does not affect the raw audio buffers)
// The array should have the right amount of samples (mNumSamples * mNumChannels)
// Samples in the array are pairs of right/left audio samples
//
//  CasualNoises    13/07/2025  First implementation
//==============================================================================
void AudioBuffer::exportAudio(float* ptr)
{
	assert(getNumChannels() == 2);

	int32_t noOfSamples = getNumSamples();

	const float* rptr1 = getReadPointer(0);
	const float* rptr2 = getReadPointer(1);
	for (int32_t j = noOfSamples - 1; j >= 0; --j)
	{
		*ptr++ = *rptr1++;
		*ptr++ = *rptr2++;
	}

}

//==============================================================================
//          normalizeAudioBuffer()
//
// Apply normalisation to the content of an audio buffer
//
//  CasualNoises    28/02/2025  First implementation
//==============================================================================
void AudioBuffer::normalizeAudioBuffer()
 {
	 int32_t noOfSamples = getNumSamples();
	 for (uint8_t i = 0; i < getNumChannels(); ++i)
	 {
		 float* ptr = mAudioBuffer[i];
		 float min =  99.9f;
		 float max = -99.9f;
		 for (int32_t j = 0; j < noOfSamples; ++j)
		 {
			 if (*ptr < min)
				 min = *ptr;
			 if (*ptr > max)
				 max = *ptr;
			 ++ptr;
		 }
		 ptr = mAudioBuffer[i];
		 for (int32_t j = 0; j < noOfSamples; ++j)
		 {
			 float sample = *ptr;
			 sample = cn_map(sample, min, max, -1.0f, +1.0f);
			 *ptr++ = sample;
		 }
	 }
 }

} // namespace CasualNoises

#endif
