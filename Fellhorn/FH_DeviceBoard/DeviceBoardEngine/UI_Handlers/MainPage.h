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
class IndexBox;
class ComboBox;
class Label;
class ProgressBar;

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

	Box*			mOuterBoxPtr		{ nullptr };
	ComboBox*		mComboBoxPtr		{ nullptr };

};

//==============================================================================
//          CalibrationPage
//==============================================================================

enum class eCalibrationPageState
{
	AwaitingMins,
	ProcessingMins,
	AwaitingMaxs,
	ProcessingMaxs,
	Completion,
};

typedef struct
{
	uint32_t minValues[NO_OF_ADC_MULTIPLEXERS][NO_OF_ADC_MULTI_CHANNELS];
	uint32_t maxValues[NO_OF_ADC_MULTIPLEXERS][NO_OF_ADC_MULTI_CHANNELS];
	bool     maskFlags[NO_OF_ADC_MULTIPLEXERS][NO_OF_ADC_MULTI_CHANNELS];
} sCalibrationData;

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

protected:
	bool handleLocalUI_event(sIncommingUI_Event* uiEvent, bool altState, Graphics& g) override;

private:

	IndexBox*		mOuterBoxPtr			{ nullptr };
	Label*			mAwaitingMinLabelPtr	{ nullptr };
	Label*			mAwaitingMaxLabelPtr	{ nullptr };
	Label*			mCompletionLabelPtr		{ nullptr };
	Label*			mContLabelPtr			{ nullptr };

	ProgressBar*	mProgressBarPtr			{ nullptr };

	uint32_t		mStartTicks				{ 0 };

	float			mPotValues[NO_OF_ADC_MULTIPLEXERS][NO_OF_ADC_MULTI_CHANNELS];
	bool			mPotValueMask[NO_OF_ADC_MULTIPLEXERS][NO_OF_ADC_MULTI_CHANNELS];

	sCalibrationData mCalibrationValues;

	eCalibrationPageState	mState		{ eCalibrationPageState::AwaitingMins };

	void 			processADC_Event ( sIncommingUI_Event* uiEvent );

	bool			allPotsAtmin () const noexcept;
	bool			allPotsAtmax () const noexcept;

	void			saveCalibrationValues () noexcept;

};

} // namespace CasualNoises
