/*
  ==============================================================================

    UI_Thread.cpp

    Handles encoders, potentiometers and led's

    Created: 25 oct 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "UI_Thread.h"

#include "SystemConfig.h"

#include "CV_InputThread.h"

namespace CasualNoises
{

//==============================================================================
//          UI_Thread
//
// Handle all UI events
//
//  CasualNoises    27/10/2024  First implementation
//==============================================================================
void UI_Thread(void* pvParameters)
{
	UI_ThreadData *UI_ThreadParams = (UI_ThreadData *)pvParameters;

	// Build the encoder pin map table (see schematics)
	sEncoderMapEntry encoderPinMap[ENCODER_COUNT];
	encoderPinMap[0].encoderA.deviceAddress			= PCF8574_ADRRESS_1;	// Encoder #1
	encoderPinMap[0].encoderA.pinNo					= 0x01 << 0;
	encoderPinMap[0].encoderB.deviceAddress			= PCF8574_ADRRESS_1;
	encoderPinMap[0].encoderB.pinNo					= 0x01 << 1;
	encoderPinMap[0].encoderSwitch.deviceAddress	= PCF8574_ADRRESS_1;
	encoderPinMap[0].encoderSwitch.pinNo			= 0x01 << 2;
	encoderPinMap[1].encoderA.deviceAddress			= PCF8574_ADRRESS_1;	// Encoder #2
	encoderPinMap[1].encoderA.pinNo					= 0x01 << 5;
	encoderPinMap[1].encoderB.deviceAddress			= PCF8574_ADRRESS_1;
	encoderPinMap[1].encoderB.pinNo					= 0x01 << 4;
	encoderPinMap[1].encoderSwitch.deviceAddress	= PCF8574_ADRRESS_2;
	encoderPinMap[1].encoderSwitch.pinNo			= 0x01 << 0;
	encoderPinMap[2].encoderA.deviceAddress			= PCF8574_ADRRESS_1;	// Encoder #3
	encoderPinMap[2].encoderA.pinNo					= 0x01 << 3;
	encoderPinMap[2].encoderB.deviceAddress			= PCF8574_ADRRESS_1;
	encoderPinMap[2].encoderB.pinNo					= 0x01 << 7;
	encoderPinMap[2].encoderSwitch.deviceAddress	= PCF8574_ADRRESS_1;
	encoderPinMap[2].encoderSwitch.pinNo			= 0x01 << 6;
	encoderPinMap[3].encoderA.deviceAddress			= PCF8574_ADRRESS_2;	// Encoder #4
	encoderPinMap[3].encoderA.pinNo					= 0x01 << 1;
	encoderPinMap[3].encoderB.deviceAddress			= PCF8574_ADRRESS_2;
	encoderPinMap[3].encoderB.pinNo					= 0x01 << 2;
	encoderPinMap[3].encoderSwitch.deviceAddress	= PCF8574_ADRRESS_2;
	encoderPinMap[3].encoderSwitch.pinNo			= 0x01 << 3;

	// Create a queue for encoder events
	QueueHandle_t encoderEventQueue = xQueueCreate(4, sizeof(uEncoderEventStruct));
	if (encoderEventQueue == nullptr) CN_ReportFault(1);

	// Create a encoder thread
	EncoderThreadData encoderThreadData;
	encoderThreadData.hi2c 						= UI_ThreadParams->hi2c;
	encoderThreadData.I2Cx_Semaphore			= UI_ThreadParams->I2Cx_Semaphore;
	encoderThreadData.clientQueueHandle			= encoderEventQueue;
	encoderThreadData.I2C_DeviceAddresses[0] 	= PCF8574_ADRRESS_1;
	encoderThreadData.I2C_DeviceAddresses[1] 	= PCF8574_ADRRESS_2;
	encoderThreadData.Encoder_PinMap			= encoderPinMap;
	BaseType_t res = CasualNoises::startEncoderThread(&encoderThreadData);
	if (res == 0) CasualNoises::CN_ReportFault(1);

	// Create a queue for potentiometer events
	QueueHandle_t potentiometerEventQueue = xQueueCreate(4, sizeof(sPotentiometerEventStruct));
	if (potentiometerEventQueue == nullptr) CN_ReportFault(1);

	// Create a potentiometer event thread
	sPotentiometerThreadData potentiometerThreadData;
	potentiometerThreadData.hadc 				= UI_ThreadParams->hadc;
	potentiometerThreadData.htim				= UI_ThreadParams->htim;
	potentiometerThreadData.clientQueueHandle	= potentiometerEventQueue;
	res = CasualNoises::startPotentiometerThread(&potentiometerThreadData);
	if (res == 0) CasualNoises::CN_ReportFault(1);

	// Handle incoming events
	for (;;)
	{

		// Handle incoming encoder events if any
		UBaseType_t cnt =  uxQueueMessagesWaiting(encoderEventQueue);
		while (cnt > 0)
		{

			// Get the event
			uEncoderEventStruct encoderEvent;
			res = xQueueReceive(encoderEventQueue, &encoderEvent, 1000);
			if (res == 0) CasualNoises::CN_ReportFault(1);

			// ToDo do something with the event

			// Any events left?
			cnt =  uxQueueMessagesWaiting(encoderEventQueue);

		}

		// Handle incoming potentiometer events if any
		cnt =  uxQueueMessagesWaiting(potentiometerEventQueue);
		while (cnt > 0)
		{

			// Get the event
			sPotentiometerEventStruct potentiometerEvent;
			res = xQueueReceive(potentiometerEventQueue, &potentiometerEvent, 1000);
			if (res == 0) CasualNoises::CN_ReportFault(1);

			// Deja Vue
			float history[3];
			history[potentiometerEvent.potentiometerId] = potentiometerEvent.potentiometerValue;

			// ToDo do something with the event
			UI_ThreadParams->synthesiserParams->morph = history[0] / 100.0f;

			// Any events left?
			cnt =  uxQueueMessagesWaiting(potentiometerEventQueue);

		}

		// Wait some time for the next round
		osDelay(10);
	}

}

//==============================================================================
//          Start_UI_Thread
//
// Handle all UI events
//
//  CasualNoises    25/10/2024  First implementation
//==============================================================================
BaseType_t Start_UI_Thread(UI_ThreadData *argument)
{

	// Create the thread to run the UI
	TaskHandle_t xHandle;
	BaseType_t res = xTaskCreate(UI_Thread,	"UI_Thread", DEFAULT_STACK_SIZE, argument,
			UI_THREAD_PRIORITY,	&xHandle);
	return res;

}

} // namespace CasualNoises

