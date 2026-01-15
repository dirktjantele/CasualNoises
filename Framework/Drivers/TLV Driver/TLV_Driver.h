/*
  ==============================================================================

    TLV_Driver.h
    Created: 02 Apr 2023 15:10:33
    Author:  Dirk Tjantele

	Handles TLV objects in NVM space

  ==============================================================================
*/

#ifdef CASUALNOISES_NVM_DRIVER_SUPPORT

#pragma once

#include "../NVM Drivers/NVM Driver/NVM_Driver.h"

namespace CasualNoises {

const uint32_t			cMagicCode 		= 0x23122031;
const uint32_t			cFreeTLV_Tag 	= 0x65657246;		// "Free"

/*
 * The TLV_Driver class
 */
class TLV_Driver final
{
public:
	TLV_Driver() = delete;
	TLV_Driver(NVM_Driver* inNVM_DriverPtr);

	bool isDriverReady() { return mNVM_DriverPtr->isDriverReady(); }

	void		flushCache ()		{ mNVM_DriverPtr->flushSectorCache (); };

	void		deleteAllTLVs ( bool flushFlag = true );

	uint32_t	findNextTLV ( uint32_t tag, uint32_t index );

	bool		addTLV ( uint32_t tag, uint32_t length, void* valuePtr, bool flushFlag = true );			// Length in words
	bool		addTLV_Bytes(uint32_t tag, uint32_t length, void* valuePtr, bool flushFlag = true)			// Length in bytes
									{
										bool flag = addTLV ( tag, (length + 3) / 4, valuePtr, flushFlag );
										return flag;
									}
	uint32_t	readTLV(uint32_t index, uint32_t length, void* valuePtr);									// Length in words
	uint32_t	readTLV_Bytes(uint32_t index, uint32_t length, void* valuePtr)								// Length in bytes
									{
										uint32_t cnt = readTLV(index, (length + 3) / 4, valuePtr);
										return cnt;
									}
	uint32_t	readTLV_Tag(uint32_t tag, uint32_t length, void* valuePtr);									// Length in words
	uint32_t	readTLV_TagBytes(uint32_t tag, uint32_t length, void* valuePtr)								// Length in bytes
									{
										uint32_t cnt = readTLV_Tag(tag, (length + 3) / 4, valuePtr);
										return cnt;
									}
	bool		updateTLV ( uint32_t index, uint32_t length, void* valuePtr, bool flushFlag = true);		// Length in words
	bool		updateTLV_Bytes(uint32_t index, uint32_t length, void* valuePtr, bool flushFlag = true)		// Length in bytes
									{
										return updateTLV ( index, (length + 3) / 4, valuePtr, flushFlag);
									}
	bool		updateTLV_Tag(uint32_t tag, uint32_t length, void* valuePtr, bool flushFlag = true);		// Length in words
	bool		updateTLV_TagBytes(uint32_t tag, uint32_t length, void* valuePtr, bool flushFlag = true)	// Length in bytes
									{
										return updateTLV_Tag ( tag, (length + 3) / 4, valuePtr, flushFlag );
									}

	void		deleteTLV ( uint32_t tag, bool deleteAll,  bool flushFlag = true );

	uint32_t	getTLV_Length(uint32_t index)			{ return  (getLength(index)) - 2; }
	uint32_t	getTLV_LengthBytes(uint32_t index)		{ return ((getLength(index)) - 2) * 4; }

	uint32_t	getLargestFreeTLV_Size();
	uint32_t	getTotalNoOfTLV_FreeBlocks();
	uint32_t	getTotalNoOfTLVs();

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

#endif
