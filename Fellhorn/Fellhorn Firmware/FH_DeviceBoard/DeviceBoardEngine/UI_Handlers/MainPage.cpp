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
#include "YellowPages.h"
#include "../../DeviceBoardEngine/UI_Definitions.h"

#include "SynthEngineMessage.h"

#include "Core/Text/String.h"
#include "GUI/GUI_Basics/Components/Box.h"
#include "GUI/GUI_Basics/Components/ComboBox.h"
#include "GUI/GUI_Basics/Components/Label.h"
#include "GUI/GUI_Basics/Components/ProgressBar.h"
#include "Graphics/Geometry/Rectangle.h"

#include "NerveNet/NerveNetMasterThread.h"

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
MainPage::MainPage (
		SSD1309_Driver* oledDriverPt,
		QueueHandle_t driverQueueHandle,
		PageManager* pageManagerPtr ) :
	RootPage ( oledDriverPt, driverQueueHandle, pageManagerPtr )
{

	// Create a border component
	mOuterBoxPtr = new Box( String( (char*) "Border" ) );
	addAndMakeVisible ( mOuterBoxPtr );

	// Build a ComboBox
	static String names[]
	{
	    String ( "Load Performance" ),
	    String ( "New Performance" ),
		String ( "Calibration" ),
		String ( "System Info" ),
	};
	mComboBoxPtr = new ComboBox( String( (char*) "ComboBox" ) );
	uint32_t itemNo = 0;
	for (auto name : names)
	{
		mComboBoxPtr->addItem(&name, ++itemNo);
	}
	mComboBoxPtr->onChange = [this] { onComboBoxChange(); };
	addAndMakeVisible ( mComboBoxPtr );

}

MainPage::~MainPage()
{
	if ( mComboBoxPtr != nullptr )	delete mComboBoxPtr;
	if ( mOuterBoxPtr != nullptr )	delete mOuterBoxPtr;
}

//==============================================================================
//          onComboBoxChange()
//
// Handle ComboBox on change events
//
//  CasualNoises    03/01/2026  First implementation
//==============================================================================
void MainPage::onComboBoxChange () noexcept
{
	uint32_t id = mComboBoxPtr->getSelectedId ();
	switch (id)
	{
	case 1:								// Load performance
		break;
	case 2:								// New performance
		break;
	case 3:								// Calibration
		mPageManagerPtr->createNewPage ( ePageId::calibrationPage );
		break;
	case 4:								// Sytem Info
		mPageManagerPtr->createNewPage ( ePageId::systemInfoPage );
		break;
	default:
		CN_ReportFault ( eErrorCodes::runtimeError );
	}
}

//==============================================================================
//          paint()
//
//  CasualNoises    26/12/2025  First implementation
//==============================================================================
void MainPage::paint ( Graphics& g )
{

	// Clear the screen
	g.fillAll ();

}

//==============================================================================
//          updateLEDs()
//
//  CasualNoises    13/04/2026  First implementation
//==============================================================================
void MainPage::updateLEDs ()
{
	dimSwitchLeds ();
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

    // Box around this page
    Rectangle<int32_t> rect = getGlobalBounds();
    mOuterBoxPtr->setBounds ( rect );

    // Main menu
    rect.reduce ( 2, 2 );
    mComboBoxPtr->setBounds ( rect );

}

//==============================================================================
//          loadContext()
//
// 	Load context from flash
//
//  CasualNoises    08/01/2026  First implementation
//==============================================================================
void MainPage::loadContext()
{
	sMainPageState state;
	state.comboBoxSelection = 0;
	uint32_t size = readTLV_TagBytes (
			mTLV_DriverQueueHandle,
			(uint32_t)eTLV_Tag::UI_MainPageState,
			sizeof ( sMainPageState ),
			(uint32_t*) &state );
	if ( size > 0 )
	{
		mComboBoxPtr->setFocus ( state.comboBoxSelection );
	}
}

