/*
  ==============================================================================

    AudioProcessor.h
    Created: 24 jul 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include  "cmsis_os.h"

namespace CasualNoises
{

class AudioBuffer;

class AudioProcessor
{
public:

		  virtual void prepareToPlay ( float sampleRate, uint32_t maximumExpectedSamplesPerBlock, void* synthParams ) noexcept 	= 0;
		  virtual void releaseResources () noexcept																				= 0;
		  virtual void processNerveNetData ( uint32_t threadNo, uint32_t size, uint8_t* ptr ) noexcept							= 0;
		  virtual void processBlock ( AudioBuffer* buffer, AudioBuffer* NN_buffer ) noexcept									= 0;

};

} // namespace CasualNoises

