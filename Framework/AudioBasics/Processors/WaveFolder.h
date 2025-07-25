/*
  ==============================================================================

    WaveFolder.h
    Created: 28/02/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef USE_AUDIO_BUFFER

#pragma once

#include "AudioBasics/Buffers/AudioBuffer.h"

namespace CasualNoises
{

float simpleFold(float x, float threshold);
float fold(float x, float level);
void applySimpleWaveFolding(AudioBuffer& audioBuffer, float threshold);
void applyWaveFolding(AudioBuffer& audioBuffer, float level1, float level2);
void applyWaveFolding(AudioBuffer& audioBuffer, float level);
void applySoftWaveFolding(AudioBuffer& audioBuffer, float threshold);

} // namespace CasualNoises

#endif
