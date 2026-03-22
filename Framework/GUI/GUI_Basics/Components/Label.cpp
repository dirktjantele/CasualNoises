/*
  ==============================================================================

    Label.cpp
    Created: 04/01/2026

    Display a text on the screen

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_DISPLAY_DRIVER

#include "Label.h"

#include <Graphics/Contexts/Graphics.h>

namespace CasualNoises
{

//==============================================================================
//          Label()
//
//  CasualNoises    04/01/2026  First implementation
//  CasualNoises    20/03/2026  Allow for anonymous labels
//==============================================================================
Label::Label ( String name, String text ) :
	Component ( name ),
	mLabelText ( text )
{
}

Label::Label ( String text ) :
	mLabelText ( text )
{
}

//==============================================================================
//          setText()
//
//  CasualNoises    04/01/2026  First implementation
//==============================================================================
void Label::setText( String text )
{
	mLabelText = text;
}

//==============================================================================
//          getHeight()
//
//  CasualNoises    18/03/2026  First implementation
//==============================================================================
uint32_t Label::getHeight () noexcept
{

	// Count the no of lines
	uint32_t length	= mLabelText.length();
	uint32_t lines 	= 0;
	for (uint32_t i = 0; i <= length; ++i)
	{

		if ( (mLabelText[i] == 0x0a ) || ( mLabelText[i] == 0x00 ) )
		{
			lines += 1;
		}

	}

	// Get the total height
	uint32_t height = ( ( mFontPtr->height + 1 ) * lines ) - 1;
	return height;

}

//==============================================================================
//          isJustificationSet()
//
//  CasualNoises    19/03/2026  First implementation
//==============================================================================
bool isJustificationSet ( eJustificationFlags flags, eJustificationFlags flag)
{
	bool res = static_cast <unsigned int>( flags ) & static_cast <unsigned int>( flag );
	return res;
}

//==============================================================================
//          paint()
//
//	Paint the component
//
//  CasualNoises    04/01/2026  First implementation
//==============================================================================
void Label::paint ( Graphics& g )
{

	// Is component resized?
	Rectangle<int32_t> bounds = getLocalBounds ();
	if ( ( bounds.getWidth() == 0 ) || ( bounds.getHeight() == 0 ) )
		return;

	// Calculate required width & height
	uint32_t length 	= mLabelText.length();
	uint32_t lines		= 0;
	uint32_t width		= 0;
	uint32_t tmpWidth	= 0;
	for (uint32_t i = 0; i <= length; ++i)
	{
		if ( ( mLabelText[i] == 0x0a ) || ( mLabelText[i] == 0x00 ) )
		{
			lines += 1;
			if ( tmpWidth > width )
				width = tmpWidth;
			tmpWidth = 0;
			mLabelText [i] = 0x00;			// Split into separate strings, will be rejoined later
			i += 1;
		}
		else
		{
			tmpWidth += 1;
		}
	}

	// Calculate justification (for 'left' we don't have to do anything
	width								= ( width * mFontPtr->width );
	uint32_t height 					= ( lines * mFontPtr->height );
	if ( ( isJustificationSet ( mJustification, eJustificationFlags::horizontallyCentred ) ) ||
		 ( isJustificationSet ( mJustification, eJustificationFlags::centred ) ) )
	{
		int midX = bounds.getX() + ( bounds.getWidth () / 2 );
		bounds.setX ( midX - ( width / 2 ) );
	}
	if ( isJustificationSet ( mJustification, eJustificationFlags::right ) )
	{
		bounds.setX ( bounds.getX() + bounds.getWidth () - width );
	}
	if ( ( isJustificationSet ( mJustification, eJustificationFlags::verticallyCentred ) ) ||
		 ( isJustificationSet ( mJustification, eJustificationFlags::centred ) ) )
	{
		int midY = bounds.getY() + ( bounds.getHeight () / 2 );
		bounds.setY ( midY - ( height / 2 ) );
	}
	// ToDo			implement other justification types

	// Paint all text lines
	uint32_t y = bounds.getY();
	char* charPtr = mLabelText.getStringPtr ();
	SSD1309_Driver* screen = g.getScreen ();
	for ( uint32_t i = 0; i < lines; ++i )
	{
		uint32_t x = bounds.getX ();
		screen->drawText ( x, y, charPtr, mFontPtr );
		y += mFontPtr->height + 1;
		char* ptr = charPtr;
		charPtr += strlen ( charPtr ) + 1;
		if (i < ( lines - 1 ) )
			ptr[strlen(ptr)] = 0x0a;		// Rejoin strings again
	}

}

} // namespace CasualNoises

#endif
