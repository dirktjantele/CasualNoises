/*
  ==============================================================================

    RootPage.cpp
    Created: 24/12/2025

    Abstract class used as root for all page objects

    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "RootPage.h"

#include "YellowPages.h"

#include <GUI/GUI_Basics/Components/ComboBox.h>
#include <xml/XmlElement.h>

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

	// Do the actual painting
	paint(g);
	for(auto comp : mChildren)
	{
		if ( comp->isVisible() )
			comp->paint ( g );
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
	bool success = handleLocalUI_event(uiEvent, altState, g);
	if ( ! success )
	{
		for (auto child : mChildren)
		{
			if (child->handleUI_event(uiEvent, altState, g))
				return true;
		}
		return false;
	}
	return success;
}

//==============================================================================
//          dimSwitchLeds()
//
// 	Set all led's except for EXIT & ALT to there dimmed state
//
//  CasualNoises    08/01/2026  First implementation
//==============================================================================
void RootPage::dimSwitchLeds ()
{
	eLED_BitNums dimmed[] = {
		eLED_BitNums::SWITCH_2, eLED_BitNums::SWITCH_3, eLED_BitNums::SWITCH_4,
		eLED_BitNums::SWITCH_5, eLED_BitNums::SWITCH_6
	};
	sLED_Event event;
	event.ledIntensity = 5;
	for (auto led : dimmed)
	{
		event.ledBitNum = (uint32_t)led;
		BaseType_t res = xQueueSend(gYellowPages.gLED_ThreadQueueHandle, &event, 10);
		if (res != pdPASS)
			CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);
	}
}

//==============================================================================
//          setSwitchLed()
//
// 	Set intensity of the given led to 100%
//
//  CasualNoises    08/01/2026  First implementation
//==============================================================================
void RootPage::setSwitchLed  ( eLED_BitNums led )
{
	sLED_Event event;
	event.ledIntensity = 100;
	event.ledBitNum = (uint32_t)led;
	BaseType_t res = xQueueSend(gYellowPages.gLED_ThreadQueueHandle, &event, 10);
	if (res != pdPASS)
		CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);
}

} // namespace CasualNoises
