/*
  ==============================================================================

    CallbackHandlers.h
    Created: Nov 8, 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "main.h"

// HAL_SPI_TxCpltCallback and HAL_SPI_TxRxCpltCallback handlers
// ------------------------------------------------------------
#ifdef CASUALNOISE_TxRxCplt_CALLBACKS

// SPI DMA Tx complete callback handlers
typedef bool (*CN_HAL_SPI_TxCpltCallback)(SPI_HandleTypeDef* hspi);
void add_HAL_SPI_TxCpltCallback(CN_HAL_SPI_TxCpltCallback callback);

// SPI DMA TxRx complete callback handlers
typedef bool (*CN_HAL_SPI_TxRxCpltCallback)(SPI_HandleTypeDef* hspi);
void add_HAL_SPI_TxRxCpltCallback(CN_HAL_SPI_TxRxCpltCallback callback);

#endif

// HAL_ADC_ConvCpltCallback handler
// --------------------------------
#ifdef CASUALNOISE_ADC_CALLBACKS

// ADC conversion complete callback handlers
typedef bool (*ADC_ConvCpltCallback)(ADC_HandleTypeDef* hadc);
void add_ADC_ConvCpltCallback(ADC_ConvCpltCallback callback);

#endif

// HAL_GPIO_EXTI_Callback handler
// ------------------------------
#ifdef CASUALNOISE_EXTI_CALLBACKS

typedef bool (*GPIO_EXTI_Callback)(uint16_t GPIO_Pin);
void add_EXTI_ConvCpltCallback(GPIO_EXTI_Callback callback);

#endif
