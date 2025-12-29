/*
  ==============================================================================

    MainPage.cpp
    Created: 24/12/2025

   Handle main page UI interaction

    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "MainPage.h"

#include "PageManager.h"

#include <GUI/GUI_Basics/Components/Box.h>
#include <GUI/GUI_Basics/Components/ComboBox.h>
#include <Graphics/Geometry/Rectangle.h>

#include "CasualNoises.h"

namespace CasualNoises
{

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
//	m_oledDriverPtr (oledDriverPt),
//	mTLV_DriverPtr (TLV_DriverPtr)
{

	// Create a border component
	mBoxPtr = new Box(String((char*)"Border"));
	addAndMakeVisible(mBoxPtr);

	// Create ComboBox
	mComboBoxPtr = new ComboBox(String((char*)"ComboBox"));
	for (uint32_t i = 1; i <= 10; ++i)
	{
		String no(i);
		String text = (String((char*)"Item ")) + no;
		mComboBoxPtr->addItem(text, i);
	}
//	mComboBoxPtr->addItem((String((char*)"Item 1")), 1);
//	mComboBoxPtr->addItem((String((char*)"Item 2")), 2);
	mComboBoxPtr->setFont(&font_7x10);
	addAndMakeVisible(mComboBoxPtr);

}

MainPage::~MainPage()
{
	if (mComboBoxPtr != nullptr)	delete mComboBoxPtr;
	if (mBoxPtr 	 != nullptr)	delete mBoxPtr;
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

//==============================================================================
//          handleUI_event()
//
// Handle UI events for this page
//
//  CasualNoises    25/12/2025  First implementation
//==============================================================================
void MainPage::handleUI_event(sIncommingUI_Event* uiEvent, bool altState)
{

}

} // namespace CasualNoises
