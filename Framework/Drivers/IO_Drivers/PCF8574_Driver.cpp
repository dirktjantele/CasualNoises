/*
  ==============================================================================

    PCF8574_Driver.h
    Created: 4 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef USE_PCF8574_Driver

#include "PCF8574_Driver.h"

#include "CasualNoises.h"

namespace CasualNoises
{

PCF8574_Driver*	PCF8574_Driver::mPCF8574_DriverPtr 	= nullptr;

//==============================================================================
//          PCF8574_Driver()
//
// Private constructor implementing the singleton design pattern
//
//  CasualNoises    04/08/2023  First implementation
//==============================================================================
PCF8574_Driver::PCF8574_Driver(I2C_HandleTypeDef hi2c, SemaphoreHandle_t I2Cx_Semaphore)
: mI2C_HandlePtr( hi2c ),
  mDriverAccessSemaphore( I2Cx_Semaphore )
{
}

//==============================================================================
//          writePin()
//
//  CasualNoises    05/08/2023  First implementation
//==============================================================================
void PCF8574_Driver::writePin(uint8_t address, uint8_t pin, GPIO_PinState state)
{

	// Take the lock
	if (mDriverAccessSemaphore != nullptr)
	{
		BaseType_t xres = xSemaphoreTake( mDriverAccessSemaphore, portMAX_DELAY );
		if (xres != pdPASS) CN_ReportFault(1);
	}

	// Write to the pin
	uint8_t buf = 0x00;
	HAL_StatusTypeDef res = HAL_I2C_Master_Receive(&mI2C_HandlePtr, address, &buf, 1, 1000);
	if (res != HAL_OK) CN_ReportFault(1);
	if (state == GPIO_PIN_SET)
		buf |= pin;
	else
		buf &= ~pin;
	res = HAL_I2C_Master_Transmit(&mI2C_HandlePtr, address, &buf, 1, 1000);
	if (res != HAL_OK) CN_ReportFault(1);

	// Release the lock
	if (mDriverAccessSemaphore != nullptr)
	{
		BaseType_t xres = xSemaphoreGive( mDriverAccessSemaphore );
		if (xres != pdPASS) CN_ReportFault(1);
	}

}

//==============================================================================
//          readPin()
//
//  CasualNoises    05/08/2023  First implementation
//==============================================================================
GPIO_PinState PCF8574_Driver::readPin(uint8_t address, uint8_t pin)
{

	// Take the lock
	if (mDriverAccessSemaphore != nullptr)
	{
		BaseType_t xres = xSemaphoreTake( mDriverAccessSemaphore, portMAX_DELAY );
		if (xres != pdPASS) CN_ReportFault(1);
	}

	// Get the state of the pin
	uint8_t buf = 0x00;
	HAL_StatusTypeDef res = HAL_I2C_Master_Receive(&mI2C_HandlePtr, address, &buf, 1, 1000);
	if (res != HAL_OK) CN_ReportFault(1);
	uint8_t pinState = buf & pin;
	GPIO_PinState state = GPIO_PIN_SET;
	if (pinState == 0x00)
		state = GPIO_PIN_RESET;

	// Release the lock
	if (mDriverAccessSemaphore != nullptr)
	{
		BaseType_t xres = xSemaphoreGive( mDriverAccessSemaphore );
		if (xres != pdPASS) CN_ReportFault(1);
	}

	return state;

}

//==============================================================================
//          readAllPins()
//
//  CasualNoises    07/08/2023  First implementation
//==============================================================================
uint8_t	PCF8574_Driver::readAllPins(uint8_t address)
{

	// Take the lock
	if (mDriverAccessSemaphore != nullptr)
	{
		BaseType_t xres = xSemaphoreTake( mDriverAccessSemaphore, portMAX_DELAY );
		if (xres != pdPASS) CN_ReportFault(1);
	}

	// Get the state of all pins
	uint8_t state = 0x00;
	HAL_StatusTypeDef res = HAL_I2C_Master_Receive(&mI2C_HandlePtr, address, &state, 1, 1000);
	if (res != HAL_OK) CN_ReportFault(1);

	// Release the lock
	if (mDriverAccessSemaphore != nullptr)
	{
		BaseType_t xres = xSemaphoreGive( mDriverAccessSemaphore );
		if (xres != pdPASS) CN_ReportFault(1);
	}

	return state;

}

} // namespace CasualNoises

#endif

