/*
  ==============================================================================

    ReportFault.h
    Created: 24 jul 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "cmsis_os.h"

#include "main.h"
#include "SystemConfig.h"
#include "FreeRTOS.h"

//#define CS4270_NUCLEO-H723ZG

#ifdef CS4270_NUCLEO-H723ZG

namespace CasualNoises
{

static void CN_ReportFault(uint32_t faultCode)
{
	// ToDo: implement this
	vTaskSuspendAll();
	for (;;)
	{

	}
}

#endif

#ifdef BreakOut_Hardware_0_1

namespace CasualNoises
{

static void CN_ReportFault(uint32_t faultCode)
{
	// ToDo: implement this
}

#endif

#ifdef M1vi_Hardware

static void CN_ReportFault(uint32_t faultCode)
{
	vTaskSuspendAll();
	HAL_GPIO_WritePin(GPIOE, STATUS_LED_1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, STATUS_LED_2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, STATUS_LED_3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, STATUS_LED_4_Pin, GPIO_PIN_SET);
	for (;;)
	{
		for (uint32_t t = 0; t < 100000; ++t)
		{
			HAL_GPIO_WritePin(GPIOE, STATUS_LED_1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, STATUS_LED_2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, STATUS_LED_3_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE, STATUS_LED_4_Pin, GPIO_PIN_SET);
		}
		for (uint32_t t = 0; t < 500000; ++t)
		{
			if (faultCode & 0x01)
				HAL_GPIO_WritePin(GPIOE, STATUS_LED_1_Pin, GPIO_PIN_RESET);
			if (faultCode & 0x02)
				HAL_GPIO_WritePin(GPIOE, STATUS_LED_2_Pin, GPIO_PIN_RESET);
			if (faultCode & 0x04)
				HAL_GPIO_WritePin(GPIOE, STATUS_LED_3_Pin, GPIO_PIN_RESET);
			if (faultCode & 0x08)
			 HAL_GPIO_WritePin(GPIOE, STATUS_LED_4_Pin, GPIO_PIN_RESET);
		}
	}
}

#endif

} // namespace CasualNoises
