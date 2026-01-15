/*
  ==============================================================================

    String.h
    Created: 02 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include <string>

#include "CasualNoises.h"
#include <Utilities/ReportFault.h>

namespace CasualNoises
{

class String final
{
public:
	 String() = default;

	//==============================================================================
	//          String ( const String& )
	//
	//  CasualNoises    14/12/2024  First implementation
	//==============================================================================
	/** Copy constructor */
	 String ( const String& other )
	{
		mStringLength = other.mStringLength;
		if (mStringPtr != nullptr)
			vPortFree(mStringPtr);
		mStringPtr = (char*)pvPortMalloc(mStringLength + 1);
		if (mStringPtr == nullptr)
			CN_ReportFault(eErrorCodes::unknowError);
		for (uint16_t i = 0; i < mStringLength; ++i)
			mStringPtr[i] = other.mStringPtr[i];
		mStringPtr[mStringLength] = 0;
	}

	//==============================================================================
	//          String ( String&& )
	//
	//  CasualNoises    14/12/2024  First implementation
	//==============================================================================
	/** Move constructor */
	String ( String&& other)
	{
		other.mStringLength = mStringLength;
		mStringLength = 0;
		other.mStringPtr = mStringPtr;
		mStringPtr = nullptr;
	}

	//==============================================================================
	//          String(char* string)
	//
	//  CasualNoises    02/08/2023  First implementation
	//==============================================================================
	/** Create a string from a character C array */
	String ( char* string )
	{
//		std::string str ( string );
//		mStringLength = str.length();
		mStringLength = strlen(string);
		mStringPtr = (char*)pvPortMalloc ( mStringLength + 1 );
		if (mStringPtr == nullptr)
			CN_ReportFault(eErrorCodes::unknowError);
		for (uint16_t i = 0; i < mStringLength; ++i)
			mStringPtr[i] = string[i];
		mStringPtr[mStringLength] = 0;
	}

	//==============================================================================
	//          ~String()
	//
	//  CasualNoises    02/08/2023  First implementation
	//==============================================================================
	 ~String ()
	{
		if (mStringPtr != nullptr)
			vPortFree(mStringPtr);
		mStringPtr = nullptr;
	}

	//==============================================================================
	//          String(float in)
	//
	//  CasualNoises    02/08/2023  First implementation
	//==============================================================================
	/** Create a string from a float */
    String ( float in )
    {
    	static char tmp[32];
    	sprintf(tmp, "%f", in);
    	std::string str (tmp);
		mStringLength = str.length();
		mStringPtr = (char*)pvPortMalloc(mStringLength + 1);
		if (mStringPtr == nullptr)
 	 	 	 CN_ReportFault(eErrorCodes::unknowError);
		for (uint16_t i = 0; i < mStringLength; ++i)
			mStringPtr[i] = tmp[i];
		mStringPtr[mStringLength] = 0;
   }

	//==============================================================================
	//          length()
	//
	//  CasualNoises    02/08/2023  First implementation
	//==============================================================================
	/** Get the length of this string */
	uint16_t length () const noexcept
	{
		return mStringLength;
	}

	//==============================================================================
	//          operator+
	//
	//  CasualNoises    02/08/2023  First implementation
	//  CasualNoises    11/01/2026  Bug fix
	//==============================================================================
    /** Adds two strings together */
    String operator+ (const String other) const noexcept
    {
    	uint16_t length = mStringLength + other.mStringLength;
    	String tmp = String ();
    	tmp.mStringPtr = (char*)pvPortMalloc ( length + 1 );
		if (mStringPtr == nullptr)
 	 	 	 CN_ReportFault(eErrorCodes::unknowError);
    	tmp.mStringLength = length;
    	uint16_t i;
		for (i = 0; i < mStringLength; ++i)
			tmp.mStringPtr[i] = mStringPtr[i];
		for (uint16_t j = 0; j < other.mStringLength; ++i, ++j)
			tmp.mStringPtr[i] = other.mStringPtr[j];
		tmp.mStringPtr[i] = 0x00;
    	return tmp;
    }

	//==============================================================================
	//          operator==
	//
	//  CasualNoises    14/12/2024  First implementation
	//==============================================================================
    /** Compare strings together (return true if equal) */
    bool operator== (const String other) const noexcept
    {
    	if (mStringLength != other.mStringLength)
    		return false;
    	else for (uint16_t i = 0; i < mStringLength; ++i)
			if (mStringPtr[i] != other.mStringPtr[i])
				return false;
     	return true;
    }

	//==============================================================================
	//          operator!=
	//
	//  CasualNoises    14/12/2024  First implementation
	//==============================================================================
    /** Compare strings together (return true if not equal) */
    bool operator!= (const String other) const noexcept
    {
    	return ! this->operator ==(other);
    }

	//==============================================================================
	//          operator=
	//
	//  CasualNoises    14/12/2024  First implementation
	//==============================================================================
    /** Assignment operator */
    void operator= (const String other) noexcept
    {
    	if (mStringLength != other.mStringLength)
    	{
    		if (mStringPtr != nullptr)
    			vPortFree(mStringPtr);
    		mStringLength = other.mStringLength;
    		mStringPtr = (char*)pvPortMalloc(mStringLength + 1);
    		if (mStringPtr == nullptr)
    			CN_ReportFault(eErrorCodes::unknowError);
    	}
		for (uint32_t i = 0; i < mStringLength; ++i)
			mStringPtr[i] = other.mStringPtr[i];
		mStringPtr[mStringLength] = 0;
    }

	//==============================================================================
	//          operator[]
	//
	//  CasualNoises    26/12/2025  First implementation
	//==============================================================================
    /** [] operator */
    char& operator[] (size_t index) noexcept
    {
    	if (index > mStringLength)
    	{
    		static char foo = 0x20;
    		return foo;
    	}
    	return mStringPtr[index];
    }

 	//==============================================================================
	//          isEmpty
	//
	//  CasualNoises    02/08/2023  First implementation
	//==============================================================================
    /** Test for empty string */
	bool isEmpty() noexcept
	{
		return mStringLength == 0;
	};

 	//==============================================================================
	//          getStringPtr
	//
	//  CasualNoises    30/12/2025  First implementation
	//==============================================================================
	char* getStringPtr()
	{
		return mStringPtr;
	}

private:
	char*		mStringPtr 		{ nullptr };
	uint16_t	mStringLength	{ 0 };
};

} // namespace CasualNoises
