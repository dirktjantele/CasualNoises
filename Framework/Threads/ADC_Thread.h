/*
  ==============================================================================

    ADC_Thread.h

    Handles ADC events

    Created: 18/04/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_ADC_THREAD

#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "SystemConfig.h"

namespace CasualNoises
{

// Abstract class used to handle adc data
class ADC_DataHandler final
{
public:
	 ADC_DataHandler() = default;
	~ADC_DataHandler() = default;

	virtual void handle_ADC_Data(uint32_t noOfEntries, uint16_t* adcDataPtr) = 0;

private:
};

// Arguments expected when starting the thread
typedef struct
{
	ADC_HandleTypeDef* 	hadc;					// Handle to the ADC to be used
	TIM_HandleTypeDef*	htim;					// Timer used to trigger ADC conversions
	ADC_DataHandler*	ADC_DataHandlerPtr;		// Pointer to an object that handles adc data
} sADC_ThreadData;

// Start thread function
BaseType_t startADC_Thread(void *argument, TaskHandle_t* xHandlePtr);

} // namespace CasualNoises

#endif
