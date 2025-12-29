/*
  ==============================================================================

    Box.cpp
    Created: 29/12/2025

    Paints a box around other components

    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "Box.h"

#include <Graphics/Contexts/Graphics.h>

namespace CasualNoises
{

//==============================================================================
//          Box()
//
//  CasualNoises    29/12/2025  First implementation
//==============================================================================
Box::Box(String name) :
	Component (name)
{
}

//==============================================================================
//          paint()
//
//	Paint the component
//
//  CasualNoises    29/12/2025  First implementation
//==============================================================================
void Box::paint(Graphics& g)
{
	SSD1309_Driver* screen = g.getScreen();
	Rectangle<int> bounds = getLocalBounds();
	screen->drawRect(bounds.getX(), bounds.getY(), bounds.getWidth() - 1, bounds.getHeight() - 1);
}

} // namespace CasualNoises
