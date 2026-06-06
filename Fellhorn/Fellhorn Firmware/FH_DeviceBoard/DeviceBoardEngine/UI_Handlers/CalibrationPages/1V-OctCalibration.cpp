/*
  ==============================================================================

    1V-OctCalibration.cpp
    Created: 03/04/2026

    Handle CV calibration

    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "1V-OctCalibration.h"

#include "Graphics/Contexts/Graphics.h"
#include "GUI/GUI_Basics/Components/Box.h"
#include "GUI/GUI_Basics/Components/Label.h"
#include "GUI/GUI_Basics/Components/LevelBar.h"
#include "GUI/GUI_Basics/Components/ValueSelection.h"

#include "SynthEngineMessage.h"

#include "NerveNet/NerveNetMasterThread.h"

namespace DeviceBoard
{

using namespace CasualNoises;

//==============================================================================
//          _1V_OctCalibrationPage() & ~_1V_OctCalibrationPage()
//
//  CasualNoises    13/04/2026  First implementation
//==============================================================================
_1V_OctCalibrationPage::_1V_OctCalibrationPage (
		SSD1309_Driver* oledDriverPt,
		QueueHandle_t driverQueueHandle,
		PageManager* pageManagerPtr ) :
	RootPage ( oledDriverPt, driverQueueHandle, pageManagerPtr ),
	TimerBase ( "Timer", 100 )
{

	// Turn on Save switch led					ToDo find alternative solution
	dimSwitchLeds ();
	setSwitchLed ( eLED_BitNums::RIGTH_ARROW_SWITCH );

	// Create a border component
	mOuterBoxPtr = new Box( String( (char*) "Border" ) );
	addAndMakeVisible ( mOuterBoxPtr );

	// Create labels
	mMessageLabelPtr = new Label ( "Target:" );
    mMessageLabelPtr->setJustification ( eJustificationFlags::centred );
	addAndMakeVisible ( mMessageLabelPtr );

	// Value selection
	mNoteSelectionPtr = new NoteSelection ( (char*) "Note" );
	mNoteSelectionPtr->setJustification ( eJustificationFlags::centred );
	mNoteSelectionPtr->setIsSelected ( true );
	addAndMakeVisible ( mNoteSelectionPtr );
	mNoteSelectionPtr->onChange = [this] { onNoteSelectionChange(); };

	// Continuation instructions
	mContLabelPtr = new Label ( "Press '>'" );
	mContLabelPtr->setJustification ( eJustificationFlags::centred );
	addAndMakeVisible ( mContLabelPtr );

	// Display CV levels
	mLevelBarPtr = new LevelBar ( "LevelBar" );
	mLevelBarPtr->setTargetLevel( -1.0f );
	addAndMakeVisible ( mLevelBarPtr );

	// Start timer
	startTimer ();

}

_1V_OctCalibrationPage::~_1V_OctCalibrationPage()
{
	if ( mOuterBoxPtr 		!= nullptr )	delete mOuterBoxPtr;
	if ( mMessageLabelPtr	!= nullptr )	delete mMessageLabelPtr;
	if ( mContLabelPtr 		!= nullptr )	delete mContLabelPtr;
	if ( mNoteSelectionPtr	!= nullptr )	delete mNoteSelectionPtr;
	if ( mLevelBarPtr 		!= nullptr )	delete mLevelBarPtr;
}

//==============================================================================
//          onNoteSelectionChange ()
//
//  CasualNoises    14/04/2026  First implementation
//==============================================================================
void _1V_OctCalibrationPage::onNoteSelectionChange () noexcept
{
	int32_t selection = mNoteSelectionPtr->getCurrentSelection();
	float target = jmap ( (float) selection,
						  (float) mNoteSelectionPtr->getMinSelection (),
						  (float) mNoteSelectionPtr->getMaxSelection (),
						  -1.0f, 1.0f );
	mLevelBarPtr->setTargetLevel ( target );
}

//==============================================================================
//          onTimer()
//
//  CasualNoises    13/04/2026  First implementation
//==============================================================================
void _1V_OctCalibrationPage::onTimer() noexcept
{
	requestADC_Data ( );
}

//==============================================================================
//          requestADC_Data()
//
//  CasualNoises    08/04/2026  First implementation
//==============================================================================
void _1V_OctCalibrationPage::requestADC_Data ( eNerveNetSourceId target )
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
//  CasualNoises    13/04/2026  First implementation
//==============================================================================
void _1V_OctCalibrationPage::paint(Graphics& g)
{
	g.fillAll();
}

//==============================================================================
//          resized()
//
//  CasualNoises    01/02/2026  First implementation
//==============================================================================
void _1V_OctCalibrationPage::resized()
{

    // Box around this page
    Rectangle<int32_t> rect = getGlobalBounds();
    mOuterBoxPtr->setBounds ( rect );

    // Place labels
    rect.reduce (2, 2);
    Rectangle<int32_t> messageRect = rect.removeFromTop( 15 );
    mMessageLabelPtr->setBounds ( messageRect.removeFromLeft( 64 ) );
    mNoteSelectionPtr->setBounds ( messageRect );
    mContLabelPtr->setBounds ( rect.removeFromBottom( 15 ) );

    // Place level bar
    rect.reduce ( 4, 0 );
    mLevelBarPtr->setBounds ( rect );

}

//==============================================================================
//          updateLEDs()
//
//  CasualNoises    13/04/2026  First implementation
//==============================================================================
void _1V_OctCalibrationPage::updateLEDs ()
{
	dimSwitchLeds ();
}

//==============================================================================
//          handleLocalUI_event()
//
// 	Handle page specific UI events
//
//  CasualNoises    13/04/2026  First implementation
//==============================================================================
bool _1V_OctCalibrationPage::handleLocalUI_event (
		sIncommingUI_Event* uiEvent,
		bool altState,
		Graphics& g,
		sSystemSettings* settingsPtr,
		bool altSwitchState )
{

	// Handle ADC reply data
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
			canProceseed = northSideReplied & southSideReplied;
			processData ( headerPtr->sourceID, dataPtr );
			return true;

		}

	}

	// Handle '>' button events
	static bool readyToExit = false;
	if ( ( uiEvent->encoderEvent.eventSourceID == eEventSourceID::encoderThreadSourceID ) &&
		 ( (eSwitchNums) uiEvent->encoderEvent.encoderNo == eSwitchNums::RIGTH_ARROW_SWITCH ) &&
		 canProceseed )
	{

		// Update range
		float average = (mCurrentNorthSideLevel + mCurrentSouthSideLevel) / 2.0f;
		if ( average > mMaxCalibrationValue )
			mMaxCalibrationValue = average;
		if ( average < mMinCalibrationValue )
			mMinCalibrationValue = average;

		uint32_t selection = mNoteSelectionPtr->getCurrentSelection ();
		mNorthSideCalbrationLevels [ selection ] = mCurrentNorthSideLevel;
		mSouthSideCalbrationLevels [ selection ] = mCurrentSouthSideLevel;
		northSideReplied = false;
		southSideReplied = false;
		if ( ( mMaxCalibrationValue - mMinCalibrationValue ) > ( 65535.0f / 5.2f ) )
		{
			dimSwitchLed ( eLED_BitNums::RIGTH_ARROW_SWITCH );
			setSwitchLed ( eLED_BitNums::EDIT_SWITCH );
			mContLabelPtr->setText( "Press 'Save'" );
			readyToExit = true;
		}
	}

	// Proceed to next step
	if ( ( uiEvent->encoderEvent.eventSourceID == eEventSourceID::encoderThreadSourceID ) &&
		 ( (eSwitchNums) uiEvent->encoderEvent.encoderNo == eSwitchNums::EDIT_SWITCH ) &&
		 readyToExit )
	{

		// Compose calibration table
		composeCalibrationTable ();

		// Send calibration values to North- & SouthSide
		mNorthSideCalbrationResults.header.sourceID 	 = eNerveNetSourceId::FellhornDeviceBoard;
		mNorthSideCalbrationResults.header.destinationID = eNerveNetSourceId::FellhornNorthSide;
		mNorthSideCalbrationResults.header.messageTag	 = (uint32_t)eSynthEngineMessageType::_1V_OCT_CalibrationData;
		mNorthSideCalbrationResults.header.messageLength = sizeof (t1V_OctCalibrationSettings );
		bool success = gNerveNetMasterThreadPtr[0]->sendMessage ( &mNorthSideCalbrationResults, sizeof ( t1V_OctCalibrationSettings ) );
		if ( ! success )
			CN_ReportFault(eErrorCodes::NerveNetThread_Error);
		mSouthSideCalbrationResults.header.sourceID 	 = eNerveNetSourceId::FellhornDeviceBoard;
		mSouthSideCalbrationResults.header.destinationID = eNerveNetSourceId::FellhornSouthSide;
		mSouthSideCalbrationResults.header.messageTag	 = (uint32_t)eSynthEngineMessageType::_1V_OCT_CalibrationData;
		mSouthSideCalbrationResults.header.messageLength = sizeof (t1V_OctCalibrationSettings );
		success = gNerveNetMasterThreadPtr[0]->sendMessage ( &mSouthSideCalbrationResults, sizeof ( t1V_OctCalibrationSettings ) );
		if ( ! success )
			CN_ReportFault(eErrorCodes::NerveNetThread_Error);

		mContLabelPtr->setText( "Press 'Exit'" );
		readyToExit = false;

	}

	return false;

}

//==============================================================================
//          processData()
//
//  CasualNoises    14/04/2026  First implementation
//==============================================================================
void _1V_OctCalibrationPage::processData (
		eNerveNetSourceId sourceId,
		float* sourceDataPtr )
{

	// Save average for each side
	float cv1 = sourceDataPtr [ 0 ];
	float cv2 = sourceDataPtr [ 1 ];
	float average = (cv1 + cv2) / 2.0f;
	if ( sourceId == eNerveNetSourceId::FellhornNorthSide )
		mCurrentNorthSideLevel = average;
	else
		mCurrentSouthSideLevel = average;

	// Update range
/*
	if ( average > mMaxCalibrationValue )
		mMaxCalibrationValue = average;
	if ( average < mMinCalibrationValue )
		mMinCalibrationValue = average;
*/
	// Update level bar
	float level = jmap ( average, 0.0f, 65535.0f, -1.0f, 1.0f );
	mLevelBarPtr->setLevels ( level );

}

