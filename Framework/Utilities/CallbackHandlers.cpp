/*
  ==============================================================================

    CallbackHandlers.cpp

    Handles various callback events

    Created: Nov 8, 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include <vector>

#include "CallbackHandlers.h"

#include "Utilities/ReportFault.h"

//==============================================================================
//          HAL_SPI_ErrorCallback
//
// Handle SPI errors
//
//  CasualNoises    04/07/2025  First implementation
//==============================================================================
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	CN_ReportFault(eErrorCodes::runtimeError);
}

// HAL_SPI_TxCpltCallback and HAL_SPI_TxRxCpltCallback handlers
// ------------------------------------------------------------
#ifdef CASUALNOISE_TxRxCplt_CALLBACKS

//==============================================================================
//          HAL_SPI_TxCpltCallback
//
// SPI DMA complete call-back related stuff
//
//  CasualNoises    12/02/2025  First implementation
//==============================================================================

std::vector<CN_HAL_SPI_TxCpltCallback> CN_HAL_SPI_TxCpltCallbacks;

void add_HAL_SPI_TxCpltCallback(CN_HAL_SPI_TxCpltCallback callback)
{
	CN_HAL_SPI_TxCpltCallbacks.push_back(callback);
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	for (auto callback : CN_HAL_SPI_TxCpltCallbacks)
	{
		bool flag = callback(hspi);
		if (flag)
			break;
	}
}

//==============================================================================
//          HAL_SPI_TxRxCpltCallback
//
// SPI DMA complete call-back related stuff
//
//  CasualNoises    12/02/2025  First implementation
//==============================================================================

std::vector<CN_HAL_SPI_TxRxCpltCallback> CN_HAL_SPI_TxRxCpltCallbacks;

void add_HAL_SPI_TxRxCpltCallback(CN_HAL_SPI_TxRxCpltCallback callback)
{
	CN_HAL_SPI_TxRxCpltCallbacks.push_back(callback);
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	for (auto callback : CN_HAL_SPI_TxRxCpltCallbacks)
	{
		bool flag = callback(hspi);
		if (flag)
			break;
	}
}

#endif

// HAL_ADC_ConvCpltCallback handler
// --------------------------------
#ifdef CASUALNOISE_ADC_CALLBACKS

//==============================================================================
//          PotentiometerConvCpltCallback
//
// ADC conversion complete call-back related stuff
//
//  CasualNoises    09/11/2024  First implementation
//==============================================================================

std::vector<ADC_ConvCpltCallback> ADC_ConvCpltCallbacks;

void add_ADC_ConvCpltCallback(ADC_ConvCpltCallback callback)
{
	ADC_ConvCpltCallbacks.push_back(callback);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	for (auto callback : ADC_ConvCpltCallbacks)
	{
		bool flag = callback(hadc);
		if (flag)
			break;
	}
}

#endif

// HAL_GPIO_EXTI_Callback handler
// ------------------------------
#ifdef CASUALNOISE_EXTI_CALLBACKS

//==============================================================================
//          HAL_GPIO_EXTI_Callback
//
// Dispatch external interrupts
//
//  CasualNoises    08/02/2025  First implementation
//==============================================================================
std::vector<GPIO_EXTI_Callback> EXTI_ConvCpltCallbacks;

void add_EXTI_ConvCpltCallback(GPIO_EXTI_Callback callback)
{
	EXTI_ConvCpltCallbacks.push_back(callback);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	for (auto callback : EXTI_ConvCpltCallbacks)
	{
		bool flag = callback(GPIO_Pin);
		if (flag)
			break;
	}
}

#endif
