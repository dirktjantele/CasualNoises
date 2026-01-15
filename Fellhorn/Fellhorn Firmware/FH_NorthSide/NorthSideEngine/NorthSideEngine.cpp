/*
  ==============================================================================

    NorthSideEngine.cpp

    Handles all kind of events, like internal clock timer, NerveNet, ...

    Created: 10/01/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "NorthSideEngine.h"

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

#include "Drivers/NVM Drivers/W25Q64 Driver/W25Qxx_Driver.h"
#include "Drivers/TLV Driver/TLV_Driver.h"

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
	CasualNoises::W25Qxx_Driver* NVM_DriverPtr = new CasualNoises::W25Qxx_Driver(NVM_DataPtr);
	CasualNoises::TLV_Driver*	 TLV_DriverPtr = new CasualNoises::TLV_Driver(NVM_DriverPtr);

	// Report engine thread ready for duty
	gYellowPages.gEngineThreadRunning = true;



	// Main thread loop
	for (;;)
	{

		osDelay(pdMS_TO_TICKS(100));

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
