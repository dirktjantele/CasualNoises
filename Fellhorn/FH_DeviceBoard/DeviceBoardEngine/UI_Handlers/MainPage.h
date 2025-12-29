/*
  ==============================================================================

    MainPage.h
    Created: 24/12/2025

    Handle main page UI interaction

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

class MainPage : public RootPage
{
public:

	 MainPage() = delete;
	 MainPage(SSD1309_Driver* m_oledDriverPtr,
			  TLV_Driver* mTLV_DriverPtr,
			  PageManager* pageManagerPtr);
	~MainPage() override;

	void paint(Graphics& g) override;
	void resized() override;

	void saveContext() override;

	void handleUI_event(sIncommingUI_Event* uiEvent, bool altState) override;

private:

	Box*			mBoxPtr				{ nullptr };
	ComboBox*		mComboBoxPtr		{ nullptr };

};

} // namespace CasualNoises
