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

#include "../NerveNet/NerveNetMasterThread.h"
#include "../NerveNet/NerveNetSlaveThread.h"

#include "ReportFault.h"

// Global NerveNet master an slave thread pointers
#ifdef CASUALNOISES_NERVENET_THREAD
extern CasualNoises::NerveNetMasterThread* gNerveNetMasterThreadPtr[MAX_NO_OF_NERVENET_MASTER_THREADS];
extern CasualNoises::NerveNetSlaveThread* gNerveNetSlaveThreadPtr[MAX_NO_OF_NERVENET_SLAVE_THREADS];
#endif

// CN_TimerPeriodElapsedCallback handlers
// --------------------------------------
#ifdef CASUALNOISES_TIMER_CALLBACKS

//==============================================================================
//          CN_TIM_PeriodElapsedCallback
//
// Handle timer call backs
//		this is called from within main.cpp by HAL_TIM_PeriodElapsedCallback()
//
//  CasualNoises    17/07/2025  First implementation
//==============================================================================

std::vector<CN_TimerPeriodElapsedCallback> CN_TIM_PeriodElapsedCallbacks;

void add_TimerPeriodElapsedCallback(CN_TimerPeriodElapsedCallback callback)
{
	CN_TIM_PeriodElapsedCallbacks.push_back(callback);
}

void CN_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	for (auto callback : CN_TIM_PeriodElapsedCallbacks)
	{
		bool flag = callback(htim);
		if (flag)
			return;
	}
	return;
}

#endif

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
#ifdef CASUALNOISES_TxCplt_CALLBACKS

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
			return;
	}
	return;
}

#endif

#ifdef CASUALNOISES_TxRxCplt_CALLBACKS

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
	bool flag = false;

	// Handle NerveNet threads first
#ifdef CASUALNOISES_NERVENET_THREAD
	for (uint32_t i = 0; i < MAX_NO_OF_NERVENET_MASTER_THREADS; ++i)
	{
		if (gNerveNetMasterThreadPtr[i] != nullptr)
			flag = gNerveNetMasterThreadPtr[0]->SPI_TxRxCpltCallback(hspi);
		if (flag)
			return;
	}
	/*	void* ptr = &gNerveNetSlaveThreadPtr;
	for (uint32_t i = 0; i < MAX_NO_OF_NERVENET_SLAVE_THREADS; ++i)
	{
		if (gNerveNetSlaveThreadPtr[i] != nullptr)
			flag = gNerveNetSlaveThreadPtr[0]->SPI_TxRxCpltCallback(hspi);
		if (flag)
			return;
	}
*/
#endif

	for (auto callback : CN_HAL_SPI_TxRxCpltCallbacks)
	{
		flag = callback(hspi);
		if (flag)
			return;
	}
	return;
}

#endif

// HAL_ADC_ConvCpltCallback handler
// --------------------------------
#ifdef CASUALNOISES_ADC_CALLBACKS

//==============================================================================
//          HAL_ADC_ConvCpltCallback
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
			return;
	}
	return;
}

#endif

// HAL_GPIO_EXTI_Callback handler
// ------------------------------
#ifdef CASUALNOISES_EXTI_CALLBACKS

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

void HAL_GPIO_EXTI_Callback ( uint16_t GPIO_Pin )
{
	bool flag = false;

	// Handle NerveNet threads first
#ifdef CASUALNOISES_NERVENET_THREAD
	for ( uint32_t i = 0; i < MAX_NO_OF_NERVENET_MASTER_THREADS; ++i )
	{
		if ( gNerveNetMasterThreadPtr[i] != nullptr )
			flag = gNerveNetMasterThreadPtr[0]->GPIO_EXTI_Callback ( GPIO_Pin );
		if ( flag )
			return;
	}
	for ( uint32_t i = 0; i < MAX_NO_OF_NERVENET_SLAVE_THREADS; ++i )
	{
		if ( gNerveNetSlaveThreadPtr[i] != nullptr )
			flag = gNerveNetSlaveThreadPtr[0]->GPIO_EXTI_Callback ( GPIO_Pin );
		if ( flag )
			return;
	}
#endif

	// Scan registered callback's
	if ( ! flag )
	{
		for ( auto callback : EXTI_ConvCpltCallbacks )
		{
			flag = callback ( GPIO_Pin );
			if ( flag )
				return;
		}
	}
	return;
}

#endif
