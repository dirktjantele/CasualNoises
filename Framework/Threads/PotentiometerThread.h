/*
  ==============================================================================

    PotentiometerThread.h
    Created: Nov 1, 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "main.h"

#include "CasualNoises.h"

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
	uint16_t	potentiometerId;
	float		potentiometerValue;
} sPotentiometerEventStruct;

// Start thread function
BaseType_t startPotentiometerThread(void *argument);

} // namespace CasualNoises
