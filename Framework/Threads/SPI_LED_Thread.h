/*
  ==============================================================================

    SPI_LED_Thread.h

    This thread handles led's connected to 74HC595 shift registers
    Communication is done using SPI using (B)DMA
    Current implementation supports up to 4 shift registers for a total of 32 led's

    Created: 04/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_LED_THREAD_SPI

#pragma once

#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"

#include "SystemConfig.h"

namespace CasualNoises
{

// Arguments expected when starting the thread
typedef struct
{
	SPI_HandleTypeDef*		LED_SPI_Handle;				// SPI interface to be used
	TIM_HandleTypeDef*		TimerHandle;				// Timer used to refresh the led's
	GPIO_TypeDef*			SER_OUT_RCLK_GPIO__Port;	// Port and pin for the
	uint16_t				SER_OUT_RCLK_GPIO_Pin;		//		parallel load signal to the shift register
} sSPI_LED_ThreadData;

// Structure of an led event
typedef struct
{
	uint32_t			ledBitNum;
	uint32_t			ledIntensity;
} sLED_Event;

// Handle led handler timer interrupts
void Handle_LED_RefreshInterrupts ();

// Start thread function
BaseType_t start_LED_Thread(void *argument, TaskHandle_t* xHandlePtr);

} // namespace CasualNoises

#endif
