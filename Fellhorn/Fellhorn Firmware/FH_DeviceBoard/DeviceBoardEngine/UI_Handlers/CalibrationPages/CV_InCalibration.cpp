/*
  ==============================================================================

    CV_InCalibration.cpp
    Created: 03/04/2026

    Handle CV calibration

    Author:  Dirk Tjantele

  ==============================================================================
*/

#include <UI_Handlers/CalibrationPages/CV_InCalibration.h>
#include "Graphics/Contexts/Graphics.h"
#include "GUI/GUI_Basics/Components/Box.h"

#include "SynthEngineMessage.h"

#include "NerveNet/NerveNetMasterThread.h"

namespace CasualNoises
{

//==============================================================================
//          CV_CalibrationPage() & ~CV_CalibrationPage()
//
// Main page initializer
//
//  CasualNoises    03/04/2026  First implementation
//==============================================================================
CV_CalibrationPage::CV_CalibrationPage (
		SSD1309_Driver* oledDriverPt,
		QueueHandle_t driverQueueHandle,
		PageManager* pageManagerPtr ) :
	RootPage ( oledDriverPt, driverQueueHandle, pageManagerPtr )
{

	// Create a border component
	mOuterBoxPtr = new Box( String( (char*) "Border" ) );
	addAndMakeVisible ( mOuterBoxPtr );

	// Send a NerveNet request for all ADC input data
	tRequestADC_Data message;
	message.header.sourceID			= eNerveNetSourceId::FellhornDeviceBoard;
	message.header.destinationID	= eNerveNetSourceId::FellhornBothSides;
	message.header.messageTag		= (uint32_t) eSynthEngineMessageType::ADC_DataRequest;
	message.header.messageLength	= sizeof ( tRequestADC_Data );
	bool success = gNerveNetMasterThreadPtr[0]->sendMessage ( &message, sizeof ( tRequestADC_Data ) );
	if ( ! success )
		CN_ReportFault(eErrorCodes::NerveNetThread_Error);

}

CV_CalibrationPage::~CV_CalibrationPage()
{
	if ( mOuterBoxPtr 	 			!= nullptr )		delete mOuterBoxPtr;
}

//==============================================================================
//          paint()
//
// Show main page
//
//  CasualNoises    02/02/2026  First implementation
//==============================================================================
void CV_CalibrationPage::paint(Graphics& g)
{
	g.fillAll();
}

//==============================================================================
//          resized()
//
// 	Set size and location of all components in the page
//
//  CasualNoises    01/02/2026  First implementation
//==============================================================================
void CV_CalibrationPage::resized()
{

    // Box around this page
    Rectangle<int32_t> rect = getGlobalBounds();
    mOuterBoxPtr->setBounds ( rect );


}

//==============================================================================
//          handleLocalUI_event()
//
// 	Handle page specific UI events
//
//  CasualNoises    08/06/2025  First implementation
//  CasualNoises    08/04/2026  altSwitchState added
//==============================================================================
bool CV_CalibrationPage::handleLocalUI_event (
		sIncommingUI_Event* uiEvent,
		bool altState,
		Graphics& g,
		sSystemSettings* settingsPtr,
		bool altSwitchState )
{

	tNerveNetMessageHeader* ptr = ( tNerveNetMessageHeader* ) uiEvent->nerveNetEvent.eventdataPtr;
	UNUSED ( ptr );

	return false;
}

//==============================================================================
//          loadContext()
//
// 	Load context from flash
//
//  CasualNoises    02/02/2026  First implementation
//==============================================================================
void CV_CalibrationPage::loadContext()
{
	// Nothing to load here
}

//==============================================================================
//          saveContext()
//
// 	Save context to flash
//
//  CasualNoises    02/02/2026  First implementation
//==============================================================================
void CV_CalibrationPage::saveContext()
{
	// Nothing to save here
}
} // namespace CasualNoises
