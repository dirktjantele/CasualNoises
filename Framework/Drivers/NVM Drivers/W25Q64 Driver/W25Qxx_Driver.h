/*
  ==============================================================================

    W25Qxx_Driver.h
    Created: 31 mar 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_NVM_DRIVER_SUPPORT

#pragma once

#include "stm32h7xx_hal.h"

#include "../NVM Driver/NVM_Driver.h"

namespace CasualNoises
{

// Command buffer size = 8, we need to send 8 dummy bytes during a fast sector read
const uint16_t	cCommandBufferSize 	  = 8;

const uint16_t	cFlashSectorSizeBytes =	(4 * 1024);		// 4 KByte sectors
const uint16_t	cFlashPageSizeBytes   =	256;			// 256 byte pages
const uint16_t	cNoOfPagesInSector	  = cFlashSectorSizeBytes / cFlashPageSizeBytes;

// Flash device command codes
enum class eFlashCommandCodes
{
	WriteStatusRegister	 = 0x01,
	PageProgram			 = 0x02,
	ReadData			 = 0x03,
	WriteDisable		 = 0x04,
	ReadStatusRegister1	 = 0x05,
	WriteEnable			 = 0x06,
	FastReadData		 = 0x0b,
	SectorErase			 = 0x20,
	ReadStatusRegister2	 = 0x35,
	EnableReset			 = 0x66,
	ManufactuereDeviceId = 0x90,
	Reset				 = 0x99,
	Read_JEDEC_ID		 = 0x9f,
	ReleasePowerDown	 = 0xab,
	ChipErase			 = 0xc7,
	DisableQPI			 = 0xff
};

// Status register bits for a flash device
typedef struct
{
	uint8_t		sStatisBits_S7_S0;
	uint8_t		sStatisBits_S15_S8;
} sStatusRegisters;

/*
 * The W25Qxx_Driver class
 */
class W25Qxx_Driver : public NVM_Driver
{

public:
	W25Qxx_Driver() = delete;
	W25Qxx_Driver(const sNVM_DriverInitData* initDataPtr);
	~W25Qxx_Driver() = default;

	uint32_t& operator[] (uint32_t index) override;

	HAL_StatusTypeDef	eraseAllDevices() override;
	HAL_StatusTypeDef 	flushSectorCache() override;

	bool				isDriverReady() override		{ return mDriverReady; }
	uint32_t			getTotalCapacity() override		{ return mDeviceCapacity * mInitDataPtr->noOfDevices; }

	bool				outOfRangeDetected() override	{ return mOutOfRangeDetected; }

	uint32_t			getNoOfDevices() override		{ return mInitDataPtr->noOfDevices; }
	uint32_t			getDeviceCapacity()	override	{ return mDeviceCapacity; }

private:
	const sNVM_DriverInitData* mInitDataPtr  { nullptr };	// Copy of the pointer found in the constructor

	bool		mDriverReady { false };						// Set if driver is ready for duty

	uint8_t		mInBuffer[cCommandBufferSize];		   		// Data received from device during sendDeviceCommand()

	uint32_t	mDeviceCapacity		{ 0 };					// For a single device in bytes (all devices should be equal!)
	uint32_t	mErrorIndex			{ 0 };					// Index (in 32 bit words) 1 word above the max index (out of range check)
	uint32_t 	mOutOfRangeWord 	{ 0xffffffff };			// Location used for out of range access
	bool		mOutOfRangeDetected { false };				// Set when of of range access is detected

	sStatusRegisters mStatusRegisters[cMaxNoOfDevices];		// Status registers contents

	// Support for random access on word entities using the subscription operator []
	bool		mSectorCacheDirty 	{ false };				// Set if cache contents is changed
	bool		mPageCacheDirty[cNoOfPagesInSector];		// Set if the contents of a single page is changed
	bool		mSectorEraseRequired { false };				// Set when we need to erase the complete sector
	uint32_t	mPreviousWordOffset { 0xffffffff };			// Index to previous accessed location
	uint32_t	mPreviousValue		{ 0x00000000 };			// Value at above location when first accessed
	uint32_t	mCurrentSectorNo 	{ 0xffffffff };			// No of the sector in cache

	bool 		checkForCacheChange();

	HAL_StatusTypeDef sendDeviceCommand(uint16_t deviceNo, eFlashCommandCodes commandCode, uint8_t length,
			uint8_t byte_1 = 0x00, uint8_t byte_2 = 0x00, uint8_t byte_3 = 0x00, uint8_t byte_4 = 0x00,
			uint8_t byte_5 = 0x00, uint8_t byte_6 = 0x00, uint8_t byte_7 = 0x00);
	HAL_StatusTypeDef readStatusRegisters(uint16_t deviceNo);

	HAL_StatusTypeDef waitUntilDeviceReady(uint16_t deviceNo);

	HAL_StatusTypeDef fastSectorRead(uint32_t address);

};

} // namespace CasualNoises

#endif
