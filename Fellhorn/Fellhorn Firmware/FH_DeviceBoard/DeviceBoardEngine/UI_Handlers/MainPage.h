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

namespace CasualNoises
{
class Box;
class IndexBox;
class ComboBox;
class Label;
class ProgressBar;
}

namespace DeviceBoard
{

using namespace CasualNoises;

// Struct used to save/load main page state
typedef struct
{
	uint32_t	comboBoxSelection;
} sMainPageState;

// Struct used to save/load main page state
typedef struct
{
	uint32_t	comboBoxSelection;
} sCalibartionPageState;

//==============================================================================
//          MainPage
//==============================================================================

class MainPage : public RootPage
{
public:

	 MainPage() = delete;
	 MainPage( SSD1309_Driver* m_oledDriverPtr,
			   QueueHandle_t driverQueueHandle,
			   PageManager* pageManagerPtr,
			   void* paramsPtr );
	~MainPage() override;

	void paint(Graphics& g) override;
	void resized() override;

	void updateLEDs () override;

	void loadContext() override;
	void saveContext() override;

private:

	Box*			mOuterBoxPtr		{ nullptr };
	ComboBox*		mComboBoxPtr		{ nullptr };

	sMainPageState	mPreviousSavedContext;

	void 			onComboBoxChange() noexcept;

};

//==============================================================================
//          CalibrationPage
//==============================================================================

class CalibrationPage : public RootPage
{
public:

	 CalibrationPage() = delete;
	 CalibrationPage( SSD1309_Driver* m_oledDriverPtr,
			 	 	  QueueHandle_t driverQueueHandle,
					  PageManager* pageManagerPtr,
					  void* paramsPtr );
	~CalibrationPage () override;

	void onComboBoxChange();

	void paint(Graphics& g) override;
	void resized() override;

	void updateLEDs () override;

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
	ComboBox*		mComboBoxPtr			{ nullptr };

	sMainPageState	mPreviousSavedContext;

};

//==============================================================================
//          PotentiometerCalibrationPage
//==============================================================================

enum class ePotentiometerCalibrationPageState
{
	AwaitingMins,
	ProcessingMins,
	AwaitingMaxs,
	ProcessingMaxs,
	Completion,
};

class PotentiometerCalibrationPage : public RootPage
{
public:

	 PotentiometerCalibrationPage () = delete;
	 PotentiometerCalibrationPage ( SSD1309_Driver* m_oledDriverPtr,
			 	 	 QueueHandle_t driverQueueHandle,
					 PageManager* pageManagerPtr,
					 void* paramsPtr );
	~PotentiometerCalibrationPage () override;

	void paint ( Graphics& g ) override;
	void resized() override;

	void updateLEDs () override;

	void loadContext () override;
	void saveContext () override;

protected:
	bool handleLocalUI_event( sIncommingUI_Event* uiEvent,
							  bool altState,
							  Graphics& g,
							  sSystemSettings* settingsPtr,
							  bool altSwitchState ) override;

private:

	IndexBox*		mOuterBoxPtr			{ nullptr };
	Label*			mAwaitingMinLabelPtr	{ nullptr };
	Label*			mAwaitingMaxLabelPtr	{ nullptr };
	Label*			mCompletionLabelPtr		{ nullptr };
	Label*			mContLabelPtr			{ nullptr };

	ProgressBar*	mProgressBarPtr			{ nullptr };

	float			mPotValues[NO_OF_ADC_MULTIPLEXERS][NO_OF_ADC_MULTI_CHANNELS];
	bool			mPotValueMask[NO_OF_ADC_MULTIPLEXERS][NO_OF_ADC_MULTI_CHANNELS];

	sCalibrationData mCalibrationValues;

	ePotentiometerCalibrationPageState	mState		{ ePotentiometerCalibrationPageState::AwaitingMins };

	void 			processADC_Event ( sIncommingUI_Event* uiEvent );

	bool			allPotsAtmin () const noexcept;
	bool			allPotsAtmax () const noexcept;

	void			saveCalibrationValues () noexcept;

};

} // namespace DeviceBoard