//==============================================================================
//          processData()
//
//  CasualNoises    14/04/2026  First implementation
//==============================================================================
void _1V_OctCalibrationPage::composeCalibrationTable () noexcept
{

	// Get pointers
	mNorthSideCalbrationResultsPtr = ( float*) ( (uint8_t*) &mNorthSideCalbrationResults + sizeof ( tNerveNetMessageHeader ) );
	mSouthSideCalbrationResultsPtr = ( float*) ( (uint8_t*) &mSouthSideCalbrationResults + sizeof ( tNerveNetMessageHeader ) );

	// Clear calibration levels
	for ( uint32_t i = 0; i < cTotalNoOfNotes; ++i )
	{
		mNorthSideCalbrationResultsPtr [ i ] = 0.0f;
		mSouthSideCalbrationResultsPtr [ i ] = 0.0f;
	}

	// Copy calibration levels into the final calibration results
	uint8_t steps []   = { 2, 2, 1, 2, 2, 2, 1 };
	uint32_t stepIndex = 0;
	for ( uint32_t s = 0, d = 0; s < cNoOfNotes; ++s )
	{
		mNorthSideCalbrationResultsPtr [ d ] = mNorthSideCalbrationLevels [ s ];
		mSouthSideCalbrationResultsPtr [ d ] = mSouthSideCalbrationLevels [ s ];
		d += steps [ stepIndex ];
		stepIndex += 1;
		if ( stepIndex >= sizeof ( steps ) )
			stepIndex = 0;
	}

	// Interpolate missing values
	if ( mNorthSideCalbrationResultsPtr [ cTotalNoOfNotes - 1] <= 1.0f )
		mNorthSideCalbrationResultsPtr [ cTotalNoOfNotes - 1] = 65535.0f;
	if ( mSouthSideCalbrationResultsPtr [ cTotalNoOfNotes - 1] <= 1.0f )
		mSouthSideCalbrationResultsPtr [ cTotalNoOfNotes - 1] = 65535.0f;
	for (uint32_t i = 0; i < cTotalNoOfNotes; )
	{
		uint32_t j = i + 1;
		for ( ; j < cTotalNoOfNotes; ++j )
		{
			if ( mNorthSideCalbrationResultsPtr [ j ] > 0.0f )
				break;
		}
		float deltaNS = ( mNorthSideCalbrationResultsPtr [ j ] - mNorthSideCalbrationResultsPtr [ i ] ) / ( j - i);
		float deltaSS = ( mSouthSideCalbrationResultsPtr [ j ] - mSouthSideCalbrationResultsPtr [ i ] ) / ( j - i);
		float valueNS = mNorthSideCalbrationResultsPtr [ i ] + deltaNS;
		float valueSS = mSouthSideCalbrationResultsPtr [ i ] + deltaSS;
		for ( ++i; i < j; ++i)
		{
			mNorthSideCalbrationResultsPtr [ i ] = valueNS;
			valueNS += deltaNS;
			mSouthSideCalbrationResultsPtr [ i ] = valueSS;
			valueSS += deltaSS;
		}
	}

}

} // namespace DeviceBoard
