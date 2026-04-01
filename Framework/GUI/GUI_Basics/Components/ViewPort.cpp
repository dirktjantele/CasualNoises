/*
  ==============================================================================

    ViewPort.cpp
    Created: 17/03/2026

    Holds a single component and allow for vertical scrolling when the component is
      higher than the given bounds

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_DISPLAY_DRIVER

#include "ViewPort.h"
#include "Graphics/Contexts/Graphics.h"

namespace CasualNoises
{

//==============================================================================
//          ViewPort()
//
//  CasualNoises    17/03/2026  First implementation
//==============================================================================
ViewPort::ViewPort ( String name ) :
	Component ( name )
{
}

//==============================================================================
//          applyVerticalScrolling()
//
//  CasualNoises    22/03/2026  First implementation
//==============================================================================
void ViewPort::applyVerticalScrolling ( int32_t amount )
{
	int32_t minY 		   = getHeight() - mContentsComponentPtr->getHeight();
	int32_t newOffset = mOffset - amount;
	if ( newOffset > 0)
		newOffset = 0;
	else if ( newOffset < minY )
		newOffset = mOffset;

	mOffset = newOffset;
}

//==============================================================================
//          paint()
//
//	Paint the component
//
//  CasualNoises    17/03/2026  First implementation
//==============================================================================
void ViewPort::paint ( Graphics& g )
{

	// Is there any content?
	if ( mContentsComponentPtr == nullptr )
		return;

	// Set clip rectangle
	Rectangle<int32_t> bounds = getLocalBounds ();
	g.setClipRect( bounds );

	// Apply Y offset to local bounds of the content
	int32_t y = bounds.getY();
	y += mOffset;
	mContentsComponentPtr->setBounds ( bounds.getX(), y, bounds.getWidth(), bounds.getHeight() );

	// Paint contents
	mContentsComponentPtr->paint( g );

	// Restore clip rectangle
	g.resetClipRect ();

}

} // namespace CasualNoises

#endif
