/*
  ==============================================================================

    DrawBarOrganEngine.h
    Created: 16/04/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "AbstractSynthEngine.h"

namespace CasualNoises
{

class LFO;
class SubharmonicGenerator;

class DrawBarOrganEngine final : public AbstractSynthEngine
{
public:
	 DrawBarOrganEngine () = default;
	~DrawBarOrganEngine ();

	void 	prepareToPlay (float sampleRate,
						   uint32_t maximumExpectedSamplesPerBlock ) noexcept override;
	void 	releaseResources() noexcept override;
	void 	processNerveNetData(uint32_t threadNo, uint32_t size, uint8_t* ptr) noexcept override;
	void 	processBlock (AudioBuffer* buffer, AudioBuffer* NN_buffer) noexcept override;

	void 	applyControlVoltages ( sControlVoltages* voltages ) override;

private:

	float 						mSampleRate 						{ 0.0f };
	uint32_t					mMaximumExpectedSamplesPerBlock 	{ 0 };

	LFO*						mLFO_Ptr							{ nullptr };
	SubharmonicGenerator*		mSubharmonicGeneratorPtr1			{ nullptr };
	SubharmonicGenerator*		mSubharmonicGeneratorPtr2			{ nullptr };
	SubharmonicGenerator*		mSubharmonicGeneratorPtr3			{ nullptr };
	SubharmonicGenerator*		mSubharmonicGeneratorPtr4			{ nullptr };
	SubharmonicGenerator*		mSubharmonicGeneratorPtr5			{ nullptr };
	SubharmonicGenerator*		mSubharmonicGeneratorPtr6			{ nullptr };

	float						mRatio1								{ 0.0f };
	float						mRatio2								{ 0.0f };
	float						mRatio3								{ 0.0f };
	float						mRatio4								{ 0.0f };
	float						mRatio5								{ 0.0f };
	float						mRatio6								{ 0.0f };

	LFO*						mDetuneLFO_Ptr						{ nullptr };
	float						mDetuneFrequency					{ 0.0f };
	float						mDetune								{ 0.0f };

	float						mGain1								{ 0.0f };
	float						mGain2								{ 0.0f };
	float						mGain3								{ 0.0f };
	float						mGain4								{ 0.0f };
	float						mGain5								{ 0.0f };
	float						mGain6								{ 0.0f };

	float						mTargetFrequency					{ 0.0f };				// From 1V/OCT 1
	float						mFrequency							{ 0.0f };
	float						mFrequencyCourse					{ 0.0f }; 				// From P1
	float						mFrequencyFine						{ 0.0f }; 				// From P2

	float						mGainOffset							{ 1.0f }; 				// From P3

	float						mSaturation							{ 0.0f }; 				// From P4

	inline void	 updateFrequency () noexcept;

	inline float getRatio ( float interval ) const noexcept;
	inline float saturate ( float value, float amount ) const noexcept;

};

} // namespace CasualNoises
