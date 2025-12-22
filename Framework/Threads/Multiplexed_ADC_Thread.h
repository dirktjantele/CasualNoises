/*
  ==============================================================================

    Multiplexed_ADC_Thread.h

    Handles ADC events using a multiplexer to handle multiple physical inputs
    	using a single MCU ADC input

    Created: 16/12/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_MULT_ADC_THREAD

#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"

#include "SystemConfig.h"

namespace CasualNoises
{

// Multiplexer signature
// The ADC multiplexer is controlled by 3 GPIO output pins
typedef struct
{
	GPIO_TypeDef*			SO_Port;
	uint16_t				SO_Pin;
	GPIO_TypeDef*			S1_Port;
	uint16_t				S1_Pin;
	GPIO_TypeDef*			S2_Port;
	uint16_t				S2_Pin;
	uint8_t					mask;
} sADC_MultiplexerSignature;

// Structure of a multiplexed ADC event
typedef struct
{
	eEventSourceID		eventSourceID;
	uint16_t	 		multiplexerNo;
	uint16_t			multiplexerChannelNo;
	uint32_t			value;
	int32_t				deviation;
} sMultiplexed_ADC_Event;

// Arguments expected when starting the thread
typedef struct
{
	sADC_MultiplexerSignature*	multiplexerSignatureArray;		// Contains noOfMultiplexers entries
	uint16_t					noOfMultiplexers;				// No of multiplexers
	ADC_HandleTypeDef* 			hadc;							// Handle to the ADC to be used
//	TIM_HandleTypeDef*			htim;							// Timer used to trigger ADC conversions
	QueueHandle_t				clientQueue;					// Queue to send ADC events to
} sADC_MultiplexerThreadData;

// Start thread function
BaseType_t startMultiplexed_ADC_Thread(void *argument, TaskHandle_t* xHandlePtr);

} // namespace CasualNoises

#endif
