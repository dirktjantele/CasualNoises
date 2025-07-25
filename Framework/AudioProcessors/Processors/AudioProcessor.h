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

		  virtual void prepareToPlay(float sampleRate, uint32_t maximumExpectedSamplesPerBlock, void* synthParams) 	= 0;
		  virtual void releaseResources() 																			= 0;
		  virtual void processNerveNetData(uint32_t threadNo, uint32_t size, uint8_t* ptr) 							= 0;
		  virtual void processBlock (AudioBuffer &buffer, AudioBuffer &NN_buffer) 									= 0;

};

} // namespace CasualNoises

