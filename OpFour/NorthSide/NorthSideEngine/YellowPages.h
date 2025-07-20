/*
  ==============================================================================

    YellowPages.h

    Usefull for communication between threads

    Created: 19/04/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

namespace CasualNoises
{

typedef struct
{
	// Info about the NerveNet master thread
	TaskHandle_t 	gNerveNetMasterThreadTaskHandle;
	QueueHandle_t	gNerveNetMasterThreadQueueHandle;
	bool			gNetMasterThreadRunning = false;

	// Info about the UI thread
	TaskHandle_t 	gUI_ThreadTaskHandle;
	QueueHandle_t	gUI_ThreadQueueHandle;
	bool			gUI_ThreadRunning = false;

	// Info about the led driver thread
	TaskHandle_t 	gLED_ThreadTaskHandle;
	QueueHandle_t	gLED_ThreadQueueHandle;
	bool			gLED_ThreadRunning = false;

} tYellowPages;

extern tYellowPages	gYellowPages;

} // namespace CasualNoises
