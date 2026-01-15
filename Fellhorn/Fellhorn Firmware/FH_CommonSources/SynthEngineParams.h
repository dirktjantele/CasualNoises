/*
  ==============================================================================

    SynthEngineParams.h
    Created: 24/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "stm32h7xx_hal.h"

namespace CasualNoises
{

typedef struct
{
	float		frequency;

	float		bmp;			// Used for internal clocking

} tSynthEngineParams;

} // namespace CasualNoises