//==============================================================================
//          saveContext()
//
// 	Save context to flash when changed
//
//  CasualNoises    08/01/2026  First implementation
//==============================================================================
void MainPage::saveContext()
{
	sMainPageState state;
	state.comboBoxSelection = mComboBoxPtr->getSelectedId ();
	uint32_t diffCnt = memcmp( &mPreviousSavedContext, &state, sizeof ( sMainPageState ) );
	if ( diffCnt != 0 )
	{
		updateTLV_Bytes (mTLV_DriverQueueHandle,
				(uint32_t)eTLV_Tag::UI_MainPageState, sizeof ( sMainPageState ), (uint32_t*) &state );
		memcpy( &mPreviousSavedContext, &state, sizeof ( sMainPageState ) );
	}
}

/*---------------------------- CalibrationPage ----------------------------*/

//==============================================================================
//          CalibrationPage() & ~CalibrationPage()
//
// Main page initializer
//
//  CasualNoises    01/02/2026  First implementation
//==============================================================================
CalibrationPage::CalibrationPage(
		SSD1309_Driver* oledDriverPt,
		QueueHandle_t driverQueueHandle,
		PageManager* pageManagerPtr) :
	RootPage ( oledDriverPt, driverQueueHandle, pageManagerPtr )
{

	// Create a border component
	mOuterBoxPtr = new Box( String( (char*) "Border" ) );
	addAndMakeVisible ( mOuterBoxPtr );

	// Build a ComboBox
	static String names[]
	{
	    String( (char*) "Pots & Sliders" ),
		String( (char*) "Control Volts." ),
		String( (char*) "1V/Oct" ),
	};
	mComboBoxPtr = new ComboBox( String( (char*) "ComboBox" ) );
	uint32_t itemNo = 0;
	for (auto name : names)
	{
		mComboBoxPtr->addItem(&name, ++itemNo);
	}
	mComboBoxPtr->onChange = [this] { onComboBoxChange(); };
	addAndMakeVisible ( mComboBoxPtr );

}

CalibrationPage::~CalibrationPage()
{
	if ( mOuterBoxPtr 	 			!= nullptr )		delete mOuterBoxPtr;
	if ( mComboBoxPtr 	 			!= nullptr )		delete mComboBoxPtr;
}

//==============================================================================
//          onComboBoxChange()
//
// Handle ComboBox on change events
//
//  CasualNoises    03/01/2026  First implementation
//==============================================================================
void CalibrationPage::onComboBoxChange ()
{
	uint32_t id = mComboBoxPtr->getSelectedId ();
	switch (id)
	{
	case 1:								// Potentiometers & sliders
		mPageManagerPtr->createNewPage ( ePageId::potCalibrationPage );
		break;
	case 2:								// Control Voltages
		mPageManagerPtr->createNewPage ( ePageId::CV_CalibrationPage );
		break;
	case 3:								// 1V/Oct
		mPageManagerPtr->createNewPage ( ePageId::_1V_OctCalibrationPage );
		break;
	default:
		CN_ReportFault ( eErrorCodes::runtimeError );
	}
}

//==============================================================================
//          paint()
//
// Show main page
//
//  CasualNoises    01/02/2026  First implementation
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
//  CasualNoises    01/02/2026  First implementation
//==============================================================================
void CalibrationPage::resized()
{

    // Box around this page
    Rectangle<int32_t> rect = getGlobalBounds();
    mOuterBoxPtr->setBounds ( rect );

    // Main menu
    rect.reduce ( 2, 2 );
    mComboBoxPtr->setBounds ( rect );

}

//==============================================================================
//          updateLEDs()
//
//  CasualNoises    13/04/2026  First implementation
//==============================================================================
void CalibrationPage::updateLEDs ()
{
	dimSwitchLeds ();
}

//==============================================================================
//          handleLocalUI_event()
//
// 	Handle page specific UI events
//
//  CasualNoises    04/01/2026  First implementation
//  CasualNoises    08/04/2026  altSwitchState added
//==============================================================================
bool CalibrationPage::handleLocalUI_event (
		sIncommingUI_Event* uiEvent,
		bool altState,
		Graphics& g,
		sSystemSettings* settingsPtr,
		bool altSwitchState )
{
	return false;
}

