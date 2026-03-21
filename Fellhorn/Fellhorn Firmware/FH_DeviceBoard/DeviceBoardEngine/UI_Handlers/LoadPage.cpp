/*
  ==============================================================================

    LoadPage.cpp
    Created: 24/12/2025

   Handle main page UI interaction

    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "LoadPage.h"

namespace CasualNoises
{

//==============================================================================
//          LoadPage()
//
// Main page initializer
//
//  CasualNoises    24/12/2025  First implementation
//==============================================================================
LoadPage::LoadPage(SSD1309_Driver* oledDriverPt,
				   QueueHandle_t driverQueueHandle,
				   PageManager* pageManagerPtr) :
	RootPage( oledDriverPt, driverQueueHandle, pageManagerPtr )
{
}

//==============================================================================
//          paint()
//
// 	Show main page
//
//  CasualNoises    24/12/2025  First implementation
//==============================================================================
void LoadPage::paint(Graphics& g)
{

	// ToDo: implement this
	m_oledDriverPtr->clearDisplay();
	m_oledDriverPtr->drawRect(0, 0, 127, 63);
	m_oledDriverPtr->drawText(10, 10, "CasualNoises", &CasualNoises::font_7x10);
	m_oledDriverPtr->drawText(20, 30, "Load Page", &CasualNoises::font_11x18);
	m_oledDriverPtr->refreshDisplay();

}

//==============================================================================
//          resized()
//
// 	Set size and location of all components in the page
//
//  CasualNoises    26/12/2025  First implementation
//==============================================================================
void LoadPage::resized()
{

}

//==============================================================================
//          loadContext()
//
// 	Load context from flash
//
//  CasualNoises    03/01/2026  First implementation
//==============================================================================
void LoadPage::loadContext()
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
void LoadPage::saveContext()
{
	// Nothing to save here
}

} // namespace CasualNoises
