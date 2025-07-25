/*
  ==============================================================================

    PotentiometerThread.h

    Handles potentiometers events

    Created: 25/10/2024
    Author:  Dirk Tjantele

  ==============================================================================
*/


#ifdef CASUALNOISES_POTENTIOMETER_THREAD

#pragma once

#include "main.h"
#include "task.h"
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
} sPotentiometerThreadData;

// Structure of an potentiometer queue event
typedef struct
{
	eEventSourceID	eventSourceID;
	uint32_t		potentiometerId;
	float			potentiometerValue;
} sPotentiometerEventStruct;

// Start thread function
BaseType_t startPotentiometerThread(void *argument, TaskHandle_t* xHandlePtr);

} // namespace CasualNoises

#endif
