/*
  ==============================================================================

    ADC_Thread.h

    Handles ADC events

    Created: 18/04/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_ADC_THREAD

#pragma once

#include "stm32h7xx_hal.h"
#include "FreeRTOS.h"
#include "queue.h"

#include "SystemConfig.h"

namespace CasualNoises
{

// Arguments expected when starting the thread
typedef struct
{
	ADC_HandleTypeDef 	*hadc;					// Handle to the ADC to be used
	TIM_HandleTypeDef	htim;					// Timer used to trigger ADC conversions
	QueueHandle_t		clientQueueHandle;		// Handle to a queue to report potentiometer events to
												//  in most cases this is the UI thread event queue
} sADC_ThreadData;



// Start thread function
BaseType_t startADC_Thread(void *argument, TaskHandle_t* xHandlePtr);

} // namespace CasualNoises

#endif
