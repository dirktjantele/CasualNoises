/*
  ==============================================================================

    UI_Thread.cpp

    Handles oled display, encoders, potentiometers and led's

    Created: 15/02/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include <UI_Handlers/PageManager.h>
#include <UI_Handlers/RootPage.h>
#include "UI_Thread.h"

#include "SystemConfig.h"

#include "YellowPages.h"

namespace CasualNoises
{

// Pointer to the current display page handler
//static DisplayHandlerRoot* gCurrentDisplayHandlerPtr	{ nullptr };

//==============================================================================
//          handleNerveNetCallBacks()
//
// Handle incoming data from NerveNet
//
//  CasualNoises    20/07/2025  First implementation
//==============================================================================
/*
void handleNerveNetCallBacks(CasualNoises::sNerveNetData* ptr)
{
	// ToDo implement GUI
//	uint32_t size 	 = ptr->size;
//	uint8_t* dataPtr = ptr->data;
}
*/

//==============================================================================
//          UI_Thread()
//
// Start all threads except for the TriggerThread which is started first in main()
// ... handle all UI events
//
//  CasualNoises    15/02/2025  First implementation
//==============================================================================
void UI_Thread(void* pvParameters)
{

	// Get parameters
	UI_ThreadData* params = (UI_ThreadData*)pvParameters;
//	void (*funcPtr)(CasualNoises::sNerveNetData* ptr) = &handleNerveNetCallBacks;
//	params->nerveNetCallBackPtr = &funcPtr;

	// Create an oled driver
	CasualNoises::sSSD1309_ConfigData* ssd1309_DataPtr = &params->oledConfigData;
	CasualNoises::SSD1309_Driver* oledDriverPtr = new CasualNoises::SSD1309_Driver(ssd1309_DataPtr);

	// Display initialization screen
	oledDriverPtr->drawRect(0, 0, 127, 63);
	oledDriverPtr->drawText(10, 10, "Starting-up...", &CasualNoises::font_7x10);
	oledDriverPtr->refreshDisplay();
	osDelay(pdMS_TO_TICKS(1000));

	// Create a thread for led handling
	TaskHandle_t xLED_ThreadHandle;
	BaseType_t res = start_LED_Thread((void *)&params->spi_LED_ThreadData, &xLED_ThreadHandle);
	if (res != pdPASS)
		CN_ReportFault(eErrorCodes::UI_ThreadError);

	// Create a TLV driver
	CasualNoises::sNVM_DriverInitData* NVM_DataPtr = &params->nvmDriverInitData;
	CasualNoises::W25Qxx_Driver* NVM_DriverPtr = new CasualNoises::W25Qxx_Driver(NVM_DataPtr);
	CasualNoises::TLV_Driver*	 TLV_DriverPtr = new CasualNoises::TLV_Driver(NVM_DriverPtr);

	// Check TLV consistency
	uint32_t noOfBlocks = TLV_DriverPtr->getTotalNoOfTLVs();
	if (noOfBlocks == 0)
	{
		oledDriverPtr->clearDisplay();
		oledDriverPtr->drawRect(0, 0, 127, 63);
		oledDriverPtr->drawText(5, 10, "Flash fail!", &CasualNoises::font_11x18);
		oledDriverPtr->drawText(20, 40, "erasing flash", &CasualNoises::font_7x10);
		oledDriverPtr->refreshDisplay();
		TLV_DriverPtr->deleteAllTLVs();
		osDelay(pdMS_TO_TICKS(5000));
	}

	// Initial UI state
	UI_StateData UI_State;
	UI_State.version			= 0x312e3076;			// v0.1

//	TLV_DriverPtr->deleteAllTLVs();																	// ToDo remove this line

	// TLV used to hold the current UI state
	bool saveState = false;
	uint32_t index = TLV_DriverPtr->findNextTLV((uint32_t)eTLV_Tag::UI_CurrentState, 0);
	if (index == 0)				// TLV does not exists, create new one
	{
		saveState = true;
	}
	else if (TLV_DriverPtr->getTLV_LengthBytes(index) != sizeof(UI_StateData))
	{
		TLV_DriverPtr->deleteTLV((uint32_t)eTLV_Tag::UI_CurrentState, true);	// Delete existing TLV
		saveState = true;
	}
	if (saveState)
	{
		if ( ! TLV_DriverPtr->addTLV_Bytes((uint32_t)eTLV_Tag::UI_CurrentState, sizeof(UI_StateData), ( uint32_t*) &UI_State) )
			CN_ReportFault(eErrorCodes::UI_ThreadError);
	}


//	bool bres = TLV_DriverPtr->updateTLV((uint32_t)eTLV_Tag::UI_CurrentState, sizeof(UI_StateData),( uint32_t*) &UI_State);
//	if (! bres)
//		CN_ReportFault(eErrorCodes::UI_ThreadError);

	// Create a queue to receive events from encoder and other threads
	const uint32_t cQueueLength = 10;
	QueueHandle_t xUI_ThreadQueue = xQueueCreate( cQueueLength, sizeof(sIncommingUI_Event) );
	if (xUI_ThreadQueue == nullptr)
		CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);
	gYellowPages.gUI_ThreadQueueHandle = xUI_ThreadQueue;

	// Create an encoder/switches thread
	TaskHandle_t xEncoderThreadHandle;
	params->spiEncoderThreadData.clientQueue = xUI_ThreadQueue;
	res = startEncoderThread((void *)&params->spiEncoderThreadData, &xEncoderThreadHandle);
	if (res != pdPASS)
		CN_ReportFault(eErrorCodes::UI_ThreadError);

	// Wait for the led handler thread is up and running...
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
	res = startMultiplexed_ADC_Thread((void *)&params->ADC_MultiplexerThreadData, &xMultiplexed_ADC_ThreadHandle);
	if (res != pdPASS)
		CN_ReportFault(eErrorCodes::UI_ThreadError);

	// Report UI thread ready for duty
	gYellowPages.gUI_ThreadRunning = true;
