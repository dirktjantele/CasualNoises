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

	// Add information text to a LabelGroup
	mLabelGroupPtr = new LabelGroup ( String( "System Info" ) );
	addAndMakeVisible ( mLabelGroupPtr );

	// Create a ViewPort to hold the contents of this page
	mViewPortPtr = new ViewPort ( String( "ViewPort" ) );
	mViewPortPtr->setContent( mLabelGroupPtr );
	addAndMakeVisible ( mViewPortPtr );

	// Test code
	Label* text = new Label ( "-> Device Board" );
	mLabelGroupPtr->addLabel( text );
	for ( uint32_t i = 0; i < 10; ++i )
	{
		Label* text = new Label ( "... info" );
		mLabelGroupPtr->addLabel( text );
	}
	uint32_t height = mLabelGroupPtr->getHeight ();
	mLabelGroupPtr->takeOwnership( true );

}

SystemInfoPage::~SystemInfoPage()
{
	if ( mOuterBoxPtr != nullptr )		delete mOuterBoxPtr;
	if ( mLabelGroupPtr != nullptr )	delete mLabelGroupPtr;
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
    Rectangle<int> rect = getGlobalBounds ();
    mOuterBoxPtr->setBounds ( rect );

    // Are for the system information
    rect.reduce( 2, 2 );
    mViewPortPtr->setBounds ( rect );

}



} // namespace CasualNoises

