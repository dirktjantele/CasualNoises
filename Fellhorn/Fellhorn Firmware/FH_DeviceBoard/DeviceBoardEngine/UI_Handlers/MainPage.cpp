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

#include <Core/Text/String.h>
#include <GUI/GUI_Basics/Components/Box.h>
#include <GUI/GUI_Basics/Components/ComboBox.h>
#include <GUI/GUI_Basics/Components/Label.h>
#include <GUI/GUI_Basics/Components/ProgressBar.h>
#include <Graphics/Geometry/Rectangle.h>

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
MainPage::MainPage(SSD1309_Driver* oledDriverPt,
				   QueueHandle_t driverQueueHandle,
				   PageManager* pageManagerPtr) :
		RootPage(oledDriverPt, driverQueueHandle, pageManagerPtr)
{

	// Create a border component
	mOuterBoxPtr = new Box( String( (char*) "Border" ) );
	addAndMakeVisible ( mOuterBoxPtr );

	// Build a ComboBox
	static String names[]
	{
	    String( (char*) "New Performance" ),
		String( (char*) "Calibration" ),
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
void MainPage::onComboBoxChange()
{
	uint32_t id = mComboBoxPtr->getSelectedId();
	switch (id)
	{
	case 1:								// New performance
		break;
	case 2:								// Calibration
		mPageManagerPtr->createNewPage ( ePageId::calibrationPage );
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
void MainPage::paint(Graphics& g)
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
    Rectangle<int> rect = getBounds();
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
typedef struct
{
	uint32_t	comboBoxSelection;
} sMainPageState;

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
	readTLV_TagBytes ( mTLV_DriverQueueHandle,
			(uint32_t)eTLV_Tag::UI_MainPageState, sizeof ( sMainPageState ), (uint32_t*) &state );
	mComboBoxPtr->setFocus ( state.comboBoxSelection );
}

//==============================================================================
//          saveContext()
//
// 	Save context to flash
//
//  CasualNoises    08/01/2026  First implementation
//==============================================================================
void MainPage::saveContext()
{
	sMainPageState state;
	state.comboBoxSelection = mComboBoxPtr->getSelectedId ();
	updateTLV_Bytes (mTLV_DriverQueueHandle,
			(uint32_t)eTLV_Tag::UI_MainPageState, sizeof ( sMainPageState ), (uint32_t*) &state );
}

/*---------------------------- CalibrationPage ----------------------------*/

//==============================================================================
//          CalibrationPage() & ~CalibrationPage()
//
// Main page initializer
//
//  CasualNoises    04/01/2026  First implementation
//==============================================================================
CalibrationPage::CalibrationPage(SSD1309_Driver* oledDriverPt,
								 QueueHandle_t driverQueueHandle,
								 PageManager* pageManagerPtr) :
	RootPage ( oledDriverPt, driverQueueHandle, pageManagerPtr )
{

	// Create border components
	mOuterBoxPtr = new IndexBox ( String((char*)"Outer border"), sizeof (eCalibrationPageState) + 1, 0 );
	addAndMakeVisible ( mOuterBoxPtr );

	// Create labels
	mAwaitingMinLabelPtr = new Label ( (char *)"Label", (char *)"Calibration\nSet pots to min" );
	addAndMakeVisible ( mAwaitingMinLabelPtr );
	mAwaitingMaxLabelPtr = new Label ( (char *)"Label", (char *)"Calibration\nSet pots to max" );
	addChildComponent ( mAwaitingMaxLabelPtr );
	mCompletionLabelPtr  = new Label ( (char *)"Label", (char *)"Calibration done,\nPress 'EXIT'" );
	addChildComponent ( mCompletionLabelPtr );

	// Create progress bar
	mProgressBarPtr = new ProgressBar ( String((char*)"ProgressBar") );
	addChildComponent ( mProgressBarPtr );

	// Continuation instructions
	mContLabelPtr = new Label ( (char *)"Label", (char *)"'>' to proceed" );
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
	mState = eCalibrationPageState::AwaitingMins;

}

CalibrationPage::~CalibrationPage()
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
void CalibrationPage::paint(Graphics& g)
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
void CalibrationPage::resized()
{

    Rectangle<int> rect = getBounds();
    UNUSED(rect);

    // Box around this page
    mOuterBoxPtr->setBounds(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());

    // Place awiting... labels
    rect.reduce(2, 2);
    mAwaitingMinLabelPtr->setBounds(rect);
    mAwaitingMinLabelPtr->setJustification( eJustificationFlags::centred );
    mAwaitingMaxLabelPtr->setBounds(rect);
    mAwaitingMaxLabelPtr->setJustification( eJustificationFlags::centred );
    mCompletionLabelPtr->setBounds(rect);
    mCompletionLabelPtr->setJustification( eJustificationFlags::centred );

    // Progress bar
    mAwaitingMinLabelPtr->setBounds ( rect );
    Rectangle<int> ProgBarRect = rect.removeFromTop ( (DISPLAY_HEIGHT * 2) / 3 );
    ProgBarRect.reduce( 14, 10 );
    mProgressBarPtr->setBounds( ProgBarRect );

    // Set text
    mContLabelPtr->setBounds(rect);

}

//==============================================================================
//          processADC_Event()
//
// 	Process any incomming ADC events by updating the progress bar
//
//  CasualNoises    08/01/2026  First implementation
//==============================================================================
void CalibrationPage::processADC_Event ( sIncommingUI_Event* uiEvent )
{

	uint32_t value = uiEvent->multiplexed_ADC_Event.value;
	if (uiEvent->multiplexed_ADC_Event.eventSourceID == eEventSourceID::multiplexerADC_ThreadSourceID)
	{
		String text;
		if ( uiEvent->multiplexed_ADC_Event.multiplexerNo == 0 )
		{
			switch ( uiEvent->multiplexed_ADC_Event.multiplexerChannelNo )
			{
			case (uint32_t)eMultiplexerChannel_0::P_1:
				text = String((char *)"P #1");
				break;
			case (uint32_t)eMultiplexerChannel_0::P_2:
				text = String( (char *)"P #2" );
				break;
			case (uint32_t)eMultiplexerChannel_0::P_3:
				text = String( (char *)"P #3" );
				break;
			case (uint32_t)eMultiplexerChannel_0::P_4:
				text = String( (char *)"P #4" );
				break;
			default:
				text = String( (char *)"<< unknown >>" );
				break;
			}
		} else if ( uiEvent->multiplexed_ADC_Event.multiplexerNo == 1 )
		{
			switch ( uiEvent->multiplexed_ADC_Event.multiplexerChannelNo )
			{
			case (uint32_t)eMultiplexerChannel_1::F_1:
				text = String( (char *)"F #1" );
				break;
			case (uint32_t)eMultiplexerChannel_1::F_2:
				text = String( (char *)"F #2" );
				break;
			case (uint32_t)eMultiplexerChannel_1::F_3:
				text = String( (char *)"F #3" );
				break;
			case (uint32_t)eMultiplexerChannel_1::F_4:
				text = String( (char *)"F #4" );
				break;
			case (uint32_t)eMultiplexerChannel_1::F_5:
				text = String( (char *)"F #5" );
				break;
			case (uint32_t)eMultiplexerChannel_1::F_6:
				text = String( (char *)"F #6" );
				break;
			case (uint32_t)eMultiplexerChannel_1::F_7:
				text = String( (char *)"F #7" );
				break;
			case (uint32_t)eMultiplexerChannel_1::F_8:
				text = String( (char *)"F #8" );
				break;
			default:
				text = String( (char *)"<< unknown >>" );
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
bool CalibrationPage::handleLocalUI_event(sIncommingUI_Event* uiEvent, bool altState, Graphics& g)
{
	switch ( mState )
	{
	case eCalibrationPageState::AwaitingMins:
	{
		mOuterBoxPtr->setCurrentTab( (uint8_t) eCalibrationPageState::AwaitingMins );
		if ( uiEvent->multiplexed_ADC_Event.eventSourceID == eEventSourceID::multiplexerADC_ThreadSourceID )
		{
			dimSwitchLeds ();
			mAwaitingMinLabelPtr->setVisible ( false );
			mProgressBarPtr->setVisible ( true );
			mState = eCalibrationPageState::ProcessingMins;
			return true;
		}
	}
	break;
	case eCalibrationPageState::ProcessingMins:
	{
		mOuterBoxPtr->setCurrentTab( (uint8_t) eCalibrationPageState::ProcessingMins );
		if ( uiEvent->multiplexed_ADC_Event.eventSourceID == eEventSourceID::multiplexerADC_ThreadSourceID )
		{
			processADC_Event ( uiEvent );
			mCalibrationValues.minValues [uiEvent->multiplexed_ADC_Event.multiplexerNo]
										 [uiEvent->multiplexed_ADC_Event.multiplexerChannelNo] = uiEvent->multiplexed_ADC_Event.value;
			if ( allPotsAtmin () )
			{
				mContLabelPtr->setVisible ( true );
				setSwitchLed ( eLED_BitNums::SWITCH_3 );
				mState = eCalibrationPageState::AwaitingMaxs;
			}
		}
		return true;
	}
	break;
	case eCalibrationPageState::AwaitingMaxs:
	{
		mOuterBoxPtr->setCurrentTab( (uint8_t) eCalibrationPageState::AwaitingMaxs );
		if (( uiEvent->encoderEvent.eventSourceID == eEventSourceID::encoderThreadSourceID ) &&
			( uiEvent->encoderEvent.encoderNo == (uint16_t)eSwitchNums::RIGTH_ARROW_SWITCH ) )
		{
			dimSwitchLeds ();
			mAwaitingMaxLabelPtr->setVisible ( true );
			mProgressBarPtr->setVisible ( false );
			mContLabelPtr->setVisible ( false );
			mState = eCalibrationPageState::ProcessingMaxs;
			return true;
		}
	}
	break;
	case eCalibrationPageState::ProcessingMaxs:
	{
		mOuterBoxPtr->setCurrentTab( (uint8_t) eCalibrationPageState::ProcessingMaxs );
		if ( uiEvent->multiplexed_ADC_Event.eventSourceID == eEventSourceID::multiplexerADC_ThreadSourceID )
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
				mState = eCalibrationPageState::Completion;
				saveCalibrationValues ();
			}
		}
		return true;
	}
	break;
	case eCalibrationPageState::Completion:
	{
		mOuterBoxPtr->setCurrentTab( (uint8_t) eCalibrationPageState::Completion );
		if (( uiEvent->encoderEvent.eventSourceID == eEventSourceID::encoderThreadSourceID ) &&
			( uiEvent->encoderEvent.encoderNo == (uint16_t)eSwitchNums::RIGTH_ARROW_SWITCH ) )
		{
			dimSwitchLeds ();
			mProgressBarPtr->setVisible ( false );
			mContLabelPtr->setVisible ( false );
			mCompletionLabelPtr->setVisible ( true );
			mState = eCalibrationPageState::ProcessingMaxs;
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
bool CalibrationPage::allPotsAtmin () const noexcept
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
bool CalibrationPage::allPotsAtmax () const noexcept
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
void CalibrationPage::saveCalibrationValues () noexcept
{
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
void CalibrationPage::loadContext()
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
void CalibrationPage::saveContext()
{
	// Nothing to save here
}

} // namespace CasualNoises
