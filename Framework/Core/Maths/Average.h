/*
  ==============================================================================

    Average.h
    Created: 28/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

namespace CasualNoises
{

template <typename T>
class Average {
public:
	 Average() = delete;

	 Average(uint32_t width)
	 : mWidth (width)
	 {
		 mValues = new T[mWidth];
		 for (uint32_t i = 0; i < mWidth; ++i)
			 mValues[i] = 0.0f;
	 }

	~Average()
	{
		delete mValues;
	}

	T nextAverage(T value)
	{
		mSum -= mValues[mIndex];
		mSum += value;
		mValues[mIndex++] = value;
		if (mIndex >= mWidth)
			mIndex = 0;
		return mSum / mWidth;
	}

private:
	uint32_t	mWidth		{ 0 };
	T*			mValues		{ nullptr };
	uint32_t	mIndex		{ 0 };
	T			mSum		{ 0 };

};

} // namespace CasualNoises
