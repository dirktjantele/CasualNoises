/*
  ==============================================================================

    Component.h
    Created: 24/12/2025

    Base class for all components

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include <Core/Text/String.h>
#include <Graphics/Geometry/Rectangle.h>

namespace CasualNoises
{

class Graphics;
class String;

class Component
{
public:
	 Component() = default;
	 Component(String name);
	~Component() = default;

	virtual void paint(Graphics& g) noexcept 			= 0;
	void setBounds(uint32_t x, uint32_t y, uint32_t w, uint32_t h) noexcept;
	void setBounds(Rectangle<int>& bounds) noexcept;
	Rectangle<int>& getLocalBounds() noexcept			{ return mLocalBounds; }

	void setVisible(bool shouldBeVisible) noexcept		{ mIsVisible = shouldBeVisible; }
	bool isVisible() const noexcept						{ return mIsVisible; }

	virtual bool handleUI_event(sIncommingUI_Event* uiEvent, bool altState, Graphics& g) = 0;

private:
	String 					mComponentName;
	Rectangle<int> 			mLocalBounds;
	bool					mIsVisible		{ false };

};

} // namespace CasualNoises
