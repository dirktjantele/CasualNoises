/*
  ==============================================================================

    ValueSelection.h
    Created: 13/04/2026

    Used to select a value from a set of possible values

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_DISPLAY_DRIVER

#pragma once

#include <functional>

#include "Graphics/Fonts/Font.h"
#include "Graphics/Placement/Justification.h"

#include "Component.h"

namespace CasualNoises
{

/****************************** ValueSelection ******************************/

class ValueSelection : public Component
{
public:
	 	 	 ValueSelection () = delete;
	 	 	 ValueSelection ( String name );
	virtual ~ValueSelection () = default;

	eJustificationFlags getJustification () 			{ return mJustification; };
	void setJustification ( eJustificationFlags just )	{ mJustification = just; };

	void setIsSelected ( bool flag ) 					{ mIsSelected = flag; };
	bool isSelected ()									{ return mIsSelected; };

	void paint(Graphics& g) noexcept;

	virtual bool handleUI_event ( void* uiEvent,
								  bool altState,
								  Graphics& g,
								  bool altSwichState );

	std::function<void()> onChange;

	virtual int32_t getCurrentSelection () 	const noexcept = 0;
	virtual int32_t getMinSelection () 		const noexcept = 0;
	virtual int32_t getMaxSelection () 		const noexcept = 0;

private:
	const sFont*			mFontPtr 			{ nullptr };
	eJustificationFlags 	mJustification		{ eJustificationFlags::left };

	bool					mIsSelected			{ false };

	virtual bool			advanceSelection ( int16_t count, bool altSwichState) = 0;
	virtual String			getSelectionString () = 0;

};

/****************************** NoteSelection ******************************/

class NoteSelection final : public ValueSelection
{
public:
			 NoteSelection () = delete;
			 NoteSelection ( String name );
	virtual ~NoteSelection () = default;

	int32_t getCurrentSelection () 	const noexcept override { return mCurrentSelection; };
	int32_t getMinSelection () 		const noexcept override { return 0; };
	int32_t getMaxSelection () 		const noexcept override { return 10 * 7; };

private:

	int32_t			mCurrentSelection	{ 0 };

	bool			advanceSelection ( int16_t count, bool altSwichState);
	String			getSelectionString ();

};

} // namespace CasualNoises

#endif
