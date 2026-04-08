/*
  ==============================================================================

    CV_InCalibration.h
    Created: 03/04/2026

    Handle CV calibration

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "../RootPage.h"

namespace CasualNoises
{

class Box;

//==============================================================================
//          CV_CalibrationPage
//==============================================================================

class CV_CalibrationPage : public RootPage
{
public:

	 CV_CalibrationPage() = delete;
	 CV_CalibrationPage( SSD1309_Driver* m_oledDriverPtr,
			 	 	  	 QueueHandle_t driverQueueHandle,
						 PageManager* pageManagerPtr );
	~CV_CalibrationPage () override;

	void paint(Graphics& g) override;
	void resized() override;

	void loadContext() override;
	void saveContext() override;

protected:
	bool handleLocalUI_event( sIncommingUI_Event* uiEvent,
							  bool altState,
							  Graphics& g,
							  sSystemSettings* settingsPtr,
							  bool altSwitchState ) override;

private:

	Box*			mOuterBoxPtr			{ nullptr };

};

} // namespace CasualNoises
