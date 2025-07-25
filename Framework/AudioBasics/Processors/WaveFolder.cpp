/*
  ==============================================================================

    WaveFolder.h
    Created: 28/02/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef USE_AUDIO_BUFFER

#include "WaveFolder.h"

#include <math.h>

namespace CasualNoises
{

//==============================================================================
//          simpleFold()
//
// Apply hard wave folding to an audio sample
//		0.0 > threshold >= 1.0
//
//  CasualNoises    28/02/2025  First implementation
//==============================================================================
float simpleFold(float x, float threshold)
{
    float sign = (x >= 0) ? 1.0f : -1.0f;
    x = fabs(x);
    while (x > threshold) {
        x = (2 * threshold) - x;
    }
    return sign * x;
}

//==============================================================================
//          applySimpleWaveFolding()
//
// Apply hard wave folding to an audio buffer
//		0.0 > threshold >= 1.0
//
//  CasualNoises    28/02/2025  First implementation
//==============================================================================
void applySimpleWaveFolding(AudioBuffer& audioBuffer, float threshold)
{
	uint32_t noOfSamples = audioBuffer.getNumSamples();
	const float* rpt1 = audioBuffer.getReadPointer(0);
	float* wpt1 = audioBuffer.getWritePointer(0);
	const float* rpt2 = nullptr;
	float* wpt2 = nullptr;
	if (audioBuffer.getNumChannels() > 1)
	{
		rpt2 = audioBuffer.getReadPointer(1);
		wpt2 = audioBuffer.getWritePointer(1);
	}
    for (size_t i = 0; i < noOfSamples; ++i)
    {
    	*wpt1++ = simpleFold(*rpt1++, threshold);
    	if (rpt2 != nullptr)
    		*wpt2++ = simpleFold(*rpt2++, threshold);
    }
}

//==============================================================================
//          fold()
//
// Apply hard wave folding to an audio sample
//		1.0 >= level >= ...
//
//  CasualNoises    01/03/2025  First implementation
//==============================================================================
float fold(float x, float level)
{
	x *= level;
	while ((x > 1.0f) || (x < -1.0f))
	{
		if (x > 1.0f)
			x -= 2 * (x - 1.0f);
		else
			x -= 2 * (x + 1.0f);
	}
	return x;
 }

//==============================================================================
//          applyWaveFolding()
//
// Apply hard wave folding to an audio buffer
//		1.0 >= level >= ...
//
//  CasualNoises    01/03/2025  First implementation
//==============================================================================
void applyWaveFolding(AudioBuffer& audioBuffer, float level1, float level2)
{
	uint32_t noOfSamples = audioBuffer.getNumSamples();
	const float* rpt1 = audioBuffer.getReadPointer(0);
	float* wpt1 = audioBuffer.getWritePointer(0);
	const float* rpt2 = nullptr;
	float* wpt2 = nullptr;
	if (audioBuffer.getNumChannels() > 1)
	{
		rpt2 = audioBuffer.getReadPointer(1);
		wpt2 = audioBuffer.getWritePointer(1);
	}
    for (size_t i = 0; i < noOfSamples; ++i)
    {
    	*wpt1++ = fold(*rpt1++, level1);
    	if (rpt2 != nullptr)
    		*wpt2++ = fold(*rpt2++, level2);
    }
}

void applyWaveFolding(AudioBuffer& audioBuffer, float level)
{
	applyWaveFolding(audioBuffer, level, level);
}

//==============================================================================
//          softFold()
//
// Apply soft wave folding to an audio sample
//		0.0 > threshold >= 1.0
//
//  CasualNoises    02/03/2025  First implementation
//==============================================================================
float softFold(float x, float threshold)
{
    return (x > threshold) ? threshold * std::tanh(x / threshold) :
           (x < -threshold) ? -threshold * std::tanh(-x / threshold) : x;
}

//==============================================================================
//          softFold()
//
// Apply hard wave folding to an audio buffer
//		0.0 > threshold >= 1.0
//
//  CasualNoises    02/03/2025  First implementation
//==============================================================================
void applySoftWaveFolding(AudioBuffer& audioBuffer, float threshold)
{
	uint32_t noOfSamples = audioBuffer.getNumSamples();
	const float* rpt1 = audioBuffer.getReadPointer(0);
	float* wpt1 = audioBuffer.getWritePointer(0);
	const float* rpt2 = nullptr;
	float* wpt2 = nullptr;
	if (audioBuffer.getNumChannels() > 1)
	{
		rpt2 = audioBuffer.getReadPointer(1);
		wpt2 = audioBuffer.getWritePointer(1);
	}
    for (size_t i = 0; i < noOfSamples; ++i)
    {
    	*wpt1++ = softFold(*rpt1++, threshold);
    	if (rpt2 != nullptr)
    		*wpt2++ = softFold(*rpt2++, threshold);
    }
}

} // namespace CasualNoises

#endif
