/*
  ==============================================================================

    NorthSideEngine.cpp

    Handles all kind of events, like internal clock timer, NerveNet, ...

    Created: 10/01/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "NorthSideEngine.h"

#include "EventHandlerThread.h"
#include "SynthEngineParams.h"
#include "SystemConfig.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <Utilities/ReportFault.h>
#include <NerveNet/NerveNetMasterThread.h>
#include <Threads/ADC_Thread.h>

#include "YellowPages.h"

#include "SynthEngineMessage.h"

#include <Drivers/NVM Drivers/W25Q64 Driver/W25Qxx_Driver.h>
#include <Drivers/TLV Driver/TLV_Driver.h>

namespace CasualNoises
{

//==============================================================================
//          handleNerveNetCallBacks()
//
// Handle incoming data from NerveNet
//
//  CasualNoises    10/01/2026  First implementation
//==============================================================================
void handleNerveNetCallBacks ( CasualNoises::sNerveNetData* ptr )
{
}

//==============================================================================
//          NorthSideEngineThread()
//
// Handle all events
//
//  CasualNoises    10/01/2026  First implementation
//==============================================================================
void NorthSideEngineThread ( void* pvParameters )
{

	// Get parameters
	sNorthSideEngineParams* params = ( sNorthSideEngineParams* ) pvParameters;
	void ( *funcPtr )( CasualNoises::sNerveNetData* ptr ) = &handleNerveNetCallBacks;
	params->nerveNetCallBackPtr = &funcPtr;

	// Create a TLV driver
	CasualNoises::sNVM_DriverInitData* NVM_DataPtr = &params->nvmDriverInitData;
	CasualNoises::W25Qxx_Driver* NVM_DriverPtr = new CasualNoises::W25Qxx_Driver ( NVM_DataPtr );
	CasualNoises::TLV_Driver*	 TLV_DriverPtr = new CasualNoises::TLV_Driver ( NVM_DriverPtr );

	UNUSED ( TLV_DriverPtr );

	// Create a queue to receive events other threads
	const uint32_t cQueueLength = 10;
	QueueHandle_t xUI_ThreadQueue = xQueueCreate ( cQueueLength, sizeof ( sIncomingEngineEvent ) );
	if (xUI_ThreadQueue == nullptr)
		CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);
	gYellowPages.gEngineThreadQueueHandle = xUI_ThreadQueue;

	// Create an ADC handler thread - CV inputs are handled by both north- and south side
//	TaskHandle_t xADC_ThreadHandle;
//	BaseType_t res = startADC_Thread ( (void *)&params->ADC_ThreadData, &xADC_ThreadHandle );
//	if ( res != pdPASS )
//		CN_ReportFault(eErrorCodes::UI_ThreadError);



	// Report engine thread ready for duty
	gYellowPages.gEngineThreadRunning = true;



	// Main thread loop
	for (;;)
	{

		// Handle incoming events
		sIncomingEngineEvent event;
		BaseType_t res = xQueueReceive ( xUI_ThreadQueue, (void *)&event, 1000 );
		if ( res == pdPASS )
		{

		}

	}

}

//==============================================================================
//          StartNorthSideEngineThread()
//
// Start north side engine thread
//
//  CasualNoises    10/01/2026  First implementation
//==============================================================================
BaseType_t StartNorthSideEngineThread ( sNorthSideEngineParams* argument )
{
	TaskHandle_t xHandlePtr;
	BaseType_t res = xTaskCreate ( NorthSideEngineThread,	"NorthSideEngineThread", DEFAULT_STACK_SIZE * 8,
			argument, ENGINE_THREAD_PRIORITY, &xHandlePtr );
	gYellowPages.gEngineThreadTaskHandle = xHandlePtr;
	return res;
}

} // namespace CasualNoises
