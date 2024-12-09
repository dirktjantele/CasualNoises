/*
  ==============================================================================

    AudioProcessor.h
    Created: 24 jul 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include  "cmsis_os.h"

//#include "AudioBasics/Buffers/AudioBuffer.h"

namespace CasualNoises
{

class AudioBuffer;
class AudioProcessor
{
public:

		  virtual void prepareToPlay(float sampleRate, uint32_t maximumExpectedSamplesPerBlock) = 0;
		  virtual void releaseResources() = 0;
		  virtual void processBlock (AudioBuffer &buffer/*, MidiBuffer &midiMessages*/) = 0;

};

} // namespace CasualNoises

