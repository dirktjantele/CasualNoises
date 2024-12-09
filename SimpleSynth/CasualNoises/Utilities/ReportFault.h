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

// ------------------------------ SIMPLE_SYNTH_0_1 ------------------------------
#ifdef SIMPLE_SYNTH_0_1

namespace CasualNoises
{

#include "../../CasualNoises/Drivers/IO_Drivers/PCF8574_Driver.h"

static void CN_ReportFault(uint32_t faultCode)
{
	uint8_t buf = ~faultCode << 4;
	uint8_t blank = 0xff;
	vTaskSuspendAll();
	for(;;)
	{
		/*HAL_StatusTypeDef res = */
		HAL_I2C_Master_Transmit(&hi2c1, CasualNoises::cPCF8574_Addres_1, &buf  , 1, 1000);
		HAL_Delay(500);
		HAL_I2C_Master_Transmit(&hi2c1, CasualNoises::cPCF8574_Addres_2, &blank, 1, 1000);
		HAL_Delay(200);
	}
}

} // namespace CasualNoises

#endif

// ------------------------------ CS4270_NUCLEO-H723ZG ------------------------------
#ifdef CS4270_NUCLEO-H723ZG

namespace CasualNoises
{

static void CN_ReportFault(uint32_t faultCode)
{
	// ToDo: implement this
	vTaskSuspendAll();
	HAL_GPIO_WritePin(GPIOB, LED_RED_Pin, GPIO_PIN_SET);
	for (;;)
	{
		HAL_Delay(500);
		HAL_GPIO_TogglePin(GPIOB, LED_RED_Pin);
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

//} // namespace CasualNoises
