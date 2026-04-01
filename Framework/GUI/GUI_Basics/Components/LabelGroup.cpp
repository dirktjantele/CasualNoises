/*
  ==============================================================================

    LabelGroup.cpp
    Created: 19/03/2026

    Group labels in a list

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_DISPLAY_DRIVER

#include "LabelGroup.h"

#include "Label.h"

#include <Graphics/Contexts/Graphics.h>

namespace CasualNoises
{

//==============================================================================
//          LabelGroup() & ~LabelGroup()
//
//  CasualNoises    19/03/2026  First implementation
//==============================================================================
LabelGroup::LabelGroup ( String name ) :
	Component (name)
{
}

LabelGroup::~LabelGroup()
{
	if ( mDeleteElements )
	{
		for (Label* ptr : mLabelGroup)
		{
			delete ptr;
		}
	}
}

//==============================================================================
//          getHeight()
//
// Calculate the total height as the sum of the height of each Label and
//		1 pixel spacing between lines
//
//  CasualNoises    19/03/2026  First implementation
//==============================================================================
uint32_t LabelGroup::getHeight ()
{

	// An empty group has 0 height
	std::size_t count = mLabelGroup.size();
	if ( count == 0 )
		return 0;

	// Sum the height of each Label + spacing
	uint32_t height = 0;
	for (Label* label : mLabelGroup)
	{
		height += label->getHeight ();
		height += 1;
    }
	return height - 1;

}

//==============================================================================
//			addLabel()
//
//	Add a Label to the end of the group
//
//  CasualNoises    19/03/2026  First implementation
//==============================================================================
void LabelGroup::addLabel ( Label* label )
{
	mLabelGroup.push_back ( label );
}

//==============================================================================
//			addLabel()
//
//	Add a Label to the end of the group
//
//  CasualNoises    23/03/2026  First implementation
//==============================================================================
void LabelGroup::addLabel ( std::string text, eJustificationFlags flags )
{
	Label* ptr = new Label ( text.c_str() );
	ptr->setJustification( flags );
	addLabel( ptr );
}

//==============================================================================
//          setBounds()
//
//	Set the bounds of each contained label according to the given bounds
//
//  CasualNoises    21/03/2026  First implementation
//==============================================================================
void LabelGroup::setBounds ( int32_t x, int32_t y, int32_t w, int32_t h ) noexcept
{
	for (Label* label : mLabelGroup)
	{
		h = label->getHeight ();
		label->setBounds ( x, y, w, h );
		y += label->getHeight() + 1;
	}
	mElementBoundsSet = true;
}

//==============================================================================
//          paint()
//
//	Paint each label contained in the group
//
//  CasualNoises    21/03/2026  First implementation
//==============================================================================
void LabelGroup::paint ( Graphics& g )
{
	if (! mElementBoundsSet )
		return;
	for (Label* label : mLabelGroup)
	{
		label->paint( g );
	}
}

} // namespace CasualNoises

#endif
