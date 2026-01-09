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
Box::Box ( String name ) :
	Component ( name )
{
}

//==============================================================================
//          paint()
//
//	Paint the component
//
//  CasualNoises    29/12/2025  First implementation
//==============================================================================
void Box::paint ( Graphics& g )
{
	SSD1309_Driver* screen = g.getScreen ();
	Rectangle<int> bounds = getLocalBounds ();
	screen->drawRect(bounds.getX(), bounds.getY(), bounds.getWidth() - 1, bounds.getHeight() - 1);
}

//==============================================================================
//          IndexBox()
//
//  CasualNoises    09/01/2026  First implementation
//==============================================================================
IndexBox::IndexBox ( String name ) :
	Box ( name )
{
}

IndexBox::IndexBox ( String name, uint8_t noOfTabs, uint8_t currentTab ) :
	Box 		( name ),
	mNoOfTabs 	( noOfTabs ),
	mCurrentTab ( currentTab )
{
}

//==============================================================================
//          paint()
//
//	Paint the component
//
//  CasualNoises    09/01/2026  First implementation
//==============================================================================
void IndexBox::paint ( Graphics& g )
{
	Box::paint(g);
	SSD1309_Driver* screen = g.getScreen ();
	Rectangle<int> bounds = getLocalBounds ();
	uint32_t x = 8;
	uint32_t y = bounds.getY ();
	for ( uint32_t i = 0; i < mNoOfTabs; ++i )
	{
		screen->drawLine ( x, y, x + 8, y, eBitOperations::ClearBitOp );
		if (i == mCurrentTab)
		{
			screen->drawLine ( x + 1, y, x + 7, y, eBitOperations::SetBitOp );
		}
		else
		{
			screen->drawLine ( x + 3, y, x + 5, y, eBitOperations::SetBitOp );
		}
		x += 8;
	}
}

} // namespace CasualNoises
