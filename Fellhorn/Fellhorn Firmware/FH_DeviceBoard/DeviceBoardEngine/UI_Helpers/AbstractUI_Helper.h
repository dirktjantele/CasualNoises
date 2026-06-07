/*
  ==============================================================================

    AbstractUI_Helper.h
    Created: 06/06/2026

    Abstract class used as root for all UI helper objects

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "Drivers/OLED/SSD1309_Driver.h"

namespace DeviceBoard
{

using namespace CasualNoises;

typedef struct
{
	SSD1309_Driver* oledDriverPtr;
} sUI_HelperInfo;


} // namespace DeviceBoard

