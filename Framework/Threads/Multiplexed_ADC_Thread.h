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

#include "../../DeviceBoardEngine/UI_Definitions.h"

namespace CasualNoises
{

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
