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
