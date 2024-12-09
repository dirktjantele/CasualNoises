/*
  ==============================================================================

    EncoderThread.h

    This thread can handle up to 5 encoders with switches connected to the mcu
    using a PCF8574 driver.

    Created: 7 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_ENCODER_THREAD

#pragma once

#include "main.h"

#include "CasualNoises.h"

namespace CasualNoises
{

typedef struct
{
	sPinMapEntry	encoderA;
	sPinMapEntry	encoderB;
	sPinMapEntry	encoderSwitch;
} sEncoderMapEntry;

// Arguments expected when starting the thread
typedef struct
{
	I2C_HandleTypeDef 		hi2c;							// I2C interface to the I/O chips
	SemaphoreHandle_t 		I2Cx_Semaphore;					// Guard on I2Cx access
	QueueHandle_t			clientQueueHandle;				// Handle to a queue to report encoder events to
															//  in most cases this is the UI thread event queue
	uint8_t					I2C_DeviceAddresses[2]; 		// I2C addresses of the two I/O chips
	sEncoderMapEntry* 		Encoder_PinMap;					// This array is owned by the caller and used
															//  by this thread, so it should be static and may not change
} EncoderThreadData;

// Encoder event types
enum class eEncoderEventType
{
	encoderRotationEvent,
	encoderSwitchEvent
};

// Encoder rotation event
typedef struct
{
	eEncoderEventType		eventType;
	uint16_t				encoderNo;
	int16_t					count;
} sRoationEventStruct;

// Encoder switch event
typedef struct
{
	eEncoderEventType		eventType;
	uint16_t				encoderNo;
} sSwitchEventStruct;

// Structure of an encoder queue event
typedef union
{
	sRoationEventStruct 	rotationEvent;
	sSwitchEventStruct		switchEvent;
} uEncoderEventStruct;

// Start thread function
BaseType_t startEncoderThread(void *argument);				// Pointer to a EncoderThreadData struct

} // namespace CasualNoises

#endif
