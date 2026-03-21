/*
  ==============================================================================

    ViewPort.cpp
    Created: 17/03/2026

    Holds a single component and allow for vertical scrolling when the component is
      higher than the given bounds

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_DISPLAY_DRIVER

#include "ViewPort.h"

namespace CasualNoises
{

//==============================================================================
//          ViewPort()
//
//  CasualNoises    17/03/2026  First implementation
//==============================================================================
ViewPort::ViewPort ( String name ) :
	Component ( name )
{
}

//==============================================================================
//          paint()
//
//	Paint the component
//
//  CasualNoises    17/03/2026  First implementation
//==============================================================================
void ViewPort::paint ( Graphics& g )
{
	// Set clip rectangle
	// Set bounds of the contained component
	// In LabelGroup, implement setBounds to set the bounds of each label according to it's bounds

	uint32_t height = mContentsComponentPtr->getHeight ();

	// Restore clip rectangle
}

} // namespace CasualNoises

#endif
