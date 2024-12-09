
/*
 * 		DEPRICATED
 */


/*
  ==============================================================================

    CS43L22.h
    Created: 24 jul 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef USE_DEPRICATED

#pragma once

#include "DAC_Driver.h"

// Convert volume from 0 to 255 into register setting
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
#define VOLUME_CONVERT(Volume)    ((Volume > 100)? 100:static_cast<uint8_t>((Volume * 255) / 100))
#pragma GCC diagnostic pop

namespace CasualNoises
{

class CS43L22 : public CodecDriver
{
public:
	 CS43L22() = default;
	~CS43L22() = default;

	//==============================================================================
	//          initializeCodec
	//
	// Initialization of a CS43L22 codec
	//
	//  CasualNoises    25/07/2023  First implementation
	//==============================================================================
	bool initializeCodec(const sCodecDriverInitData* const initDataPtr) override
	{
		mCodec_i2c_ptr  = initDataPtr->codec_i2c_ptr;
		mCodecI2C_Adres = initDataPtr->codecI2C_Adres;

		// Reset codec
		HAL_GPIO_WritePin(initDataPtr->codecResetPinPort, initDataPtr->codecResetPinNo, GPIO_PIN_RESET);
		osDelay(10);		// Min 1 ms according to specification
		HAL_GPIO_WritePin(initDataPtr->codecResetPinPort, initDataPtr->codecResetPinNo, GPIO_PIN_SET);

		// Read chip id & revision register
		uint8_t buf = 0x01;
		HAL_StatusTypeDef res = HAL_I2C_Master_Transmit(mCodec_i2c_ptr, mCodecI2C_Adres, &buf, 1, 1000);
		if (res != HAL_OK) return false;
		res = HAL_I2C_Master_Receive (mCodec_i2c_ptr, mCodecI2C_Adres, &buf, 1, 1000);
		if (res != HAL_OK) return false;
		const uint8_t codecIdRevision = 0b11100011;
		if (buf != codecIdRevision) return false;

		// Set codec in power down mode
		if ( codecSetRegister(0x02, 0x01) != HAL_OK ) return false;

		// Turn headphone always on and speaker always off
		if ( codecSetRegister(0x04, 0xAF) != HAL_OK ) return false;

		// Clocking control, enable auto-detect and set divide by 2
		if ( codecSetRegister(0x05, 0x81) != HAL_OK ) return false;

		// Set slave mode on and interface format I2S up to 24 bit, audio word length 16 bit
		if ( codecSetRegister(0x06, 0x07) != HAL_OK ) return false;

		// Set master volume control default
		if ( codecSetMasterVolumeControl( 70 ) != HAL_OK ) return false;

		// Set headphone volume to 0 dB
		if ( codecSetRegister(0x22, 0x00) != HAL_OK ) return false;
		if ( codecSetRegister(0x23, 0x00) != HAL_OK ) return false;

		// No input pass through
		if ( codecSetRegister(0x08, 0x00) != HAL_OK ) return false;
		if ( codecSetRegister(0x09, 0x00) != HAL_OK ) return false;

		// Set codec in power on mode
		if ( codecSetRegister(0x02, 0x9E) != HAL_OK ) return false;

		// No soft ramp on zero crossing
		if ( codecSetRegister(0x0A, 0x05) != HAL_OK ) return false;

		// Disable the limiter attack level
		if ( codecSetRegister(0x27, 0x00) != HAL_OK ) return false;

		// Adjust Bass and Treble levels
		if ( codecSetRegister(0x1F, 0x88) != HAL_OK ) return false;

		// Adjust PCM volume level
		if ( codecSetRegister(0x1A, 0x00) != HAL_OK ) return false;
		if ( codecSetRegister(0x1B, 0x00) != HAL_OK ) return false;

		// Initialization is completed
		mIsInitialized = true;
		return mIsInitialized;

	};

	//==============================================================================
	//          codecSetRegister
	//
	// Set codec master volume control
	//
	//  CasualNoises    25/07/2023  First implementation
	//==============================================================================
	HAL_StatusTypeDef codecSetMasterVolumeControl(uint8_t volume)
	{
		uint8_t buf[2] = { 0x20, VOLUME_CONVERT(volume) };
		buf[1] = 0x00; // **************
		HAL_StatusTypeDef res = HAL_I2C_Master_Transmit(mCodec_i2c_ptr, mCodecI2C_Adres, &buf[0], 2, 1000);
		if (res != HAL_OK) return res;
		buf[0] = 0x21;
		return HAL_I2C_Master_Transmit(mCodec_i2c_ptr, mCodecI2C_Adres, &buf[0], 2, 1000);
	}

private:

	I2C_HandleTypeDef*	mCodec_i2c_ptr;		// I2C device to use, if I2C is not used set to nullptr
	uint8_t				mCodecI2C_Adres;

	//==============================================================================
	//          codecSetRegister
	//
	//  CasualNoises    25/07/2023  First implementation
	//==============================================================================
	HAL_StatusTypeDef codecSetRegister(uint8_t address, uint8_t value)
	{
		uint8_t buf[2] = { address, value };
		return HAL_I2C_Master_Transmit(mCodec_i2c_ptr, mCodecI2C_Adres, &buf[0], 2, 1000);
	}

};

} // namespace CasualNoises

#endif
