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

#include <SynthEngineMessage.h>

#include <Core/Text/String.h>
#include <GUI/GUI_Basics/Components/Box.h>
#include <GUI/GUI_Basics/Components/ComboBox.h>
#include <GUI/GUI_Basics/Components/Label.h>
#include <GUI/GUI_Basics/Components/ProgressBar.h>
#include <Graphics/Geometry/Rectangle.h>

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
void MainPage::onComboBoxChange ()
{
	uint32_t id = mComboBoxPtr->getSelectedId ();
	switch (id)
	{
	case 1:								// New performance
		break;
	case 2:								// Calibration
		mPageManagerPtr->createNewPage ( ePageId::calibrationPage );
		break;
	case 3:								// Sytem Info
		mPageManagerPtr->createNewPage ( ePageId::systemInfoPage );
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
//  CasualNoises    26/12/2025  First implementation
//==============================================================================
void MainPage::paint ( Graphics& g )
{

	// Activate SETUP & LOAD switch led's
	eLED_BitNums leds[] = {
		eLED_BitNums::SWITCH_4, eLED_BitNums::SWITCH_5
	};
	sLED_Event event;
	event.ledIntensity = 100;
	for (auto led : leds)
	{
		event.ledBitNum = (uint32_t) led;
		BaseType_t res = xQueueSend ( gYellowPages.gLED_ThreadQueueHandle, &event, 10 );
		if (res != pdPASS)
			CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);
	}

	// Clear the screen
	g.fillAll ();

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
//          sMainPageState
//
// 	Struct used to save/load main page state
//
//  CasualNoises    08/01/2026  First implementation
//==============================================================================
/*typedef struct
{
	uint32_t	comboBoxSelection;
} sMainPageState;
*/
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
//          handleLocalUI_event()
//
// 	Handle page specific UI events
//
//  CasualNoises    04/01/2026  First implementation
//==============================================================================
bool CalibrationPage::handleLocalUI_event (
		sIncommingUI_Event* uiEvent,
		bool altState,
		Graphics& g,
		sSystemSettings* settingsPtr )
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
    mLocalBounds = getGlobalBounds();
    mAwaitingMinLabelPtr->setBounds ( rect );
    mLocalBounds.removeFromTop ( 10 );
    rect = mLocalBounds.removeFromTop ( 25 );
    rect.removeFromLeft ( 10 );
    rect.removeFromRight ( 10 );
    mProgressBarPtr->setBounds( rect );

    // Set text
    mContLabelPtr->setBounds ( mLocalBounds );
    mContLabelPtr->setJustification ( eJustificationFlags::centred );

}

//==============================================================================
//          processADC_Event()
//
// 	Process any incomming ADC events by updating the progress bar
//
//  CasualNoises    08/01/2026  First implementation
//==============================================================================
void PotentiometerCalibrationPage::processADC_Event ( sIncommingUI_Event* uiEvent )
{

	uint32_t value = uiEvent->multiplexed_ADC_Event.value;
	if ( uiEvent->multiplexed_ADC_Event.eventSourceID == eEventSourceDestinationID::multiplexerADC_ThreadSourceID )
	{
		String text;
		if ( uiEvent->multiplexed_ADC_Event.multiplexerNo == 0 )
		{
			switch ( uiEvent->multiplexed_ADC_Event.multiplexerChannelNo )
			{
			case (uint32_t)eMultiplexerChannel_0::P_1:
				text = String( "P #1" );
				break;
			case (uint32_t)eMultiplexerChannel_0::P_2:
				text = String( "P #2" );
				break;
			case (uint32_t)eMultiplexerChannel_0::P_3:
				text = String( "P #3" );
				break;
			case (uint32_t)eMultiplexerChannel_0::P_4:
				text = String( "P #4" );
				break;
			default:
				text = String( "<< unknown >>" );
				break;
			}
		} else if ( uiEvent->multiplexed_ADC_Event.multiplexerNo == 1 )
		{
			switch ( uiEvent->multiplexed_ADC_Event.multiplexerChannelNo )
			{
			case (uint32_t)eMultiplexerChannel_1::F_1:
				text = String( "F #1" );
				break;
			case (uint32_t)eMultiplexerChannel_1::F_2:
				text = String( "F #2" );
				break;
			case (uint32_t)eMultiplexerChannel_1::F_3:
				text = String( "F #3" );
				break;
			case (uint32_t)eMultiplexerChannel_1::F_4:
				text = String( "F #4" );
				break;
			case (uint32_t)eMultiplexerChannel_1::F_5:
				text = String( "F #5" );
				break;
			case (uint32_t)eMultiplexerChannel_1::F_6:
				text = String( "F #6" );
				break;
			case (uint32_t)eMultiplexerChannel_1::F_7:
				text = String( "F #7" );
				break;
			case (uint32_t)eMultiplexerChannel_1::F_8:
				text = String( "F #8" );
				break;
			default:
				text = String( "<< unknown >>" );
				break;
			}
			value = 65535 - value;
		} else
		{
			CN_ReportFault ( eErrorCodes::runtimeError );
		}

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
//==============================================================================
bool PotentiometerCalibrationPage::handleLocalUI_event (
		sIncommingUI_Event* uiEvent,
		bool altState,
		Graphics& g,
		sSystemSettings* settingsPtr )
{
	switch ( mState )
	{
	case ePotentiometerCalibrationPageState::AwaitingMins:
	{
		mOuterBoxPtr->setCurrentTab( (uint8_t) ePotentiometerCalibrationPageState::AwaitingMins );
		if ( uiEvent->multiplexed_ADC_Event.eventSourceID == eEventSourceDestinationID::multiplexerADC_ThreadSourceID )
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
		if ( uiEvent->multiplexed_ADC_Event.eventSourceID == eEventSourceDestinationID::multiplexerADC_ThreadSourceID )
		{
			processADC_Event ( uiEvent );
			mCalibrationValues.minValues [uiEvent->multiplexed_ADC_Event.multiplexerNo]
										 [uiEvent->multiplexed_ADC_Event.multiplexerChannelNo] = uiEvent->multiplexed_ADC_Event.value;
			if ( allPotsAtmin () )
			{
				mContLabelPtr->setVisible ( true );
				setSwitchLed ( eLED_BitNums::SWITCH_3 );
				mState = ePotentiometerCalibrationPageState::AwaitingMaxs;
			}
		}
		return true;
	}
	break;
	case ePotentiometerCalibrationPageState::AwaitingMaxs:
	{
		mOuterBoxPtr->setCurrentTab( (uint8_t) ePotentiometerCalibrationPageState::AwaitingMaxs );
		if (( uiEvent->encoderEvent.eventSourceID == eEventSourceDestinationID::encoderThreadSourceID ) &&
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
		if ( uiEvent->multiplexed_ADC_Event.eventSourceID == eEventSourceDestinationID::multiplexerADC_ThreadSourceID )
		{
			mAwaitingMaxLabelPtr->setVisible ( false );
			mProgressBarPtr->setVisible ( true );
			processADC_Event ( uiEvent );
			mCalibrationValues.maxValues [uiEvent->multiplexed_ADC_Event.multiplexerNo]
										 [uiEvent->multiplexed_ADC_Event.multiplexerChannelNo] = uiEvent->multiplexed_ADC_Event.value;
			if ( allPotsAtmax () )
			{
				setSwitchLed ( eLED_BitNums::SWITCH_3 );
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
		if (( uiEvent->encoderEvent.eventSourceID == eEventSourceDestinationID::encoderThreadSourceID ) &&
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

/*---------------------------- CV_CalibrationPage ----------------------------*/

//==============================================================================
//          CV_CalibrationPage() & ~CV_CalibrationPage()
//
// Main page initializer
//
//  CasualNoises    02/02/2026  First implementation
//==============================================================================
CV_CalibrationPage::CV_CalibrationPage (
		SSD1309_Driver* oledDriverPt,
		QueueHandle_t driverQueueHandle,
		PageManager* pageManagerPtr ) :
	RootPage ( oledDriverPt, driverQueueHandle, pageManagerPtr )
{

	// Create a border component
	mOuterBoxPtr = new Box( String( (char*) "Border" ) );
	addAndMakeVisible ( mOuterBoxPtr );

	// Send a NerveNet request for all ADC input data
	tRequestADC_Data message;
	message.header.messageTag		= (uint32_t) eSynthEngineMessageType::ADC_DataRequest;
	message.header.messageLength	= sizeof ( tRequestADC_Data );
	bool success = gNerveNetMasterThreadPtr[0]->sendMessage ( &message, sizeof ( tRequestADC_Data ) );
	if ( ! success )
		CN_ReportFault(eErrorCodes::NerveNetThread_Error);

}

CV_CalibrationPage::~CV_CalibrationPage()
{
	if ( mOuterBoxPtr 	 			!= nullptr )		delete mOuterBoxPtr;
}

//==============================================================================
//          paint()
//
// Show main page
//
//  CasualNoises    02/02/2026  First implementation
//==============================================================================
void CV_CalibrationPage::paint(Graphics& g)
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
void CV_CalibrationPage::resized()
{

    // Box around this page
    Rectangle<int32_t> rect = getGlobalBounds();
    mOuterBoxPtr->setBounds ( rect );


}

//==============================================================================
//          handleLocalUI_event()
//
// 	Handle page specific UI events
//
//  CasualNoises    02/02/2026  First implementation
//==============================================================================
bool CV_CalibrationPage::handleLocalUI_event (
		sIncommingUI_Event* uiEvent,
		bool altState,
		Graphics& g,
		sSystemSettings* settingsPtr )
{
	return false;
}

//==============================================================================
//          loadContext()
//
// 	Load context from flash
//
//  CasualNoises    02/02/2026  First implementation
//==============================================================================
void CV_CalibrationPage::loadContext()
{
	// Nothing to load here
}

//==============================================================================
//          saveContext()
//
// 	Save context to flash
//
//  CasualNoises    02/02/2026  First implementation
//==============================================================================
void CV_CalibrationPage::saveContext()
{
	// Nothing to save here
}

} // namespace CasualNoises
