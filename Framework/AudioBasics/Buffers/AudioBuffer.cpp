/*
  ==============================================================================

    AudioBuffer.cpp
    Created: 24 jul 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "AudioBuffer.h"

#include "Utilities/ReportFault.h"

namespace CasualNoises
{

//==============================================================================
//          AudioBuffer()     /       ~AudioBuffer
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
 {

	 // Buffers used to hold the float representation of the audio data for processing
	 for (uint8_t i = 0; i < getNumChannels(); ++i)
	 {
		 mAudioBuffer[i] = (float*)pvPortMalloc(getNumSamples() * sizeof(float));
		 if (mAudioBuffer[i] == nullptr) CN_ReportFault(1);
	 }

	 // Clear audio data buffers
	 clearAudioBuffer();

 }

 AudioBuffer::~AudioBuffer()
 {
	 vPortFree((void *)mAudioBuffer);
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
	 for (uint8_t i = 0; i < getNumChannels(); ++i)
	 {
		 float* ptr = mAudioBuffer[i];
		 for (int32_t j = noOfSamples - 1; j >= 0; --j)
		 {
			 *ptr++ = 0.0f;
		 }
	 }
 }

} // namespace CasualNoises

