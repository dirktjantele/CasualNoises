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
				   TLV_Driver* TLV_DriverPtr,
				   PageManager* pageManagerPtr) :
	RootPage(oledDriverPt, TLV_DriverPtr, pageManagerPtr)
{
/*
	eTLV_Tag tag = eTLV_Tag::UI_LoadPageState;

	// Set initial state, will be overwritten if state data is found in flash memory
	mPageState.version = mVersion;

	// Load state from flash when found, create it if it does not exists yet
	bool saveState = false;
	uint32_t index = TLV_DriverPtr->findNextTLV((uint32_t)tag, 0);
	if (index == 0)				// TLV does not exists, create new one
	{
		saveState = true;
	}
	else if (TLV_DriverPtr->getTLV_LengthBytes(index) != sizeof(sLoadPageState))
	{
		TLV_DriverPtr->deleteTLV((uint32_t)tag, true);	// Delete existing TLV
		saveState = true;
	}
	if (saveState)
	{
		TLV_DriverPtr->deleteTLV((uint32_t)tag, true);
		if ( ! TLV_DriverPtr->addTLV_Bytes((uint32_t)tag, sizeof(sLoadPageState), (uint32_t*) &mPageState) )
			CN_ReportFault(eErrorCodes::UI_ThreadError);
	}
	uint32_t stateIndex    	= TLV_DriverPtr->findNextTLV ((uint32_t)tag, 0);									// ToDo remove debug lines
	uint32_t stateCnt		= TLV_DriverPtr->readTLV_Bytes(stateIndex, sizeof(sLoadPageState), (uint32_t*) &mPageState);
	UNUSED(stateCnt);
*/
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
