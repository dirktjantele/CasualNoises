/*
  ==============================================================================

    SetupPage.cpp
    Created: 24/12/2025

   Handle main page UI interaction

    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "SetupPage.h"

namespace CasualNoises
{

//==============================================================================
//          SetupPage()
//
// Main page initializer
//
//  CasualNoises    24/12/2025  First implementation
//==============================================================================
SetupPage::SetupPage(SSD1309_Driver* oledDriverPt,
					 QueueHandle_t driverQueueHandle,
					 PageManager* pageManagerPtr) :
	RootPage(oledDriverPt, driverQueueHandle, pageManagerPtr)
{
}

//==============================================================================
//          paint()
//
// 	Show main page
//
//  CasualNoises    24/12/2025  First implementation
//==============================================================================
void SetupPage::paint(Graphics& g)
{

	// ToDo: implement this
	m_oledDriverPtr->clearDisplay();
	m_oledDriverPtr->drawRect(0, 0, 127, 63);
	m_oledDriverPtr->drawText(10, 10, "CasualNoises", &CasualNoises::font_7x10);
	m_oledDriverPtr->drawText(10, 30, "Setup Page", &CasualNoises::font_11x18);
	m_oledDriverPtr->refreshDisplay();

}

//==============================================================================
//          resized()
//
// 	Set size and location of all components in the page
//
//  CasualNoises    26/12/2025  First implementation
//==============================================================================
void SetupPage::resized()
{

}

//==============================================================================
//          updateLEDs()
//
//  CasualNoises    13/04/2026  First implementation
//==============================================================================
void SetupPage::updateLEDs ()
{
	dimSwitchLeds ();
}

//==============================================================================
//          loadContext()
//
// 	Load context from flash
//
//  CasualNoises    03/01/2026  First implementation
//==============================================================================
void SetupPage::loadContext()
{
	// Nothing to save here
}

//==============================================================================
//          saveContext()
//
// 	Save context to flash
//
//  CasualNoises    26/12/2025  First implementation
//==============================================================================
void SetupPage::saveContext()
{

}

//==============================================================================
//          handleUI_event()
//
// 	Handle UI events for this page
//
//  CasualNoises    25/12/2025  First implementation
//==============================================================================
/*
bool SetupPage::handleUI_event(sIncommingUI_Event* uiEvent, bool altState)
{
	return false;
}
*/
} // namespace CasualNoises
