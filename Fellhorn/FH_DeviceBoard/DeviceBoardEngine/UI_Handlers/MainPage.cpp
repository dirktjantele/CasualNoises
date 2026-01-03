/*
  ==============================================================================

    MainPage.cpp
    Created: 24/12/2025

	Handle main page UI interaction, also contains the calibration page

    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "MainPage.h"

#include "PageManager.h"

#include <Core/Text/String.h>
#include <GUI/GUI_Basics/Components/Box.h>
#include <GUI/GUI_Basics/Components/ComboBox.h>
#include <Graphics/Geometry/Rectangle.h>
#include <GUI/GUI_Basics/UI_Definitions.h>

namespace CasualNoises
{

/*---------------------------- MainPage ----------------------------*/

//==============================================================================
//          MainPage() & ~MainPage()
//
// Main page initializer
//
//  CasualNoises    24/12/2025  First implementation
//==============================================================================
MainPage::MainPage(SSD1309_Driver* oledDriverPt,
				   TLV_Driver* TLV_DriverPtr,
				   PageManager* pageManagerPtr) :
		RootPage(oledDriverPt, TLV_DriverPtr, pageManagerPtr)
{

	// Create a border component
	mBoxPtr = new Box(String((char*)"Border"));
	addAndMakeVisible(mBoxPtr);

	// Build a ComboBox
	static String names[]
	{
	    String((char*)"New Performance"),
		String((char*)"Calibration"),
	};
	mComboBoxPtr = new ComboBox(String((char*)"ComboBox"));
	uint32_t itemNo = 0;
	for (auto name : names)
	{
		mComboBoxPtr->addItem(&name, ++itemNo);
	}
	mComboBoxPtr->onChange = [this]{ onComboBoxChange(); };
	addAndMakeVisible(mComboBoxPtr);

}

MainPage::~MainPage()
{
	if (mComboBoxPtr != nullptr)	delete mComboBoxPtr;
	if (mBoxPtr 	 != nullptr)	delete mBoxPtr;
}

//==============================================================================
//          onComboBoxChange()
//
// Handle ComboBox on change events
//
//  CasualNoises    03/01/2026  First implementation
//==============================================================================
void MainPage::onComboBoxChange()
{
	uint32_t id = mComboBoxPtr->getSelectedId();
	switch (id)
	{
	case 1:								// New performance
		break;
	case 2:								// Calibration
		mPageManagerPtr->createNewPage(ePageId::calibrationPage);
		break;
	default:
		CN_ReportFault(eErrorCodes::runtimeError);
	}
}

//==============================================================================
//          paint()
//
// Show main page
//
//  CasualNoises    26/12/2025  First implementation
//==============================================================================
void MainPage::paint(Graphics& g)
{
	g.fillAll();
}

//==============================================================================
//          resized()
//
// 	Set size and location of all components in the page
//
//  CasualNoises    26/12/2025  First implementation
//==============================================================================
void MainPage::resized()
{

    Rectangle<int> rect = getBounds();
    UNUSED(rect);

    mBoxPtr->setBounds(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());

    rect.reduce(2, 2);
    mComboBoxPtr->setBounds(rect);

}

//==============================================================================
//          loadContext()
//
// 	Load context from flash
//
//  CasualNoises    03/01/2026  First implementation
//==============================================================================
void MainPage::loadContext()
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
void MainPage::saveContext()
{
	// Nothing to save here
}

/*---------------------------- CalibrationPage ----------------------------*/

//==============================================================================
//          CalibrationPage() & ~CalibrationPage()
//
// Main page initializer
//
//  CasualNoises    04/01/2026  First implementation
//==============================================================================
CalibrationPage::CalibrationPage(SSD1309_Driver* oledDriverPt,
				   	   	   	    TLV_Driver* TLV_DriverPtr,
								PageManager* pageManagerPtr) :
		RootPage(oledDriverPt, TLV_DriverPtr, pageManagerPtr)
{

	// Create a border component
	mBoxPtr = new Box(String((char*)"Border"));
	addAndMakeVisible(mBoxPtr);

}

CalibrationPage::~CalibrationPage()
{
	if (mBoxPtr 	 != nullptr)	delete mBoxPtr;
}

//==============================================================================
//          paint()
//
// Show main page
//
//  CasualNoises    04/01/2026  First implementation
//==============================================================================
void CalibrationPage::paint(Graphics& g)
{
	g.fillAll();
}

//==============================================================================
//          resized()
//
// 	Set size and location of all components in the page
//
//  CasualNoises    04/01/2026  First implementation
//==============================================================================
void CalibrationPage::resized()
{

    Rectangle<int> rect = getBounds();
    UNUSED(rect);

    mBoxPtr->setBounds(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());

    rect.reduce(2, 2);
//    mComboBoxPtr->setBounds(rect);

}

//==============================================================================
//          loadContext()
//
// 	Load context from flash
//
//  CasualNoises    04/01/2026  First implementation
//==============================================================================
void CalibrationPage::loadContext()
{
	// Nothing to save here
}

//==============================================================================
//          saveContext()
//
// 	Save context to flash
//
//  CasualNoises    04/01/2026  First implementation
//==============================================================================
void CalibrationPage::saveContext()
{
	// Nothing to save here
}

} // namespace CasualNoises
