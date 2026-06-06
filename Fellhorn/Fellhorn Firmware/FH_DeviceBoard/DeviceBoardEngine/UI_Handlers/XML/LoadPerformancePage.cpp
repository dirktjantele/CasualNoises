/*
  ==============================================================================

    SystemInfoPage.cpp
    Created: 17/03/2026

    Handle various system information display as an aid for firmware development

    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "LoadPerformancePage.h"

#include "../PageManager.h"
#include "../../YellowPages.h"
#include "../../UI_Definitions.h"

#include "SynthEngineMessage.h"

#include "Core/Text/String.h"
#include "GUI/GUI_Basics/Components/Box.h"
#include "GUI/GUI_Basics/Components/ComboBox.h"
#include "GUI/GUI_Basics/Components/Label.h"

#include "TinyXML/tinyxml2.h"

using namespace tinyxml2;

namespace DeviceBoard
{

using namespace CasualNoises;

//==============================================================================
//          LoadPerformancePage() & ~LoadPerformancePage()
//
// Load performance page initializer
//
//  CasualNoises    05/06/2026  First implementation
//==============================================================================
LoadPerformancePage::LoadPerformancePage (
		SSD1309_Driver* oledDriverPt,
		QueueHandle_t driverQueueHandle,
		PageManager* pageManagerPtr ) :
	RootPage ( oledDriverPt, driverQueueHandle, pageManagerPtr )
{

	// Create a border component
	mOuterBoxPtr = new Box ( "Border" );
	addAndMakeVisible ( mOuterBoxPtr );

	// Get preset names from flash
	buildPresetNameList ();

	// Build a ComboBox
	uint32_t id = 0;
	mComboBoxPtr = new ComboBox( String( (char*) "ComboBox" ) );
	for ( auto name : mPresetNameList )
		mComboBoxPtr->addItem( &name, id++ );
	mComboBoxPtr->onChange = [this] { onComboBoxChange(); };
	addAndMakeVisible ( mComboBoxPtr );

}

LoadPerformancePage::~LoadPerformancePage()
{
	if ( mOuterBoxPtr != nullptr )		delete mOuterBoxPtr;
}

//==============================================================================
//          onComboBoxChange()
//
//  CasualNoises    06/06/2026  First implementation
//==============================================================================
void LoadPerformancePage::onComboBoxChange () noexcept
{
	uint32_t id = mComboBoxPtr->getSelectedId ();

/*	switch (id)
	{
	case 1:								// Load performance
		mPageManagerPtr->createNewPage( ePageId::loadPage );
		break;
	case 2:								// New performance
		break;
	case 3:								// Calibration
		mPageManagerPtr->createNewPage ( ePageId::calibrationPage );
		break;
	case 4:								// Sytem Info
		mPageManagerPtr->createNewPage ( ePageId::systemInfoPage );
		break;

	default:
		CN_ReportFault ( eErrorCodes::runtimeError );
	}
*/
}

//==============================================================================
//          handleLocalUI_event()
//
// 	Handle page specific UI events
//
//  CasualNoises    05/06/2026  First implementation
//==============================================================================
bool LoadPerformancePage::handleLocalUI_event ( sIncommingUI_Event* uiEvent,
									   	   bool altState, Graphics& g,
										   sSystemSettings* settingsPtr,
										   bool altSwitchState)
{
	return false;
}

//==============================================================================
//          paint ()
//
//  CasualNoises    05/06/2026  First implementation
//==============================================================================
void LoadPerformancePage::paint ( Graphics& g )
{

	// Clear the screen
	g.fillAll ();

}

//==============================================================================
//          resized ()
//
//  CasualNoises    05/06/2026  First implementation
//==============================================================================
void LoadPerformancePage::resized()
{

    // Box around this page
    Rectangle<int32_t> rect = getGlobalBounds ();
    mOuterBoxPtr->setBounds ( rect );

    // Main menu
    rect.reduce ( 2, 2 );
    mComboBoxPtr->setBounds ( rect );

}

//==============================================================================
//          updateLEDs()
//
//  CasualNoises    05/06/2026  First implementation
//==============================================================================
void LoadPerformancePage::updateLEDs ()
{
	dimSwitchLeds ();
}

//==============================================================================
//          buildPresetNameList()
//
//  CasualNoises    05/06/2026  First implementation
//==============================================================================
void LoadPerformancePage::buildPresetNameList ()
{

	// Start with a blank slate
	mPresetNameList.clear ();

	// Load xml preset data
	uint32_t index = findNextTLV ( gYellowPages.gTLV_DriverThreadQueueHandle, (uint32_t)eTLV_Tag::FactoryPresets, 0 );
	if ( index == 0 )
		CN_ReportFault( eErrorCodes::UI_ThreadError );
	uint32_t size = getTLV_LengthBytes ( gYellowPages.gTLV_DriverThreadQueueHandle, index );
	char* bufferPtr = ( char* ) malloc( ( size + 1 ) * sizeof ( char) );
	uint32_t length = readTLV_TagBytes ( gYellowPages.gTLV_DriverThreadQueueHandle,
										 (uint32_t) eTLV_Tag::FactoryPresets,
										 size,
										 ( uint32_t* ) bufferPtr );
	bufferPtr [ size ] = 0x00;
	if ( length != size )
		CN_ReportFault( eErrorCodes::UI_ThreadError );

	// Parse XML and extract performance names
	XMLDocument doc;
	doc.Parse ( bufferPtr );

    XMLElement* root = doc.FirstChildElement("PERFORMANCES");
    if ( ! root )
    {
        CN_ReportFault( eErrorCodes::UI_ThreadError );
    }

    for (XMLElement* perf = root->FirstChildElement("PERFORMANCE");
         perf != nullptr;
         perf = perf->NextSiblingElement("PERFORMANCE"))
    {
        XMLElement* info = perf->FirstChildElement("INFO");
        if (info)
        {
            const char* name = info->FirstChildElement ( "NAME" )->GetText ();
            String* nameString = new String ( name );
            mPresetNameList.push_back ( *nameString );
        }
    }

	// Don't forget to free the buffer here...
	free ( bufferPtr );

}

} // namespace DeviceBoard