/*
	// Wait for the NerveNet thread to become online
	while ( ! gYellowPages.gNetMasterThreadRunning)
		vTaskDelay(pdMS_TO_TICKS(1));

	// Send init synth engine message to the SouthSide
	tInitMessage initMessage;
	initMessage.header.messageTag 	 = (uint32_t)eSynthEngineMessageType::initSynthEngine;
	initMessage.header.messageLength = sizeof(tInitMessage);
	initMessage.initMessage			 = eSynthEngineInitType::hardInit;
	bool success = gNerveNetMasterThreadPtr[0]->sendMessage(&initMessage, sizeof(tInitMessage));
	if ( ! success)
		CN_ReportFault(eErrorCodes::NerveNetThread_Error);

	// Send setup info request
	tRequestSetupInfoMessageData setupMessage;
	setupMessage.header.messageTag 	 = (uint32_t)eSynthEngineMessageType::requestSetupInfo;
	setupMessage.header.messageLength = sizeof(tRequestSetupInfoMessageData);
	success = gNerveNetMasterThreadPtr[0]->sendMessage(&setupMessage, sizeof(tRequestSetupInfoMessageData));
	if ( ! success)
		CN_ReportFault(eErrorCodes::NerveNetThread_Error);

	// Enter main display
	gCurrentDisplayHandlerPtr = new MainDisplayHandler();
	gCurrentDisplayHandlerPtr->openDisplayPage(oledDriverPtr);
*/

	// Display start-up screen																ToDo move this into the display handler
	osDelay(500);
	oledDriverPtr->clearDisplay();
	oledDriverPtr->drawRect(0, 0, 127, 63);
	oledDriverPtr->drawText(10, 10, "CasualNoises", &CasualNoises::font_7x10);
	oledDriverPtr->drawText(20, 30, "Fellhorn", &CasualNoises::font_11x18);
	oledDriverPtr->refreshDisplay();
	osDelay(pdMS_TO_TICKS(1000));

	// Enter last saved display page
	PageManager* pageManagerPtr = new PageManager(oledDriverPtr, TLV_DriverPtr);

	// Main thread loop
	for (;;)
	{
		osDelay(1);

		// Handle incoming events
		sIncommingUI_Event event;
		BaseType_t res = xQueueReceive(xUI_ThreadQueue, (void *)&event, 1000);
		if (res == pdPASS)
		{
			pageManagerPtr->handleUI_event(&event);
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
	BaseType_t res = xTaskCreate(UI_Thread,	"UI_Thread", DEFAULT_STACK_SIZE, argument,
			UI_THREAD_PRIORITY,	&xHandlePtr);
//	gYellowPages.gUI_ThreadTaskHandle = xHandlePtr;
	return res;

}

} // namespace CasualNoises
