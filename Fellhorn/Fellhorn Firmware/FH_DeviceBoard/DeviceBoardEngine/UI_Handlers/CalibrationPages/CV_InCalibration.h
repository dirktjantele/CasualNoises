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

#include "SynthEngineMessage.h"

#include "Utilities/TimerBase.h"

namespace CasualNoises
{
class Box;
class Label;
class LevelBar;
}

namespace DeviceBoard
{

using namespace CasualNoises;

//==============================================================================
//          CV_CalibrationPage
//==============================================================================

enum class eCV_CalibrationPhase
{
	openCV_Inputs,
	cv1_min5,
	cv2_min5,
	cv3_min5,
	cv4_min5,
	cv5_min5,
	cv6_min5,
	cv7_min5,
	cv1_plus5,
	cv2_plus5,
	cv3_plus5,
	cv4_plus5,
	cv5_plus5,
	cv6_plus5,
	cv7_plus5,
	confirm,
	exit,
	Count,			// Required to be able to step through the enum using next ()
};

class CV_CalibrationPage : public RootPage, private TimerBase
{
public:

	 CV_CalibrationPage() = delete;
	 CV_CalibrationPage( SSD1309_Driver* m_oledDriverPtr,
			 	 	  	 QueueHandle_t driverQueueHandle,
						 PageManager* pageManagerPtr );
	~CV_CalibrationPage () override;

	void paint(Graphics& g) override;
	void resized() override;

	void updateLEDs () override;

	void loadContext() override { };
	void saveContext() override { };

protected:
	bool handleLocalUI_event( sIncommingUI_Event* uiEvent,
							  bool altState,
							  Graphics& g,
							  sSystemSettings* settingsPtr,
							  bool altSwitchState ) override;

	void onTimer() override;

private:

	Box*					mOuterBoxPtr			{ nullptr };

	Label*					mContLabelPtr			{ nullptr };
	Label*					mMessageLabelPtr		{ nullptr };
	LevelBar*				mLevelBarPtr			{ nullptr };

	eCV_CalibrationPhase	mCurrentPhase			{ eCV_CalibrationPhase::openCV_Inputs };

	tCV_InputCalibrationSettings	mNorthSideSettings;
	tCV_InputCalibrationSettings	mSouthSideSettings;

	void			requestADC_Data	( eNerveNetSourceId target = eNerveNetSourceId::FellhornBothSides );
	bool			processData ( eCV_CalibrationPhase phase, eNerveNetSourceId sourceId, float* sourceDataPtr );

};

} // namespace DeviceBoard
