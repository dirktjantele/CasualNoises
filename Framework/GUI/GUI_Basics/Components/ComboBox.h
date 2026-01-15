/*
  ==============================================================================

    ComboBox.h
    Created: 24/12/2025

    Used to select a string out of a list of strings

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_DISPLAY_DRIVER

#pragma once

#include <functional>

#include "Component.h"

namespace CasualNoises
{

typedef struct
{
	String*		text;
	uint32_t 	id;
} sComboBoxItem;

class ComboBox final : public Component
{
public:
	 ComboBox() = delete;
	 ComboBox(String name);
	virtual ~ComboBox();

	void paint(Graphics& g) noexcept;

	void setFont (const sFont* fontPtr);
	void addItem (String* text, uint32_t value);

	uint32_t getNumItems	()				 const noexcept { return mItemPtrs.size(); }
	int32_t  getItemId 		(uint32_t index) const noexcept
	{
		if (index >= getNumItems()) return -1;
		return mItemPtrs[index]->id;
	}
	uint32_t getSelectedId () const noexcept { return getItemId (mFocus); }
	void setFocus ( uint32_t focus ) noexcept;

	bool handleUI_event(sIncommingUI_Event* uiEvent, bool altState, Graphics& g) override;

	std::function<void()> onChange;

private:
	const sFont*					mFontPtr { nullptr };
	std::vector<sComboBoxItem*> 	mItemPtrs;

	int32_t							mFocus { 0 };

};

} // namespace CasualNoises

#endif
