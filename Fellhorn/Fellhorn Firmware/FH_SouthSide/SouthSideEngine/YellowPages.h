/*
  ==============================================================================

    YellowPages.h

    Usefull for communication between threads

    Created: 13/04/2026
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

class TLV_Driver;

typedef struct
{

	// Info about the TLV driver thread
	QueueHandle_t	gTLV_DriverThreadQueueHandle;
	bool			gTLV_DriverThreadThreadRunning = false;

} tYellowPages;

extern tYellowPages	gYellowPages;

} // namespace CasualNoises
