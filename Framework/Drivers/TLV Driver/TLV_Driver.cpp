/*
  ==============================================================================

    TLV_Driver.cpp
    Created: 02 Apr 2023 15:10:33
    Author:  Dirk Tjantele

	Handles TLV objects in NVM space

  ==============================================================================
*/

#ifdef CASUALNOISES_NVM_DRIVER_SUPPORT

#include "TLV_Driver.h"

#include "../../Utilities/ReportFault.h"

namespace CasualNoises {

//==============================================================================
//          TLV_Driver()
//
//  CasualNoises    02/04/2023  First implementation
//==============================================================================
TLV_Driver::TLV_Driver(NVM_Driver* inNVM_DriverPtr) :
		mNVM_DriverPtr(inNVM_DriverPtr)
{

	// First word should be the magic code
	uint32_t magic = (*mNVM_DriverPtr)[0];
	if (magic != cMagicCode)
	{

		// Erase all devices if magic was not found
		mNVM_DriverPtr->eraseAllDevices();

		// Initialise NVM / create first free TLV
		(*mNVM_DriverPtr)[0] = cMagicCode;
		deleteAllTLVs();

	}

	// NVM space ends here
	mNVM_AfterEndIndex = mNVM_DriverPtr->getTotalCapacity() / 4;

	// Find first empty TLV
	mFreeTLV_Index = TLV_Driver::findNextTLV (cFreeTLV_Tag, 0);

}

//==============================================================================
//          deleteAllTLVs()
//
// Delete all TLV's by creating a large free TLV as the first one in NVM
//
//  CasualNoises    02/04/2023  First implementation
//==============================================================================
void TLV_Driver::deleteAllTLVs()
{
	// Replace the first tag with a FREE tag
	uint32_t freeSpace = (mNVM_DriverPtr->getTotalCapacity() / 4) - 1;
	(*mNVM_DriverPtr)[1] = cFreeTLV_Tag;
	(*mNVM_DriverPtr)[2] = freeSpace;
	mNVM_DriverPtr->flushSectorCache();

}

//==============================================================================
//          findNextTLV()
//
// Find a TLV with given tag starting from the given index (index not included)
// index = 0: start at the start of NVM space
//
//  CasualNoises    02/04/2023  First implementation
//==============================================================================
uint32_t TLV_Driver::findNextTLV (uint32_t tag, uint32_t index)
{

	// Skip current TLV or start from the first one?
	if (index < 1)
		index = 1;
	else
		index += getLength(index);

	// Find TLV until end of NVM space
	while ((index < mNVM_AfterEndIndex) &&
			(getTag(index) != tag))
	{
		uint32_t step = getLength(index);
		if (step ==0)
			return 0;
		index += step;
	}

	// Return index if TLV found
	if (index >= mNVM_AfterEndIndex)
		return 0;
	else
		return index;

}

//==============================================================================
//          addTLV()
//
// Add a TLV to the NVM storage with a given tag, length and value
// Tags can be duplicated in TLV space, so no check is done to see if a TLV with
//		given tag already exists
// length: TLV length in words
// Return: false if no free NVM space was available
//
//  CasualNoises    30/10/2023  First implementation
//  CasualNoises    24/12/2025	Bug fix
//
//==============================================================================
bool TLV_Driver::addTLV(uint32_t tag, uint32_t length, uint32_t* valuePtr)
{

	// Find free TLV that is large enough
	uint32_t index = findNextTLV (cFreeTLV_Tag, 0);
	while (index != 0)
	{
		if (getLength(index) >= length + 2)
			break;
		index = findNextTLV (cFreeTLV_Tag, index);
	}
	if (index == 0)
		return false;

	// Create a new free TLV after this one
	if (getLength(index) > (length + 8))
	{
		uint32_t newIndex = index + length + 2;
		setTag(newIndex, cFreeTLV_Tag);
		setLength(newIndex, getLength(index) - length - 2);
	}

	// Create new tag
	setTag(index, tag);
	setLength(index, length + 2);
	for (uint32_t i = 0; i < length; ++i)
	{
		setValue(index + i, valuePtr[i]);
	}

	// Save changes into the flash devices
	mNVM_DriverPtr->flushSectorCache();

	// Mission successful
	return true;

}

//==============================================================================
//          readTLV()
//
// Read a TLV from flash into memory
// length: TLV length in words
// Return: no of words read
//
//  CasualNoises    23/12/2024  First implementation
//==============================================================================
uint32_t TLV_Driver::readTLV(uint32_t index, uint32_t length, uint32_t* valuePtr)
{
	uint32_t tlvLength = getLength(index) - 2;
	if (tlvLength > length)
		tlvLength = length;
	for (uint32_t i = 0; i < tlvLength; ++i)
	{
		valuePtr[i] = getValue(index + i);
	}
	return tlvLength;
}

//==============================================================================
//          deleteTLV()
//
// Delete a TLV from the flash
// deleteAll: false -> delete only the first TLV with the given tag
//			  true  -> delete all TLV's with the given tag
//
//  CasualNoises    31/10/2023  First implementation
//==============================================================================
void TLV_Driver::deleteTLV(uint32_t tag, bool deleteAll)
{

	bool proceed = true;
	uint32_t index = findNextTLV (tag, 0);
	while ((index != 0) && proceed)
	{
		setTag(index, cFreeTLV_Tag);
		index = findNextTLV (tag, index);
		proceed = deleteAll;
	}

	// ToDo join with next free tlv if there is one

	// Save changes into the flash devices
	mNVM_DriverPtr->flushSectorCache();

}

//==============================================================================
//          updateTLV()
//
// Update a TLV in the flash
// If the TLV does not already exists, create one
// If the TLV exists, but differs in size, delete it and create a new one
// 	otherwise, update the TLV in location
//
//  CasualNoises    24/12/2025  First implementation
//==============================================================================
bool TLV_Driver::updateTLV(uint32_t index, uint32_t length, uint32_t* valuePtr)
{
	uint32_t tag = getTag(index);
	setTag(index, cFreeTLV_Tag);
	bool res = addTLV(tag, length, valuePtr);
	return res;
}

//==============================================================================
//          getLargestFreeTLV_Size()
//
// Scan all TLV's and return the size of the largest free TLV
//
//  CasualNoises    13/02/2025  First implementation
//==============================================================================
uint32_t TLV_Driver::getLargestFreeTLV_Size()
{
	uint32_t index = findNextTLV (cFreeTLV_Tag, 0);
	uint32_t size = 0;
	while (index > 0)
	{
		if (getLength(index) > size)
			size = getLength(index);
		index = findNextTLV (cFreeTLV_Tag, index);
	}
	return size;
}

//==============================================================================
//          getTotalNoOfTLV_FreeBlocks()
//
// Scan all TLV's and return the no of free TLV blocks found
//
//  CasualNoises    13/02/2025  First implementation
//==============================================================================
uint32_t TLV_Driver::getTotalNoOfTLV_FreeBlocks()
{
	uint32_t index = findNextTLV (cFreeTLV_Tag, 0);
	uint32_t count = 0;
	while (index > 0)
	{
		count += 1;
		index = findNextTLV (cFreeTLV_Tag, index);
	}
	return count;
}

//==============================================================================
//          getTotalNoOfTLVs()
//
// Scan all TLV's and return the no of TLV blocks found
//		return 0 if the flash is inconsistent
//
//  CasualNoises    23/12/2025  First implementation
//==============================================================================
uint32_t TLV_Driver::getTotalNoOfTLVs()
{
	uint32_t index = 1;
	uint32_t count = 0;
	while (index <  mNVM_AfterEndIndex)
	{
		count += 1;
		uint32_t step = getLength(index);
		if (step == 0)
			return 0;
		index += step;
	}
	if (index > (mNVM_AfterEndIndex + 1))
		return 0;
	return count;
}

} // namespace CasualNoises

#endif