//==============================================================================
//          loadContext()
//
// 	Load context from flash
//
//  CasualNoises    01/02/2026  First implementation
//==============================================================================
void CalibrationPage::loadContext()
{
	sCalibartionPageState state;
	state.comboBoxSelection = 0;
	uint32_t size = readTLV_TagBytes (
			mTLV_DriverQueueHandle,
			(uint32_t)eTLV_Tag::UI_CalibrationPageState,
			sizeof ( sMainPageState ),
			(uint32_t*) &state );
	if ( size > 0 )
	{
		mComboBoxPtr->setFocus ( state.comboBoxSelection );
	}
}

//==============================================================================
//          saveContext()
//
// 	Save context to flash
//
//  CasualNoises    01/02/2026  First implementation
//==============================================================================
void CalibrationPage::saveContext()
{
	sCalibartionPageState state;
	state.comboBoxSelection = mComboBoxPtr->getSelectedId ();
	uint32_t diffCnt = memcmp( &mPreviousSavedContext, &state, sizeof ( sCalibartionPageState ) );
	if ( diffCnt != 0 )
	{
		updateTLV_Bytes (mTLV_DriverQueueHandle,
				(uint32_t)eTLV_Tag::UI_CalibrationPageState, sizeof ( sCalibartionPageState ), (uint32_t*) &state );
		memcpy( &mPreviousSavedContext, &state, sizeof ( sMainPageState ) );
	}
}

/*---------------------------- PotentiometerCalibrationPage ----------------------------*/

//==============================================================================
//          PotentiometerCalibrationPage() & ~PotentiometerCalibrationPage()
//
// Main page initializer
//
//  CasualNoises    04/01/2026  First implementation
//==============================================================================
PotentiometerCalibrationPage::PotentiometerCalibrationPage (
		SSD1309_Driver* oledDriverPt,
		QueueHandle_t driverQueueHandle,
		PageManager* pageManagerPtr) :
	RootPage ( oledDriverPt, driverQueueHandle, pageManagerPtr )
{

	// Create border components
	mOuterBoxPtr = new IndexBox ( "Outer border", sizeof (ePotentiometerCalibrationPageState) + 1, 0 );
	addAndMakeVisible ( mOuterBoxPtr );

	// Create labels
	mAwaitingMinLabelPtr = new Label ( "Calibration\nSet pots to min" );
	addAndMakeVisible ( mAwaitingMinLabelPtr );
	mAwaitingMaxLabelPtr = new Label ( "Calibration\nSet pots to max" );
	addChildComponent ( mAwaitingMaxLabelPtr );
	mCompletionLabelPtr  = new Label ( "Done...\nPress 'EXIT'" );
	addChildComponent ( mCompletionLabelPtr );

	// Create progress bar
	mProgressBarPtr = new ProgressBar ( "ProgressBar" );
	addChildComponent ( mProgressBarPtr );

	// Continuation instructions
	mContLabelPtr = new Label ( "'>' to proceed" );
	addChildComponent ( mContLabelPtr );

	// Set all potentiometer values to 50%
	for (uint32_t i = 0; i < NO_OF_ADC_MULTIPLEXERS; ++i)
	{
		for (uint32_t j = 0; j < NO_OF_ADC_MULTI_CHANNELS; ++j)
		{
			mPotValues[i][j] 	= 50.0f;
			mPotValueMask[i][j] = true;
		}
	}
	mPotValueMask[0][(uint32_t)eMultiplexerChannel_0::P_NC_1] = false;
	mPotValueMask[0][(uint32_t)eMultiplexerChannel_0::P_NC_2] = false;
	mPotValueMask[0][(uint32_t)eMultiplexerChannel_0::P_NC_3] = false;
	mPotValueMask[0][(uint32_t)eMultiplexerChannel_0::P_NC_4] = false;

	// Current state awaiting minimal pot values
	mState = ePotentiometerCalibrationPageState::AwaitingMins;

}

