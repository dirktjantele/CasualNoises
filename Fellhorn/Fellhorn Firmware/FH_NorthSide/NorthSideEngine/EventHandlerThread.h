/*
  ==============================================================================

    EventHandlerThread.h
    Created: 23/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "stm32h7xx_hal.h"

#include "SynthEngineParams.h"

namespace CasualNoises
{

namespace EventHandlerThread
{

// Arguments expected when starting the thread
typedef struct
{
	TIM_HandleTypeDef*		TimerHandle;				// Timer used to handle the internal synth clock
	tSynthEngineParams*		SynthEngineParamsPtr;
} sEventHandlerThreadData;

// Handle event handler timer interrupts
void eventHandlerTimerInterrupts();

// Start thread function
BaseType_t StartEventHandlerThread(sEventHandlerThreadData* params);

} // namespace EventHandlerThread

} // namespace CasualNoises
