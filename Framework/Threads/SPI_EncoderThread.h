/*
  ==============================================================================

    SPI_EncoderThread.h

    This thread handles encoders and switches connected to 74HC165 shift registers
    Communication is done using SPI with DMA mode to minimise cpu load

    Created: 24/02/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_ENCODER_THREAD_SPI

#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"

#include "SystemConfig.h"

namespace CasualNoises
{

// Encoder signature
// A switch is a simplified encoder, device no's/pin no's for encoder switches set to 0xff
typedef struct
{
	uint8_t		encoderNo;
	uint8_t		switchDevNo;
	uint8_t		switchPinNo;
	uint8_t		enc_A_DevNo;
	uint8_t		enc_A_PinNo;
	uint8_t		enc_B_DevNo;
	uint8_t		enc_B_PinNo;
} sEncoderSignature;

// Encoder event types
enum class eEncoderEventType
{
	encoderSwitch,
	encoderCount
};

// Structure of an encoder event
typedef struct
{
	eEventSourceID		eventSourceID;
	eEncoderEventType 	eventType;
	uint16_t			encoderNo;
	int16_t				encoderCount;
} sEncoderEvent;

// Arguments expected when starting the thread
typedef struct
{
	sEncoderSignature*		encoderSignatureArray;		// Contains noOfEncoders entries
	uint16_t				noOfEncoders;				// No of encoders/switches
	SPI_HandleTypeDef*		encoder_SPI_Handle;			// SPI interfav-ce to be used
	GPIO_TypeDef*			serInPL_Port;				// Port and pin for the
	uint16_t				serInPL_Pin;				//		parallel load signal to the shift register
	uint16_t				noOfDevices;				// No of available shift registers
	QueueHandle_t			clientQueue;				// Queue to send encoder events to
} sSPI_EncoderThreadData;

// Start thread function
BaseType_t startEncoderThread(void *argument, TaskHandle_t* xHandlePtr);

} // namespace CasualNoises

#endif
