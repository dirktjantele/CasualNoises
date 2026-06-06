/*
  ==============================================================================

    LoadPerformancePage.h
    Created: 05/06/2026

    Handles the load performance page

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include <vector>

#include "../RootPage.h"

namespace CasualNoises
{
class Box;
class ComboBox;
}

namespace DeviceBoard
{

using namespace CasualNoises;

//==============================================================================
//          LoadPerformancePage
//==============================================================================

class LoadPerformancePage : public RootPage
{
public:

	 LoadPerformancePage() = delete;
	 LoadPerformancePage( SSD1309_Driver* m_oledDriverPtr,
			 	 	 QueueHandle_t driverQueueHandle,
					 PageManager* pageManagerPtr);
	~LoadPerformancePage() override;

	virtual bool handleLocalUI_event ( sIncommingUI_Event* uiEvent,
									   bool altState, Graphics& g,
									   sSystemSettings* settingsPtr,
									   bool altSwitchState ) override;

	void paint(Graphics& g) override;
	void resized() override;

	void updateLEDs () override;

	void loadContext() override {};
	void saveContext() override {};

private:

	Box*			mOuterBoxPtr		{ nullptr };
	ComboBox*		mComboBoxPtr		{ nullptr };

	std::vector<String>	mPresetNameList;

	void 			buildPresetNameList ();
	void 			onComboBoxChange () noexcept;

};

} // namespace DeviceBoard
