/*
  ==============================================================================

    AudioThread.h
    Created: 31 oct 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_AUDIO_THREAD

#pragma once

#include "main.h"

#include "CasualNoises.h"

namespace CasualNoises
{

typedef struct
{
	I2S_HandleTypeDef*  hi2sHandlePtr;
	void*				synthesizerParamsPtr;
} sAudioThreadInitData;

BaseType_t StartAudioThread(void *argument);

} // namespace CasualNoises

#endif
