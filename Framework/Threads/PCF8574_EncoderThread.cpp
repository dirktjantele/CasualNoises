/*
  ==============================================================================

    EncoderThread.cpp

    This thread can handle up to 5 encoders with switches connected to the mcu
    using a PCF8574 driver.

    Created: 7 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_ENCODER_THREAD_PCF8574

#include <Threads/PCF8574_EncoderThread.h>

#include "SystemConfig.h"

namespace CasualNoises
{

//==============================================================================
//          EncoderThread
//
// Handle all UI events
//
//  CasualNoises    28/10/2024  First implementation
//					13/12/2024	Send encoder switch event when switch state changed
//==============================================================================
void encoderThread(void* pvParameters)
{

	// Get arguments
	EncoderThreadData* arguments 		= (EncoderThreadData*)pvParameters;
	I2C_HandleTypeDef I2C_Handle 		= arguments->hi2c;
	SemaphoreHandle_t I2Cx_Semaphore	= arguments->I2Cx_Semaphore;
	QueueHandle_t clientQueueHandle 	= arguments->clientQueueHandle;
	uint8_t I2C_DeviceAddresses[PCF8574_COUNT];
	I2C_DeviceAddresses[0]				= arguments->I2C_DeviceAddresses[0];
	I2C_DeviceAddresses[1]				= arguments->I2C_DeviceAddresses[1];

	// Encoder rotation counters
	static int16_t encoderCounter[ENCODER_COUNT];
	for (uint32_t i = 0; i < ENCODER_COUNT; ++i)
		encoderCounter[i] = 0;

	// Get a PCF8574 driver
	PCF8574_Driver* driver = PCF8574_Driver::getPCF8574_Driver(I2C_Handle, I2Cx_Semaphore);

	// Build a mask of all pins of interest
	uint16_t encoder_A_Mask = 0x0000;
	uint16_t encoder_B_Mask = 0x0000;
	sEncoderMapEntry* encoder_PinMap = arguments->Encoder_PinMap;
	for (uint32_t i = 0; i < ENCODER_COUNT; ++i)
	{
		if (encoder_PinMap[i].encoderA.deviceAddress == I2C_DeviceAddresses[1])
		{
			encoder_PinMap[i].encoderA.pinNo <<= 8;
			encoder_PinMap[i].encoderB.pinNo <<= 8;
		}
		encoder_A_Mask |= encoder_PinMap[i].encoderA.pinNo;
		encoder_B_Mask |= encoder_PinMap[i].encoderB.pinNo;
	}
	uint16_t encoder_Switch_Mask = 0x0000;
	for (uint32_t i = 0; i < ENCODER_COUNT; ++i)
	{
		if (encoder_PinMap[i].encoderSwitch.deviceAddress == I2C_DeviceAddresses[1])
			encoder_PinMap[i].encoderSwitch.pinNo <<= 8;
		encoder_Switch_Mask |= encoder_PinMap[i].encoderSwitch.pinNo;
	}

	// Get current state
	uint16_t newState = 0x0000;
	for (int32_t i = PCF8574_COUNT - 1; i >= 0 ; --i)
	{
		uint8_t buf = 0x00;
		buf = driver->readAllPins(I2C_DeviceAddresses[i]);
		newState <<= 8;
		newState |= buf;
	}
	uint16_t oldState = newState;

	// Scan encoders and look for changes
	uint16_t cycles = 0;
	for (;;)
	{

		// Get current state and check against previous one
		newState = 0x0000;
		for (int32_t i = PCF8574_COUNT - 1; i >= 0 ; --i)
		{
			uint8_t buf = 0x00;
			buf = driver->readAllPins(I2C_DeviceAddresses[i]);
			newState <<= 8;
			newState |= buf;
		}
		uint16_t flags = oldState ^ newState;

		// High to low transition found for an encoder A pin?
		if ((flags & encoder_B_Mask) != 0x0000)
		{

			// Update encoder rotation counters
			for (uint32_t encoder = 0; encoder < ENCODER_COUNT; ++encoder)
			{

				uint16_t pin = encoder_PinMap[encoder].encoderB.pinNo & flags;
				if (pin != 0x0000)
				{
					bool directionRight = (encoder_PinMap[encoder].encoderA.pinNo  & newState) == 0x0000;
					if (directionRight)
						encoderCounter[encoder] += 1;
					else
						encoderCounter[encoder] -= 1;
					cycles = 0;
				}
			}

		}

		// State transition found for an encoder switch pin?
		if ((flags & encoder_Switch_Mask) != 0x0000)
		{
			// Send encoder switch events to the client queue
			for (uint16_t encoder = 0; encoder < ENCODER_COUNT; ++encoder)
			{
				if ((encoder_PinMap[encoder].encoderSwitch.pinNo & flags) != 0x0000)
				{
					uint16_t state = newState & flags & encoder_Switch_Mask;
					uEncoderEventStruct encoderInfo;
					encoderInfo.switchEvent.eventType = eEncoderEventType::encoderSwitchEvent;
					encoderInfo.switchEvent.encoderNo = encoder;
					encoderInfo.switchEvent.state	  = state != 0x0000;
					BaseType_t res = xQueueSendToBack(clientQueueHandle, (void*)&encoderInfo, 10);
					if (res != pdPASS) CN_ReportFault(4);
				}
			}
		}

		// Have to queue events to the client queue?
		if (++cycles >= 200)
		{
			uEncoderEventStruct encoderInfo;
			encoderInfo.rotationEvent.eventType = eEncoderEventType::encoderRotationEvent;
			for (uint32_t i = 0; i < ENCODER_COUNT; ++i)
				if (encoderCounter[i] != 0)
				{
					encoderInfo.rotationEvent.encoderNo = i;
					encoderInfo.rotationEvent.count = encoderCounter[i];
					BaseType_t res = xQueueSendToBack(clientQueueHandle, (void*)&encoderInfo, 10);
					if (res != pdPASS) CN_ReportFault(4);
					encoderCounter[i] = 0;
				}
			cycles = 0;
		}

		// Update oldState
		oldState = newState;

		// Wait x ticks
		osDelay(1);

	}

}

//==============================================================================
//          startEncoderThread
//
// Handle all encoder events
//
//  CasualNoises    07/08/2023  First implementation
//==============================================================================
BaseType_t startEncoderThread(void *argument)
{

	// Create the thread to scan the encoders
	TaskHandle_t xHandle;
	BaseType_t res = xTaskCreate(encoderThread,	"EncoderThread", DEFAULT_STACK_SIZE / 2, argument,
			UI_THREAD_PRIORITY,	&xHandle);
	return res;

}

} // namespace CasualNoises

#endif

