/*
  ==============================================================================

    MainPage.h
    Created: 24/12/2025

    Handle main page UI interaction, also contains the calibration page

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "RootPage.h"

#include "Drivers/OLED/SSD1309_Driver.h"

namespace CasualNoises
{

class Box;
class ComboBox;

//==============================================================================
//          MainPage
//==============================================================================
class MainPage : public RootPage
{
public:

	 MainPage() = delete;
	 MainPage(SSD1309_Driver* m_oledDriverPtr,
			  TLV_Driver* mTLV_DriverPtr,
			  PageManager* pageManagerPtr);
	~MainPage() override;

	void onComboBoxChange();

	void paint(Graphics& g) override;
	void resized() override;

	void loadContext() override;
	void saveContext() override;

private:

	Box*			mBoxPtr				{ nullptr };
	ComboBox*		mComboBoxPtr		{ nullptr };

};

//==============================================================================
//          CalibrationPage
//==============================================================================
class CalibrationPage : public RootPage
{
public:

	 CalibrationPage() = delete;
	 CalibrationPage(SSD1309_Driver* m_oledDriverPtr,
			  TLV_Driver* mTLV_DriverPtr,
			  PageManager* pageManagerPtr);
	~CalibrationPage() override;

	void paint(Graphics& g) override;
	void resized() override;

	void loadContext() override;
	void saveContext() override;

private:

	Box*			mBoxPtr				{ nullptr };

};

} // namespace CasualNoises
