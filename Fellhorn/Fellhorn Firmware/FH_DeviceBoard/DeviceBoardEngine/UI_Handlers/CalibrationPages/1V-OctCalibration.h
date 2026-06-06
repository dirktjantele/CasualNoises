/*
  ==============================================================================

    1V-OctCalibration.cpp
    Created: 13/04/2026

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
class NoteSelection;
class LevelBar;
}

namespace DeviceBoard
{

using namespace CasualNoises;

class _1V_OctCalibrationPage : public RootPage, private TimerBase
{
public:

	 _1V_OctCalibrationPage() = delete;
	 _1V_OctCalibrationPage( SSD1309_Driver* m_oledDriverPtr,
			 	 	  	 	 QueueHandle_t driverQueueHandle,
							 PageManager* pageManagerPtr );
	~_1V_OctCalibrationPage () override;

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

	void onTimer() noexcept override;

private:

	Box*					mOuterBoxPtr			{ nullptr };

	Label*					mContLabelPtr			{ nullptr };
	Label*					mMessageLabelPtr		{ nullptr };
	LevelBar*				mLevelBarPtr			{ nullptr };

	NoteSelection*			mNoteSelectionPtr		{ nullptr };

	float					mCurrentNorthSideLevel	{ 0.0f };
	float					mCurrentSouthSideLevel	{ 0.0f };

	static constexpr uint32_t cNoOfNotes = ( 10 * 7 ) + 1 ;

	float					mNorthSideCalbrationLevels [ cNoOfNotes ] { 0.0f };
	float					mSouthSideCalbrationLevels [ cNoOfNotes ] { 0.0f };

	float					mMinCalibrationValue	{ 65535.0f };
	float					mMaxCalibrationValue	{ 0.0f };

	t1V_OctCalibrationSettings mNorthSideCalbrationResults;
	t1V_OctCalibrationSettings mSouthSideCalbrationResults;

	float*					mNorthSideCalbrationResultsPtr { nullptr };
	float*					mSouthSideCalbrationResultsPtr { nullptr };

	void			requestADC_Data	( eNerveNetSourceId target = eNerveNetSourceId::FellhornBothSides );
	void			processData ( eNerveNetSourceId sourceId, float* sourceDataPtr );

	void 			onNoteSelectionChange () noexcept;

	void			composeCalibrationTable () noexcept;

};

} // namespace DeviceBoard
