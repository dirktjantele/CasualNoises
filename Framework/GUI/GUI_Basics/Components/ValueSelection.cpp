/*
  ==============================================================================

    ValueSelection.cpp
    Created: 13/04/2026

    Used to select a value from a set of possible values

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_DISPLAY_DRIVER

#include "ValueSelection.h"

#include "Label.h"

#include "../../../DeviceBoardEngine/UI_Definitions.h"				// ToDo find a way to get rid of this include

namespace CasualNoises
{

/****************************** ValueSelection ******************************/

//==============================================================================
//          ValueSelection() & ~ValueSelection()
//
//  CasualNoises    13/04/2026  First implementation
//==============================================================================
ValueSelection::ValueSelection ( String name ) :
	Component (name)
{
}

//==============================================================================
//          paint()
//
//  CasualNoises    13/04/2026  First implementation
//==============================================================================
void ValueSelection::paint ( Graphics& g ) noexcept
{

	// Set to default font when no font is specified
	if ( mFontPtr == nullptr )
		mFontPtr = &font_7x10;

	// Compose value
	String text;
	if ( mIsSelected )
		text += String ( (char*) "<" );
	else
		text += String ( (char*) " " );
	text += getSelectionString ();
	if ( mIsSelected )
		text += String ( (char*) ">" );
	else
		text += String ( (char*) " " );

	Label textLabel = Label ( "text", text );
	textLabel.setBounds ( getLocalBounds() );
	textLabel.setJustification (getJustification () );
	textLabel.setVisible ( isVisible () );
	textLabel.paint ( g );

}

//==============================================================================
//          handleUI_event()
//
//  CasualNoises    13/04/2026  First implementation
//==============================================================================
bool ValueSelection::handleUI_event (
		void* uiEvent,
		bool altState,
		Graphics& g,
		bool altSwichState )
{
	sIncommingUI_Event* eventPtr = static_cast< sIncommingUI_Event* > ( uiEvent );
	if ( eventPtr->encoderEvent.encoderNo == (uint16_t)eEncoderNums::MAIN_ENCODER )
	{
		if ( eventPtr->encoderEvent.eventType == eEncoderEventType::encoderCount )
		{
			int16_t count = eventPtr->encoderEvent.encoderCount;
			if ( ( advanceSelection ( count, altSwichState ) ) &&
				 onChange )
				onChange ();
			return true;
		}
	}

	return false;
}

/****************************** NoteSelection ******************************/

//==============================================================================
//          NoteSelection() & ~NoteSelection()
//
//  CasualNoises    13/04/2026  First implementation
//==============================================================================
NoteSelection::NoteSelection ( String name ) :
	ValueSelection (name)
{
}

//==============================================================================
//          advanceSelection()
//
// Return true if selection is changed
//
//  CasualNoises    13/04/2026  First implementation
//==============================================================================
bool NoteSelection::advanceSelection ( int16_t count, bool altSwichState)
{
	int32_t oldSelection = mCurrentSelection;
	if ( altSwichState )
		count *= 7;
	mCurrentSelection += count;
	if ( mCurrentSelection < getMinSelection () )
		mCurrentSelection = getMinSelection ();
	if ( mCurrentSelection > getMaxSelection () )
		mCurrentSelection = getMaxSelection ();
	return oldSelection != mCurrentSelection;
}

//==============================================================================
//          getSelectionString()
//
//  CasualNoises    13/04/2026  First implementation
//==============================================================================
String NoteSelection::getSelectionString ()
{
	uint32_t letter = 'C' + mCurrentSelection % 7;
	if ( letter > 'G' )
		letter -= 7;
	uint32_t no     = '0' + mCurrentSelection / 7;
	if ( no > '9' )
		no = 0x3031; //'01';
	return String ( String ( (char*) &letter ) + String ( (char*) &no ) );
}

} // namespace CasualNoises

#endif
