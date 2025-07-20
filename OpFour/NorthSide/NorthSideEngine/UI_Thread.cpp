/*
  ==============================================================================

    UI_Thread.cpp

    Handles oled display, encoders, potentiometers and led's

    Created: 15/02/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "UI_Thread.h"

#include "SystemConfig.h"

#include "YellowPages.h"

#include "../../Common Sources/SynthEngineMessage.h"

#include "UI_DisplayHandlers/MainDisplayHandler.h"

namespace CasualNoises
{

// Pointer to the current display page handler
DisplayHandlerRoot*				currentDisplayHandlerPtr	{ nullptr };

//==============================================================================
//          handleNerveNetCallBacks()
//
// Handle incoming data from NerveNet
//
//  CasualNoises    20/07/2025  First implementation
//==============================================================================
void handleNerveNetCallBacks(CasualNoises::sNerveNetData* ptr)
{
	uint32_t size 	 = ptr->size;
	if (size > 0)
	{
		uint8_t* dataPtr = ptr->data;
	}
}

//==============================================================================
//          UI_Thread()
//
// Start all threads except for the TriggerThread which is started first
// ... handle all UI events
//
//  CasualNoises    15/02/2025  First implementation
//==============================================================================
void UI_Thread(void* pvParameters)
{

	UI_ThreadData* params = (UI_ThreadData*)pvParameters;
	void (*funcPtr)(CasualNoises::sNerveNetData* ptr) = &handleNerveNetCallBacks;
	params->nerveNetCallBackPtr = &funcPtr;

	// Create a TLV driver
//	CasualNoises::sNVM_DriverInitData* NVM_DataPtr = &params->nvmDriverInitData;
//	CasualNoises::W25Qxx_Driver* NVM_DriverPtr = new CasualNoises::W25Qxx_Driver(NVM_DataPtr);
//	CasualNoises::TLV_Driver*	 TLV_DriverPtr = new CasualNoises::TLV_Driver(NVM_DriverPtr);

	// Create an oled driver
	CasualNoises::sSSD1309_ConfigData* ssd1309_DataPtr = &params->oledConfigData;
	CasualNoises::SSD1309_Driver* oledDriverPtr = new CasualNoises::SSD1309_Driver(ssd1309_DataPtr);

	// Display start-up screen
	oledDriverPtr->drawRect(0, 0, 127, 63);
	oledDriverPtr->drawText(10, 10, "CasualNoises", &CasualNoises::font_7x10);
	oledDriverPtr->drawText(20, 30, "Fellhorn", &CasualNoises::font_11x18);
	oledDriverPtr->refreshDisplay();

	// Create a queue to receive events from encoder and other threads
	const uint32_t cQueueLength = 10;
	QueueHandle_t xUI_ThreadQueue = xQueueCreate( cQueueLength, sizeof(sIncommingUI_Event) );
	if (xUI_ThreadQueue == nullptr)
		CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);
	gYellowPages.gUI_ThreadQueueHandle = xUI_ThreadQueue;

	// Create an encoder/switches thread
	TaskHandle_t xEncoderThreadHandle;
	params->spiEncoderThreadData.clientQueue = xUI_ThreadQueue;
	BaseType_t res = startEncoderThread((void *)&params->spiEncoderThreadData, &xEncoderThreadHandle);
	if (res != pdPASS)
		CN_ReportFault(eErrorCodes::UI_ThreadError);

	// Create a thread for led handling
	TaskHandle_t xLED_ThreadHandle;
	res = start_LED_Thread((void *)&params->spi_LED_ThreadData, &xLED_ThreadHandle);
	if (res != pdPASS)
		CN_ReportFault(eErrorCodes::UI_ThreadError);

	// Wait for the led handler thread is up and running...
	while ( ! gYellowPages.gLED_ThreadRunning )
	{
		vTaskDelay(pdMS_TO_TICKS(1));
	}

	// Set all led's to there initial brightness
	constexpr eLED_BitNums all_leds[] = {
			eLED_BitNums::LED_1_RED, eLED_BitNums::LED_1_BLUE, eLED_BitNums::LED_1_GREEN,
			eLED_BitNums::LED_2_RED, eLED_BitNums::LED_2_BLUE, eLED_BitNums::LED_2_GREEN,
			eLED_BitNums::LED_3_RED, eLED_BitNums::LED_3_BLUE, eLED_BitNums::LED_3_GREEN,
			eLED_BitNums::LED_4_RED, eLED_BitNums::LED_4_BLUE, eLED_BitNums::LED_4_GREEN,
			eLED_BitNums::LED_5,
			eLED_BitNums::SWITCH_1_LED, eLED_BitNums::SWITCH_2_LED, eLED_BitNums::SWITCH_3_LED, eLED_BitNums::SWITCH_4_LED
	};
	for (auto led : all_leds)
	{
		sLED_Event event;
		event.ledBitNum    = (uint32_t)led;
		event.ledIntensity = 0;
		BaseType_t res = xQueueSend(gYellowPages.gLED_ThreadQueueHandle, &event, 10);
		if (res != pdPASS)
			CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);
	}
	constexpr eLED_BitNums dimmed_leds[] = {
			eLED_BitNums::LED_1_BLUE, eLED_BitNums::LED_2_BLUE,
			eLED_BitNums::LED_3_BLUE, eLED_BitNums::LED_4_BLUE,
			eLED_BitNums::SWITCH_1_LED, eLED_BitNums::SWITCH_2_LED, eLED_BitNums::SWITCH_3_LED, eLED_BitNums::SWITCH_4_LED
	};
	for (auto led : dimmed_leds)
	{
		sLED_Event event;
		event.ledBitNum    = (uint32_t)led;
		event.ledIntensity = 5;
		BaseType_t res = xQueueSend(gYellowPages.gLED_ThreadQueueHandle, &event, 10);
		if (res != pdPASS)
			CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);
	}

	// Create a potentiometer handler thread
	TaskHandle_t xPotentiometerThreadHandle;
	params->potentiometerThreadData.clientQueueHandle = xUI_ThreadQueue;
	res = startPotentiometerThread((void *)&params->potentiometerThreadData, &xPotentiometerThreadHandle);

	// Create an ADC handler thread - CV inputs are handled by the southside ToDo: remove this code
	TaskHandle_t xADC_ThreadHandle;
	params->ADC_ThreadData.clientQueueHandle = xUI_ThreadQueue;
	res = startADC_Thread((void *)&params->ADC_ThreadData, &xADC_ThreadHandle);

	// Short delay while the init display is visual
	vTaskDelay(pdMS_TO_TICKS(1000));

	// Report UI thread ready for duty
	gYellowPages.gUI_ThreadRunning = true;

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
	currentDisplayHandlerPtr = new MainDisplayHandler();
	currentDisplayHandlerPtr->openDisplayPage(oledDriverPtr);

	// Main thread loop
	for (;;)
	{

		// Handle incoming events
		bool refresh = false;
		sIncommingUI_Event event;
		BaseType_t res = xQueueReceive(xUI_ThreadQueue, (void *)&event, 1000);
		if (res == pdPASS)
		{
			if (event.encoderEvent.eventSourceID == eEventSourceID::encoderThreadSourceID)
			{
				uint16_t encoderNo = event.encoderEvent.encoderNo;
				if (event.encoderEvent.eventType == eEncoderEventType::encoderSwitch)
				{
				}
				else if (event.encoderEvent.eventType == eEncoderEventType::encoderCount)
				{
				}
			}
			else if (event.potentiometerEvent.eventSourceID == eEventSourceID::potentiometerThreadSourceID)
			{
				uint32_t potId    = event.potentiometerEvent.potentiometerId;
           		uint32_t potValue = event.potentiometerEvent.potentiometerValue;

           		tPotValueMessage potValueMessage;
           		potValueMessage.header.messageTag 		= (uint32_t)eSynthEngineMessageType::potentiometerValue;
           		potValueMessage.header.messageLength	= sizeof(tPotValueMessage);
           		potValueMessage.potId					= potId;
           		potValueMessage.potValue				= potValue;

           		bool success = gNerveNetMasterThreadPtr[0]->sendMessage(&potValueMessage, sizeof(tPotValueMessage));
           		if ( ! success)
           			CN_ReportFault(eErrorCodes::NerveNetThread_Error);		// This call should return success
			}
		}

	}

}

//==============================================================================
//          Start_UI_Thread()
//
// Handle all UI events
//
//  CasualNoises    15/02/2025  First implementation
//==============================================================================
BaseType_t Start_UI_Thread(UI_ThreadData *argument)
{

	// Create the thread to run the UI
	TaskHandle_t xHandlePtr;
	BaseType_t res = xTaskCreate(UI_Thread,	"UI_Thread", DEFAULT_STACK_SIZE, argument,
			UI_THREAD_PRIORITY,	&xHandlePtr);
	gYellowPages.gUI_ThreadTaskHandle = xHandlePtr;
	return res;

}

} // namespace CasualNoises
