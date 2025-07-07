/*
  ==============================================================================

    AudioThread.h
    Created: 31 oct 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_AUDIO_THREAD

#pragma once

#include "stm32h7xx_hal.h"
#include "FreeRTOS.h"
#include "queue.h"

#include "AudioProcessors/Processors/AudioProcessor.h"

namespace CasualNoises
{

typedef struct
{
	AudioProcessor*		audioProcessorPtr;
	I2S_HandleTypeDef*  hi2sHandlePtr;
	void*				synthesizerParamsPtr;
} sAudioThreadInitData;

BaseType_t StartAudioThread(void *argument);

} // namespace CasualNoises

#endif
