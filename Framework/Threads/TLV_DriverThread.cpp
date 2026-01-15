/*
  ==============================================================================

    TLV_DriverThread.cpp
    Created: 11 jan 2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_NVM_DRIVER_SUPPORT

#include "TLV_DriverThread.h"

#include <Drivers/TLV Driver/TLV_Driver.h>
#include <Utilities/ReportFault.h>

#include "string.h"
#include "semphr.h"

namespace CasualNoises
{

#include "SystemConfig.h"

//==============================================================================
//          local data
//==============================================================================

QueueHandle_t gxUI_ThreadQueue = nullptr;
bool		  gRunningFlag	   = false;

// Size of the temp. data buffer
constexpr uint32_t cQueueLength  = 10;
constexpr uint32_t cBufferSize	 = 1024;

// Structure of a TLV message data buffer
typedef struct
{
	sTLV_Event eventData;
} sTLV_BufferItem;

// TLV message data buffer
sTLV_BufferItem gTLV_MessageBuffer [ cQueueLength ];

//==============================================================================
//          post_TLV_Event()
//
// Post an event onto the TLV driver thread's event queue
//	**** Use this function whenever possible ISO sending events directly to the queue
// - Make a copy of the TLV data whenever the size fits a temporary buffer
//		this is done in order to handle dynamically allocated data
// - Wait for completion of the request -> should be done in order for read
//		operations to work
//
//  CasualNoises    12/01/2026  First implementation
//==============================================================================
BaseType_t post_TLV_Event ( QueueHandle_t queueHandle, sTLV_Event* inEventPtr, bool inWait )
{
	if ( queueHandle == nullptr )
		CN_ReportFault ( eErrorCodes::UI_ThreadError );

	// Wait for driver thread to become on-line
	while ( ! gRunningFlag )
		vTaskDelay ( pdMS_TO_TICKS ( 10 ) );

	// Copy event data when required
	bool copyDataFlag = ( ( inEventPtr->lengthPtr != nullptr ) &&
						  *(inEventPtr->lengthPtr) <= cBufferSize ) &&
						  ! inWait;
	inEventPtr->deleteValue = copyDataFlag;
	if ( copyDataFlag )
	{
		uint8_t* ptr = (uint8_t*) pvPortMalloc ( *(inEventPtr->lengthPtr) );
		if ( ptr == nullptr )
			CN_ReportFault(eErrorCodes::UI_ThreadError);
		memcpy ( ptr, inEventPtr->valuePtr, *(inEventPtr->lengthPtr) );
		inEventPtr->valuePtr = ptr;
	}

	// Send request
	if ( inEventPtr->statePtr != nullptr )
		*(inEventPtr->statePtr) = eTLV_OperationState::posted;
	BaseType_t res = xQueueSend ( queueHandle, inEventPtr, 10 );

	// Await completion (also check inEventPtr->statePtr -> has to be nullptr on no wait)
	if ( inWait )
	{
		if ( inEventPtr->statePtr == nullptr )
			CN_ReportFault ( eErrorCodes::UI_ThreadError );
		while ( *inEventPtr->statePtr != eTLV_OperationState::done )
			vTaskDelay ( pdMS_TO_TICKS ( 10 ) );
	} else if ( inEventPtr->statePtr != nullptr )
	{
		CN_ReportFault ( eErrorCodes::UI_ThreadError );
	}

	return res;
}

//==============================================================================
//          getTotalNoOfTLVs()
//
// Get the no of TLV's in flash memory
//
//  CasualNoises    12/01/2026  First implementation
//==============================================================================
uint32_t getTotalNoOfTLVs ( QueueHandle_t queueHandle )
{
	sTLV_Event _TLV_Event;
	uint32_t	count  = 0;
	uint32_t	length = sizeof ( count );
	eTLV_OperationState state = eTLV_OperationState::idle;
	_TLV_Event.opCode		= eTLV_Operation::getTotalNoOfTLVs;
	_TLV_Event.tag			= 0;
	_TLV_Event.length		= length;
	_TLV_Event.lengthPtr	= &length;
	_TLV_Event.valuePtr		= (uint8_t*) &count;
	_TLV_Event.deleteValue	= false;
	_TLV_Event.statePtr = &state;
	BaseType_t res = post_TLV_Event ( queueHandle, &_TLV_Event, true );
	if ( res != pdPASS )
		CN_ReportFault ( eErrorCodes::UI_ThreadError );
	uint32_t noOfBlocks = *( (uint32_t*) _TLV_Event.valuePtr );
	return noOfBlocks;
}

//==============================================================================
//          findNextTLV()
//
// Find next TLV with a given tag starting at an index in flash memory
//
//  CasualNoises    12/01/2026  First implementation
//==============================================================================
uint32_t findNextTLV ( QueueHandle_t queueHandle, uint32_t tag, uint32_t inIndex )
{
	sTLV_Event _TLV_Event;
	uint32_t	index = inIndex;
	uint32_t	length = sizeof ( uint32_t );
	eTLV_OperationState state = eTLV_OperationState::idle;
	_TLV_Event.opCode		= eTLV_Operation::findNextTLV;
	_TLV_Event.tag			= tag;
	_TLV_Event.length		= length;
	_TLV_Event.lengthPtr	= &length;
	_TLV_Event.valuePtr		= (uint8_t*) &index;
	_TLV_Event.deleteValue	= false;
	_TLV_Event.statePtr 	= &state;
	BaseType_t res = post_TLV_Event ( queueHandle, &_TLV_Event, true );
	if ( res != pdPASS )
		CN_ReportFault ( eErrorCodes::UI_ThreadError );
	index = *( (uint32_t*) _TLV_Event.valuePtr );
	return index;
}

//==============================================================================
//          deleteAllTLVs()
//
// Delete all TLV's from flash memory
//
//  CasualNoises    13/01/2026  First implementation
//==============================================================================
void deleteAllTLVs ( QueueHandle_t queueHandle )
{
	sTLV_Event _TLV_Event;
	_TLV_Event.opCode		= eTLV_Operation::deleteAllTLVs;
	_TLV_Event.statePtr 	= nullptr;
	_TLV_Event.length		= 0;
	_TLV_Event.lengthPtr	= nullptr;
	_TLV_Event.valuePtr		= nullptr;
	_TLV_Event.deleteValue	= false;
	BaseType_t res = post_TLV_Event ( queueHandle, &_TLV_Event, false );
	if ( res != pdPASS )
		CN_ReportFault ( eErrorCodes::UI_ThreadError );
}

//==============================================================================
//          deleteTLV()
//
// Delete one/all TLV's with a given tag from flash memory
//
//  CasualNoises    13/01/2026  First implementation
//==============================================================================
void deleteTLV ( QueueHandle_t queueHandle, uint32_t tag, bool deleteAll )
{
	sTLV_Event _TLV_Event;
	_TLV_Event.opCode		= eTLV_Operation::deleteTLV;
	_TLV_Event.tag			= tag;
	_TLV_Event.statePtr 	= nullptr;
	_TLV_Event.length		= deleteAll;
	_TLV_Event.lengthPtr	= nullptr;
	_TLV_Event.valuePtr		= nullptr;
	_TLV_Event.deleteValue	= false;
	BaseType_t res = post_TLV_Event ( queueHandle, &_TLV_Event, false );
	if ( res != pdPASS )
		CN_ReportFault ( eErrorCodes::UI_ThreadError );
}

//==============================================================================
//          getTLV_LengthBytes()
//
// Get the length of a given TLV data in bytes
//
//  CasualNoises    13/01/2026  First implementation
//==============================================================================
uint32_t getTLV_LengthBytes ( QueueHandle_t queueHandle, uint32_t index )
{
	sTLV_Event _TLV_Event;
	uint32_t	length  = index;
	uint32_t	_length = sizeof ( uint32_t );
	eTLV_OperationState state = eTLV_OperationState::idle;
	_TLV_Event.opCode		= eTLV_Operation::getTLV_LengthBytes;
	_TLV_Event.length		= 0;
	_TLV_Event.lengthPtr	= &_length;
	_TLV_Event.valuePtr		= (uint8_t*) &length;
	_TLV_Event.deleteValue	= false;
	_TLV_Event.statePtr 	= &state;
	BaseType_t res = post_TLV_Event ( queueHandle, &_TLV_Event, true );
	if ( res != pdPASS )
		CN_ReportFault ( eErrorCodes::UI_ThreadError );
	length = *( (uint32_t*) _TLV_Event.valuePtr );
	return length;
}

//==============================================================================
//          addTLV_Bytes()
//
// Add a TLV with given tag, length in bytes & value
//
//  CasualNoises    13/01/2026  First implementation
//==============================================================================
bool addTLV_Bytes ( QueueHandle_t queueHandle, uint32_t tag, uint32_t length, uint32_t* valuePtr )
{

	// Can make a copy of the data?
	bool useCopy = false;
	if ( length <  cBufferSize )
	{
		useCopy = true;
		uint32_t* ptr = (uint32_t*) pvPortMalloc ( length );
		memcpy ( ptr, valuePtr, length );
		valuePtr = ptr;
	}

	// Post request
	sTLV_Event _TLV_Event;
	_TLV_Event.opCode		= eTLV_Operation::addTLV_Bytes;
	_TLV_Event.tag			= tag;
	_TLV_Event.length		= length;
	_TLV_Event.lengthPtr	= nullptr;
	_TLV_Event.valuePtr		= (uint8_t*) valuePtr;
	_TLV_Event.deleteValue	= useCopy;
	_TLV_Event.statePtr 	= nullptr;
	BaseType_t res = post_TLV_Event ( queueHandle, &_TLV_Event, false );
	if ( res != pdPASS )
		CN_ReportFault ( eErrorCodes::UI_ThreadError );
	return length > 0;

}

//==============================================================================
//          updateTLV_Bytes()
//
// Update TLV with given tag, length in bytes & value
//
//  CasualNoises    13/01/2026  First implementation
//==============================================================================
bool updateTLV_Bytes ( QueueHandle_t queueHandle, uint32_t tag, uint32_t length, uint32_t* valuePtr )
{
	deleteTLV ( queueHandle, tag, true );
	length = addTLV_Bytes ( queueHandle, tag, length, valuePtr );
	return length > 0;
}

//==============================================================================
//          readTLV_TagBytes()
//
// Read TLV with given tag from flash
//
//  CasualNoises    14/01/2026  First implementation
//==============================================================================
uint32_t readTLV_TagBytes ( QueueHandle_t queueHandle, uint32_t tag, uint32_t length, uint32_t* valuePtr )
{
	sTLV_Event _TLV_Event;
	eTLV_OperationState state = eTLV_OperationState::idle;
	_TLV_Event.opCode		= eTLV_Operation::readTLV_TagBytes;
	_TLV_Event.tag			= tag;
	_TLV_Event.length		= length;
	_TLV_Event.lengthPtr	= &length;
	_TLV_Event.valuePtr		= (uint8_t*) valuePtr;
	_TLV_Event.deleteValue	= false;
	_TLV_Event.statePtr 	= &state;
	BaseType_t res = post_TLV_Event ( queueHandle, &_TLV_Event, true );
	if ( res != pdPASS )
		CN_ReportFault ( eErrorCodes::UI_ThreadError );
	return length;
}

//==============================================================================
//          TLV_DriverThread()
//
// Handle all TLV related messages
//
//  CasualNoises    12/01/2026  First implementation
//==============================================================================
void TLV_DriverThread ( void* pvParameters )
{

	// Get initial parameters
	sTLV_DriverThreadParams* paramsPtr = (sTLV_DriverThreadParams*) pvParameters;
	NVM_Driver* NVM_DriverPtr = paramsPtr->NVM_DriverPtr;

	// Create a queue for incoming messages
	QueueHandle_t xUI_ThreadQueue = xQueueCreate ( cQueueLength, sizeof(sTLV_Event) );
	if ( xUI_ThreadQueue == nullptr )
		CN_ReportFault ( eErrorCodes::FreeRTOS_ErrorRes );
	*( paramsPtr->queueHandlePtr ) = xUI_ThreadQueue;
	gxUI_ThreadQueue			   = xUI_ThreadQueue;

	// Create a TLV driver
	CasualNoises::TLV_Driver* TLV_DriverPtr = new CasualNoises::TLV_Driver ( NVM_DriverPtr );

	// Clear message buffers
	for (uint32_t i = 0; i < cQueueLength; ++i )
	{
		gTLV_MessageBuffer[i].eventData.opCode			= eTLV_Operation::nop;
		gTLV_MessageBuffer[i].eventData.statePtr		= nullptr;
		gTLV_MessageBuffer[i].eventData.tag				= 0;
		*(gTLV_MessageBuffer[i].eventData.lengthPtr)	= 0;
		gTLV_MessageBuffer[i].eventData.valuePtr		= nullptr;
	}

	// Main thread loop
	*( paramsPtr->runningFlagPtr ) = true;
	gRunningFlag				   = true;
	for (;;)
	{

		// Get next event
		sTLV_Event event;
		BaseType_t res = xQueueReceive ( xUI_ThreadQueue, &event, 10000 );
		if (res == pdPASS)								// Process event if any received
		{

			// Dispatch operation code
			if ( event.statePtr != nullptr )
				*event.statePtr = eTLV_OperationState::processing;
			switch ( event.opCode )
			{
				case eTLV_Operation::deleteAllTLVs:
					TLV_DriverPtr->deleteAllTLVs( false );
					break;
				case eTLV_Operation::deleteTLV:
					{
						bool deleteAll = event.length;
						TLV_DriverPtr->deleteTLV ( event.tag, deleteAll, false );
					}
					break;
				case eTLV_Operation::getTotalNoOfTLVs:
					{
						uint32_t count = TLV_DriverPtr->getTotalNoOfTLVs ();
						uint32_t* ptr = (uint32_t*) event.valuePtr;
						*ptr = count;
					}
					break;
				case eTLV_Operation::findNextTLV:
					{
						uint32_t* ptr = (uint32_t*) event.valuePtr;
						uint32_t index = *ptr;
						index = TLV_DriverPtr->findNextTLV( event.tag, index );
						*ptr = index;
					}
					break;
				case eTLV_Operation::getTLV_LengthBytes:
					{
						uint32_t* ptr = (uint32_t*) event.valuePtr;
						uint32_t index = *ptr;
						uint32_t length = TLV_DriverPtr->getTLV_LengthBytes ( index );
						*ptr = length;
					}
					break;
				case eTLV_Operation::addTLV_Bytes:
					{
						bool res = TLV_DriverPtr->addTLV_Bytes ( event.tag, event.length, event.valuePtr, false );
						if ( ! res )
							*(event.lengthPtr) = 0;
					}
					break;
				case eTLV_Operation::readTLV_TagBytes:
					{
						uint32_t count = TLV_DriverPtr->readTLV_TagBytes ( event.tag, *(event.lengthPtr), event.valuePtr );
						*(event.lengthPtr) = count * sizeof ( uint32_t );
					}
					break;
				default:
					CN_ReportFault(eErrorCodes::UI_ThreadError);
			}
			if ( event.statePtr != nullptr )
				*event.statePtr = eTLV_OperationState::done;

			// Delete any temp. buffer
			if ( event.deleteValue )
				vPortFree ( event.valuePtr );

		} else {										// Time to flush the cache
			TLV_DriverPtr->flushCache ();
		}

	}

}

//==============================================================================
//          startTLV_DriverThread()
//
// Just loop around and toggle trigger pin
//
//  CasualNoises    11/01/2026  First implementation
//==============================================================================
BaseType_t startTLV_DriverThread ( void* pvParameters, TaskHandle_t* xHandlePtr )
{
	BaseType_t res = xTaskCreate(TLV_DriverThread, "TLV_DriverThread", DEFAULT_STACK_SIZE, pvParameters,
			UI_THREAD_PRIORITY, xHandlePtr);
	return res;
}

} // namespace CasualNoises

#endif
