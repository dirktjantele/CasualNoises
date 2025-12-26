/*
  ==============================================================================

    ComboBox.h
    Created: 24/12/2025

    Used to select a string out of a list of strings

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "Component.h"

#include "../CasualNoises/Core/Text/String.h"

namespace CasualNoises
{

typedef struct
{
	String		text;
	uint32_t 	value;
} sComboBoxItem;

class ComboBox final : public Component
{
public:
	 ComboBox() = delete;
	 ComboBox(String name);
	~ComboBox() = default;

	void addItem(String text, uint32_t value);

private:
	std::vector<sComboBoxItem> 		mItems;

};

} // namespace CasualNoises
