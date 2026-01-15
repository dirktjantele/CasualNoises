/*
  ==============================================================================

    Graphics.cpp
    Created: 27/12/2025

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_DISPLAY_DRIVER

#include "Graphics.h"
#include <Graphics/Geometry/Point.h>
#include <Graphics/Geometry/Rectangle.h>

#include "SystemConfig.h"

namespace CasualNoises
{

//==============================================================================
//          Graphics()
//
//  CasualNoises    19/08/2023  First implementation
//  CasualNoises    27/12/2025  Adapted for Fellhorn
//==============================================================================
Graphics::Graphics(SSD1309_Driver* screen)
		: mScreen ( screen )
{
}

//==============================================================================
//          fillAll()
//
//
//  CasualNoises    19/08/2023  First implementation
//  CasualNoises    27/12/2025  Adapted for Fellhorn
//==============================================================================
void Graphics::fillAll ( eBitOperations op )
{
	fillRect ( 0, 0, DISPLY_WIDTH, DISPLAY_HEIGHT, op );
}

//==============================================================================
//          fillRect()
//
//  CasualNoises    19/08/2023  First implementation
//  CasualNoises    27/12/2025  Adapted for Fellhorn
//==============================================================================
void Graphics::fillRect( int x, int y, int width, int height, eBitOperations op )
{
	mScreen->fillRect ( x, y, width, height, op );
}

//==============================================================================
//          fillRect()
//
//  CasualNoises    05/01/2026  First implementation
//==============================================================================
void Graphics::fillRect(Rectangle<int> rect, eBitOperations op)
{
	mScreen->fillRect(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight(), op);
}

} // namespace CasualNoises

#endif
