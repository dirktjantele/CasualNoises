/*
 * CS4270_Driver.h
 *
 *  Created on: Oct 23, 2024
 *      Author: dirk tjantele
 */

#pragma once

#include "Codec_Driver.h"

namespace CasualNoises
{

// CS4270 Register addresses
const uint8_t CS4270_REG_DEVICEID			= 0x01;
const uint8_t CS4270_REG_POWERCONTROL		= 0x02;
const uint8_t CS4270_REG_MODECONTROL		= 0x03;
const uint8_t CS4270_REG_ADCDACCONTROL		= 0x04;
const uint8_t CS4270_REG_TRANSISTIONCONTROL	= 0x05;
const uint8_t CS4270_REG_MUTECONTROL		= 0x06;
const uint8_t CS4270_REG_DACAVOLCONTROL		= 0x07;
const uint8_t CS4270_REG_DACBVOLCONTROL		= 0x08;

// I2C Address
const uint8_t CS4270_I2C_ADDRESS			= 0x48 << 1;

// Device ID
const uint8_t CS4270_DEVICEID				= 0xC0;

typedef struct CS4270_DriverParams
{
	GPIO_TypeDef*		codecResetPort;
	uint16_t	  		codecResetPin;
	I2C_HandleTypeDef 	*hi2cHandle;
};

class CS4270_Driver : private Codec_Driver
{
public:
	~CS4270_Driver() = default;

	static Codec_Driver& CreateDriver(CS4270_DriverParams& params)
	{
		static CS4270_Driver instance(params);
		return instance;
	}

	virtual HAL_StatusTypeDef initializeCodec()
	{
		HAL_StatusTypeDef res = HAL_OK;

		// Reset the codec
		resetCodec();

		// Put codec in power down mode before adjusting configuration params
		res = CS4270_RegWrite(CS4270_REG_POWERCONTROL, 0xA3);
		if (res != HAL_OK)
			CN_ReportFault(2);

		// Verify the device id
		uint8_t deviceId = 0x00;
		res = CS4270_RegRead(CS4270_REG_DEVICEID, &deviceId);
		if ((res  != HAL_OK) || ((deviceId & 0xF0) != CS4270_DEVICEID))
			CN_ReportFault(2);

		// Default configuration register settings in reversed order
		uint8_t confgSettings[8] =
		{
				CS4270_REG_DEVICEID,
				0x00,					// Power up
				0x31,					// Mode (slave, normal speed, pop guard enabled)
				0x09,					// ADC/DAC (disable HPF, enable I2S format
				0x00,					// Transition (independent volume controls)
				0x00,					// No mute on channels A & b
				0x00,					// DAC A Vol (0 db)
				0x00,					// DAC A Vol (0 db)
		};

		// Update control registers (read-back and verify)
		for (int8_t regIndex = CS4270_REG_DACBVOLCONTROL; regIndex >= CS4270_REG_POWERCONTROL; --regIndex)
		{

			// Write 'default' value to register
			res = CS4270_RegWrite(regIndex, confgSettings[regIndex - 1]);
			if (res != HAL_OK) CN_ReportFault(2);

			// Read-back register value
			uint8_t regData;
			res = CS4270_RegRead(regIndex, &regData);
			if ((res != HAL_OK) || (regData != confgSettings[regIndex - 1]))
				CN_ReportFault(2);

		}

		// Success...
		return HAL_OK;

	}

private:
	 CS4270_Driver(CS4270_DriverParams& params)
	 {
		 mParams = params;
	 }

	 void resetCodec()
	 {
			HAL_GPIO_WritePin(mParams.codecResetPort, mParams.codecResetPin, GPIO_PIN_SET);
			HAL_Delay(10);
			HAL_GPIO_WritePin(mParams.codecResetPort, mParams.codecResetPin, GPIO_PIN_RESET);
			HAL_Delay(25);
			HAL_GPIO_WritePin(mParams.codecResetPort, mParams.codecResetPin, GPIO_PIN_SET);
			HAL_Delay(25);
	 }

	 HAL_StatusTypeDef CS4270_RegWrite (uint8_t regAddr, uint8_t regData)
	 {
		 return HAL_I2C_Mem_Write (mParams.hi2cHandle, CS4270_I2C_ADDRESS, regAddr, I2C_MEMADD_SIZE_8BIT, &regData, I2C_MEMADD_SIZE_8BIT, HAL_MAX_DELAY);
	 }

	 HAL_StatusTypeDef CS4270_RegRead (uint8_t regAddr, uint8_t *regData)
	 {
		 return HAL_I2C_Mem_Read(mParams.hi2cHandle, CS4270_I2C_ADDRESS, regAddr, I2C_MEMADD_SIZE_8BIT, regData, I2C_MEMADD_SIZE_8BIT, HAL_MAX_DELAY);
	 }

	 CS4270_DriverParams	mParams;

};

}

