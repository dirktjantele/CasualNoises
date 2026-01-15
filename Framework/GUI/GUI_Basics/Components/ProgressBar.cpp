/*
  ==============================================================================

    ProgressBar.h
    Created: 04/01/2026

    Simple progress bar

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_DISPLAY_DRIVER

#include "ProgressBar.h"

#include <Graphics/Contexts/Graphics.h>
#include <GUI/GUI_Basics/Components/Label.h>

#include "SystemConfig.h"

namespace CasualNoises
{

//==============================================================================
//          ProgressBar() & ~ProgressBar()
//
//  CasualNoises    04/01/2026  First implementation
//==============================================================================
ProgressBar::ProgressBar( String name ) :
	Component ( name )
{
	mLabelPtr = new Label ( String(), String() );
}

ProgressBar::ProgressBar ( String name, String text ) :
	Component ( name ),
	mText ( text )
{
}

ProgressBar::~ProgressBar()
{
	if ( mLabelPtr != nullptr ) delete mLabelPtr;
}

//==============================================================================
//          paint()
//
//	Paint the component
//
//  CasualNoises    29/12/2025  First implementation
//==============================================================================
void ProgressBar::paint(Graphics& g)
{

	// Outer bounds
	SSD1309_Driver* screen = g.getScreen();
	Rectangle<int> bounds = getLocalBounds();
	screen->drawRect(bounds);

	// Paint text
	mLabelPtr->setBounds(bounds);
	mLabelPtr->setText ( mText );
	mLabelPtr->paint(g);

	// Fill progress bar
	mValue += 0.1f;
	if ( mValue <   0.0f ) mValue =   0.0f;
	if ( mValue > 100.0f ) mValue = 100.0f;
	Rectangle<int> fill = bounds;
	fill.reduce ( 1, 1 );
	fill.setHeight( fill.getHeight() + 1 );
	fill.setWidth ( ( fill.getWidth() * mValue ) / 100 );
	fill.setWidth ( ( fill.getWidth() + 1 ) );
	g.fillRect( fill, eBitOperations::InvertBitOp );

}

} // namespace CasualNoises

#endif
