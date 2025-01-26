/*
  ==============================================================================

    DAC8164_Driver.h
    Created: 19 oct 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "DAC_Driver.h"

//#include "HAL_Includes.h"

#include "main.h"

namespace CasualNoises
{

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
	 DAC8164_Driver() = delete;
	 DAC8164_Driver(sDAC8164_DriverConfig* initData)
	 : mDAC8164_DriverConfig(*initData) {}
	~DAC8164_Driver() = default;

	//==============================================================================
	//          startDriver
	//
	// Start the driver
	//
	// noOfChannels: number of channels (should be 4 for a DAC8164
	// noOfSamples:  number of samples (uint32_t words) for each channel
	//
	//  CasualNoises    20/10/2023  First implementation
	//  CasualNoises    10/12/2024  Resuscitated
	//==============================================================================
	void startDriver(uint32_t noOfChannels, uint32_t noOfSamples) override
	{
		mNoOfChannels = noOfChannels;
		mNoOfSamples  = noOfSamples;
		if (mNoOfChannels != 4)
			CN_ReportFault(5);

		// We have to send data only when I2Sx_WS (SYNC) is low (mono)
		mSize = mNoOfSamples * noOfChannels * 2;		// ToDo can we send mono audio data???

		// Get a buffer and clean it up
		mBufferPtr = (uint32_t*)pvPortMalloc(mSize * sizeof(uint32_t));
		if (mBufferPtr == nullptr)
			CN_ReportFault(2);
		for (uint32_t i = 0; i < mSize; ++i)
			mBufferPtr[i] = 0x00000000;

		// Enable the device & start DMA transmit
		HAL_GPIO_WritePin(mDAC8164_DriverConfig.enablePinPort, mDAC8164_DriverConfig.enablePinNumber, GPIO_PIN_RESET);
		HAL_StatusTypeDef res = HAL_I2S_Transmit_DMA (mDAC8164_DriverConfig.hi2s_ptr, (uint16_t *) mBufferPtr, mSize);
		if (res != HAL_OK)
			CN_ReportFault(3);

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
	//  CasualNoises    10/12/2024  Resuscitated
	//==============================================================================
	const uint32_t	cLoadDAC_bit = 0x01 << 20; //0x10000000;

	void sendAudio(float** sourcePointer, bool transmitCompleted) override
	{

		// Get a pointer into the audio output buffer
		uint32_t* outputPtr = &mBufferPtr[0];
		if (transmitCompleted)
			outputPtr = &mBufferPtr[mSize / 2];

		// Convert samples from float to int32_t, adjust and put them in the right context
		float scale = static_cast<float>(0x00007fff);
		for (uint32_t s = 0; s < mNoOfSamples; ++s)
		{
			for (uint32_t c = 0; c < mNoOfChannels; ++c)
			{
				float peek = sourcePointer[c][s];
				uint32_t value = static_cast<uint32_t>((sourcePointer[c][s] + 1.0f) * scale);

				value = value | (c << 17);
				if (c < (mNoOfChannels) - 1)
					value = value | (0b00000001 << 20);
				else
					value = value | (0b00000010 << 20);

				*outputPtr++ = value << 9;
				outputPtr += 1;
			}
		}
/*
		float scale = static_cast<float>(0x00007fff);
		for (uint32_t out = 0, in = 0; out < mSize / 2; out += 2, ++in)
		{
			uint32_t value = static_cast<uint32_t>((sourcePointer[in] + 1.0f) * scale);
			value |= cLoadDAC_bit;
			outputPtr[out] = value << 1;

			int32_t value = static_cast<int32_t>(sourcePointer[in] * 32767.0f);

			if (value > 0)
				value -= 0x7fff;
			else
				value += 0x7fff;
			value = ~value;

			value = ((value & 0x0000ffff) << 8) | cLoadDAC_bit;
			outputPtr[out] = value << 1;		// ToDo: try to get rid of the shift


		}
*/
	}

private:
	uint32_t	mNoOfChannels	{ 0 };			// No of channels
	uint32_t	mNoOfSamples	{ 0 };			// No of samples per channel

	uint32_t 	mSize 			{ 0 };
	uint32_t* 	mBufferPtr 		{ nullptr };

	sDAC8164_DriverConfig	mDAC8164_DriverConfig;

};

} // namespace CasualNoises
