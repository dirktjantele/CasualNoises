/*
  ==============================================================================

    NVM_Driver.h
    Created: 02 Apr 2023 13:25:33
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_NVM_DRIVER_SUPPORT

#pragma once

#include "stm32h7xx_hal.h"

namespace CasualNoises {

constexpr uint16_t	cMaxNoOfDevices 	  = 8;

/*
 * Data structure used to initialise the driver
 */
typedef struct
{

	uint16_t		noOfDevices;							// Actual no of devices present

	GPIO_TypeDef*	deviceSelectPorts[cMaxNoOfDevices];		// Ports...
	uint16_t 		deviceSelectPins[cMaxNoOfDevices];		// ... pins for the device select lines

	SPI_HandleTypeDef* hspix_ptr;							// SPI device to use for communication

} sNVM_DriverInitData;

/*
 * The NVM_Driver class
 */
class NVM_Driver
{
public:

	virtual uint32_t& operator[] (uint32_t index) 	= 0;

	virtual HAL_StatusTypeDef	eraseAllDevices() 	= 0;
	virtual HAL_StatusTypeDef 	flushSectorCache() 	= 0;

	virtual bool				isDriverReady() 	= 0;
	virtual uint32_t			getTotalCapacity() 	= 0;

	virtual bool				outOfRangeDetected() = 0;

	virtual uint32_t			getNoOfDevices() 		= 0;
	virtual uint32_t			getDeviceCapacity() 	= 0;

};

} // namespace CasualNoises

#endif
