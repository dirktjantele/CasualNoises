/*
  ==============================================================================

    Component.cpp
    Created: 18 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

// 23/1/2024 updated in order to be able to use the ILI9341 in parallel mode (8080 mode)
#ifdef USE_ILI9341_SERIAL_MODE

#include "Component.h"

namespace CasualNoises
{

//==============================================================================
//          paint()
//
// Paint the component and all children onto the screen
//		should be called by the UI thread at regular intervals
//
//  CasualNoises    18/08/2023  First implementation
//==============================================================================
void Component::paint(Graphics& g)
{

	// Paint component if required
	if (mNeedsRepainting)
	{
		g.setColour(colour::LIGHTGREY);
		g.fillAll();
	}

	// Paint all children
	for (auto it = mChildren.begin(); it != mChildren.end(); ++it)
		(*it)->paint(g);

}

//==============================================================================
//          addChildComponent()
//
//  CasualNoises    19/08/2023  First implementation
//==============================================================================
void Component::addChildComponent (Component *child)
{
	mChildren.push_back(child);
}

} // namespace CasualNoises

#endif
