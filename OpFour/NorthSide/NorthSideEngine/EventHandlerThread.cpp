/*
  ==============================================================================

    EventHandlerThread.cpp

    Handles events, like internal clock timer

    Created: 23/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "EventHandlerThread.h"

#include "SystemConfig.h"
#include "Utilities/ReportFault.h"

namespace CasualNoises
{

SemaphoreHandle_t	gEventHandlerSemaphoreHandle 	= nullptr;

//==============================================================================
//          eventHandlerTimerInterrupts()
//
// Timer interrupts are used to drive the synth clock when set to internal
// Timer should run at 100Hz
//
//  CasualNoises    23/07/2025  First implementation
//==============================================================================
void eventHandlerTimerInterrupts()
{

}

//==============================================================================
//          EventHandlerThread()
//
// Run event handler thread
//
//  CasualNoises    23/07/2025  First implementation
//==============================================================================
void EventHandlerThread(void* pvParameters)
{

	// Get parameters
	sEventHandlerThreadData* params = (sEventHandlerThreadData*)pvParameters;

	// Start refresh interrupt timer
	HAL_StatusTypeDef res = HAL_TIM_Base_Start_IT (params->TimerHandle);
	if (res != HAL_OK)
		CN_ReportFault(eErrorCodes::runtimeError);


	for (;;)
	{
		vTaskDelay(pdMS_TO_TICKS(10));
	}

}

//==============================================================================
//          StartEventHandlerThread()
//
// Start event handler thread
//
//  CasualNoises    15/02/2025  First implementation
//==============================================================================
BaseType_t StartEventHandlerThread(sEventHandlerThreadData* params)
{

	// Create the thread to run the event handler
	TaskHandle_t xHandlePtr;
	BaseType_t res = xTaskCreate(EventHandlerThread, "EventHandlerThread", DEFAULT_STACK_SIZE, params,
			EVENT_THREAD_PRIORITY,	&xHandlePtr);
//	gYellowPages.gUI_ThreadTaskHandle = xHandlePtr;
	return res;

}

} // namespace CasualNoises