PotentiometerCalibrationPage::~PotentiometerCalibrationPage()
{
	if ( mOuterBoxPtr 	 		!= nullptr )		delete mOuterBoxPtr;
	if ( mAwaitingMinLabelPtr 	!= nullptr )		delete mAwaitingMinLabelPtr;
	if ( mAwaitingMaxLabelPtr 	!= nullptr )		delete mAwaitingMaxLabelPtr;
	if ( mCompletionLabelPtr 	!= nullptr )		delete mCompletionLabelPtr;
	if ( mProgressBarPtr		!= nullptr )		delete mProgressBarPtr;
	if ( mContLabelPtr			!= nullptr )		delete mContLabelPtr;
}

//==============================================================================
//          paint()
//
// Show main page
//
//  CasualNoises    04/01/2026  First implementation
//==============================================================================
void PotentiometerCalibrationPage::paint(Graphics& g)
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
void PotentiometerCalibrationPage::resized()
{

    // Box around this page
    Rectangle<int32_t> rect = getGlobalBounds();
    mOuterBoxPtr->setBounds ( rect );

    // Place labels
    rect.reduce (2, 2);
    mAwaitingMinLabelPtr->setBounds ( rect );
    mAwaitingMinLabelPtr->setJustification ( eJustificationFlags::centred );
    mAwaitingMaxLabelPtr->setBounds ( rect );
    mAwaitingMaxLabelPtr->setJustification ( eJustificationFlags::centred );
    mCompletionLabelPtr->setBounds ( rect );
    mCompletionLabelPtr->setJustification ( eJustificationFlags::centred );

    // Progress bar
    Rectangle<int32_t> tmp;
    mLocalBounds = getGlobalBounds();
    mAwaitingMinLabelPtr->setBounds ( rect );
    tmp = mLocalBounds.removeFromTop ( 10 );
    rect = mLocalBounds.removeFromTop ( 25 );
    tmp = rect.removeFromLeft ( 10 );
    tmp = rect.removeFromRight ( 10 );
    mProgressBarPtr->setBounds( rect );

    // Set text
    mContLabelPtr->setBounds ( mLocalBounds );
    mContLabelPtr->setJustification ( eJustificationFlags::centred );

}

//==============================================================================
//          updateLEDs()
//
//  CasualNoises    13/04/2026  First implementation
//==============================================================================
void PotentiometerCalibrationPage::updateLEDs ()
{
	dimSwitchLeds ();
}

//==============================================================================
//          processADC_Event()
//
// 	Process any incoming ADC events by updating the progress bar
//
//  CasualNoises    08/01/2026  First implementation
//==============================================================================
void PotentiometerCalibrationPage::processADC_Event ( sIncommingUI_Event* uiEvent )
{

	uint32_t value = uiEvent->multiplexed_ADC_Event.value;
	if ( uiEvent->multiplexed_ADC_Event.eventSourceID == eEventSourceID::multiplexerADC_ThreadSourceID )
	{

		// Get device name
		String text;
		eLogicalPotId logicalPotId = uiEvent->multiplexed_ADC_Event.logicalPotId;
		switch ( logicalPotId )
		{
		case eLogicalPotId::pot_1:
			text = String( "P #1" );
			break;
		case eLogicalPotId::pot_2:
			text = String( "P #2" );
			break;
		case eLogicalPotId::pot_3:
			text = String( "P #3" );
			break;
		case eLogicalPotId::pot_4:
			text = String( "P #4" );
			break;
		case eLogicalPotId::slider_1:
			text = String( "F #1" );
			break;
		case eLogicalPotId::slider_2:
			text = String( "F #2" );
			break;
		case eLogicalPotId::slider_3:
			text = String( "F #3" );
			break;
		case eLogicalPotId::slider_4:
			text = String( "F #4" );
			break;
		case eLogicalPotId::slider_5:
			text = String( "F #5" );
			break;
		case eLogicalPotId::slider_6:
			text = String( "F #6" );
			break;
		case eLogicalPotId::slider_7:
			text = String( "F #7" );
			break;
		case eLogicalPotId::slider_8:
			text = String( "F #8" );
			break;
		default:
			text = String( "<< unknown >>" );
			break;
		}

		// Sliders are mounted upside down!!! Compensate for this...					ToDo find a better solution
		if ( uiEvent->multiplexed_ADC_Event.multiplexerNo == 1 )
			value = 65535 - value;

		// Update potentiometer values
		float fvalue = ( (float)(  value  * 100 )  / 65535 );
		mPotValues[uiEvent->multiplexed_ADC_Event.multiplexerNo]
 				  [uiEvent->multiplexed_ADC_Event.multiplexerChannelNo] = fvalue;

		// Update progress bar
		char buf[10];
		sprintf ( buf, "%.1f", fvalue );
		String svalue = String ( buf );
		String pbText = text + String ( (char*)" ") + svalue + String ( (char*)"%" );
		mProgressBarPtr->setText ( pbText );
		mProgressBarPtr->setValue ( fvalue );

	}
}

