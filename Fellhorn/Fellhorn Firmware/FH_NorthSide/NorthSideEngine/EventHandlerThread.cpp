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

#include "YellowPages.h"

#include "SynthEngineParams.h"

#include "SynthEngineMessage.h"
#include "NerveNet/NerveNetMasterThread.h"

namespace CasualNoises
{

namespace EventHandlerThread
{

SemaphoreHandle_t	gEventHandlerSemaphoreHandle = nullptr;

int32_t			g_bpm_downCounter	= 0;
uint32_t		g_bpm_beatNo		= 0;

//==============================================================================
//          eventHandlerTimerInterrupts()
//
// Timer interrupts are used to drive the synth clock when set to internal
// Timer should run at 100Hz
// Called from main::HAL_TIM_PeriodElapsedCallback()
//
//  CasualNoises    24/07/2025  First implementation
//==============================================================================
void eventHandlerTimerInterrupts()
{

	if (g_bpm_downCounter-- == 0)
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		BaseType_t res = xSemaphoreGiveFromISR(gEventHandlerSemaphoreHandle, &xHigherPriorityTaskWoken);
		if (res != pdTRUE)
			CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);
	}

}

//==============================================================================
//          EventHandlerThread()
//
// Run event handler thread
//
//  CasualNoises    24/07/2025  First implementation
//  CasualNoises    09/01/2026  Adapted for Fellhorn rev 2
//==============================================================================
void EventHandlerThread(void* pvParameters)
{

	// Get parameters
	sEventHandlerThreadData* threadParamsPtr = (sEventHandlerThreadData*)pvParameters;
	tSynthEngineParams*		 engineParamsPtr = threadParamsPtr->SynthEngineParamsPtr;

	// Create synchronization semaphore and perform a take on it so it will block in the loop
	vSemaphoreCreateBinary(gEventHandlerSemaphoreHandle);
	if (gEventHandlerSemaphoreHandle == nullptr)
		CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);
	BaseType_t rtosRes = xSemaphoreTake(gEventHandlerSemaphoreHandle, 1);
	if (rtosRes != pdTRUE)
		CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);

	// Calculate down counter from bpm and timer frequency (100Hz)
	int32_t downCounterValue = (uint32_t)((1.0f / (engineParamsPtr->bmp / 60.0)) * 100);
	g_bpm_downCounter = downCounterValue;

	// Start refresh interrupt timer
	HAL_StatusTypeDef halRes = HAL_TIM_Base_Start_IT (threadParamsPtr->TimerHandle);
	if (halRes != HAL_OK)
		CN_ReportFault(eErrorCodes::runtimeError);

	// Thread is up and running
	gYellowPages.gEventHandlerThreadRunning = true;

	for (;;)
	{

		// Send trigger event to SouthSide each beat
		rtosRes = xSemaphoreTake(gEventHandlerSemaphoreHandle, 10);
		if (rtosRes == pdPASS)
		{

			// Reload down counter and update beat no
			g_bpm_downCounter = downCounterValue;
			g_bpm_beatNo += 1;

			// Send trigger event message to the SouthSide
			tTriggerMessage triggerMessage;
			triggerMessage.header.messageTag	= (uint32_t)eSynthEngineMessageType::triggerEvent;
			triggerMessage.header.messageLength	= sizeof(tTriggerMessage);
			triggerMessage.beatNo				= g_bpm_beatNo;
			if ( gNerveNetMasterThreadPtr[0] != nullptr )
			{
				bool success = gNerveNetMasterThreadPtr[0]->sendMessage ( &triggerMessage, sizeof ( tTriggerMessage ) );
				if ( ! success)
					CN_ReportFault ( eErrorCodes::NerveNetThread_Error );
			}

		}

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
	gYellowPages.gEventHandlerThreadTaskHandle = xHandlePtr;
	return res;

}

} // namespace EventHandlerThread

} // namespace CasualNoises

