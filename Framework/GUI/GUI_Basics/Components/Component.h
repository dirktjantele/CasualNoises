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

	virtual void paint ( Graphics& g ) noexcept {};
	virtual void setBounds ( int32_t x, int32_t y, int32_t w, int32_t h ) noexcept;
	virtual void setBounds ( Rectangle<int32_t>& bounds ) noexcept;
	virtual Rectangle<int32_t>& getLocalBounds () noexcept
		{ return mLocalBounds; }

	virtual uint32_t getHeight () { return mLocalBounds.getHeight (); }

	void setVisible ( bool shouldBeVisible ) noexcept	{ mIsVisible = shouldBeVisible; }
	bool isVisible () const noexcept					{ return mIsVisible; }

	virtual bool handleUI_event ( sIncommingUI_Event* uiEvent,
								  bool altState,
								  Graphics& g ) { return false; };
private:

	String 					mComponentName;
	Rectangle<int32_t>		mLocalBounds;
	bool					mIsVisible		{ false };

};

} // namespace CasualNoises

#endif
