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

	void paint(Graphics& g) noexcept;

	void setFont (const sFont* fontPtr);
	void addItem (String text, uint32_t value);

	int getNumItems	()	const { return mItems.size(); }


private:
	const sFont*					mFontPtr { nullptr };
	std::vector<sComboBoxItem> 		mItems;

};

} // namespace CasualNoises
