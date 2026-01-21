/*
  ==============================================================================

    UI_Thread.cpp

    Handles oled display, encoders, potentiometers and led's

    Created: 15/02/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "UI_Thread.h"

#include <UI_Handlers/PageManager.h>
#include <UI_Handlers/RootPage.h>
#include <Threads/TLV_DriverThread.h>
#include <NerveNet/NerveNetMessage.h>
#include <NerveNet/NerveNetMasterThread.h>

#include "UI_Handlers/MainPage.h"
#include "SystemConfig.h"

#include "YellowPages.h"

#include <SynthEngineMessage.h>

namespace CasualNoises
{

//==============================================================================
//          handleNerveNetCallBacks ()
//
// Handle incoming data from NerveNet
//
//  CasualNoises    20/07/2025  First implementation
//==============================================================================
void handleNerveNetCallBacks ( uint32_t size, uint8_t* ptr )
{
	// ToDo implement GUI
//	uint32_t size 	 = ptr->size;
//	uint8_t* dataPtr = ptr->data;
}

//==============================================================================
//          createInitialTLVs ()
//
//  CasualNoises    15/01/2026  First implementation
//==============================================================================
void createInitialTLVs ( QueueHandle_t TLV_DriverQueue )
{

	// Default calibration values
	uint32_t index = findNextTLV ( TLV_DriverQueue, (uint32_t)eTLV_Tag::CalibrationValues, 0 );
	if ( index == 0 )
	{

		sCalibrationData calibrationValues;
		for (uint32_t i = 0; i < NO_OF_ADC_MULTIPLEXERS; ++i)
		{
			for (uint32_t j = 0; j < NO_OF_ADC_MULTI_CHANNELS; ++j)
			{
				if ( i == 0 )
				{
					calibrationValues.minValues[i][j] = 0x00000000;
					calibrationValues.maxValues[i][j] = 0x0000ffff;
				} else
				{
					calibrationValues.minValues[i][j] = 0x0000ffff;
					calibrationValues.maxValues[i][j] = 0x00000000;
				}
				calibrationValues.maskFlags[i][j] = true;
			}
		}
		calibrationValues.maskFlags[0][(uint32_t)eMultiplexerChannel_0::P_NC_1] = false;
		calibrationValues.maskFlags[0][(uint32_t)eMultiplexerChannel_0::P_NC_2] = false;
		calibrationValues.maskFlags[0][(uint32_t)eMultiplexerChannel_0::P_NC_3] = false;
		calibrationValues.maskFlags[0][(uint32_t)eMultiplexerChannel_0::P_NC_4] = false;

		addTLV_Bytes ( TLV_DriverQueue, (uint32_t)eTLV_Tag::CalibrationValues, sizeof ( sCalibrationData ), (uint32_t*) &calibrationValues );

	}

}

//==============================================================================
//          UI_Thread()
//
// 	Create UI related threads
//	Handle all UI events
//
//  CasualNoises    15/02/2025  First implementation
//  CasualNoises    08/01/2026  Adapted for Fellhorn
//==============================================================================
void UI_Thread(void* pvParameters)
{

	// Get parameters
	UI_ThreadData* params = (UI_ThreadData*)pvParameters;
	void ( *funcPtr ) ( uint32_t size, uint8_t* ptr ) = &handleNerveNetCallBacks;
	params->nerveNetCallBackPtr = &funcPtr;

	// Create an OLED driver
	CasualNoises::sSSD1309_ConfigData* ssd1309_DataPtr = &params->oledConfigData;
	CasualNoises::SSD1309_Driver* oledDriverPtr = new CasualNoises::SSD1309_Driver(ssd1309_DataPtr);

	// Display initialization screen
	oledDriverPtr->drawRect(0, 0, 127, 63);
	oledDriverPtr->drawText(10, 10, "Starting-up...", &CasualNoises::font_7x10);
	oledDriverPtr->refreshDisplay();
	osDelay(pdMS_TO_TICKS(1000));

	// Create a thread for led handling
	BaseType_t res;
	TaskHandle_t xLED_ThreadHandle;
	res = start_LED_Thread ( (void *)&params->spi_LED_ThreadData, &xLED_ThreadHandle );
	if (res != pdPASS)
		CN_ReportFault ( eErrorCodes::UI_ThreadError );

	// Create a TLV driver
	CasualNoises::sNVM_DriverInitData* NVM_DataPtr = &params->nvmDriverInitData;
	CasualNoises::W25Qxx_Driver* NVM_DriverPtr = new CasualNoises::W25Qxx_Driver(NVM_DataPtr);

	// Create a thread to manage TLV's in flash memory
	sTLV_DriverThreadParams _sTLV_DriverThreadParams;
	TaskHandle_t xTLV_DriverThreadHandle;
	_sTLV_DriverThreadParams.NVM_DriverPtr 	= NVM_DriverPtr;
	_sTLV_DriverThreadParams.runningFlagPtr	= &gYellowPages.gTLV_DriverThreadThreadRunning;
	_sTLV_DriverThreadParams.queueHandlePtr	= &gYellowPages.gTLV_DriverThreadQueueHandle;
	res = startTLV_DriverThread ( &_sTLV_DriverThreadParams, &xTLV_DriverThreadHandle );
	if ( res != pdPASS )
		CN_ReportFault ( eErrorCodes::UI_ThreadError );

	// Wait until the TLV driver thread is up and running...
	while ( ! gYellowPages.gTLV_DriverThreadThreadRunning )
	{
		vTaskDelay ( pdMS_TO_TICKS ( 1 ) );
	}
	QueueHandle_t driverQueueHandle	= gYellowPages.gTLV_DriverThreadQueueHandle;

	// ToDo		comment out this line
//	deleteAllTLVs ( driverQueueHandle );

	// Check TLV consistency
	uint32_t noOfBlocks = getTotalNoOfTLVs ( driverQueueHandle );
	if (noOfBlocks == 0)
	{
		oledDriverPtr->clearDisplay();
		oledDriverPtr->drawRect(0, 0, 127, 63);
		oledDriverPtr->drawText(5, 10, "Flash fail!", &CasualNoises::font_11x18);
		oledDriverPtr->drawText(20, 40, "erasing flash", &CasualNoises::font_7x10);
		oledDriverPtr->refreshDisplay();
		deleteAllTLVs ( driverQueueHandle );
		osDelay(pdMS_TO_TICKS(5000));
	}

	// Initial UI state
	static UI_StateData UI_State;
	UI_State.version	= 0x312e3076;			// v0.1
	UI_State.count		= 0;
	uint32_t* ptr 		= ( uint32_t* ) &UI_State;

	// Create initial TLV's
	createInitialTLVs ( driverQueueHandle );

	// TLV used to hold the current UI state
	bool saveState = false;
	uint32_t index = findNextTLV ( driverQueueHandle, (uint32_t)eTLV_Tag::UI_CurrentState, 0 );
	if ( index == 0 )																// TLV does not exists, create new one
	{
		saveState = true;
	}
	else if ( getTLV_LengthBytes ( driverQueueHandle, index ) != sizeof ( UI_StateData ) )
	{
		deleteTLV ( driverQueueHandle, (uint32_t) eTLV_Tag::UI_CurrentState, true );
		saveState = true;
	}
	if (saveState)																	// Save the new TLV
	{
		addTLV_Bytes ( driverQueueHandle, (uint32_t) eTLV_Tag::UI_CurrentState, sizeof ( UI_StateData ), ptr );
	}

	// Update cycle counter
	readTLV_TagBytes (  driverQueueHandle, (uint32_t) eTLV_Tag::UI_CurrentState, sizeof ( UI_StateData ), ptr );
	UI_State.count += 1;
	updateTLV_Bytes ( driverQueueHandle, (uint32_t) eTLV_Tag::UI_CurrentState, sizeof ( UI_StateData ), ptr  );

	// Create a queue to receive events from encoder and other threads
	const uint32_t cQueueLength = 10;
	QueueHandle_t xUI_ThreadQueue = xQueueCreate ( cQueueLength, sizeof(sIncommingUI_Event) );
	if (xUI_ThreadQueue == nullptr)
		CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);
	gYellowPages.gUI_ThreadQueueHandle = xUI_ThreadQueue;

	// Create an encoder/switches thread
	TaskHandle_t xEncoderThreadHandle;
	params->spiEncoderThreadData.clientQueue = xUI_ThreadQueue;
	res = startEncoderThread ( (void *)&params->spiEncoderThreadData, &xEncoderThreadHandle );
	if (res != pdPASS)
		CN_ReportFault ( eErrorCodes::UI_ThreadError );

	// Wait until the led handler thread is up and running...
	while ( ! gYellowPages.gLED_ThreadRunning )
	{
		vTaskDelay(pdMS_TO_TICKS(1));
	}

	// Set all led's to there initial state
	constexpr eLED_BitNums initial_leds[] = {
			eLED_BitNums::SWITCH_1, eLED_BitNums::SWITCH_4, eLED_BitNums::SWITCH_5
	};
	for (auto led : initial_leds)
	{
		sLED_Event event;
		event.ledBitNum    = (uint32_t)led;
		event.ledIntensity = 100;
		BaseType_t res = xQueueSend(gYellowPages.gLED_ThreadQueueHandle, &event, 10);
		if (res != pdPASS)
			CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);
	}
	constexpr eLED_BitNums intensity_leds1[] = {
			eLED_BitNums::FADER_1, eLED_BitNums::FADER_2, eLED_BitNums::FADER_3, eLED_BitNums::FADER_4,
			eLED_BitNums::FADER_5, eLED_BitNums::FADER_6, eLED_BitNums::FADER_7, eLED_BitNums::FADER_8
	};
	for (auto led : intensity_leds1)
	{
		sLED_Event event;
		event.ledBitNum    = (uint32_t)led;
		event.ledIntensity = 50;
		BaseType_t res = xQueueSend(gYellowPages.gLED_ThreadQueueHandle, &event, 10);
		if (res != pdPASS)
			CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);
	}
	constexpr eLED_BitNums intensity_leds2[] = {
			eLED_BitNums::SWITCH_2, eLED_BitNums::SWITCH_3, eLED_BitNums::SWITCH_6,
			eLED_BitNums::EXIT_SWITCH
	};
	for (auto led : intensity_leds2)
	{
		sLED_Event event;
		event.ledBitNum    = (uint32_t)led;
		event.ledIntensity = 5;
		BaseType_t res = xQueueSend(gYellowPages.gLED_ThreadQueueHandle, &event, 10);
		if (res != pdPASS)
			CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);
	}

	// Create a multiplexed ADC thread to handle potentiometers and sliders
	TaskHandle_t xMultiplexed_ADC_ThreadHandle;
	params->ADC_MultiplexerThreadData.clientQueue = xUI_ThreadQueue;
	res = startMultiplexed_ADC_Thread ( (void *)&params->ADC_MultiplexerThreadData, &xMultiplexed_ADC_ThreadHandle );
	if (res != pdPASS)
		CN_ReportFault(eErrorCodes::UI_ThreadError);

	// Report UI thread ready for duty
	gYellowPages.gUI_ThreadRunning = true;

	// Wait for the NerveNet thread to become online
	while ( ! gYellowPages.gNetMasterThreadRunning )
		vTaskDelay ( pdMS_TO_TICKS ( 1 )  );

	// Send initial messages to the North Side
	tInitMessage initMessage;
	initMessage.header.messageTag 	 = (uint32_t) eSynthEngineMessageType::initSynthEngine;
	initMessage.header.messageLength = sizeof ( tInitMessage );
	initMessage.initMessage			 = eSynthEngineInitType::hardInit;
	bool success = gNerveNetMasterThreadPtr[0]->sendMessage ( &initMessage, sizeof ( tInitMessage ) );
	if ( ! success)
		CN_ReportFault(eErrorCodes::NerveNetThread_Error);

	// Send setup info request
	tRequestSetupInfoMessageData setupMessage;
	setupMessage.header.messageTag 	 = (uint32_t) eSynthEngineMessageType::requestSetupInfo;
	setupMessage.header.messageLength = sizeof ( tRequestSetupInfoMessageData );
	success = gNerveNetMasterThreadPtr[0]->sendMessage ( &setupMessage, sizeof(tRequestSetupInfoMessageData) );
	if ( ! success )
		CN_ReportFault(eErrorCodes::NerveNetThread_Error);

	// Display start-up screen
	osDelay ( 500 );
	oledDriverPtr->clearDisplay ();
	oledDriverPtr->drawRect ( 0, 0, 127, 63 );
	oledDriverPtr->drawText ( 10, 10, "CasualNoises", &CasualNoises::font_7x10 );
	oledDriverPtr->drawText ( 20, 30, "Fellhorn", &CasualNoises::font_11x18 );
	oledDriverPtr->refreshDisplay ();
	osDelay ( pdMS_TO_TICKS ( 1000 ) );

	// Flush event queue, delete all TLV's if ALT & SAVE switches are pressed at boot
    res = pdPASS;
	sIncommingUI_Event event;
	while (res == pdPASS)
	{
		res = xQueueReceive ( xUI_ThreadQueue, (void *) &event, 1000 );
		if ( res == pdPASS)
		{
			if ( event.encoderEvent.eventSourceID == eEventSourceID::encoderThreadSourceID )
			{
				bool altState  = ! ( event.encoderEvent.switchBitMap & ( 0x00000001 << (uint32_t)eSwitchBitmapPos::ALT_SWITCH ) );
				bool saveState = ! ( event.encoderEvent.switchBitMap & ( 0x00000001 << (uint32_t)eSwitchBitmapPos::SAVE_SWITCH ) );
				if ( altState && saveState )
				{
					deleteAllTLVs ( driverQueueHandle );
				}
			}
		}
		osDelay ( pdMS_TO_TICKS ( 10 ) );
	}

	// Enter last saved display page
	PageManager* pageManagerPtr = new PageManager ( oledDriverPtr, driverQueueHandle );

	// ToDo: move this code to a better place
	size_t xFreeHeapSize = xPortGetFreeHeapSize();
	uint32_t used = getInitFreeHeapSize() - xFreeHeapSize;
	UNUSED ( used );

	UBaseType_t numTasks = uxTaskGetNumberOfTasks();
	TaskStatus_t *taskArray = (TaskStatus_t *)pvPortMalloc(numTasks * sizeof(TaskStatus_t));
	uint32_t totalRunTime;
	numTasks = uxTaskGetSystemState(taskArray, numTasks, &totalRunTime);
	for (UBaseType_t i = 0; i < numTasks; i++)
	{
		const char* name = taskArray[i].pcTaskName;
		UNUSED ( name );
		eTaskState state = taskArray[i].eCurrentState;
		UNUSED ( state );
		UBaseType_t prio = taskArray[i].uxCurrentPriority;
		UNUSED ( prio );
		vTaskDelay ( pdMS_TO_TICKS ( 1 ) );
	}
	vPortFree(taskArray);

	// Wait for NerveNet master thread to become idle
	while ( ( gNerveNetMasterThreadPtr[0] == nullptr ) ||
			( ! gNerveNetMasterThreadPtr[0]->isThreadRunning () ) )
	{
		vTaskDelay ( pdMS_TO_TICKS ( 10 ) );
	}

	// Main thread loop
	for (;;)
	{
		vTaskDelay ( pdMS_TO_TICKS ( 1 ) );

		// Handle incoming events
		res = xQueueReceive ( xUI_ThreadQueue, (void *)&event, 20 );
		if (res == pdPASS)
		{
			pageManagerPtr->handleUI_event ( &event );
		}
		else
		{

			gNerveNetMasterThreadPtr[0]->startNewDataExchange ();
		}

	}

}

//==============================================================================
//          Start_UI_Thread()
//
// StartGUI thread
//
//  CasualNoises    15/02/2025  First implementation
//==============================================================================
BaseType_t Start_UI_Thread(UI_ThreadData *argument)
{

	// Create the thread to run the UI
	TaskHandle_t xHandlePtr;
	BaseType_t res = xTaskCreate(UI_Thread,	"UI_Thread", UI_THREAD_STACK_SIZE, argument,
			UI_THREAD_PRIORITY,	&xHandlePtr);
//	gYellowPages.gUI_ThreadTaskHandle = xHandlePtr;
	return res;

}

} // namespace CasualNoises
