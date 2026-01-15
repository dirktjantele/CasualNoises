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
//==============================================================================
Label::Label ( String name, String text ) :
	Component (name),
	mLabelText (text)
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
//          paint()
//
//	Paint the component
//
//  CasualNoises    04/01/2026  First implementation
//==============================================================================
void Label::paint ( Graphics& g )
{

	// Calculate required width & height
	uint32_t length = mLabelText.length();
	uint32_t lines		= 0;
	uint32_t width		= 0;
	uint32_t tmpWidth	= 0;
	for (uint32_t i = 0; i <= length; ++i)
	{
		if ((mLabelText[i] == 0x0a) || (mLabelText[i] == 0x00))
		{
			lines += 1;
			if (tmpWidth > width)
				width = tmpWidth;
			tmpWidth = 0;
			mLabelText[i] = 0x00;
			i += 1;
		}
		else
		{
			tmpWidth += 1;
		}
	}

	// Calculate justification
	width								= ( width * ( mFontPtr->width + 1)) - 1;
	uint32_t height 					= ( lines * ( mFontPtr->height + 1)) - 1;
//	eJustificationFlags justification	= mJustification;
	Rectangle<int> bounds				= getLocalBounds ();
//	eJustificationFlags just 						= static_cast<eJustificationFlags>(mJustification);
	if ( (uint32_t)mJustification & ( uint32_t)eJustificationFlags::horizontallyCentred )
	{
		bounds.setX ( bounds.getX() + bounds.getWidth() / 2 );
	}
	if ( (uint32_t)mJustification & (uint32_t)eJustificationFlags::verticallyCentred )
	{
		bounds.setY ( bounds.getY() + (bounds.getHeight() / 2) - (height / 2) );
	}
	// ToDo			implement other justification types

	// Paint all text lines
	uint32_t y = bounds.getY(); // - ((lines / 2) * mFontPtr->height);
	char* charPtr = mLabelText.getStringPtr();
	SSD1309_Driver* screen = g.getScreen();
	for (uint32_t i = 0; i < lines; ++i)
	{
		uint32_t x = bounds.getX();
		if ( (uint32_t)mJustification & (uint32_t)eJustificationFlags::horizontallyCentred )
		{
			x -= (strlen(charPtr) * mFontPtr->width) / 2;
		}
		screen->drawText(x, y, charPtr, mFontPtr);
		y += mFontPtr->height + 1;
		char* ptr = charPtr;
		charPtr += strlen(charPtr) + 1;
		if (i < (lines - 1))
			ptr[strlen(ptr)] = 0x0a;
	}


}

} // namespace CasualNoises

#endif
