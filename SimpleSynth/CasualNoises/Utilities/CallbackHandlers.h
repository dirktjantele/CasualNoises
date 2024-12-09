/*
  ==============================================================================

    CallbackHandlers.h
    Created: Nov 8, 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISE_ADC_CALLBACKS

#pragma once

#include "main.h"

// ADC conversion complete callback handlers
typedef void (*ADC_ConvCpltCallback)(ADC_HandleTypeDef* hadc);
void add_ADC_ConvCpltCallback(ADC_ConvCpltCallback callback);

#endif
