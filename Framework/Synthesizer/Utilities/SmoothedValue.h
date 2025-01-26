/*
  ==============================================================================

    SmoothedValue.h
    Created: 15 dec 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

namespace CasualNoises
{

//class SmoothedValue;
class SmoothedValue
{
public:
	 SmoothedValue () = default;
	~SmoothedValue () = default;

	SmoothedValue (float newTarget)
	: mTarget (newTarget)
	{}

	SmoothedValue (float newTarget, uint32_t newCountDown)
	: mTarget        (newTarget),
	  mCountdown     (newCountDown),
	  mInitCountdown (newCountDown)
	{
		mStep = mTarget / mCountdown;
	}

	inline bool isSmoothing() const noexcept
	{
		return mCountdown > 0;
	}

	inline void reset() noexcept
	{
		mCurrentValue = 0.0f;
		mCountdown    = mInitCountdown;
	}

	inline float getCurrentValue() const noexcept
	{
		return mCurrentValue;
	}

	inline float getNextSmoothedValue() noexcept
	{
		if (isSmoothing())
		{
			mCurrentValue += mStep;
			mCountdown	  -= 1;
		}
		return mCurrentValue;
	}

	void applyGain(AudioBuffer& buffer) noexcept
	{
		uint32_t noChannels = buffer.getNumChannels();
		uint32_t noSamples  = buffer.getNumSamples();
		assert(noChannels == 2);
		float* lwptr = buffer.getWritePointer(0);
		float* rwptr = buffer.getWritePointer(1);
		if (isSmoothing())
		for (uint32_t i = 0; i < noSamples; ++i)
		{
			*lwptr++ *= getNextSmoothedValue();
			*rwptr++ *= getCurrentValue();
		}
	}

private:

    float		mCurrentValue 		{ 0.0f };
    float		mTarget				{ mCurrentValue };
    float		mStep				{ 0.0f };
    uint32_t	mInitCountdown 		{ 0 };
    uint32_t	mCountdown 			{ 0 };

};

} // namespace CasualNoises
