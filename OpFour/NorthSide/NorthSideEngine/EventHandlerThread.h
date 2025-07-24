/*
  ==============================================================================

    EventHandlerThread.h
    Created: 23/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "stm32h7xx_hal.h"

namespace CasualNoises
{

// Arguments expected when starting the thread
typedef struct
{
	TIM_HandleTypeDef*		TimerHandle;				// Timer used to handle the internal synth clock
} sEventHandlerThreadData;

// Handle event handler timer interrupts
void eventHandlerTimerInterrupts();

// Start thread function
BaseType_t StartEventHandlerThread(sEventHandlerThreadData* params);

} // namespace CasualNoises
