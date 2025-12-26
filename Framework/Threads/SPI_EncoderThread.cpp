/*
  ==============================================================================

    SPI_EncoderThread.cpp

    This thread handles encoders and switches connected to 74HC165 shift registers
    Communication is done using SPI with DMA mode to minimise cpu load

    Created: 24/02/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_ENCODER_THREAD_SPI

#include "SPI_EncoderThread.h"

#include "Utilities/ReportFault.h"

namespace CasualNoises
{

enum class signalType
{
	unknown,
	enc_sw,
	enc_a,
	enc_b
};

//==============================================================================
//          EncoderThread()
//
// Handle all encoder events
//
//  CasualNoises    26/02/2025  First implementation
//==============================================================================
void encoderThread(void* pvParameters)
{

	// Get initial data
	sSPI_EncoderThreadData* dataPtr = (sSPI_EncoderThreadData*)pvParameters;
	QueueHandle_t clientQueue = dataPtr->clientQueue;

	// Build look-up tables
	uint32_t noOfEncoders = dataPtr->noOfEncoders;
	uint32_t noOfDevices  = dataPtr->noOfDevices;
	sEncoderSignature* signatures = dataPtr->encoderSignatureArray;
	uint32_t tableSize = noOfDevices << 3;
	signalType* typeTablePtr  = new signalType[tableSize];
	uint8_t*    encNoTablePtr = new uint8_t[tableSize];
	for (uint32_t i = 0; i < tableSize; ++i)
	{
		typeTablePtr[i]  = signalType::unknown;
		encNoTablePtr[i] = 0xff;
	}
	for (uint32_t encNo = 0; encNo < noOfEncoders; ++encNo)
	{
		uint32_t indx;
		indx = (signatures[encNo].switchDevNo << 3) + signatures[encNo].switchPinNo;
		typeTablePtr[indx]  = signalType::enc_sw;
		encNoTablePtr[indx] = signatures[encNo].encoderNo;
		if (signatures[encNo].enc_A_DevNo < 0xff)
		{
			indx = (signatures[encNo].enc_A_DevNo << 3) + signatures[encNo].enc_A_PinNo;
			typeTablePtr[indx]  = signalType::enc_a;
			encNoTablePtr[indx] = signatures[encNo].encoderNo;
		}
		if (signatures[encNo].enc_B_DevNo < 0xff)
		{
			indx = (signatures[encNo].enc_B_DevNo << 3) + signatures[encNo].enc_B_PinNo;
			typeTablePtr[indx]  = signalType::enc_b;
		}
	}

	// Table of encoder increments
	int16_t* encIncTable = new int16_t[noOfEncoders];
	for (uint32_t encNo = 0; encNo < noOfEncoders; ++encNo)
	{
		encIncTable[encNo] = 0;
	}

	// Load current encoder signals
	uint8_t* currentData = new uint8_t[noOfDevices];
	HAL_GPIO_WritePin(dataPtr->serInPL_Port, dataPtr->serInPL_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(dataPtr->serInPL_Port, dataPtr->serInPL_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(dataPtr->serInPL_Port, dataPtr->serInPL_Pin, GPIO_PIN_SET);
	HAL_StatusTypeDef res = HAL_SPI_Receive (dataPtr->encoder_SPI_Handle, currentData, noOfDevices, HAL_MAX_DELAY);
	if (res != HAL_OK)
		CN_ReportFault(eErrorCodes::runtimeError);
	uint8_t* newData = new uint8_t[noOfDevices];
	bool* curStateTable = new bool[tableSize];
	bool* newStateTable = new bool[tableSize];

	// Copy the current encoder state
	for (uint32_t devNo = 0; devNo < noOfDevices; ++devNo)
	{
		uint8_t flags = currentData[devNo];
		for (uint32_t bitNo = 0; bitNo < 8; ++bitNo)
		{
			curStateTable[(devNo << 3) + bitNo] = flags & 0x01;
			flags >>= 1;
		}
	}

	// Main thread loop
	uint16_t loopCnt = 0;
	for (;;)
	{
		loopCnt += 1;

		// Load new encoder signals
		HAL_GPIO_WritePin(dataPtr->serInPL_Port, dataPtr->serInPL_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(dataPtr->serInPL_Port, dataPtr->serInPL_Pin, GPIO_PIN_SET);
		res =  HAL_SPI_Receive (dataPtr->encoder_SPI_Handle, newData, noOfDevices, HAL_MAX_DELAY);
		if (res != HAL_OK)
			CN_ReportFault(eErrorCodes::runtimeError);

		// Copy the new encoder state
		for (uint32_t devNo = 0; devNo < noOfDevices; ++devNo)
		{
			uint8_t flags = newData[devNo];
			for (uint32_t bitNo = 0; bitNo < 8; ++bitNo)
			{
				newStateTable[(devNo << 3) + bitNo] = flags & 0x01;
				flags >>= 1;
			}
		}

		// Look for changes from high state to low state
		for (uint32_t signalNo = 0; signalNo < tableSize; ++signalNo)
		{

			// Act upon the type of the signal
			bool    flag  = (newStateTable[signalNo] ^ curStateTable[signalNo]) & curStateTable[signalNo];
			uint8_t encNo = encNoTablePtr[signalNo];
			if (flag)
			{
				switch (typeTablePtr[signalNo])
				{
				case signalType::enc_sw:
				{
					// Send encoder event to the client
					if (clientQueue != nullptr)
					{
						sEncoderEvent event;
						event.eventSourceID = eEventSourceID::encoderThreadSourceID;
						event.eventType     = eEncoderEventType::encoderSwitch;
						event.encoderNo     = signatures[encNo].encoderNo;
						event.encoderCount  = 1;
						event.switchBitMap	= 0x00000000;
						for (int32_t i = noOfDevices - 1; i >= 0; --i)
							event.switchBitMap = (event.switchBitMap << 8) + newData[i];
						BaseType_t res = xQueueSendToBack(clientQueue, (void*)&event, 10);
						if (res != pdPASS)
							CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);
					}
				}
					encIncTable[encNo] = 0;
					break;
				case signalType::enc_a:
				{
					uint32_t indx = (signatures[encNo].enc_B_DevNo << 3) + signatures[encNo].enc_B_PinNo;
					if (newStateTable[indx])
						encIncTable[encNo] += 1;
					else
						encIncTable[encNo] -= 1;
				}
					break;
				case signalType::enc_b:
					// Ignore these signals...
					break;
				default:
					CN_ReportFault(eErrorCodes::runtimeError);
				}
			}

			// Save current state
			curStateTable[signalNo] = newStateTable[signalNo];
		}

		// Time to send encoder counts to the client?
		if (loopCnt >= 100)
		{
			for (uint32_t encNo = 0; encNo < noOfEncoders; ++encNo)
			{
				if (encIncTable[encNo] != 0)
				{
					// Send encoder event to the client
					if (clientQueue != nullptr)
					{
						sEncoderEvent event;
						event.eventSourceID = eEventSourceID::encoderThreadSourceID;
						event.eventType     = eEncoderEventType::encoderCount;
						event.encoderNo     = signatures[encNo].encoderNo;
						event.encoderCount  = encIncTable[encNo];
						BaseType_t res = xQueueSendToBack(clientQueue, (void*)&event, 10);
						if (res != pdPASS)
							CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);
					}
					encIncTable[encNo] = 0;
				}
			}
			loopCnt = 0;
		}

		// Scan encoders at about 1 KHz
		vTaskDelay(pdMS_TO_TICKS(1));

	}

}

//==============================================================================
//          startEncoderThread()
//
// Start the thread with given parameters and return a handle to it
//
//  CasualNoises    24/02/2025  First implementation
//==============================================================================
BaseType_t startEncoderThread(void *argument, TaskHandle_t* xHandlePtr)
{

	// Create the thread to scan the encoders
	BaseType_t res = xTaskCreate(encoderThread, "EncoderThread", DEFAULT_STACK_SIZE / 2, argument,
			UI_THREAD_PRIORITY,	xHandlePtr);
	return res;

}

} // namespace CasualNoises

#endif
