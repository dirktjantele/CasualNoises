/*
  ==============================================================================

    Component.h
    Created: 24/12/2025

    Base class for all components

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_DISPLAY_DRIVER

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
	 Component () = default;
	 Component ( String name );
	~Component () = default;

	virtual void paint ( Graphics& g ) noexcept 			= 0;
	virtual void setBounds ( uint32_t x, uint32_t y, uint32_t w, uint32_t h ) noexcept;
	virtual void setBounds ( Rectangle<int>& bounds ) noexcept;
	virtual Rectangle<int>& getLocalBounds () noexcept			{ return mLocalBounds; }

	virtual uint32_t getHeight () { return mLocalBounds.getHeight (); }

	void setVisible ( bool shouldBeVisible ) noexcept	{ mIsVisible = shouldBeVisible; }
	bool isVisible () const noexcept					{ return mIsVisible; }

	virtual bool handleUI_event ( sIncommingUI_Event* uiEvent,
								  bool altState,
								  Graphics& g ) { return false; };
/*
    Rectangle removeFromBottom (uint32_t amountToRemove) noexcept
    {
        amountToRemove = jmin (amountToRemove, h);
        const Rectangle r (mLocalBounds., mLocalBounds.y + h - amountToRemove, w, amountToRemove);
        h -= amountToRemove;
        return r;
    }
*/

private:

	String 					mComponentName;
	Rectangle<int> 			mLocalBounds;
	bool					mIsVisible		{ false };

};

} // namespace CasualNoises

#endif
