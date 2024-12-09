/*
 * CS4272_Driver.h
 *
 *  Created on: Dec 1, 2024
 *      Author: dirk tjantele
 */

#pragma once

#include "Codec_Driver.h"

namespace CasualNoises
{

// CS4272 Register addresses
constexpr uint8_t CS4272_MODE_CNTRL_1		= 0x01;
constexpr uint8_t CS4272_DAC_CNTRL			= 0x02;
constexpr uint8_t CS4272_DAC_VOL_MIX_CNTRL	= 0x03;
constexpr uint8_t CS4272_DAC_CHA_VOL_CNTRL	= 0x04;
constexpr uint8_t CS4272_DAC_CHB_VOL_CNTRL	= 0x05;
constexpr uint8_t CS4272_ADC_CNTRL			= 0x06;
constexpr uint8_t CS4272_MODE_CNTRL_2		= 0x07;
constexpr uint8_t CS4272_CHIP_ID			= 0x08;

// I2C Address
const uint8_t CS4272_I2C_ADDRESS			= 0x10 << 1;

// Device ID
const uint8_t CS4272_DEVICEID				= 0x00;

typedef struct CS4272_DriverParams
{
	GPIO_TypeDef*		codecResetPort;
	uint16_t	  		codecResetPin;
	I2C_HandleTypeDef 	*hi2cHandle;
};

class CS4272_Driver : private Codec_Driver
{
public:
	~CS4272_Driver() = default;

	static Codec_Driver& CreateDriver(CS4272_DriverParams& params)
	{
		static CS4272_Driver instance(params);
		return instance;
	}

	virtual HAL_StatusTypeDef initializeCodec()
	{
		HAL_StatusTypeDef res = HAL_OK;

		// Reset the codec
		resetCodec();

		// Put codec in power down mode before adjusting configuration params
		res = CS4272_RegWrite(CS4272_MODE_CNTRL_2, 0x03);
		if (res != HAL_OK)
			CN_ReportFault(2);

		// Verify the device id
		uint8_t deviceId = 0x00;
		res = CS4272_RegRead(CS4272_CHIP_ID, &deviceId);
		if ((res  != HAL_OK) || ((deviceId & 0xF0) != CS4272_DEVICEID))
			CN_ReportFault(2);

		// Default configuration register settings
		uint8_t confgSettings[8] =
		{
				0x00,		// CS4272_MODE_CNTRL_1: single speed, Left Justified, up to 24-bit data (default)
				0x05,		// CS4272_DAC_CNTRL: no auto mute, slow roll off filter, 44.1 kHz de-emphasis,
							//							immediate un-mute, no inversion of the signal polarity
				0x06,		// CS4272_DAC_VOL_MIX_CNTRL: B!=A, no soft ramp, zero cross enabled, AOUTA = aR & AOUTB = bL
				0x00,		// CS4272_DAC_CHA_VOL_CNTRL: no mute, volume 0dB
				0x00,		// CS4272_DAC_CHB_VOL_CNTRL: see above
				0x00,		// CS4272_ADC_CNTRL: no dither, 24bit left justified, no mute, enable HPF
				0x02		// CS4272_MODE_CNTRL_2: no loop back, MUTECA!=B, no freeze, CPEN open, no PDN
		};

		// Update control registers (read-back and verify)
		for (int8_t regIndex = 0; regIndex < CS4272_CHIP_ID; ++regIndex)
		{

			// Write 'default' value to register
			res = CS4272_RegWrite(regIndex + 1, confgSettings[regIndex]);
			if (res != HAL_OK) CN_ReportFault(2);

			// Read-back register value
			uint8_t regData;
			res = CS4272_RegRead(regIndex + 1, &regData);
			if ((res != HAL_OK) || (regData != confgSettings[regIndex]))
				CN_ReportFault(2);

		}

		// Success...
		return HAL_OK;

	}

private:
	 CS4272_Driver(CS4272_DriverParams& params)
	 {
		 mParams = params;
	 }

	 void resetCodec()
	 {
			HAL_GPIO_WritePin(mParams.codecResetPort, mParams.codecResetPin, GPIO_PIN_SET);
			HAL_Delay(5);
			HAL_GPIO_WritePin(mParams.codecResetPort, mParams.codecResetPin, GPIO_PIN_RESET);
			HAL_Delay(25);
			HAL_GPIO_WritePin(mParams.codecResetPort, mParams.codecResetPin, GPIO_PIN_SET);
			HAL_Delay(5);
	 }

	 HAL_StatusTypeDef CS4272_RegWrite (uint8_t regAddr, uint8_t regData)
	 {
		 return HAL_I2C_Mem_Write (mParams.hi2cHandle, CS4272_I2C_ADDRESS, regAddr, I2C_MEMADD_SIZE_8BIT, &regData, I2C_MEMADD_SIZE_8BIT, HAL_MAX_DELAY);
	 }

	 HAL_StatusTypeDef CS4272_RegRead (uint8_t regAddr, uint8_t *regData)
	 {
		 return HAL_I2C_Mem_Read(mParams.hi2cHandle, CS4272_I2C_ADDRESS, regAddr, I2C_MEMADD_SIZE_8BIT, regData, I2C_MEMADD_SIZE_8BIT, HAL_MAX_DELAY);
	 }

	 CS4272_DriverParams	mParams;

};

}

