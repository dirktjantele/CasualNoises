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

	// Info about the engine thread master thread
	TaskHandle_t 	gEngineThreadTaskHandle;
	QueueHandle_t	gEngineThreadQueueHandle;
	bool			gEngineThreadRunning = false;

	// Info about the NerveNet master thread
	TaskHandle_t 	gNerveNetMasterThreadTaskHandle;
	QueueHandle_t	gNerveNetMasterThreadQueueHandle;
	bool			gNetMasterThreadRunning = false;

	// Info about the NerveNet slave thread
	TaskHandle_t 	gNerveNetSlaveThreadTaskHandle;
	QueueHandle_t	gNerveNetSlaveThreadQueueHandle;
	bool			gNetSlaveThreadRunning = false;

	// Info about the event handler thread
	TaskHandle_t 	gEventHandlerThreadTaskHandle;
	QueueHandle_t	gEventHandlerThreadQueueHandle;
	bool			gEventHandlerThreadRunning = false;

} tYellowPages;

extern tYellowPages	gYellowPages;

} // namespace CasualNoises
