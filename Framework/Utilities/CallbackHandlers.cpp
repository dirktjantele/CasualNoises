/*
  ==============================================================================

    CallbackHandlers.cpp

    Handles various callback events

    Created: Nov 8, 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISE_ADC_CALLBACKS

#include <vector>

#include "CallbackHandlers.h"

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
		callback(hadc);
	}
}

#endif
