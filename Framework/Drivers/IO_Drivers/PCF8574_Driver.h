/*
  ==============================================================================

    PCF8574_Driver.h
    Created: 4 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "FreeRTOS.h"
#include "semphr.h"

#include "main.h"

namespace CasualNoises
{

constexpr uint8_t cPCF8574_Addres_1		= 0x20 << 1;
constexpr uint8_t cPCF8574_Addres_2		= 0x21 << 1;

// Struct used to hold the I2C device address of the PCF8574 and the pin no
typedef struct
{
	uint8_t		deviceAddress;
	uint16_t	pinNo;				// Should be 16 bit, see EncoderThread
} sPinMapEntry;

class PCF8574_Driver
{
public:
	 PCF8574_Driver() = delete;
	~PCF8574_Driver() = default;

	static PCF8574_Driver* getPCF8574_Driver(I2C_HandleTypeDef hi2c, SemaphoreHandle_t I2Cx_Semaphore)
	{
		if (hi2c.State != HAL_I2C_STATE_READY)
			return nullptr;
		if (mPCF8574_DriverPtr == nullptr)
			mPCF8574_DriverPtr = new PCF8574_Driver(hi2c, I2Cx_Semaphore);
		return mPCF8574_DriverPtr;
	}

	void 			writePin(uint8_t address, uint8_t pin, GPIO_PinState state);
	GPIO_PinState	readPin(uint8_t address, uint8_t pin);
	uint8_t			readAllPins(uint8_t address);

private:
	PCF8574_Driver(I2C_HandleTypeDef hi2c, SemaphoreHandle_t I2Cx_Semaphore);

	static PCF8574_Driver*	mPCF8574_DriverPtr;

	I2C_HandleTypeDef		mI2C_HandlePtr { nullptr };
	SemaphoreHandle_t 		mDriverAccessSemaphore { nullptr };

};

} // namespace CasualNoises
