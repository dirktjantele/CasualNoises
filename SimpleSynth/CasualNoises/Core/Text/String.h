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
#include "../../Utilities/ReportFault.h"

namespace CasualNoises
{

class String final
{
public:
	 String() = default;
	~String()
	{
		if (mStringPtr != nullptr)
			vPortFree(mStringPtr);
	}

	/** Create a string from a character C array */
	String(char* string)
	{
		std::string str (string);
		mStringLength = str.length();
		mStringPtr = (char*)pvPortMalloc(mStringLength);
		if (mStringPtr == nullptr) CN_ReportFault(1);
		for (int i = 0; i < mStringLength; ++i)
			mStringPtr[i] = string[i];
	}

	/** Create a string from a float */
	// ToDo: in project properties C/C++ Build -> Settings -> MCU Settings enable sprintf with floats
    String(float in)
    {
    	static char tmp[32];
    	sprintf(tmp, "%f", in);
    	std::string str (tmp);
		mStringLength = str.length();
		mStringPtr = (char*)pvPortMalloc(mStringLength);
		if (mStringPtr == nullptr)
 	 	 	 CN_ReportFault(1);
		for (int i = 0; i < mStringLength; ++i)
			mStringPtr[i] = tmp[i];
    }

	/** Get the length of this string */
	int length()
	{
		return mStringLength;
	}

    /** Adds two strings together */
    String operator+ (String other) const noexcept
    {
    	int length = mStringLength + other.mStringLength;
    	String tmp = String();
    	tmp.mStringPtr = (char*)pvPortMalloc(length);
    	tmp.mStringLength = length;
    	int i;
		for (i = 0; i < mStringLength; ++i)
			tmp.mStringPtr[i] = mStringPtr[i];
		for (int j = 0; j < other.mStringLength; ++i, ++j)
			tmp.mStringPtr[i] = other.mStringPtr[j];
    	return tmp;
    }

	bool	isEmpty() { return mStringLength == 0; };

private:
	char*		mStringPtr 		{ nullptr };
	int			mStringLength	{ 0 };
};

} // namespace CasualNoises
