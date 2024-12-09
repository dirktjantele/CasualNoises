/*
  ==============================================================================

    TriggerThread.h
    Created: 2 dec 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_TRIGGER_THREAD

#pragma once

#include "main.h"

#include "CasualNoises.h"

namespace CasualNoises
{
/*
typedef struct
{
	I2S_HandleTypeDef*  hi2sHandlePtr;
} sAudioThreadInitData;

BaseType_t StartAudioThread(void *argument);
*/

	void TriggerThread(void* pvParameters);

} // namespace CasualNoises

#endif
