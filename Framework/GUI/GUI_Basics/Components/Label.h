/*
  ==============================================================================

    Label.h
    Created: 04/01/2026

    Display a text on the screen

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_DISPLAY_DRIVER

#pragma once

#include "Component.h"

#include <Graphics/Fonts/Font.h>
#include <Graphics/Placement/Justification.h>

namespace CasualNoises
{

class Justification;

class Label final : public Component
{
public:
	 Label () = delete;
	 Label ( String name, String text ) ;
	~Label () = default;

	void setText( String text );

	void setFont ( const sFont* font ) noexcept
	{
		mFontPtr = font;
	}

	eJustificationFlags getJustification () 		   { return mJustification; };
	void setJustification ( eJustificationFlags just ) { mJustification = just; };

	void paint ( Graphics& g ) noexcept override;

private:
	String 					mLabelText;
	const sFont*			mFontPtr 			{ &font_7x10 };
	eJustificationFlags 	mJustification		{ eJustificationFlags::left };

};

} // namespace CasualNoises

#endif
