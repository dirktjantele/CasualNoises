/*
  ==============================================================================

    GraphicsContext.cpp
    Created: 19 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

// 23/1/2024 updated in order to be able to use the ILI9341 in parallel mode (8080 mode)
#ifdef USE_ILI9341_SERIAL_MODE

#include "GraphicsContext.h"

#include "SystemConfig.h"

#include "../../ILI9341/ili9341_gfx.h"

namespace CasualNoises
{

//==============================================================================
//          Graphics()
//
//  CasualNoises    19/08/2023  First implementation
//==============================================================================
Graphics::Graphics(ili9341_t* screen)
		: mScreen ( screen )
{
}

//==============================================================================
//          fillAll()
//
//  CasualNoises    19/08/2023  First implementation
//==============================================================================
void Graphics::fillAll() const
{
	ili9341_fill_rect(mScreen, (ili9341_color_t)mColor, 0, 0, DISPLY_WIDTH, DISPLAY_HEIGHT);
}

//==============================================================================
//          fillRect()
//
//  CasualNoises    19/08/2023  First implementation
//==============================================================================
void Graphics::fillRect(int x, int y, int width, int height) const
{
	ili9341_fill_rect(mScreen, (ili9341_color_t)mColor, x, y, width, height);
}

} // namespace CasualNoises

#endif

