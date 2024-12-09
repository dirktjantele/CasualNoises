/*
  ==============================================================================

    DAC8164_Driver.h
    Created: 19 oct 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "DAC_Driver.h"

#include "HAL_Includes.h"

#include "main.h"

namespace CasualNoises
{

const uint32_t	cLoadDAC_bit = 0x10000000;

//==============================================================================
//         DAC8164_Driver configuration data
//
// The driver uses a global defined struct, called gDAC8164_DriverConfig
//
//  CasualNoises    20/10/2023  First implementation
//==============================================================================
typedef struct
{
	GPIO_TypeDef*		enablePinPort;
	uint16_t 			enablePinNumber;
	I2S_HandleTypeDef*	hi2s_ptr;
} sDAC8164_DriverConfig;

//==============================================================================
//         DAC8164_Driver
//==============================================================================
class DAC8164_Driver : DAC_Driver
{
public:
	 DAC8164_Driver() = default;
	~DAC8164_Driver() = default;

	//==============================================================================
	//          startDriver
	//
	// Start the driver
	// size: number of samples (uint32_t words) to  be send to the device (buffer size)
	//
	//  CasualNoises    20/10/2023  First implementation
	//==============================================================================
	void startDriver(uint32_t size) override
	{

		// We have to send data only when I2Sx_WS (SYNC) is low (mono)
		mSize = size * 2;

		// Get a buffer and clean it up
		mBufferPtr = (uint32_t*)pvPortMalloc(mSize * sizeof(uint32_t));
		if (mBufferPtr == nullptr)
			CN_ReportFault(eFaultCode::fcMallocFault);
		for (uint32_t i = 0; i < mSize; ++i)
			if (i % 2 == 0)
				mBufferPtr[i] = cLoadDAC_bit;
			else
				mBufferPtr[i] = 0x00000000;

		// Configuration info for the DAC7164 driver
		mDAC8164_DriverConfig.enablePinPort 	= DAC_ENABLE_GPIO_Port;
		mDAC8164_DriverConfig.enablePinNumber	= DAC_ENABLE_Pin;
		mDAC8164_DriverConfig.hi2s_ptr			= getDAC_I2S_HandlePtr();

		// Enable the device & start DMA transmit
		HAL_GPIO_WritePin(mDAC8164_DriverConfig.enablePinPort, mDAC8164_DriverConfig.enablePinNumber, GPIO_PIN_RESET);
		HAL_StatusTypeDef res = HAL_I2S_Transmit_DMA (mDAC8164_DriverConfig.hi2s_ptr, (uint16_t *) mBufferPtr, mSize);
		if (res != HAL_OK)
			CN_ReportFault(eFaultCode::fcInitializationFault);

	};

	//==============================================================================
	//          sendAudio
	//
	// Send a block of audio data to the DAC using ping-pong buffering
	// sourcePointer: points to an array of float's with half the length of buffer size
	//
	// transmitCompleted: this indicates which half of the buffer is to be filled
	//
	//  CasualNoises    20/10/2023  First implementation
	//==============================================================================
	void sendAudio(const float* sourcePointer, bool transmitCompleted) override
	{
//		HAL_GPIO_WritePin(GPIOB,TIME_MARKER_3_Pin, GPIO_PIN_SET);

		// Get a pointer into the audio output buffer
		uint32_t* outputPtr = &mBufferPtr[0];
		if (transmitCompleted)
			outputPtr = &mBufferPtr[mSize / 2];

		// Convert samples from float to int32_t, adjust and put them in the right context
		for (uint32_t out = 0, in = 0; out < mSize / 2; out += 2, ++in)
		{
			int32_t value = static_cast<int32_t>(sourcePointer[in] * 32767.0f);

			if (value > 0)
				value -= 0x7fff;
			else
				value += 0x7fff;
			value = ~value;

			value = ((value & 0x0000ffff) << 8) | cLoadDAC_bit;
			outputPtr[out] = value << 1;		// ToDo: try to get rid of the shift

		}

//		HAL_GPIO_WritePin(GPIOB,TIME_MARKER_3_Pin, GPIO_PIN_RESET);
	}

private:
	uint32_t mSize 			{ 0 };
	uint32_t* mBufferPtr 	{ nullptr };

	sDAC8164_DriverConfig	mDAC8164_DriverConfig;

};

} // namespace CasualNoises

