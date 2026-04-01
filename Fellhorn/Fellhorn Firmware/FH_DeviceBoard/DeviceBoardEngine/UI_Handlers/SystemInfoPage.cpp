/*
  ==============================================================================

    SystemInfoPage.cpp
    Created: 17/03/2026

    Handle various system information display as an aid for firmware development

    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "SystemInfoPage.h"

#include "PageManager.h"
#include "YellowPages.h"
#include "../../DeviceBoardEngine/UI_Definitions.h"

#include <SynthEngineMessage.h>

#include <Core/Text/String.h>
#include <GUI/GUI_Basics/Components/Box.h>
#include <GUI/GUI_Basics/Components/ViewPort.h>
#include <GUI/GUI_Basics/Components/Label.h>
#include <GUI/GUI_Basics/Components/LabelGroup.h>
#include <GUI/GUI_Basics/Components/ProgressBar.h>
#include <Graphics/Geometry/Rectangle.h>

namespace CasualNoises
{

/*---------------------------- SystemInfoPage ----------------------------*/

//==============================================================================
//          SystemInfoPage() & ~SystemInfoPage()
//
// Main page initializer
//
//  CasualNoises    17/03/2026  First implementation
//==============================================================================
SystemInfoPage::SystemInfoPage (
		SSD1309_Driver* oledDriverPt,
		QueueHandle_t driverQueueHandle,
		PageManager* pageManagerPtr ) :
	RootPage ( oledDriverPt, driverQueueHandle, pageManagerPtr )
{

	// Create a border component
	mOuterBoxPtr = new Box ( "Border" );
	addAndMakeVisible ( mOuterBoxPtr );

	// Create data collection Label
	mDataCollectionLabelPtr = new Label ( "Collecting\nData..." );
	mDataCollectionLabelPtr->setJustification( eJustificationFlags::centred );
	addAndMakeVisible ( mDataCollectionLabelPtr );

	// Add information text to a LabelGroup
	mLabelGroupPtr = new LabelGroup ( String( "System Info" ) );
	addAndMakeVisible ( mLabelGroupPtr );

	// Create a ViewPort to hold the contents of this page
	mViewPortPtr = new ViewPort ( String( "ViewPort" ) );
	mViewPortPtr->setContent( mLabelGroupPtr );
	addChildComponent ( mViewPortPtr );

	// Set the content of the ViewPort
	mViewPortPtr->setContent( mLabelGroupPtr );

	// Start collecting data
	startDataCollection ();

}


SystemInfoPage::~SystemInfoPage()
{
	if ( mOuterBoxPtr != nullptr )		delete mOuterBoxPtr;
	if ( mViewPortPtr != nullptr )		delete mViewPortPtr;
	if ( mLabelGroupPtr != nullptr )	delete mLabelGroupPtr;
}

bool SystemInfoPage::handleLocalUI_event ( sIncommingUI_Event* uiEvent,
									   	   bool altState, Graphics& g,
										   sSystemSettings* settingsPtr )
{
//	if ( uiEvent->encoderEvent.eventSourceID == eEventSourceID::nerveNetSourceID )
	if ( uiEvent->encoderEvent.eventSourceID == eEventSourceDestinationID::nerveNetNorthSideSourceID )
	{
		sNerveNetEvent* messagePtr = &uiEvent->nerveNetEvent;

		return false;
	} else if  ( uiEvent->encoderEvent.eventSourceID == eEventSourceDestinationID::encoderThreadSourceID )
	{
		sEncoderEvent* eventPtr = &uiEvent->encoderEvent;
		mViewPortPtr->applyVerticalScrolling( eventPtr->encoderCount * 10 );
		return true;
	}




	return false;
}

//==============================================================================
//          SystemInfoPage()
//
// Show main page
//
//  CasualNoises    17/03/2026  First implementation
//==============================================================================
#include <task.h>
void SystemInfoPage::startDataCollection ( )
{

	mLabelGroupPtr->addLabel( new Label ( "-> Device Board" ) );

	uint32_t numTasks = uxTaskGetNumberOfTasks();
	std::string result = " -" + std::to_string ( numTasks ) + " threads:";
	mLabelGroupPtr->addLabel ( result );
/*
	char buf[32];
	TaskStatus_t *taskStatusArray = (TaskStatus_t *) pvPortMalloc( numTasks * sizeof ( TaskStatus_t ) );
	uint32_t totalRuntTime = 0;
	uxTaskGetSystemState( taskStatusArray, numTasks, &totalRuntTime );
	for (uint32_t i = 0; i < numTasks; i++) {
		uint32_t stackHighWaterMark = taskStatusArray[i].usStackHighWaterMark;
		eTaskState state = taskStatusArray[i].eCurrentState;
	    sprintf ( buf, taskStatusArray[i].pcTaskName);
	    Label* ptr = new Label ( buf );
	    ptr->setJustification( eJustificationFlags::right );
	    mLabelGroupPtr->addLabel( ptr );
	}
*/

	TaskStatus_t *taskStatusArray = (TaskStatus_t *) pvPortMalloc( numTasks * sizeof ( TaskStatus_t ) );
	uxTaskGetSystemState( taskStatusArray, numTasks, nullptr );
	for (uint32_t i = 0; i < numTasks; i++)
	{
		const char* name = taskStatusArray[i].pcTaskName;
		mLabelGroupPtr->addLabel( name, eJustificationFlags::right );
		uint32_t stackHighWaterMark = taskStatusArray[i].usStackHighWaterMark;
		eTaskState state = taskStatusArray[i].eCurrentState;
		std::string stateName;
		switch ( state )
		{
		case eTaskState::eBlocked:
			stateName = "Blocked";
			break;
		case eTaskState::eDeleted:
			stateName = "Deleted";
			break;
		case eTaskState::eInvalid:
			stateName = "Invalid";
			break;
		case eTaskState::eReady:
			stateName = "Ready";
			break;
		case eTaskState::eRunning:
			stateName = "Running";
			break;
		case eTaskState::eSuspended:
			stateName = "Suspended";
			break;
		}
		std::string info = std::to_string ( stackHighWaterMark ) + " / " + stateName + " ";
		mLabelGroupPtr->addLabel( info, eJustificationFlags::right );
	}

	mViewPortPtr->setVisible ( true );
	mDataCollectionLabelPtr->setVisible ( false );

}

//==============================================================================
//          SystemInfoPage()
//
// Show main page
//
//  CasualNoises    17/03/2026  First implementation
//==============================================================================
void SystemInfoPage::paint ( Graphics& g )
{

	// Clear the screen
	g.fillAll ();

}

//==============================================================================
//          resized()
//
// 	Set size and location of all components in the page
//
//  CasualNoises    17/03/2026  First implementation
//==============================================================================
void SystemInfoPage::resized()
{

    // Box around this page
    Rectangle<int32_t> rect = getGlobalBounds ();
    mOuterBoxPtr->setBounds ( rect );

    // Are for the system information
    rect.reduce( 2, 2 );
    mDataCollectionLabelPtr->setBounds ( rect );
    mViewPortPtr->setBounds ( rect );

}



} // namespace CasualNoises

