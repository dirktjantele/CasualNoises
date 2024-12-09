
/*
 * 		DEPRICATED
 */


/*
  ==============================================================================

    CodecDriver.h
    Created: 25 jul 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef USE_DEPRICATED

#pragma once

namespace CasualNoises
{

typedef struct
{
	I2C_HandleTypeDef*	codec_i2c_ptr;		// I2C device to use, if I2C is not used set to nullptr
	uint8_t				codecI2C_Adres;
	GPIO_TypeDef*		codecResetPinPort;	// GPIO port for codec reset pin
	uint16_t 			codecResetPinNo;	// GPIO pin no for codec reset pin
	uint16_t			audioWordLength;	// Word length used by the codec
} sCodecDriverInitData;

class CodecDriver
{
public:
	 CodecDriver() = default;
	~CodecDriver() = default;

	//==============================================================================
	//          initializeCodec
	//
	// Should be overwritten by the actual driver class
	// If I2C is used, make shure that the hardware is initialized using HAL_I2S_Init
	//
	//  CasualNoises    25/07/2023  First implementation
	//==============================================================================
	virtual bool		initializeCodec(const sCodecDriverInitData* const) = 0;

	bool		isInialized() { return mIsInitialized; }

protected:
	bool		mIsInitialized = false;

};

} // namespace CasualNoises

#endif
