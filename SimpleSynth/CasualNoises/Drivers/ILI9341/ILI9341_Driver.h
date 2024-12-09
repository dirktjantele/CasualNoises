/*
  ==============================================================================

    ILI9341_Driver.h
    Created: 20 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

// 23/1/2024 updated in order to be able to use the ILI9341 in parallel mode (8080 mode)
#ifdef USE_ILI9341_SERIAL_MODE

#pragma once

#include "stm32h7xx_hal.h"

#include "ILI9341_Registers.h"

namespace CasualNoises
{

namespace ILI9341_Driver
{

typedef struct
{
	SPI_HandleTypeDef*		SPI_HandlePtr;
	GPIO_TypeDef*			reset_port;
	uint16_t 				reset_pin;
	GPIO_TypeDef*			select_port;
	uint16_t 				select_pin;
	GPIO_TypeDef*			dc_port;
	uint16_t 				dc_pin;
} ILI9341_Config;

static void resetDevice(ILI9341_Config* configPtr);

static HAL_StatusTypeDef configDevice(ILI9341_Config* configPtr);

} // namespace ILI9341_Driver

} // namespace CasualNoises

#endif
