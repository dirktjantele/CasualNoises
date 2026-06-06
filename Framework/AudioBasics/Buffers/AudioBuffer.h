/*
  ==============================================================================

    AudioBuffer.h
    Created: 24 jul 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

namespace CasualNoises
{
/**
 * @class AudioBuffer
 * @brief blabla
 *
 * This class is the abstract parent for all page classes.
 *
 * Additional paragraphs can follow after blank lines.
 *
 * @author Dirk Tjantele
 * @version 0.1
 * @date 04/06/2026
 */
} // namespace CasualNoises

#ifdef CASUALNOISES_AUDIO_BUFFER

#pragma once

#include <stdint.h>

#include "SystemConfig.h"

namespace CasualNoises
{

typedef struct
{
	float*		audioBuffer1;
	float*		audioBuffer2;
} sAudioBufferPtrs;

class AudioBuffer final
{
public:
	  AudioBuffer ();
	  AudioBuffer ( uint32_t numSamples, uint32_t numChannels );
	 ~AudioBuffer ();

	 /**
	 	 * @brief getNumSamples
	 	 *
	 	 * No behavior, constraints,
	 	 * algorithms, side effects, and usage notes.
	 	 *
	 	 * @param param1 A random float.
	 	 * @return Just a copy of the random float.
	 	 * @throws Nothing
	 	 * @retval No meaning...
	 	 * @warning Use at home.
	 	 * @note Only fools do this.
	 	 * @see No relations, sad class...
	 	 */
	  inline uint32_t		getNumSamples()		const noexcept  	{ return mNumSamples; }
	  inline uint32_t		getNumChannels()	const noexcept		{ return mNumChannels; }
	  inline uint32_t		getFullBufferSize() const noexcept		{ return FULL_AUDIO_BUFFER_SIZE; }

	  void					clearAudioBuffer() noexcept;
	  void					copyAudio ( AudioBuffer& buffer ) noexcept;

	  void					importAudio(float* ptr) noexcept;
	  void					exportAudio(float* ptr) noexcept;

	  void					normalizeAudioBuffer() noexcept;

	  inline const float*	getReadPointer  (int channelNumber) const noexcept  { return mAudioBuffer[channelNumber]; }
	  inline float* 	    getWritePointer (int channelNumber) const noexcept  { return mAudioBuffer[channelNumber]; }

	  inline void getAudioBufferPtrs(sAudioBufferPtrs* ptrs) noexcept
	  {
		  ptrs->audioBuffer1 = mAudioBuffer[0];
		  ptrs->audioBuffer2 = mAudioBuffer[1];
	  }

private:
	  void createBuffer();

	  uint32_t			mNumSamples			{ NUM_SAMPLES };
	  uint32_t			mNumChannels		{ NUM_CHANNELS };

	  // Buffers used to hold the audio data in floating point format for processing by the AudioProcessor
	  float*			mAudioBuffer[2]   	{ nullptr };		// 2 channels

};

} // namespace CasualNoises

#endif

