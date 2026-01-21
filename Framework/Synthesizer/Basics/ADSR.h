/*
  ==============================================================================

    ADSR.h
    Created: 1 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

namespace CasualNoises
{

enum class eADSR_Type
{
	oneShot,		// Go through all stages based upon mCycleTime
	triggerOff,		// Wait in sustain until trigger, then proceed to release
	retrigger,		// Wait in sustain until trigger, then proceed to attack
	cyclic			// Cycle through the adsr based upon mCycleTime
};

enum class eADSR_State
{
	waiting = 1,	// Wait for a trigger to start cycle
	hold1,
	attack,
	hold2,
	decay,
	sustain,
	release
};

typedef struct
{
	eADSR_Type	type			{ eADSR_Type::triggerOff };

	// Times below are in seconds
	float		holdTime1		{ 0.0f };
	float		attackTime		{ 0.0f };
	float		holdTime2		{ 0.0f };
	float		decayTime		{ 0.0f };
	float		sustainLevel	{ 0.0f };
	float		releaseTime		{ 0.0f };
	float		cycleTime		{ 0.0f };
} tADSR_Settings;

class ADSR final
{
public:
	 ADSR() 				= delete;
	 ADSR ( const ADSR& ) 	= default;
	 ADSR ( ADSR&& ) 		= default;
	~ADSR() 				= default;

	//==============================================================================
	//          ADSR()
	//
	//  CasualNoises    14/12/2024  First implementation
	//==============================================================================
	ADSR(float sampleRate)
	: mSampleRate (sampleRate)
	{
		mStateTable[0] = eADSR_State::waiting;

	};

	//==============================================================================
	//          nextValue()
	//
	//  CasualNoises    25/07/2025  First implementation
	//==============================================================================
	float nextValue() noexcept
	{

		// Restart envelop?
		if (mResetCycle)
		{
			mCurrentStateIndex 	= 0;
			mADSR_State     	= mStateTable[mCurrentStateIndex];
			mStateCountDown		= mFirstStateCountDown;
			mValueIncrement 	= mValueIncrementTable[mCurrentStateIndex];
			mCurrentValue		= 0.0f;
			mResetCycle 		= false;
		}

		// Proceed to next stage?
		if ((mADSR_State != eADSR_State::waiting) && (mStateCountDown <= 0))
		{
			mCurrentStateIndex += 1;
			mADSR_State     	= mStateTable[mCurrentStateIndex];
			mValueIncrement 	= mValueIncrementTable[mCurrentStateIndex];
			switch (mADSR_State)
			{
			case eADSR_State::waiting:
				mCurrentValue		= 0.0f;
				mValueIncrement		= 0.0f;
				break;
			case eADSR_State::hold1:
				mStateCountDown = mHoldTime1;
				break;
			case eADSR_State::attack:
				mStateCountDown = mAttackTime;
				break;
			case eADSR_State::hold2:
				mStateCountDown = mHoldTime2;
				break;
			case eADSR_State::decay:
				mStateCountDown = mDecayTime;
				break;
			case eADSR_State::sustain:
				mStateCountDown = mSustainTime;
				break;
			case eADSR_State::release:
				mStateCountDown = mReleaseTime;
				break;
			}
		}

		// Update state
		float value      = mCurrentValue;
		mCurrentValue   += mValueIncrement;
		if (mStateCountDown > 0)
			mStateCountDown -= 1;

		return value;
	}

	//==============================================================================
	//          handleTrigger()
	//
	//  CasualNoises    25/07/2025  First implementation
	//==============================================================================
	void handleTrigger() noexcept
	{

		// When waiting, just start...
		if ( mADSR_State == eADSR_State::waiting )
		{
			mResetCycle = true;
			return;
		}

		// Else, act according the envelop type
		switch ( mADSR_Type )
		{
		case eADSR_Type::cyclic:
		case eADSR_Type::oneShot:
			if ( mADSR_State == eADSR_State::waiting )
			{
				mADSR_State		= mStateTable[0];
				mValueIncrement = mValueIncrementTable[0];
				mStateCountDown = mFirstStateCountDown;
			}
			else
			{
				mValueIncrement += 0;						// Why do we come here???
			}
			break;
		case eADSR_Type::triggerOff:
			if (mADSR_State == eADSR_State::sustain)
			{
				mCurrentStateIndex += 1;
				mValueIncrement = mValueIncrementTable[mCurrentStateIndex];
				mStateCountDown = mReleaseTime;
			}
			else										// ToDo what to do on a trigger when not in sustain state???
			{
				mResetCycle = true;
			}
			mADSR_State 	= mStateTable[mCurrentStateIndex];
			mValueIncrement	= mValueIncrementTable[mCurrentStateIndex];
			break;
		case eADSR_Type::retrigger:
			mCurrentStateIndex = 0;

		}
	}

	//==============================================================================
	//          getSettings()
	//
	//  CasualNoises    25/07/2025  First implementation
	//==============================================================================
	const inline tADSR_Settings* getSettings() noexcept
	{
		return &mSettings;
	}

	//==============================================================================
	//          updateSettings()
	//
	//  CasualNoises    25/07/2025  First implementation
	//==============================================================================
	void updateSettings(tADSR_Settings* settings) noexcept
	{
		mADSR_Type		= settings->type;
		mHoldTime1		= settings->holdTime1 	 * mSampleRate;
		mAttackTime		= settings->attackTime	 * mSampleRate;
		mHoldTime2		= settings->holdTime2	 * mSampleRate;
		mDecayTime		= settings->decayTime	 * mSampleRate;
		mSustainLevel	= settings->sustainLevel;
		mReleaseTime	= settings->releaseTime	 * mSampleRate;
		mCycleTime		= settings->cycleTime	 * mSampleRate;
		mSettings		= *settings;

		// Calculate sustain time
		uint32_t sum = mHoldTime1 + mAttackTime + mHoldTime2 + mDecayTime + mReleaseTime;
		if (sum >= mCycleTime)
		{
			mSustainTime = 0;
		} else if ((mADSR_Type == eADSR_Type::cyclic) || (mADSR_Type == eADSR_Type::oneShot))
		{
			if (mSustainLevel > 0.0f)
				mSustainTime = mCycleTime - sum;
		} else
		{
			mSustainTime = 0xffffffff;
		}

		if (mReleaseTime == 0.0f)
		{
			mReleaseTime += 0.01f * mSampleRate;
			mSustainTime -= 0.01f * mSampleRate;
		}

		// Recalculate state table
		mFirstStateCountDown = 0;
		mCurrentStateIndex = 0;
		if (mHoldTime1 > 0)
		{
			mStateTable[mCurrentStateIndex]     = eADSR_State::hold1;
			mValueIncrementTable[mCurrentStateIndex] = 0.0f;
			mFirstStateCountDown = mHoldTime1;
			mCurrentStateIndex += 1;
		}
		if (mAttackTime > 0)
		{
			mStateTable[mCurrentStateIndex]     = eADSR_State::attack;
			mValueIncrementTable[mCurrentStateIndex] = 1.0f / mAttackTime;
			if (mFirstStateCountDown == 0)
				mFirstStateCountDown = mAttackTime;
			mCurrentStateIndex += 1;
		}
		if (mHoldTime2 > 0)
		{
			mStateTable[mCurrentStateIndex] 	= eADSR_State::hold2;
			mValueIncrementTable[mCurrentStateIndex] = 0.0f ;
			if (mFirstStateCountDown == 0)
				mFirstStateCountDown = mHoldTime2;
			mCurrentStateIndex += 1;
		}
		if (mDecayTime > 0)
		{
			mStateTable[mCurrentStateIndex] = eADSR_State::decay;
			mValueIncrementTable[mCurrentStateIndex] = (mSustainLevel - 1.0f) / mDecayTime;
			if (mFirstStateCountDown == 0)
				mFirstStateCountDown = mDecayTime;
			mCurrentStateIndex += 1;
		}
		if (mSustainTime > 0)
		{
			mStateTable[mCurrentStateIndex] = eADSR_State::sustain;
			mValueIncrementTable[mCurrentStateIndex] = 0.0f ;
			if (mFirstStateCountDown == 0)
				mFirstStateCountDown = mSustainTime;
			mCurrentStateIndex += 1;
		}
		if (mReleaseTime > 0)
		{
			mStateTable[mCurrentStateIndex] = eADSR_State::release;
			mValueIncrementTable[mCurrentStateIndex] = (mSustainLevel / mReleaseTime) * -1.0f;
			if (mFirstStateCountDown == 0)
				mFirstStateCountDown = mReleaseTime;
			mCurrentStateIndex += 1;
		}
		mStateTable[mCurrentStateIndex] = eADSR_State::waiting;
		mValueIncrementTable[mCurrentStateIndex] = 0.0f;
		mCurrentStateIndex	= 0;
		mCurrentValue		= 0.0f;

	}

private:
	float			mSampleRate 			{ 0.0f };

	// adsr type and current state
	eADSR_Type		mADSR_Type				{ eADSR_Type::triggerOff };
	eADSR_State		mADSR_State				{ eADSR_State::waiting };

	// Times below are in samples
	uint32_t		mHoldTime1				{ 0 };
	uint32_t		mAttackTime				{ 0 };
	uint32_t		mHoldTime2				{ 0 };
	uint32_t		mDecayTime				{ 0 };
	float			mSustainLevel			{ 0.0f };
	uint32_t		mSustainTime			{ 0 };
	uint32_t		mReleaseTime			{ 0 };
	uint32_t		mCycleTime				{ 0 };

	// adsr settings
	tADSR_Settings	mSettings;

	// State table
	eADSR_State		mStateTable[(uint32_t)eADSR_State::release];
	float			mValueIncrementTable[(uint32_t)eADSR_State::release];
	uint32_t		mCurrentStateIndex		{ 0 };
	uint32_t		mFirstStateCountDown	{ 0 };
	int32_t			mStateCountDown			{ 0 };
	bool			mResetCycle				{ false };

	// Current value and increment
	float			mCurrentValue			{ 0 };
	float 			mValueIncrement			{ 0 };

};

} // namespace CasualNoises
