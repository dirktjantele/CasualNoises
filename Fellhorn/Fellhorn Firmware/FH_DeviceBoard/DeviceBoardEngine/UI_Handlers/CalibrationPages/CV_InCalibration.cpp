/*
  ==============================================================================

    CV_InCalibration.cpp
    Created: 03/04/2026

    Handle CV calibration

    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "CV_InCalibration.h"

#include "Graphics/Contexts/Graphics.h"
#include "GUI/GUI_Basics/Components/Box.h"
#include "GUI/GUI_Basics/Components/Label.h"
#include "GUI/GUI_Basics/Components/LevelBar.h"

#include "NerveNet/NerveNetMasterThread.h"

#include "Utilities/enumClassUtilities.h"

namespace DeviceBoard
{

using namespace CasualNoises;

//==============================================================================
//          CV_CalibrationPage() & ~CV_CalibrationPage()
//
//  CasualNoises    09/04/2026  First implementation
//==============================================================================
CV_CalibrationPage::CV_CalibrationPage ( SSD1309_Driver* oledDriverPt,
										 QueueHandle_t driverQueueHandle,
										 PageManager* pageManagerPtr,
										 void* paramsPtr ) :
	RootPage ( oledDriverPt, driverQueueHandle, pageManagerPtr, paramsPtr ),
	TimerBase ( "Timer", 100 )
{

	// All LED's off
	dimSwitchLeds ();

	// Create a border component
	mOuterBoxPtr = new Box( String( (char*) "Border" ) );
	addAndMakeVisible ( mOuterBoxPtr );

	// Create labels
	mMessageLabelPtr = new Label ( "Disconnect CV's" );
    mMessageLabelPtr->setJustification ( eJustificationFlags::centred );
	addAndMakeVisible ( mMessageLabelPtr );

	// Continuation instructions
	mContLabelPtr = new Label ( "'>' to proceed" );
	mContLabelPtr->setJustification ( eJustificationFlags::centred );
	addChildComponent ( mContLabelPtr );

	// Display CV levels
	mLevelBarPtr = new LevelBar ( "LevelBar" );
	addAndMakeVisible ( mLevelBarPtr );

	// Start timer
	startTimer();

}

CV_CalibrationPage::~CV_CalibrationPage()
{
	if ( mOuterBoxPtr 		!= nullptr )	delete mOuterBoxPtr;
	if ( mMessageLabelPtr	!= nullptr )	delete mMessageLabelPtr;
	if ( mContLabelPtr 		!= nullptr )	delete mContLabelPtr;
	if ( mLevelBarPtr 		!= nullptr )	delete mLevelBarPtr;
}

//==============================================================================
//          onTimer()
//
//  CasualNoises    11/04/2026  First implementation
//==============================================================================
void CV_CalibrationPage::onTimer()
{
	requestADC_Data ( );
}

//==============================================================================
//          requestADC_Data()
//
//  CasualNoises    08/04/2026  First implementation
//==============================================================================
void CV_CalibrationPage::requestADC_Data ( eNerveNetSourceId target )
{

	// Send a NerveNet request for all ADC input data
	tRequestADC_Data message;
	message.header.sourceID			= eNerveNetSourceId::FellhornDeviceBoard;
	message.header.destinationID	= target;
	message.header.messageTag		= (uint32_t) eSynthEngineMessageType::ADC_DataRequest;
	message.header.messageLength	= sizeof ( tRequestADC_Data );
	bool success = gNerveNetMasterThreadPtr[0]->sendMessage ( &message, sizeof ( tRequestADC_Data ) );
	if ( ! success )
		CN_ReportFault(eErrorCodes::NerveNetThread_Error);

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

    // Place labels
    rect.reduce (2, 2);
    mMessageLabelPtr->setBounds ( rect.removeFromTop ( 15 ) );
    mContLabelPtr->setBounds ( rect.removeFromBottom ( 15 ) );

    // Place level bar
    rect.reduce ( 4, 0 );
    mLevelBarPtr->setBounds ( rect );

}

//==============================================================================
//          updateLEDs()
//
//  CasualNoises    13/04/2026  First implementation
//==============================================================================
void CV_CalibrationPage::updateLEDs ()
{
	dimSwitchLeds ();
}

//==============================================================================
//          handleLocalUI_event()
//
// 	Handle page specific UI events
//
//  CasualNoises    08/06/2025  First implementation
//  CasualNoises    08/04/2026  altSwitchState added
//==============================================================================
bool CV_CalibrationPage::handleLocalUI_event (
		sIncommingUI_Event* uiEvent,
		bool altState,
		Graphics& g,
		sSystemSettings* settingsPtr,
		bool altSwitchState )
{

	// Handle ADC reply data
	bool success = false;
	static bool canProceseed 		= false;
	static bool northSideReplied	= false;
	static bool southSideReplied	= false;
	if ( uiEvent->nerveNetEvent.eventSourceID == eEventSourceID::nerveNetSourceID)
	{

		tNerveNetMessageHeader* headerPtr	= ( tNerveNetMessageHeader* ) uiEvent->nerveNetEvent.eventdataPtr;
		eSynthEngineMessageType tag 		= ( eSynthEngineMessageType ) headerPtr->messageTag;
		float* dataPtr 						= ( float* ) ( ( uint8_t* ) headerPtr + sizeof ( tNerveNetMessageHeader ) );
		if ( tag == eSynthEngineMessageType::ADC_DataReply )
		{

			// Check whether both side replied on the request
			if ( headerPtr->sourceID == eNerveNetSourceId::FellhornNorthSide )
				northSideReplied = true;
			if ( headerPtr->sourceID == eNerveNetSourceId::FellhornSouthSide )
				southSideReplied = true;

			// Handle current phase
			switch ( mCurrentPhase )
			{
			case eCV_CalibrationPhase::openCV_Inputs:
			case eCV_CalibrationPhase::cv1_min5:
			case eCV_CalibrationPhase::cv2_min5:
			case eCV_CalibrationPhase::cv3_min5:
			case eCV_CalibrationPhase::cv4_min5:
			case eCV_CalibrationPhase::cv5_min5:
			case eCV_CalibrationPhase::cv6_min5:
			case eCV_CalibrationPhase::cv7_min5:
			case eCV_CalibrationPhase::cv1_plus5:
			case eCV_CalibrationPhase::cv2_plus5:
			case eCV_CalibrationPhase::cv3_plus5:
			case eCV_CalibrationPhase::cv4_plus5:
			case eCV_CalibrationPhase::cv5_plus5:
			case eCV_CalibrationPhase::cv6_plus5:
			case eCV_CalibrationPhase::cv7_plus5:
				if ( processData ( mCurrentPhase, headerPtr->sourceID, dataPtr ) )
				{
					mContLabelPtr->setVisible( true );
					setSwitchLed ( eLED_BitNums::RIGTH_ARROW_SWITCH );
					canProceseed = northSideReplied & southSideReplied;
				} else
				{
					mContLabelPtr->setVisible( false );
					dimSwitchLed ( eLED_BitNums::RIGTH_ARROW_SWITCH );
					canProceseed = false;
				}
				break;
			case eCV_CalibrationPhase::confirm:
				{
					mNorthSideSettings.header.sourceID		= eNerveNetSourceId::FellhornDeviceBoard;
					mNorthSideSettings.header.destinationID	= eNerveNetSourceId::FellhornNorthSide;
					mNorthSideSettings.header.messageTag	= (uint32_t)eSynthEngineMessageType::ADC_CalibrationData;
					mNorthSideSettings.header.messageLength = sizeof ( tCV_InputCalibrationSettings );
					bool success = gNerveNetMasterThreadPtr[0]->sendMessage ( &mNorthSideSettings, sizeof ( tCV_InputCalibrationSettings ) );
					if ( ! success )
						CN_ReportFault(eErrorCodes::NerveNetThread_Error);
					mSouthSideSettings.header.sourceID		= eNerveNetSourceId::FellhornDeviceBoard;
					mSouthSideSettings.header.destinationID	= eNerveNetSourceId::FellhornSouthSide;
					mSouthSideSettings.header.messageTag	= (uint32_t)eSynthEngineMessageType::ADC_CalibrationData;
					mSouthSideSettings.header.messageLength = sizeof ( tCV_InputCalibrationSettings );
					success = gNerveNetMasterThreadPtr[0]->sendMessage ( &mSouthSideSettings, sizeof ( tCV_InputCalibrationSettings ) );
					if ( ! success )
						CN_ReportFault(eErrorCodes::NerveNetThread_Error);
					mCurrentPhase = eCV_CalibrationPhase::exit;
				}
				break;
			case eCV_CalibrationPhase::exit:
				stopTimer ();
				break;
			default:
				CN_ReportFault( eErrorCodes::UI_ThreadError );
			}

			// Event is handled by
			success = true;

		}

	// Proceed to next phase on '>' button
	} else if (( uiEvent->encoderEvent.eventSourceID == eEventSourceID::encoderThreadSourceID ) &&
			   ( uiEvent->encoderEvent.encoderNo == (uint16_t)eSwitchNums::RIGTH_ARROW_SWITCH ) &&
			   canProceseed )
	{

		// Get next phase
		mCurrentPhase = next ( mCurrentPhase );
		success 	 	 = true;
		canProceseed 	 = false;
		northSideReplied = false;
		southSideReplied = false;

		// Update message label
		if ( mCurrentPhase != eCV_CalibrationPhase::confirm )
		{
			String prefix;
			int32_t CV_no;
			if ( ( mCurrentPhase >= eCV_CalibrationPhase::cv1_min5 ) &&
				 ( mCurrentPhase <= eCV_CalibrationPhase::cv7_min5 ) )
			{
				prefix = String ( "-" );
				CV_no = static_cast<uint32_t> ( mCurrentPhase ) - static_cast<uint32_t> ( eCV_CalibrationPhase::cv1_min5 ) + 1;
			}
			else if ( ( mCurrentPhase >= eCV_CalibrationPhase::cv1_plus5 ) &&
					  ( mCurrentPhase <= eCV_CalibrationPhase::cv7_plus5 ) )
			{
				prefix = String ( "+" );
				CV_no = static_cast<uint32_t> ( mCurrentPhase ) - static_cast<uint32_t> ( eCV_CalibrationPhase::cv1_plus5 ) + 1;
			}
			String message = String ( "Set CV") + String ( (int) CV_no ) + String ( " to ") + prefix + String ( "5V" );
			mMessageLabelPtr->setText( message );
		} else
		{
			mMessageLabelPtr->setText( "Calibration done" );
			mLevelBarPtr->setVisible( false );
			mContLabelPtr->setText( "Press EXIT");
		}

	}

	return success;
}

//==============================================================================
//          processData()
//
// 	Process CV input values and validate them according to the calibration phase
//
//  CasualNoises    10/04/2026  First implementation
//==============================================================================
bool CV_CalibrationPage::processData (
		eCV_CalibrationPhase phase,
		eNerveNetSourceId sourceId,
		float* sourceDataPtr )
{
	tCV_InputCalibrationSettings* settingsPtr = nullptr;
	if ( sourceId == eNerveNetSourceId::FellhornNorthSide )
		settingsPtr = &mNorthSideSettings;
	else
		settingsPtr = &mSouthSideSettings;

	switch ( phase )
	{
	case eCV_CalibrationPhase::openCV_Inputs:
		{
			float max =  0;
			for (uint32_t i = 2; i < TOTAL_NUM_CV_INPUTS; ++i )
			{
				float value = sourceDataPtr [ i ];
				settingsPtr->openInputValues [ i - 2] = value;
				if ( fabs ( value ) > fabs ( max ) )
					max = value;
			}
			mLevelBarPtr->setAllLevels ( 0.0f, max, max );
			return fabs ( max ) < 0.05f;
		}
		break;
	case eCV_CalibrationPhase::cv1_min5:
	case eCV_CalibrationPhase::cv2_min5:
	case eCV_CalibrationPhase::cv3_min5:
	case eCV_CalibrationPhase::cv4_min5:
	case eCV_CalibrationPhase::cv5_min5:
	case eCV_CalibrationPhase::cv6_min5:
	case eCV_CalibrationPhase::cv7_min5:
		{
			uint32_t index = static_cast<uint32_t> ( phase ) - static_cast<uint32_t> ( eCV_CalibrationPhase::cv1_min5 );
			float value = sourceDataPtr [ index + 2 ];
			settingsPtr->min5V_InputValues [ index ] = value;
			mLevelBarPtr->setAllLevels ( -1.0f, value, value );
			return value < -0.8f;
		}
		break;
	case eCV_CalibrationPhase::cv1_plus5:
	case eCV_CalibrationPhase::cv2_plus5:
	case eCV_CalibrationPhase::cv3_plus5:
	case eCV_CalibrationPhase::cv4_plus5:
	case eCV_CalibrationPhase::cv5_plus5:
	case eCV_CalibrationPhase::cv6_plus5:
	case eCV_CalibrationPhase::cv7_plus5:
		{
			uint32_t index = static_cast<uint32_t> ( phase ) - static_cast<uint32_t> ( eCV_CalibrationPhase::cv1_plus5 );
			float value = sourceDataPtr [ index + 2 ];
			settingsPtr->plus5V_InputValues [ index ] = value;
			mLevelBarPtr->setAllLevels ( 1.0f, value, value );
			return value > 0.8f;
		}
		break;
	default:
		CN_ReportFault( eErrorCodes::UI_ThreadError );
	}

	return false;
}

} // namespace DeviceBoard
