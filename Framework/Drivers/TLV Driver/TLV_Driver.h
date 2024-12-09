/*
  ==============================================================================

    TLV_Driver.h
    Created: 02 Apr 2023 15:10:33
    Author:  Dirk Tjantele

	Handles TLV objects in NVM space

  ==============================================================================
*/

#pragma once

#include "../NVM Drivers/NVM Driver/NVM_Driver.h"

namespace CasualNoises {

const uint32_t			cMagicCode 		= 0x09122024;
const uint32_t			cFreeTLV_Tag 	= 'eerF';

/*
 * The TLV_Driver class
 */
class TLV_Driver
{
public:
	TLV_Driver() = delete;
	TLV_Driver(NVM_Driver* inNVM_DriverPtr);

	bool isDriverReady() { return mNVM_DriverPtr->isDriverReady(); }

	void		deleteAllTLVs();
	uint32_t	findNextTLV (uint32_t tag, uint32_t index);
	bool		addTLV(uint32_t tag, uint32_t length, uint32_t* valuePtr);
	void		deleteTLV(uint32_t tag, bool deleteAll);

	NVM_Driver*	getNVM_DriverPtr()	{ return mNVM_DriverPtr; }

private:
	NVM_Driver* 			mNVM_DriverPtr;						// Driver used for NVM access
	uint32_t				mFreeTLV_Index { 0xffffffff };		// Index of the first free/empty TLV
	uint32_t				mNVM_AfterEndIndex  { 0 };			// Index one after last available index

	inline uint32_t 		getTag(uint32_t index) const noexcept						{ return (*mNVM_DriverPtr)[index + 0]; }
	inline void				setTag(uint32_t index, uint32_t tag) const noexcept			{ (*mNVM_DriverPtr)[index + 0] = tag; }
	inline uint32_t			getLength(uint32_t index) const noexcept					{ return (*mNVM_DriverPtr)[index + 1]; }
	inline void				setLength(uint32_t index, uint32_t length) const noexcept	{ (*mNVM_DriverPtr)[index + 1] = length; }
	inline uint32_t			getValue(uint32_t index) const noexcept						{ return (*mNVM_DriverPtr)[index + 2]; }
	inline void				setValue(uint32_t index, uint32_t value) const noexcept		{ (*mNVM_DriverPtr)[index + 2] = value; }

};

} // namespace CasualNoises

