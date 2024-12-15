/*
  ==============================================================================

    UI_Thread.h
    Created: 25 oct 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "main.h"

#include "CasualNoises.h"

#include "OpFourAudioProcessor.h"

namespace CasualNoises
{

// Arguments expected when starting the thread
typedef struct
{
	I2C_HandleTypeDef 		hi2c;					// I2C interface to the I/O chips
	SemaphoreHandle_t 		I2Cx_Semaphore;			// Guard on I2Cx access
	ADC_HandleTypeDef 		*hadc;					// Handle to the ADC being used
	TIM_HandleTypeDef		htim;					// Timer used to trigger ADC conversions
	sSynthesiserParams*		synthesiserParams;		// Pointer to the synthesiser parameters
} UI_ThreadData;

// Start thread function
BaseType_t Start_UI_Thread(UI_ThreadData *argument);

} // namespace CasualNoises
