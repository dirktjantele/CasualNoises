/*
  ==============================================================================

    RootPage.cpp
    Created: 24/12/2025

    Abstract class used as root for all page objects

    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "RootPage.h"

#include <GUI/GUI_Basics/Components/ComboBox.h>

namespace CasualNoises
{

//==============================================================================
//          RootPage
//
//  CasualNoises    28/12/2025  First implementation
//==============================================================================
RootPage::RootPage(SSD1309_Driver* m_oledDriverPtr,
		  TLV_Driver* mTLV_DriverPtr,
		  PageManager* pageManagerPtr) :
		m_oledDriverPtr (m_oledDriverPtr),
		mTLV_DriverPtr (mTLV_DriverPtr),
		mPageManagerPtr (pageManagerPtr)
{
	mLocalBounds.setPosition(0, 0);
	mLocalBounds.setWidth(DISPLY_WIDTH);
	mLocalBounds.setHeight(DISPLAY_HEIGHT);
}

//==============================================================================
//          paintAll
//
//	Paint all components in this page.
//
//  CasualNoises    29/12/2025  First implementation
//==============================================================================
void RootPage::paintAll(Graphics& g)
{
	paint(g);
	for(auto comp : mChildren)
	{
		comp->paint(g);
	}
	m_oledDriverPtr->refreshDisplay();
}

//==============================================================================
//          addChildComponent()
//
// 	This container does not take ownership of the component,
//		it must be deleted by the caller
//
//  CasualNoises    26/12/2025  First implementation
//==============================================================================
void RootPage::addChildComponent (Component *child)
{
	mChildren.push_back(child);
}

//==============================================================================
//          addAndMakeVisible()
//
// 	This container does not take ownership of the component,
//		it must be deleted by the caller
//
//  CasualNoises    26/12/2025  First implementation
//==============================================================================
void RootPage::addAndMakeVisible (Component* child)
{
	child->setVisible(true);
	mChildren.push_back(child);
}

//==============================================================================
//          getBounds()
//
// 	Returns the bouns of the parent page if there is one,
//		otherwise return the display size
//
//  CasualNoises    26/12/2025  First implementation
//==============================================================================
Rectangle< int > RootPage::getBounds () const
{
	return mLocalBounds;
}

//==============================================================================
//          handleUI_event()
//
// 	Iterate over all child components & call handleUI_event
//
//  CasualNoises    26/12/2025  First implementation
//==============================================================================
bool RootPage::handleUI_event(sIncommingUI_Event* uiEvent, bool altState, Graphics& g)
{
	for (auto child : mChildren)
	{
		if (child->handleUI_event(uiEvent, altState, g))
			return true;
	}
	return false;
}

} // namespace CasualNoises
