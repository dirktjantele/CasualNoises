/*
  ==============================================================================

    Component.cpp
    Created: 24/12/2025

    Base class for all components

    Author:  Dirk Tjantele

  ==============================================================================
*/

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
Component::Component(String name) :
		mComponentName (name)
{

}

//==============================================================================
//          setBounds()
//
// 	Set local bouds
//
//  CasualNoises    24/12/2025  First implementation
//==============================================================================
void Component::setBounds(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
	mLocalBounds.setBounds(x, y, w, h);
}

//==============================================================================
//          setBounds()
//
// 	Main page initializer
//
//  CasualNoises    29/12/2025  First implementation
//==============================================================================
void Component::setBounds(Rectangle<int>& bounds)
{
	mLocalBounds.setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
}

} // namespace CasualNoises

