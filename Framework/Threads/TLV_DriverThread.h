/*
  ==============================================================================

    TLV_DriverThread.h
    Created: 11 jan 2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "FreeRTOS.h"
#include "queue.h"

#ifdef CASUALNOISES_TLV_DRIVER_THREAD_SUPPORT

namespace CasualNoises
{

class NVM_Driver;

// Arguments expected when starting the thread
typedef struct
{
	NVM_Driver*		NVM_DriverPtr;
	bool*			runningFlagPtr;
	QueueHandle_t*	queueHandlePtr;
} sTLV_DriverThreadParams;

// Driver operation codes
enum class eTLV_Operation
{
	nop = 0x30,
	deleteAllTLVs,
	deleteTLV,
	getTotalNoOfTLVs,
	findNextTLV,
	getTLV_LengthBytes,
	addTLV_Bytes,
	updateTLV_Bytes,
	readTLV_TagBytes,
};

// Current state of a TLV operation
enum class eTLV_OperationState
{
	idle = 0x40,
	processing,
	done,
	posted,
};

// Structure of an TLV driver event
typedef struct
{
	eTLV_Operation			opCode;
	eTLV_OperationState*	statePtr;
	uint32_t 				tag;
	uint32_t				length;
	uint32_t*				lengthPtr;
	uint8_t*				valuePtr;
	bool					deleteValue;
} sTLV_Event;

// Start thread function
BaseType_t startTLV_DriverThread ( void* pvParameters, TaskHandle_t* xHandlePtr );

// Post an event to the incoming event queue
BaseType_t post_TLV_Event ( QueueHandle_t queueHandle, sTLV_Event* inEventPtr, bool inWait = false );

// Some shorthand TLV functions
uint32_t getTotalNoOfTLVs ( QueueHandle_t queueHandle );
uint32_t findNextTLV ( QueueHandle_t queueHandle, uint32_t tag, uint32_t index );
void	 deleteAllTLVs ( QueueHandle_t queueHandle );
void	 deleteTLV ( QueueHandle_t queueHandle, uint32_t tag, bool deleteAll );
uint32_t getTLV_LengthBytes ( QueueHandle_t queueHandle, uint32_t index );
bool	 addTLV_Bytes ( QueueHandle_t queueHandle, uint32_t tag, uint32_t length, uint32_t* valuePtr );
bool	 updateTLV_Bytes ( QueueHandle_t queueHandle, uint32_t tag, uint32_t length, uint32_t* valuePtr );
uint32_t readTLV_TagBytes ( QueueHandle_t queueHandle, uint32_t tag, uint32_t length, uint32_t* valuePtr );


} // namespace CasualNoises

#endif
