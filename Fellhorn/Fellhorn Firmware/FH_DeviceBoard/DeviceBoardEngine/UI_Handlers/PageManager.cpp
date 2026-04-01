/*
  ==============================================================================

    PageManager.cpp
    Created: 22/12/2025

    Handles opening pages and moving around pages

    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "PageManager.h"

#include "MainPage.h"
#include "SetupPage.h"
#include "LoadPage.h"

#include "SystemInfoPage.h"

#include "YellowPages.h"

#include <SynthEngineMessage.h>

#include <Utilities/ReportFault.h>
#include <Threads/TLV_DriverThread.h>

#include <NerveNet/NerveNetMasterThread.h>

#include <Core/Maths/MathsFunctions.h>

namespace CasualNoises
{

//==============================================================================
//          PageManager() & ~PageManager
//
// 	Main page manager
//
//  CasualNoises    24/12/2025  First implementation
//  CasualNoises    14/01/2026  Using TLV_DriverThread
//==============================================================================
PageManager::PageManager ( SSD1309_Driver* oledDriverPtr, QueueHandle_t driverQueueHandle ) :
	m_oledDriverPtr ( oledDriverPtr ),
	mTLV_DriverQueueHandle ( driverQueueHandle )
{

	// Create a graphics object for display access
	mGraphics = new Graphics(m_oledDriverPtr);

	// Clear page id & object pointer stack
	for (uint32_t i = 0; i < cPageIdStackSize; ++i)
	{
		mPageIdStack[i] = (ePageId) 0;
		mPageObjectStack[i] = nullptr;
	}

	// Load page stack from flash, create one if it does not exists
	bool saveState = false;
	uint32_t index = findNextTLV ( mTLV_DriverQueueHandle, (uint32_t)eTLV_Tag::UI_PageStack, 0 );
	if ( index == 0 )														// TLV does not exists, create new one
	{
		saveState = true;
	}
	else if ( getTLV_LengthBytes ( mTLV_DriverQueueHandle, index ) != sizeof(mPageIdStack) )
	{
		deleteTLV ( mTLV_DriverQueueHandle, (uint32_t)eTLV_Tag::UI_PageStack, true );	// Delete existing TLV
		saveState = true;
	}
	if ( saveState )															// Add TLV
	{
		deleteTLV ( mTLV_DriverQueueHandle, (uint32_t)eTLV_Tag::UI_PageStackPtr, true );
		if ( ! addTLV_Bytes ( mTLV_DriverQueueHandle, (uint32_t)eTLV_Tag::UI_PageStack, sizeof ( mPageIdStack ), (uint32_t*) mPageIdStack ) )
			CN_ReportFault(eErrorCodes::UI_ThreadError);
		if ( ! addTLV_Bytes ( mTLV_DriverQueueHandle, (uint32_t)eTLV_Tag::UI_PageStackPtr, sizeof ( mPageIdStackPtr ), (uint32_t*) mPageIdStackPtr ) )
			CN_ReportFault(eErrorCodes::UI_ThreadError);
	}
	uint32_t stackCnt		= readTLV_TagBytes ( mTLV_DriverQueueHandle, (uint32_t)eTLV_Tag::UI_PageStack, sizeof ( mPageIdStack ), (uint32_t*) mPageIdStack );
	UNUSED ( stackCnt );
	uint32_t stackPtrCnt	= readTLV_TagBytes ( mTLV_DriverQueueHandle, (uint32_t)eTLV_Tag::UI_PageStackPtr, sizeof ( mPageIdStackPtr ), (uint32_t*) &mPageIdStackPtr );
	UNUSED ( stackPtrCnt );

	// If stack is empty, add a mainPage id to the stack
	if ( mPageIdStackPtr == 0 )
	{
		createPage ( ePageId::mainPage, true, 0 );

	// Otherwise load all pages on the stack and show the last page
	} else
	{
		for ( uint32_t i = 0; i < mPageIdStackPtr; ++i )
		{
			createPage ( mPageIdStack[i], false, i );
		}
	}

	// Show top most stacked page
	mPageObjectStack [mPageIdStackPtr - 1]->resized ();
	mPageObjectStack [mPageIdStackPtr - 1]->paintAll ( *mGraphics );

	// Set exit switch led
	sLED_Event event;
	event.ledBitNum    = (uint32_t)eLED_BitNums::EXIT_SWITCH;
	event.ledIntensity = 100;
	if ( mPageIdStackPtr == 1 )
		event.ledIntensity = 5;
	BaseType_t res = xQueueSend ( gYellowPages.gLED_ThreadQueueHandle, &event, 10 );
	if ( res != pdPASS )
		CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);

}

PageManager::~PageManager()
{

}

//==============================================================================
//          createPage()
//
// 	Create a page with a given target page id and add it to the stack if appropriate
//		pageId:			id for the new page
//		updateIdStack:	false when reopening pages from a saved page stack
//		stackPtr:		stack pointer used when avove is true
//
//  CasualNoises    24/12/2025  First implementation
//==============================================================================
void PageManager::createPage(ePageId pageId, bool updateIdStack, uint32_t stackPtr)
{

	// Create an object to handle this page
	RootPage* pagePtr = nullptr;
	switch ( pageId )
	{
	case ePageId::mainPage:
		pagePtr = new MainPage ( m_oledDriverPtr, mTLV_DriverQueueHandle, this );
		break;
	case ePageId::setupPage:
		pagePtr = new SetupPage ( m_oledDriverPtr, mTLV_DriverQueueHandle, this );
		break;
	case ePageId::loadPage:
		pagePtr = new LoadPage ( m_oledDriverPtr, mTLV_DriverQueueHandle, this );
		break;
	case ePageId::calibrationPage:
		pagePtr = new CalibrationPage ( m_oledDriverPtr, mTLV_DriverQueueHandle, this );
		break;
	case ePageId::potCalibrationPage:
		pagePtr = new PotentiometerCalibrationPage ( m_oledDriverPtr, mTLV_DriverQueueHandle, this );
		break;
	case ePageId::CV_CalibrationPage:
		pagePtr = new CV_CalibrationPage ( m_oledDriverPtr, mTLV_DriverQueueHandle, this );
		break;
	case ePageId::systemInfoPage:
		pagePtr = new SystemInfoPage ( m_oledDriverPtr, mTLV_DriverQueueHandle, this );
		break;
	default:
		CN_ReportFault ( eErrorCodes::PageManagerError );
	};
	if (pagePtr == nullptr)
		CN_ReportFault ( eErrorCodes::PageManagerError );

	// Update page stack
	mPageObjectStack[stackPtr]	= pagePtr;
	if (updateIdStack)
	{
		mPageIdStack[stackPtr] 	= pageId;
		++mPageIdStackPtr;
		if (mPageIdStackPtr >= cPageIdStackSize)
			CN_ReportFault(eErrorCodes::PageManagerError);
	} else
	{
		mPageObjectStack [ stackPtr ]->loadContext ();
	}

	// Save context of current page before opening a new page
	if (stackPtr > 0)
		mPageObjectStack[ stackPtr - 1 ]->saveContext ();

	// If this page is on top of the stack, show it
	if ( stackPtr == ( mPageIdStackPtr - 1) )
	{
		setExitSwitchLedIntensity ();
		pagePtr->loadContext ();
		pagePtr->resized ();
		pagePtr->paintAll ( *mGraphics );
	}

	// Update flash
	if ( updateIdStack )
	{
		bool res = updateTLV_Bytes ( mTLV_DriverQueueHandle,
									 (uint32_t) eTLV_Tag::UI_PageStack,
									 sizeof ( mPageIdStack ),
									 (uint32_t*) mPageIdStack );
		if ( ! res )
			CN_ReportFault ( eErrorCodes::UI_ThreadError );
		res = updateTLV_Bytes ( mTLV_DriverQueueHandle,
							    (uint32_t) eTLV_Tag::UI_PageStackPtr,
								sizeof ( mPageIdStackPtr ),
								(uint32_t*) &mPageIdStackPtr );
		if ( ! res )
			CN_ReportFault ( eErrorCodes::UI_ThreadError );
	}

}

//==============================================================================
//          createNewPage()
//
// 	Create a new page with a given target page id and add it to the stack
//		pageId:			id for the new page
//		updateIdStack:	false when reopening pages from a saved page stack
//		stackPtr:		stack pointer used when above is true
//
//  CasualNoises    03/02/2026  First implementation
//==============================================================================
void PageManager::createNewPage ( ePageId pageId )
{
	createPage ( pageId, true, mPageIdStackPtr );
}

//==============================================================================
//          handleUI_event()
//
// Handle UI events, hand over event to the current page but
//		handle exit button events local
//
//  CasualNoises    24/12/2025  First implementation
//==============================================================================
void PageManager::handleUI_event ( sIncommingUI_Event* uiEvent,
								   sSystemSettings* settingsPtr )
{

	// Handle encoder and switch events
	if (( uiEvent->encoderEvent.eventSourceID == eEventSourceDestinationID::encoderThreadSourceID) &&
		((uiEvent->encoderEvent.eventType == eEncoderEventType::encoderSwitch) ||
		 (uiEvent->encoderEvent.eventType == eEncoderEventType::encoderCount)))
	{

		// Get current 'ALT' switch state
		bool altState = ! ( uiEvent->encoderEvent.switchBitMap & (0x00000001 << (uint32_t)eSwitchNums::ALT_SWITCH ) );

		// Handle exit switch events local
		if (uiEvent->encoderEvent.encoderNo == (uint16_t)eSwitchNums::EXIT_SWITCH)
		{
			handleExitSwitch ( altState );
		}

		// Handle setup switch																		ToDo assign another function to the setup switch
		else if ( ( uiEvent->encoderEvent.encoderNo == (uint16_t)eSwitchNums::SETUP_SWITCH ) &&
				  ( mPageIdStack[mPageIdStackPtr - 1] == ePageId::mainPage ) )
		{
			handleExitSwitch ( true, false );				// pretend that this is equal to a ALT-EXIT switch press
			createPage ( ePageId::setupPage, true, mPageIdStackPtr );
		}

		// Handle LOAD switch
		else if ( ( uiEvent->encoderEvent.encoderNo == (uint16_t)eSwitchNums::LOAD_SWITCH ) &&
				  ( mPageIdStack[mPageIdStackPtr - 1] == ePageId::mainPage ) )
		{
			handleExitSwitch ( true, false );				// pretend that this is equal to a ALT-EXIT switch press
			createPage ( ePageId::loadPage, true, mPageIdStackPtr );
		}

		// Skip 'ALT' switch events, but use the state
		else if ( uiEvent->encoderEvent.encoderNo != (uint16_t)eSwitchNums::ALT_SWITCH )
		{
			mPageObjectStack[mPageIdStackPtr - 1]->handleUI_event ( uiEvent, altState, *mGraphics, settingsPtr );
		}

		// Adjust exit switch led intensity & paint screen
		setExitSwitchLedIntensity ();
		mPageObjectStack[mPageIdStackPtr - 1]->paintAll ( *mGraphics );

	// Handle ADC events
	} else if ( uiEvent->multiplexed_ADC_Event.eventSourceID == eEventSourceDestinationID::multiplexerADC_ThreadSourceID )
	{

		// Try current page to handle the event
		if ( mPageObjectStack[mPageIdStackPtr - 1]->handleUI_event ( uiEvent, false, *mGraphics, settingsPtr ) )
		{
			 mPageObjectStack[mPageIdStackPtr - 1]->paintAll ( *mGraphics );
		}
		else

		// Otherwise process the event here...
		{

			// Send ADC data over NerveNet
			forwardADC_Event ( uiEvent, settingsPtr );

		}

	// Handle NerveNet events
//	} else if ( uiEvent->nerveNetEvent.eventSourceID == eEventSourceID::nerveNetSourceID )
	} else if ( uiEvent->nerveNetEvent.eventSourceID == eEventSourceDestinationID::nerveNetNorthSideSourceID )
	{

		// Get first event ptr and no of bytes to process
		tNerveNetMessageHeader* headerPtr 	= (tNerveNetMessageHeader*) uiEvent->nerveNetEvent.eventdataPtr;
		uint32_t eventSize 					= uiEvent->nerveNetEvent.eventLength;

		// Process all data
		while ( eventSize > 0 )
		{

			// Extract a single event
			sIncommingUI_Event event;
//			event.nerveNetEvent.eventSourceID = eEventSourceID::nerveNetSourceID;
			event.nerveNetEvent.eventSourceID = eEventSourceDestinationID::nerveNetNorthSideSourceID;
			event.nerveNetEvent.eventLength	  = sizeof ( sIncommingUI_Event );
			event.nerveNetEvent.eventdataPtr  = (uint8_t*) headerPtr;

			// Try current page to handle the event
			if ( ! mPageObjectStack[mPageIdStackPtr - 1]->handleUI_event ( &event, false, *mGraphics, settingsPtr ) )

			// Otherwise process the event here...
			{
				handleNerveNetEvent ( headerPtr );
			}

			// Next event
			uint32_t size = headerPtr->messageLength;
			eventSize -= size;
			headerPtr = (tNerveNetMessageHeader*)((uint8_t*)headerPtr + size);

		}

	// Unknown event source?
	} else
	{
		CN_ReportFault(eErrorCodes::UI_ThreadError);
	}

}

//==============================================================================
//          saveContext
//
// Save the context of the top page
// This method is called from the main UI thread loop about every sec
//
//  CasualNoises    15/03/2026  First implementation
//==============================================================================
void PageManager::saveContext ()
{
	mPageObjectStack [ mPageIdStackPtr - 1 ]->saveContext();
}

//==============================================================================
//          setExitSwitchLedIntensity()
//
// Set the intensity of the exit & ALT switch according to the page
//
//  CasualNoises    25/12/2025  First implementation
//==============================================================================
void PageManager::setExitSwitchLedIntensity ()
{

	// Dim all switch LED's except for exit & ALT switch
	eLED_BitNums leds[] = {
		eLED_BitNums::EXIT_SWITCH, eLED_BitNums::SWITCH_1
	};
	sLED_Event event;
	event.ledIntensity = 100;
	if ( mPageIdStackPtr == 1 )
		event.ledIntensity = 5;
	for ( auto led : leds )
	{
		event.ledBitNum = (uint32_t)led;
		BaseType_t res = xQueueSend ( gYellowPages.gLED_ThreadQueueHandle, &event, 10 );
		if (res != pdPASS)
			CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);
	}

}

//==============================================================================
//          handleExitSwitch()
//
// Go to previous page on the stack, if any and update the exit switch led
//
//  CasualNoises    24/12/2025  First implementation
//==============================================================================
void PageManager::handleExitSwitch(bool altState, bool doPaint)									// ToDo: go to main page when altState is true
{

	// When ALT is pressed, close all pages except for the mainPage
	if ( altState )
	{
		for (uint32_t i = 1; i < mPageIdStackPtr; ++i)
		{
			if (mPageObjectStack[i] != nullptr)
			{
				mPageObjectStack[i]->saveContext();
				delete mPageObjectStack[i];
				mPageObjectStack[i] = nullptr;
				mPageIdStack[i] = ePageId::empty;
			}
		}
		mPageIdStackPtr = 1;
	}

	// Dim exit switch led when this is the top page
	sLED_Event event;
	event.ledBitNum    = (uint32_t)eLED_BitNums::EXIT_SWITCH;
	event.ledIntensity = 100;
	if (mPageIdStackPtr == 1)
		event.ledIntensity = 5;
	BaseType_t res = xQueueSend( gYellowPages.gLED_ThreadQueueHandle, &event, 10 );
	if (res != pdPASS)
		CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);

	// If in any other pages than the top page, close this page ... otherwise ignore event
	if ( mPageIdStackPtr > 1 )
	{
		mPageIdStackPtr -= 1;
		delete mPageObjectStack[mPageIdStackPtr];
		mPageObjectStack[mPageIdStackPtr] = nullptr;
		mPageIdStack[mPageIdStackPtr] = ePageId::empty;
	}

	// Save new state in flash
	bool success = updateTLV_Bytes ( mTLV_DriverQueueHandle,
								 	 (uint32_t) eTLV_Tag::UI_PageStack,
									 sizeof ( mPageIdStack ),
									 (uint32_t*) mPageIdStack );
	if ( ! success )
		CN_ReportFault ( eErrorCodes::UI_ThreadError );
	success = updateTLV_Bytes ( mTLV_DriverQueueHandle,
						    	(uint32_t) eTLV_Tag::UI_PageStackPtr,
								sizeof ( mPageIdStackPtr ),
								(uint32_t*) &mPageIdStackPtr );
	if ( ! success )
		CN_ReportFault ( eErrorCodes::UI_ThreadError );

	// Show new top page
	if (doPaint)
	{
		mPageObjectStack[mPageIdStackPtr - 1]->resized();
		mPageObjectStack[mPageIdStackPtr - 1]->paintAll(*mGraphics);
	}

}

//==============================================================================
//          forwardADC_Event()
//
// Forward event to both North & South Side
//  CasualNoises    22/01/2026  First implementation
//==============================================================================
void PageManager::forwardADC_Event ( sIncommingUI_Event* uiEvent,
		   	   	   	   	   	   	   	 sSystemSettings* settingsPtr )
{

	// Apply calibration
	sMultiplexed_ADC_Event* eventPtr = &uiEvent->multiplexed_ADC_Event;
	sCalibrationData* calibrationPtr = &settingsPtr->calibrationData;
	if ( ! calibrationPtr->maskFlags [ eventPtr->multiplexerNo ] [ eventPtr->multiplexerChannelNo ] )
		return;
	float value = jmap ( (float) eventPtr->value,
						 (float) calibrationPtr->minValues [ eventPtr->multiplexerNo ] [ eventPtr->multiplexerChannelNo ],
						 (float) calibrationPtr->maxValues [ eventPtr->multiplexerNo ] [ eventPtr->multiplexerChannelNo ],
						 (float) 0x0000,
						 (float) 0xffff );

	// Send NerveNet event
	tPotValueMessage message;
	message.header.sourceID			= eNerveNetSourceId::FellhornDeviceBoard;
	message.header.destinationID	= eNerveNetSourceId::FellhornBothSides;
	message.header.messageTag		= (uint32_t) eSynthEngineMessageType::potentiometerValue;
	message.header.messageLength 	= sizeof ( tPotValueMessage );
	message.multiplexerNo			= eventPtr->multiplexerNo;
	message.multiplexerChannelNo	= eventPtr->multiplexerChannelNo;
	message.potValue				= (uint32_t) value;
	message.deviation				= eventPtr->deviation;
	gNerveNetMasterThreadPtr[0]->sendMessage ( &message, sizeof ( tPotValueMessage ) );

}

//==============================================================================
//          handleNerveNetEvent()
//
//  CasualNoises    02/02/2026  First implementation
//==============================================================================
void PageManager::handleNerveNetEvent ( tNerveNetMessageHeader* eventPtr )
{
	eSynthEngineMessageType type = (eSynthEngineMessageType) eventPtr->messageTag;
	switch ( type )
	{
	case eSynthEngineMessageType::requestSetupInfo:
		break;
	default:
		break;		// Just ignore unhandled events
	}
}

} // namespace CasualNoises
