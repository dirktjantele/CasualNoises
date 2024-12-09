/*
  ==============================================================================

    I2C_SoftwareDriver.h
    Created: 24 jul 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "main.h"

namespace CasualNoises
{

class I2C_SoftwareDriver final
{
public:
	 I2C_SoftwareDriver() = delete;
	~I2C_SoftwareDriver() = default;

	//==============================================================================
	//          I2C_SoftwareDriver()
	//
	// Initialise both SCL and SDA pins
	//
	//  CasualNoises    03/08/2023  First implementation
	//==============================================================================
	I2C_SoftwareDriver(GPIO_TypeDef* SCL_Port, uint16_t SCL_Pin,
					   GPIO_TypeDef* SDA_Port, uint16_t SDA_Pin,
					   uint16_t cpuClock = 100, 					// CPU speed in MHz
					   uint16_t maxDeviceClock = 100)				// Max device clock in KHz
	: mSCL_Port (SCL_Port)
	, mSCL_Pin  (SCL_Pin)
	, mSDA_Port (SDA_Port)
	, mSDA_Pin  (SDA_Pin)
	{

		// Set both pins high
		HAL_GPIO_WritePin(mSCL_Port, mSCL_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(mSDA_Port, mSDA_Pin, GPIO_PIN_SET);

		// Make sure that the pins are configured as should
		mGPIO_InitStruct.Pin = SCL_Pin;							// SCL pin first
		mGPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		mGPIO_InitStruct.Pull = GPIO_NOPULL;
		mGPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(mSCL_Port, &mGPIO_InitStruct);

		mGPIO_InitStruct.Pin = SDA_Pin;							// SDA pin last
		mGPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		mGPIO_InitStruct.Pull = GPIO_NOPULL;
		mGPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(mSDA_Port, &mGPIO_InitStruct);

		// Calculate the delay count needed for max performance
		float _cpuClock = (float)cpuClock;
		float _maxDeviceClock = (float)maxDeviceClock;
		mDelayCount = (uint16_t)(((0.16f * _cpuClock) / _maxDeviceClock) * 100.0f);

		// Send a dummy start condition, should be done for some chips after reset
		HAL_GPIO_WritePin(mSDA_Port, mSDA_Pin, GPIO_PIN_RESET);
		_delay();
		HAL_GPIO_WritePin(mSDA_Port, mSDA_Pin, GPIO_PIN_SET);
		_delay();

	}

	//==============================================================================
	//          I2C_Master_Transmit()
	//
	// Send data to the I2C device
	//
	//  CasualNoises    03/08/2023  First implementation
	//==============================================================================
	HAL_StatusTypeDef I2C_Master_Transmit(uint8_t address, uint8_t* data, uint16_t size) noexcept
	{
		address &= 0xfe;														// Clear R/W bit
		HAL_StatusTypeDef res = I2C_Transmit(&address, 1, true, false);			// Send address byte
		if (res != HAL_OK) return res;
		res = I2C_Transmit(data, size, false, true);							// Send data
		HAL_GPIO_WritePin(mSCL_Port, mSCL_Pin, GPIO_PIN_SET);					// Set pins to idle state
		HAL_GPIO_WritePin(mSCL_Port, mSDA_Pin, GPIO_PIN_SET);
		return res;
	}

	//==============================================================================
	//          I2C_Master_Receive()
	//
	// Receive data from the I2C device
	//
	//  CasualNoises    03/08/2023  First implementation
	//==============================================================================
	HAL_StatusTypeDef I2C_Master_Receive(uint8_t address, uint8_t* data, uint16_t size) noexcept
	{
		address |= 0x01;														// Set R/W bit
		HAL_StatusTypeDef res = I2C_Transmit(&address, 1, true, false);			// Send address byte
		if (res != HAL_OK) return res;
		res = I2C_Receive(data, size);											// Receive data
		HAL_GPIO_WritePin(mSCL_Port, mSCL_Pin, GPIO_PIN_SET);					// Set pins to idle state
		HAL_GPIO_WritePin(mSCL_Port, mSDA_Pin, GPIO_PIN_SET);
		return res;
	}

private:
	GPIO_TypeDef*		mSCL_Port;
	uint16_t 			mSCL_Pin;
	GPIO_TypeDef*		mSDA_Port;
	uint16_t 			mSDA_Pin;

	GPIO_InitTypeDef 	mGPIO_InitStruct {0};

	uint16_t			mDelayCount { 100 };

	//==============================================================================
	//          _delay()
	//
	// Simple delay loop
	//
	//  CasualNoises    03/08/2023  First implementation
	//==============================================================================
	void _delay() const noexcept
	{
		for (uint32_t cnt = 0; cnt < mDelayCount; )
			++cnt;
	}

	//==============================================================================
	//          I2C_Transmit()
	//
	// Send data to the I2C device
	//
	//  CasualNoises    03/08/2023  First implementation
	//==============================================================================
	HAL_StatusTypeDef I2C_Transmit(uint8_t* data, uint16_t size, bool start, bool stop) noexcept
	{

		// Set SDA pin as output
		mGPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		HAL_GPIO_Init(mSDA_Port, &mGPIO_InitStruct);

		// Start condition
		if (start)
		{
			HAL_GPIO_WritePin(mSCL_Port, mSCL_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(mSDA_Port, mSDA_Pin, GPIO_PIN_RESET);
			_delay();
			HAL_GPIO_WritePin(mSCL_Port, mSCL_Pin, GPIO_PIN_RESET);
			_delay();
		}

		// Shift data out
		for (auto i = 0; i < size; ++i)
		{

			// Shift out all bits (most significant first)
			for (auto j = 0; j < 8; ++j)
			{
				if (data[i] & 0x80)
					HAL_GPIO_WritePin(mSDA_Port, mSDA_Pin, GPIO_PIN_SET);
				else
					HAL_GPIO_WritePin(mSDA_Port, mSDA_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(mSCL_Port, mSCL_Pin, GPIO_PIN_SET);
				_delay();
				HAL_GPIO_WritePin(mSCL_Port, mSCL_Pin, GPIO_PIN_RESET);
				if (j < 7)
					_delay();
				data[i] <<= 1;
			}

			// Check acknowledge reply
			mGPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			HAL_GPIO_Init(mSDA_Port, &mGPIO_InitStruct);
			HAL_GPIO_WritePin(mSCL_Port, mSCL_Pin, GPIO_PIN_SET);
			_delay();
			GPIO_PinState ack = HAL_GPIO_ReadPin(mSDA_Port, mSDA_Pin);
			HAL_GPIO_WritePin(mSCL_Port, mSCL_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(mSDA_Port, mSDA_Pin, GPIO_PIN_SET);
			mGPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
			HAL_GPIO_Init(mSDA_Port, &mGPIO_InitStruct);
			if (ack != GPIO_PIN_RESET)
				return HAL_ERROR;

			// Send stop or repeated start
			if (i == 1 && stop)
			{
				HAL_GPIO_WritePin(mSDA_Port, mSDA_Pin, GPIO_PIN_RESET);		// Stop condition
				HAL_GPIO_WritePin(mSCL_Port, mSCL_Pin, GPIO_PIN_SET);
				_delay();
				HAL_GPIO_WritePin(mSDA_Port, mSDA_Pin, GPIO_PIN_SET);
				_delay();
			} else
			{
				HAL_GPIO_WritePin(mSDA_Port, mSDA_Pin, GPIO_PIN_SET);		// Repeated start condition
			}

		}

		return HAL_OK;

	}

	//==============================================================================
	//          I2C_Receive()
	//
	// Receive data from the I2C device
	//
	//  CasualNoises    03/08/2023  First implementation
	//==============================================================================
	HAL_StatusTypeDef I2C_Receive(uint8_t* data, uint16_t size) noexcept
	{

		// Set SDA pin as input
		mGPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		HAL_GPIO_Init(mSDA_Port, &mGPIO_InitStruct);

		// Shift data in
		for (auto i = 0; i < size; ++i)
		{

			// Shift in all bits (most significant first)
			data[i] = 0x00;
			for (auto j = 0; j < 8; ++j)
			{

				HAL_GPIO_WritePin(mSCL_Port, mSCL_Pin, GPIO_PIN_SET);
				_delay();

				GPIO_PinState state = HAL_GPIO_ReadPin(mSDA_Port, mSDA_Pin);
				data[i] <<= 1;
				if (state == GPIO_PIN_SET)
					data[i] |= 0x01;

				HAL_GPIO_WritePin(mSCL_Port, mSCL_Pin, GPIO_PIN_RESET);
				_delay();

			}

			HAL_GPIO_WritePin(mSCL_Port, mSCL_Pin, GPIO_PIN_SET);
			_delay();
			GPIO_PinState ack = HAL_GPIO_ReadPin(mSDA_Port, mSDA_Pin);
			if (ack != GPIO_PIN_RESET && i < (size - 1))
				return HAL_ERROR;

		}


		// Set SDA pin as output
		mGPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		HAL_GPIO_Init(mSDA_Port, &mGPIO_InitStruct);

		return HAL_OK;

	}

};

} // namespace CasualNoises

