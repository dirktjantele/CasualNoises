/*
  ==============================================================================

    ComboBox.cpp
    Created: 24/12/2025

    Used to select a string out of a list of strings

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_DISPLAY_DRIVER

#include "ComboBox.h"

#include <Graphics/Contexts/Graphics.h>

#include "SystemConfig.h"

namespace CasualNoises
{

//==============================================================================
//          ComboBox() & ~ComboBox()
//
//  CasualNoises    24/12/2025  First implementation
//==============================================================================
ComboBox::ComboBox(String name) :
	Component (name)
{
}

ComboBox::~ComboBox()
{
	for (auto ptr : mItemPtrs)
	{
		delete ptr->text;
		delete ptr;
	}
}

//==============================================================================
//          paint()
//
//	Paint the component
//
//  CasualNoises    29/12/2025  First implementation
//==============================================================================
void ComboBox::paint ( Graphics& g )
{

	// Set to default font when no font is specified
	if ( mFontPtr == nullptr )
		mFontPtr = &font_7x10;

	// Calculate working stuff...
	Rectangle<int> bounds 	= getLocalBounds ();
	int32_t textHeight 		= mFontPtr->height + 1;
	int32_t visibleItems 	= bounds.getHeight () / textHeight;
	int32_t topSpacing 		= ( ( bounds.getHeight() - ( visibleItems * (mFontPtr->height + 1 ) ) ) / 2 ) + 4;
	int32_t leftIndent 		= bounds.getX() + mFontPtr->width + 1;
	int32_t firstLineIdx	= 0;
	int32_t cursorLine		= 0;

	// Determine first line & focus position
	if ( (int32_t)mItemPtrs.size() <=  visibleItems )									// All items are on one screen
	{
		firstLineIdx = 0;
		cursorLine	 = mFocus;
		visibleItems = (int32_t)mItemPtrs.size();
	} else
	{
		if ( mFocus <= (visibleItems / 2) )											// Move cursor up/down in top half
		{
			cursorLine = mFocus;
		} else if ( ( (int32_t)mItemPtrs.size() - mFocus) > (visibleItems / 2) )		// Cursor in the middle -> move contents up/down
		{
			cursorLine 		= visibleItems / 2;
			firstLineIdx	= (mFocus - 1) - ((visibleItems / 2) - 1);
		} else																		// Move cursor up/down in bottom half
		{
			cursorLine 		= mFocus - visibleItems;   // ((visibleItems / 2) + ((int32_t)mItemPtrs.size() - mFocus)) - 1;
			firstLineIdx	= (int32_t)mItemPtrs.size() - visibleItems;
		}
	}

	// Paint ComboBox
	g.fillRect ( bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), eBitOperations::ClearBitOp );
	SSD1309_Driver* screen = g.getScreen();
	uint32_t y = topSpacing;
	for (int32_t i = 0; i < visibleItems; ++i)
	{
		String text ( mItemPtrs[firstLineIdx + i]->text->getStringPtr() );
		screen->drawText ( leftIndent, y, text.getStringPtr(), mFontPtr );
		y += textHeight;
	}
	screen->drawText ( bounds.getX() + 1, topSpacing + (cursorLine * textHeight), (char*)">", mFontPtr );

}

//==============================================================================
//          setFont()
//
//  CasualNoises    29/12/2025  First implementation
//==============================================================================
void ComboBox::setFont(const sFont* fontPtr)
{
	mFontPtr = fontPtr;
}

//==============================================================================
//          addItem()
//
//  CasualNoises    26/12/2025  First implementation
//==============================================================================
void ComboBox::addItem(String* textPtr, uint32_t id)
{
	sComboBoxItem* itemPtr = new sComboBoxItem;
	itemPtr->text = new String(*textPtr);
	itemPtr->id	= id;
	mItemPtrs.push_back(itemPtr);
}

//==============================================================================
//          setFocus()
//
//  CasualNoises    08/01/2026  First implementation
//==============================================================================
void ComboBox::setFocus ( uint32_t focus ) noexcept
{
	if ( focus >= mItemPtrs.size() )
		focus = mItemPtrs.size() - 1;
	mFocus = focus;
}

//==============================================================================
//          handleUI_event()
//
//  CasualNoises    02/0/2026  First implementation
//==============================================================================
bool ComboBox::handleUI_event(sIncommingUI_Event* uiEvent, bool altState, Graphics& g)
{
	if (uiEvent->encoderEvent.encoderNo == (uint16_t)eEncoderNums::MAIN_ENCODER)
	{
		if (uiEvent->encoderEvent.eventType == eEncoderEventType::encoderSwitch)
		{
			onChange();
		} else if (uiEvent->encoderEvent.eventType == eEncoderEventType::encoderCount)
		{
			int32_t focus = mFocus + uiEvent->encoderEvent.encoderCount;
			if (focus < 0)
				focus = 0;
			if (focus >= (int32_t)mItemPtrs.size())
				focus = (int32_t)mItemPtrs.size() - 1;
			mFocus = focus;
			paint(g);
			g.refreshDisplay();
		}
		return true;
	}
	return false;
}

} // namespace CasualNoises

#endif
