/*
  ==============================================================================

    W25Qxx_Driver.h
    Created: 31 Mar 2023 13:45:33
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef NVM_DRIVER_SUPPORT

#include "W25Qxx_Driver.h"

namespace CasualNoises {

// A cache for fast in-sector access
static uint32_t	mSectorCache[cFlashSectorSizeBytes / 4];

//==============================================================================
//          W25Qxx_Driver()
//
//  CasualNoises    31/03/2023  First implementation
//==============================================================================
W25Qxx_Driver::W25Qxx_Driver(const sNVM_DriverInitData* initDataPtr)
: mInitDataPtr(initDataPtr)
{

	// Disable all devices
	for (uint16_t i = 0; i < mInitDataPtr->noOfDevices; ++i)
	{
		HAL_GPIO_WritePin(mInitDataPtr->deviceSelectPorts[i], mInitDataPtr->deviceSelectPins[i], GPIO_PIN_SET);
	}

	// Enable SPI and initialise the flash devices
	__HAL_SPI_ENABLE(mInitDataPtr->hspix_ptr);
	HAL_StatusTypeDef res;
	for (uint16_t i = 0; i < mInitDataPtr->noOfDevices; ++i)
	{
		res = sendDeviceCommand(i, eFlashCommandCodes::ReleasePowerDown, 5);
		if (res != HAL_OK) goto error;
		res = sendDeviceCommand(i, eFlashCommandCodes::EnableReset, 1);
		if (res != HAL_OK) goto error;
		res = sendDeviceCommand(i, eFlashCommandCodes::Reset, 1);
		if (res != HAL_OK) goto error;
		HAL_Delay(1);					// Give the device time to reset (more than 64uSec)
		res = sendDeviceCommand(i, eFlashCommandCodes::ManufactuereDeviceId, 6);
		if ((res != HAL_OK) ||
				(((mInBuffer[4] != 0xef) || (mInBuffer[5] != 0x15)) &&
				 ((mInBuffer[4] != 0xef) || (mInBuffer[5] != 0x17))))
			goto error;
		uint32_t capacity = 8 * 1024 * 1024;
		if (mInBuffer[5] == 0x17)
			capacity = 16 * 1024 * 1024;
		if ((i > 0) && (capacity != mDeviceCapacity))
			goto error;
		mDeviceCapacity = capacity;
		if (res != HAL_OK) goto error;
		res = readStatusRegisters(i);
		if (res != HAL_OK) goto error;
	}

	// Index 1 word above the upper limit, this is used for index error checking
	mErrorIndex = (mDeviceCapacity * mInitDataPtr->noOfDevices) / 4;

	// Load the first sector of the first device into the cache
	res = fastSectorRead(0);
	mCurrentSectorNo = 0;

	// On success
	mDriverReady = true;
	return;

	// On failure
	error:
	mDriverReady = false;
	return;

}

//==============================================================================
//          checkForCacheChange()
//
// ToDo fill this in...
//
//  CasualNoises    31/03/2023  First implementation
//==============================================================================
bool W25Qxx_Driver::checkForCacheChange()
{

	if ((mPreviousWordOffset != 0xffffffff) &&
		(mSectorCache[mPreviousWordOffset] != mPreviousValue))
	{

		// Set corresponding dirty flags
		mSectorCacheDirty = true;
		uint32_t pageNo = (mPreviousWordOffset * 4) / cFlashPageSizeBytes;
		mPageCacheDirty[pageNo] = true;

		// Do we have to erase the complete sector
		// This has to be done when a 0 bit in flash is changed to a logical 1
		mSectorEraseRequired |= ~mPreviousValue & mSectorCache[mPreviousWordOffset];

		// Update previous value and report change
		mPreviousValue = mSectorCache[mPreviousWordOffset];
		return true;

	}
	else
		return false;

}

//==============================================================================
//          [] operator
//
// Handle the subscription operator,
//	supports random access in the flash device array
// The flash memory is projected onto an array of 32 bit words, so index is
//	a 32 bit word index
//
//  CasualNoises    31/03/2023  First implementation
//==============================================================================
uint32_t& W25Qxx_Driver::operator[] (uint32_t index)
{

	// Some useful values
	uint32_t byteIndex  = index * 4;
	uint32_t sectorNo   = byteIndex / cFlashSectorSizeBytes;
	uint32_t byteOffset = byteIndex - (sectorNo * cFlashSectorSizeBytes);
	uint32_t wordOffset = byteOffset / 4;

	// Valid index?
	if (index >= mErrorIndex)
	{
		mOutOfRangeDetected = true;
		return mOutOfRangeWord;
	}

	// Access to the same location?
	if ((wordOffset == mPreviousWordOffset) &&
		(sectorNo   == mCurrentSectorNo))
	{
		return mSectorCache[wordOffset];
	}

	// Did previous location change?
	checkForCacheChange();

	// Have to change sector?
	if (sectorNo != mCurrentSectorNo)
	{
		fastSectorRead(sectorNo * cFlashSectorSizeBytes);
	}

	// Remember this access and return reference
	mPreviousWordOffset = wordOffset;
	mPreviousValue 		= mSectorCache[wordOffset];
	return mSectorCache[mPreviousWordOffset];

}

//==============================================================================
//          eraseAllDevices()
//
// Erase all flash devices
//
//  CasualNoises    01/04/2023  First implementation
//==============================================================================
HAL_StatusTypeDef W25Qxx_Driver::eraseAllDevices()
{

	// Erase devices one by one
	HAL_StatusTypeDef res;
	for (uint16_t deviceNo = 0; deviceNo < mInitDataPtr->noOfDevices; ++deviceNo)
	{

		// Erase a single device
		res = sendDeviceCommand(deviceNo, eFlashCommandCodes::WriteEnable, 1);
		if (res != HAL_OK) goto error;
		res = sendDeviceCommand(deviceNo, eFlashCommandCodes::ChipErase, 1);
		if (res != HAL_OK) goto error;
		res = waitUntilDeviceReady(deviceNo);
		if (res != HAL_OK) goto error;
		res = sendDeviceCommand(deviceNo, eFlashCommandCodes::WriteDisable, 1);
		if (res != HAL_OK) goto error;

	}

	// Load the first sector of the first device into the cache
	res = fastSectorRead(0);
	mCurrentSectorNo = 0;

	error:
	return res;

}

//==============================================================================
//          sendDeviceCommand()
//
// Send a command to a flash device using the SPI port
//
//  CasualNoises    31/03/2023  First implementation
//==============================================================================
HAL_StatusTypeDef W25Qxx_Driver::sendDeviceCommand(uint16_t deviceNo, eFlashCommandCodes commandCode, uint8_t length,
		uint8_t byte_1, uint8_t byte_2, uint8_t byte_3, uint8_t byte_4, uint8_t byte_5, uint8_t byte_6, uint8_t byte_7)
{

	// Set chip select low (make sure it toggles)
	uint16_t deviceSelectPin = mInitDataPtr->deviceSelectPins[deviceNo];
	HAL_GPIO_WritePin (mInitDataPtr->deviceSelectPorts[deviceNo], deviceSelectPin, GPIO_PIN_SET);
	HAL_GPIO_WritePin (mInitDataPtr->deviceSelectPorts[deviceNo], deviceSelectPin, GPIO_PIN_RESET);

	// Fill output buffer with command code and clear input buffer
	uint8_t outBuffer[cCommandBufferSize];		   // Data to send
	outBuffer[0] = (uint8_t)commandCode;
	outBuffer[1] = byte_1;
	outBuffer[2] = byte_2;
	outBuffer[3] = byte_3;
	outBuffer[4] = byte_4;
	outBuffer[5] = byte_5;
	outBuffer[6] = byte_6;
	outBuffer[7] = byte_7;
	for (uint8_t i = 0; i < cCommandBufferSize; ++i)
		mInBuffer[i]  = 0x00;

	// Handle the command to the device and receive the response
	HAL_StatusTypeDef res = HAL_SPI_TransmitReceive (mInitDataPtr->hspix_ptr, outBuffer, mInBuffer, length, HAL_MAX_DELAY);

	// Chip select goes high to end the transaction
	HAL_GPIO_WritePin (mInitDataPtr->deviceSelectPorts[deviceNo], deviceSelectPin, GPIO_PIN_SET);

	return res;

}

//==============================================================================
//          readStatusRegisters()
//
// Read status registers for a given device into mStatusRegisters
//
//  CasualNoises    31/03/2023  First implementation
//==============================================================================
HAL_StatusTypeDef W25Qxx_Driver::readStatusRegisters(uint16_t deviceNo)
{
	HAL_StatusTypeDef res = sendDeviceCommand(deviceNo, eFlashCommandCodes::ReadStatusRegister1, 2);
	if (res != HAL_OK)
		return res;
	mStatusRegisters[deviceNo].sStatisBits_S7_S0 = mInBuffer[1];
	res = sendDeviceCommand(deviceNo, eFlashCommandCodes::ReadStatusRegister2, 2);
	mStatusRegisters[deviceNo].sStatisBits_S15_S8 = mInBuffer[1];
	return res;
}

//==============================================================================
//          waitUntilDeviceReady()
//
// Wait until the busy bit in the device status register is cleared
// Performs a busy form of waiting!
//
//  CasualNoises    01/04/2023  First implementation       ;)
//==============================================================================
HAL_StatusTypeDef W25Qxx_Driver::waitUntilDeviceReady(uint16_t deviceNo)
{
	HAL_StatusTypeDef res = HAL_OK;
	while (true)
	{
		res = readStatusRegisters(deviceNo);
		if ((res != HAL_OK) || ((mStatusRegisters[deviceNo].sStatisBits_S7_S0 & 0x01) == 0))
			break;
		HAL_Delay(1);
	}
	return res;
}

//==============================================================================
//          fastSectorRead()
//
// Read the contents of a sector (4 KBytes) into the mSectorBuffer using fast read command
// Flush the current cache contents first, if the cache is dirty
//
//  CasualNoises    31/03/2023  First implementation
//==============================================================================
// address holds the address of the first byte of the sector, must be 4 KB aligned
HAL_StatusTypeDef W25Qxx_Driver::fastSectorRead(uint32_t address)
{

	// Flush the cache if it is dirty (even if the same sector is requested as the one in cache)
	if (mSectorCacheDirty)
	{
		flushSectorCache();
	}

	// Get the device no from the address
	uint32_t deviceNo = address / mDeviceCapacity;
	uint32_t startAddress = address - (deviceNo * mDeviceCapacity);

	// Set chip select low (make sure it toggles)
	uint16_t deviceSelectPin = mInitDataPtr->deviceSelectPins[deviceNo];
	HAL_GPIO_WritePin (mInitDataPtr->deviceSelectPorts[deviceNo], deviceSelectPin, GPIO_PIN_SET);
	HAL_GPIO_WritePin (mInitDataPtr->deviceSelectPorts[deviceNo], deviceSelectPin, GPIO_PIN_RESET);

	// Send a fast read command with the desired address
	uint8_t commandBuffer[4];
	commandBuffer[0] = (uint8_t)eFlashCommandCodes::FastReadData;
	commandBuffer[1] = startAddress >> 16;
	commandBuffer[2] = startAddress >> 8;
	commandBuffer[3] = startAddress;
	HAL_StatusTypeDef res;
	res = HAL_SPI_TransmitReceive (mInitDataPtr->hspix_ptr, commandBuffer, mInBuffer, 4, HAL_MAX_DELAY);
	if (res != HAL_OK) goto error;

	// Send dummy 1 dummy byte (8 clock cycles) to set-up the device
	res = HAL_SPI_TransmitReceive (mInitDataPtr->hspix_ptr, commandBuffer, mInBuffer, 1, HAL_MAX_DELAY);
	if (res != HAL_OK) goto error;

	// Read the sector data
	res = HAL_SPI_TransmitReceive (mInitDataPtr->hspix_ptr, (uint8_t *)mSectorCache, (uint8_t *)mSectorCache, cFlashSectorSizeBytes, HAL_MAX_DELAY);
	if (res != HAL_OK) goto error;

	// Cache is clean now
	mSectorCacheDirty = false;
	for (uint16_t i = 0; i < cNoOfPagesInSector; ++i)
		mPageCacheDirty[i] = false;
	mSectorEraseRequired = false;
	mCurrentSectorNo = address / cFlashSectorSizeBytes;

	error:

	// Chip select goes high to end the transaction
	HAL_GPIO_WritePin (mInitDataPtr->deviceSelectPorts[deviceNo], deviceSelectPin, GPIO_PIN_SET);

	return res;

}

//==============================================================================
//          flushSectorCache()
//
// Write the contents of the cache to the flash device
//
//  CasualNoises    02/04/2023  First implementation
//==============================================================================
HAL_StatusTypeDef W25Qxx_Driver::flushSectorCache()
{

	// Only do this when the cache is dirty
	checkForCacheChange();
	if ( ! mSectorCacheDirty )
		return HAL_OK;

	// Get the device no from the current sector no & sector no in device
	uint32_t noOfSectors = mDeviceCapacity / cFlashSectorSizeBytes;
	uint32_t deviceNo    = mCurrentSectorNo / noOfSectors;
	uint32_t sectorNo	 = mCurrentSectorNo - (deviceNo * noOfSectors);

	// Erase sector if required
	HAL_StatusTypeDef res;
	uint16_t deviceSelectPin;
	if (mSectorEraseRequired)
	{

		// Enable write on device
		res = sendDeviceCommand(deviceNo, eFlashCommandCodes::WriteEnable, 1);
		if (res != HAL_OK) goto error;

		// Start sector erase
		uint32_t startAddress = sectorNo * cFlashSectorSizeBytes;
		res = sendDeviceCommand(deviceNo, eFlashCommandCodes::SectorErase, 4,
				startAddress >> 16, startAddress >> 8, startAddress);
		if (res != HAL_OK) goto error;

		// Wait until the device is ready
		res = waitUntilDeviceReady(deviceNo);
		if (res != HAL_OK) goto error;

		// Disable write on device
		res = sendDeviceCommand(deviceNo, eFlashCommandCodes::WriteDisable, 1);
		if (res != HAL_OK) goto error;

	}

	// Write current sector to flash page after page
	for (uint32_t pageNo = 0; pageNo < cNoOfPagesInSector; ++pageNo)
	{

		// Have to write this page?
		if (mSectorEraseRequired || mPageCacheDirty[pageNo])
		{

			// Enable write on device
			res = sendDeviceCommand(deviceNo, eFlashCommandCodes::WriteEnable, 1);
			if (res != HAL_OK) goto error;

			// Set chip select low (make sure it toggles)
			uint16_t deviceSelectPin = mInitDataPtr->deviceSelectPins[deviceNo];
			HAL_GPIO_WritePin (mInitDataPtr->deviceSelectPorts[deviceNo], deviceSelectPin, GPIO_PIN_SET);
			HAL_GPIO_WritePin (mInitDataPtr->deviceSelectPorts[deviceNo], deviceSelectPin, GPIO_PIN_RESET);

			// Send destination address to the flash device
			uint32_t destAddress = (sectorNo * cFlashSectorSizeBytes) + (pageNo * cFlashPageSizeBytes);
			uint8_t commandBuffer[4];
			commandBuffer[0] = (uint8_t)eFlashCommandCodes::PageProgram;
			commandBuffer[1] = destAddress >> 16;
			commandBuffer[2] = destAddress >> 8;
			commandBuffer[3] = destAddress;
			res = HAL_SPI_TransmitReceive (mInitDataPtr->hspix_ptr, commandBuffer, mInBuffer, 4, HAL_MAX_DELAY);
			if (res != HAL_OK) goto error;

			// Perform the page write
			uint8_t* srcAddress = (uint8_t*)((uint32_t)mSectorCache) + (pageNo * cFlashPageSizeBytes);
			res = HAL_SPI_TransmitReceive (mInitDataPtr->hspix_ptr, srcAddress, srcAddress, cFlashPageSizeBytes, HAL_MAX_DELAY);

			// Chip select goes high to end the transaction
			HAL_GPIO_WritePin (mInitDataPtr->deviceSelectPorts[deviceNo], deviceSelectPin, GPIO_PIN_SET);

			// Wait until the device is ready
			res = waitUntilDeviceReady(deviceNo);
			if (res != HAL_OK) goto error;

			// Disable write on device
			res = sendDeviceCommand(deviceNo, eFlashCommandCodes::WriteDisable, 1);
			if (res != HAL_OK) goto error;

		}

	}

	// Reload the cache from flash (the page program command overwrites the cache...)
	mSectorCacheDirty = false;
	fastSectorRead(mCurrentSectorNo * cFlashSectorSizeBytes);

	error:

	// Chip select goes high to end the transaction
	HAL_GPIO_WritePin (mInitDataPtr->deviceSelectPorts[deviceNo], deviceSelectPin, GPIO_PIN_SET);

	return res;

}

} // namespace CasualNoises

#endif