//==============================================================================
//          handleLocalUI_event()
//
// 	Handle page specific UI events
//
//  CasualNoises    04/01/2026  First implementation
//  CasualNoises    08/04/2026  altSwitchState added
//==============================================================================
bool PotentiometerCalibrationPage::handleLocalUI_event (
		sIncommingUI_Event* uiEvent,
		bool altState,
		Graphics& g,
		sSystemSettings* settingsPtr,
		bool altSwitchState)
{
	switch ( mState )
	{
	case ePotentiometerCalibrationPageState::AwaitingMins:
	{
		mOuterBoxPtr->setCurrentTab( (uint8_t) ePotentiometerCalibrationPageState::AwaitingMins );
		if ( uiEvent->multiplexed_ADC_Event.eventSourceID == eEventSourceID::multiplexerADC_ThreadSourceID )
		{
			dimSwitchLeds ();
			mAwaitingMinLabelPtr->setVisible ( false );
			mProgressBarPtr->setVisible ( true );
			mState = ePotentiometerCalibrationPageState::ProcessingMins;
			return true;
		}
	}
	break;
	case ePotentiometerCalibrationPageState::ProcessingMins:
	{
		mOuterBoxPtr->setCurrentTab( (uint8_t) ePotentiometerCalibrationPageState::ProcessingMins );
		if ( uiEvent->multiplexed_ADC_Event.eventSourceID == eEventSourceID::multiplexerADC_ThreadSourceID )
		{
			processADC_Event ( uiEvent );
			mCalibrationValues.minValues [uiEvent->multiplexed_ADC_Event.multiplexerNo]
										 [uiEvent->multiplexed_ADC_Event.multiplexerChannelNo] = uiEvent->multiplexed_ADC_Event.value;
			if ( allPotsAtmin () )
			{
				mContLabelPtr->setVisible ( true );
				setSwitchLed ( eLED_BitNums::RIGTH_ARROW_SWITCH );
				mState = ePotentiometerCalibrationPageState::AwaitingMaxs;
			}
		}
		return true;
	}
	break;
	case ePotentiometerCalibrationPageState::AwaitingMaxs:
	{
		mOuterBoxPtr->setCurrentTab( (uint8_t) ePotentiometerCalibrationPageState::AwaitingMaxs );
		if (( uiEvent->encoderEvent.eventSourceID == eEventSourceID::encoderThreadSourceID ) &&
			( uiEvent->encoderEvent.encoderNo == (uint16_t)eSwitchNums::RIGTH_ARROW_SWITCH ) )
		{
			dimSwitchLeds ();
			mAwaitingMaxLabelPtr->setVisible ( true );
			mProgressBarPtr->setVisible ( false );
			mContLabelPtr->setVisible ( false );
			mState = ePotentiometerCalibrationPageState::ProcessingMaxs;
			return true;
		}
	}
	break;
	case ePotentiometerCalibrationPageState::ProcessingMaxs:
	{
		mOuterBoxPtr->setCurrentTab( (uint8_t) ePotentiometerCalibrationPageState::ProcessingMaxs );
		if ( uiEvent->multiplexed_ADC_Event.eventSourceID == eEventSourceID::multiplexerADC_ThreadSourceID )
		{
			mAwaitingMaxLabelPtr->setVisible ( false );
			mProgressBarPtr->setVisible ( true );
			processADC_Event ( uiEvent );
			mCalibrationValues.maxValues [uiEvent->multiplexed_ADC_Event.multiplexerNo]
										 [uiEvent->multiplexed_ADC_Event.multiplexerChannelNo] = uiEvent->multiplexed_ADC_Event.value;
			if ( allPotsAtmax () )
			{
				setSwitchLed ( eLED_BitNums::RIGTH_ARROW_SWITCH );
				mContLabelPtr->setVisible ( true );
				mState = ePotentiometerCalibrationPageState::Completion;
				saveCalibrationValues ();
			}
		}
		return true;
	}
	break;
	case ePotentiometerCalibrationPageState::Completion:
	{
		mOuterBoxPtr->setCurrentTab( (uint8_t) ePotentiometerCalibrationPageState::Completion );
		if (( uiEvent->encoderEvent.eventSourceID == eEventSourceID::encoderThreadSourceID ) &&
			( uiEvent->encoderEvent.encoderNo == (uint16_t)eSwitchNums::RIGTH_ARROW_SWITCH ) )
		{
			dimSwitchLeds ();
			mProgressBarPtr->setVisible ( false );
			mContLabelPtr->setVisible ( false );
			mCompletionLabelPtr->setVisible ( true );
			return true;
		}
	}
	break;
	default:
		CN_ReportFault(eErrorCodes::runtimeError);
	}

	return false;
}

