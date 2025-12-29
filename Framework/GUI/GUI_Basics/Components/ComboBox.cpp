/*
  ==============================================================================

    ComboBox.cpp
    Created: 24/12/2025

    Used to select a string out of a list of strings

    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "ComboBox.h"

namespace CasualNoises
{

//==============================================================================
//          ComboBox()
//
//  CasualNoises    24/12/2025  First implementation
//==============================================================================
ComboBox::ComboBox(String name) :
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
void ComboBox::paint(Graphics& g)
{

	// Set to default font when no font is specified
	if (mFontPtr == nullptr)
		mFontPtr = &font_7x10;

	Rectangle<int> bounds = getLocalBounds();
	uint32_t textHeight = mFontPtr->height + 1;
	uint32_t visibleItems = bounds.getHeight() / textHeight;
	uint32_t spacing = (bounds.getHeight() - (visibleItems * mFontPtr->height + 1)) / 2;
	uint32_t leftIndent = mFontPtr->width + 1;



	uint32_t noItems = mItems.size();

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
void ComboBox::addItem(String text, uint32_t value)
{
	sComboBoxItem item;
	item.text	= text;
	item.value	= value;
	mItems.push_back(item);

	for (auto item : mItems)
	{
		String text 	= item.text;
		uint32_t value	= item.value;
	}

}


} // namespace CasualNoises
