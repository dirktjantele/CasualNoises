/*
  ==============================================================================

    ProgressBar.h
    Created: 04/01/2026

    Simple progress bar

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_DISPLAY_DRIVER

#pragma once

#include "Component.h"

namespace CasualNoises
{

class Label;

class ProgressBar final : public Component
{
public:
	 ProgressBar () = delete;
	 ProgressBar ( String name );
	 ProgressBar ( String name, String text );
	~ProgressBar ();

	void paint(Graphics& g) noexcept;

	void setFont  ( const sFont* fontPtr ) noexcept { mFontPtr = fontPtr; };
	void setText  ( String text ) noexcept 			{ mText = text; };;
	void setValue ( float value ) noexcept 			{ mValue = value; };		// Value in % (0% <= 100%)

private:
	const sFont*		mFontPtr 	{ &font_7x10 };
	String				mText	 	{ (char*)"" };
	float				mValue	 	{ 0 };

	Label*				mLabelPtr	{ nullptr };

};

} // namespace CasualNoises

#endif
