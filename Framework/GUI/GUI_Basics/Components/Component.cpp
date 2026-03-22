/*
  ==============================================================================

    Component.cpp
    Created: 24/12/2025

    Base class for all components

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_DISPLAY_DRIVER

#include "Component.h"

namespace CasualNoises
{

//==============================================================================
//          Component()
//
// 	Main page initializer
//
//  CasualNoises    24/12/2025  First implementation
//==============================================================================
Component::Component ( String name ) :
		mComponentName ( name )
{

}

//==============================================================================
//          setBounds()
//
// 	Set local bounds
//
//  CasualNoises    24/12/2025  First implementation
//==============================================================================
void Component::setBounds ( int32_t x, int32_t y, int32_t w, int32_t h )
{
	mLocalBounds.setBounds ( x, y, w, h );
}

//==============================================================================
//          setBounds()
//
// 	Main page initializer
//
//  CasualNoises    29/12/2025  First implementation
//==============================================================================
void Component::setBounds ( Rectangle<int32_t>& bounds )
{
	mLocalBounds.setBounds ( bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight() );
}

} // namespace CasualNoises

#endif