//==============================================================================
//          allPotsAtmin()
//
// 	Verify all pots in minimum position
//
//  CasualNoises    04/01/2026  First implementation
//==============================================================================
bool PotentiometerCalibrationPage::allPotsAtmin () const noexcept
{
	for (uint32_t i = 0; i < NO_OF_ADC_MULTIPLEXERS; ++i)
	{
		for ( uint32_t j = 0; j < NO_OF_ADC_MULTI_CHANNELS; ++j )
		{
			if ( (mPotValues[i][j] > 1.0f ) && mPotValueMask[i][j] )
				return false;
		}
	}
	return true;
}

//==============================================================================
//          allPotsAtmax()
//
// 	Verify all pots in maximum position
//
//  CasualNoises    04/01/2026  First implementation
//==============================================================================
bool PotentiometerCalibrationPage::allPotsAtmax () const noexcept
{
	for (uint32_t i = 0; i < NO_OF_ADC_MULTIPLEXERS; ++i)
	{
		for ( uint32_t j = 0; j < NO_OF_ADC_MULTI_CHANNELS; ++j )
		{
			if ( (mPotValues[i][j] < 99.0f ) && mPotValueMask[i][j] )
				return false;
		}
	}
	return true;
}

//==============================================================================
//          saveCalibrationValues()
//
// 	Save new calibration values to flash
//
//  CasualNoises    09/01/2026  First implementation
//==============================================================================
void PotentiometerCalibrationPage::saveCalibrationValues () noexcept
{
	mCalibrationValues.reveserved_1 = '>';			// Just an eye catcher
	mCalibrationValues.isCalibrated = 't';
	mCalibrationValues.reveserved_2 = 0x2b3c;		// '<+'
	for (uint32_t i = 0; i < NO_OF_ADC_MULTIPLEXERS; ++i)
	{
		for (uint32_t j = 0; j < NO_OF_ADC_MULTI_CHANNELS; ++j)
		{
			mCalibrationValues.maskFlags [i][j] = mPotValueMask [i][j];
		}
	}
	updateTLV_Bytes ( mTLV_DriverQueueHandle,
			(uint32_t)eTLV_Tag::CalibrationValues, sizeof ( mCalibrationValues ), (uint32_t*) &mCalibrationValues );
}

//==============================================================================
//          loadContext()
//
// 	Load context from flash
//
//  CasualNoises    04/01/2026  First implementation
//==============================================================================
void PotentiometerCalibrationPage::loadContext()
{
	// Nothing to load here
}

//==============================================================================
//          saveContext()
//
// 	Save context to flash
//
//  CasualNoises    04/01/2026  First implementation
//==============================================================================
void PotentiometerCalibrationPage::saveContext()
{
	// Nothing to save here
}

} // namespace CasualNoises
