/*
  ==============================================================================

    PerformanceInfoPage.cpp
    Created: 07/06/2026

    Handles the load performance info page

    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "PerformanceInfoPage.h"
#include "LoadPerformancePage.h"

#include "../PageManager.h"
#include "../../YellowPages.h"

#include "Core/Text/String.h"
#include "GUI/GUI_Basics/Components/Box.h"
#include "GUI/GUI_Basics/Components/Label.h"

#include "XML/XML_Tools.h"

using namespace tinyxml2;

namespace DeviceBoard
{

using namespace CasualNoises;

//==============================================================================
//          PerformanceInfoPage() & ~PerformanceInfoPage()
//
//  CasualNoises    07/06/2026  First implementation
//==============================================================================
PerformanceInfoPage::PerformanceInfoPage ( SSD1309_Driver* oledDriverPt,
										   QueueHandle_t driverQueueHandle,
										   PageManager* pageManagerPtr,
										   void* paramsPtr ) :
	RootPage ( oledDriverPt, driverQueueHandle, pageManagerPtr, paramsPtr )
{

	// Get selected preset
	if ( paramsPtr != nullptr )
	{
		sLoadPerformancePageState* statePtr = ( sLoadPerformancePageState* ) paramsPtr;
		mPerformanceId   = statePtr->performanceId;
		mPerformanceName = statePtr->performanceName;
	}

	// Create a border component
	mOuterBoxPtr = new Box ( "Border" );
	addAndMakeVisible ( mOuterBoxPtr );

	// Create labels
	mNameLabelPtr = new Label ( "...name..." );
	addAndMakeVisible ( mNameLabelPtr );
	mTypeLabelPtr = new Label ( "...type..." );
	addAndMakeVisible ( mTypeLabelPtr );
	mDescriptionLabel1_Ptr  = new Label ( "...description..." );
	addAndMakeVisible ( mDescriptionLabel1_Ptr );
	mDescriptionLabel2_Ptr  = new Label ( "...description..." );
	addAndMakeVisible ( mDescriptionLabel2_Ptr );

	// Set '>' led on
	updateLEDs ();

}

PerformanceInfoPage::~PerformanceInfoPage()
{
	if ( mOuterBoxPtr != nullptr )				delete mOuterBoxPtr;
	if ( mNameLabelPtr != nullptr )				delete mNameLabelPtr;
	if ( mTypeLabelPtr != nullptr )				delete mTypeLabelPtr;
	if ( mDescriptionLabel1_Ptr != nullptr )	delete mDescriptionLabel1_Ptr;
	if ( mDescriptionLabel2_Ptr != nullptr )	delete mDescriptionLabel2_Ptr;
}

//==============================================================================
//          loadContext()
//
//  CasualNoises    07/06/2026  First implementation
//==============================================================================
void PerformanceInfoPage::loadContext()
{

	// Get performance id
	if ( mPerformanceId == 0 )
	{
		sLoadPerformancePageState state;
		uint32_t size = readTLV_TagBytes ( mTLV_DriverQueueHandle,
										   (uint32_t)eTLV_Tag::CurrentPresetId,
										   sizeof ( sLoadPerformancePageState ),
										   (uint32_t*) &state );
		if ( size > 0 )
		{
			mPerformanceId = state.performanceId;
		} else
		{
			mPerformanceId = 1;
		}
	}

	// Update labels with XML performance data
	updateLabelsFromXML ();

}

//==============================================================================
//          saveContext()
//
//  CasualNoises    07/06/2026  First implementation
//==============================================================================
void PerformanceInfoPage::saveContext()
{
	sLoadPerformancePageState state;
	state.performanceId = mPerformanceId;
	updateTLV_Bytes (mTLV_DriverQueueHandle,
					 (uint32_t)eTLV_Tag::CurrentPresetId,
					 sizeof ( sLoadPerformancePageState ),
					 (uint32_t*) &state );

}

//==============================================================================
//          updateLabelsFromXML()
//
//  CasualNoises    07/06/2026  First implementation
//==============================================================================
void PerformanceInfoPage::updateLabelsFromXML ()
{

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

	// Create a XML document & parse it
	XMLDocument doc;
	doc.Parse ( bufferPtr );

	String name;
	String type;
	String description1;
	String description2;

	if ( getPerformanceInfo( doc, mPerformanceId, name, type, description1, description2 ) )
	{
		mPerformanceName = name;
		mNameLabelPtr->setText ( name );
		mTypeLabelPtr->setText ( type );
		mDescriptionLabel1_Ptr->setText ( description1 );
		mDescriptionLabel2_Ptr->setText ( description2 );
	} else
		CN_ReportFault( eErrorCodes::UI_ThreadError );

}

//==============================================================================
//          handleLocalUI_event()
//
//  CasualNoises    07/06/2026  First implementation
//==============================================================================
bool PerformanceInfoPage::handleLocalUI_event ( sIncommingUI_Event* uiEvent,
												bool altState, Graphics& g,
												sSystemSettings* settingsPtr,
												bool altSwitchState )
{
	// Handle '>' button events
	if ( ( uiEvent->encoderEvent.eventSourceID == eEventSourceID::encoderThreadSourceID ) &&
		 ( (eSwitchNums) uiEvent->encoderEvent.encoderNo == eSwitchNums::RIGTH_ARROW_SWITCH ) )
	{
		sIncommingUI_Event event;
		event.launchPerformanceEvent.eventSourceID = eEventSourceID::UI_ThreadSourceId;
		event.launchPerformanceEvent.performanceId = mPerformanceId;
		uint32_t length = mPerformanceName.length ();
		memcpy ( (void*) &event.launchPerformanceEvent.performanceName, (void*) mPerformanceName.getStringPtr(), length );
		event.launchPerformanceEvent.performanceName [ length ] = 0x00;

		UBaseType_t count = uxQueueSpacesAvailable ( gYellowPages.gUI_ThreadQueueHandle );
		while ( count == 0 )
		{
			osDelay ( pdMS_TO_TICKS ( 10 ) );
			count = uxQueueSpacesAvailable ( gYellowPages.gUI_ThreadQueueHandle );
		}
		BaseType_t res = xQueueSendToBack ( gYellowPages.gUI_ThreadQueueHandle, ( void* )&event, 10 );
		if (res != pdPASS)
			CN_ReportFault( eErrorCodes::FreeRTOS_ErrorRes );

		return true;
	}

	return false;
}

//==============================================================================
//          paint ()
//
//  CasualNoises    07/06/2026  First implementation
//==============================================================================
void PerformanceInfoPage::paint ( Graphics& g )
{

	// Clear the screen
	g.fillAll ();

}

//==============================================================================
//          resized ()
//
//  CasualNoises    07/06/2026  First implementation
//==============================================================================
void PerformanceInfoPage::resized()
{

    // Box around this page
    Rectangle<int32_t> rect = getGlobalBounds ();
    mOuterBoxPtr->setBounds ( rect );

    // Place labels
    rect.reduce (2, 2);
    mNameLabelPtr->setBounds ( rect );
    mNameLabelPtr->setJustification ( eJustificationFlags::left );

    Rectangle<int32_t> tmp;
    tmp = rect.removeFromTop ( 10 );
    mTypeLabelPtr->setBounds ( rect );
    mTypeLabelPtr->setJustification ( eJustificationFlags::right );

    tmp = rect.removeFromTop ( 20 );
    mDescriptionLabel1_Ptr->setBounds ( rect );
    mDescriptionLabel1_Ptr->setJustification ( eJustificationFlags::left );

    tmp = rect.removeFromTop ( 10 );
    mDescriptionLabel2_Ptr->setBounds ( rect );
    mDescriptionLabel2_Ptr->setJustification ( eJustificationFlags::right );

}

//==============================================================================
//          updateLEDs()
//
//  CasualNoises    07/06/2026  First implementation
//==============================================================================
void PerformanceInfoPage::updateLEDs ()
{
	dimSwitchLeds ();
	setSwitchLed ( eLED_BitNums::RIGTH_ARROW_SWITCH );
}

} // namespace DeviceBoard
