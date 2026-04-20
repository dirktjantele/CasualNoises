/*
  ==============================================================================

    SouthSideAudioProcessor.h
    Created: 13/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "AudioProcessors/Processors/AudioProcessor.h"

#include "Threads/ADC_Thread.h"

#include "Core/Maths/Average.h"

#include "SynthEngineMessage.h"

#include "TLV_Definitions.h"

#include "Utilities/ReportFault.h"

#include "NerveNet/NerveNetMessage.h"

namespace CasualNoises
{

typedef struct
{
	float 		frequency;
} sSynthesiserParams;

class AbstractSynthEngine;
class ADSR;

class SouthSideAudioProcessor :
		public AudioProcessor,
		public ADC_DataHandler
{
public:
	 SouthSideAudioProcessor () = default;
	~SouthSideAudioProcessor () = default;

	//==============================================================================
	//          getSouthSideAudioProcessor
	//
	// This class is implemented using a singleton design pattern
	// getSouthSideAudioProcessor() can only be called once to prevent creation of multiple
	// instances of the audio processor class
	//
	//  CasualNoises    13/07/2025  First implementation for Fellhorn
	//==============================================================================
	// Prevent multiple users accessing this object, only the AudioThread should use it
	static SouthSideAudioProcessor* getSouthSideAudioProcessor()
	{
		if ( ! mIsAllocated )
		{
			mIsAllocated = true;
			return &mSouthSideAudioProcessor;
		}
		CN_ReportFault(eErrorCodes::runtimeError);
		return nullptr;
	}

	void 	prepareToPlay (float sampleRate,
						   uint32_t maximumExpectedSamplesPerBlock ) noexcept override;
	void 	releaseResources() noexcept override;
	void 	processNerveNetData( uint32_t threadNo, uint32_t size, uint8_t* ptr ) noexcept override;
	void 	processBlock ( AudioBuffer* buffer, AudioBuffer* NN_buffer ) noexcept override;

	void 	handle_ADC_Data ( uint32_t noOfEntries, uint16_t* adcDataPtr );

	void	loadCalibrationValues ( bool reload = false ) noexcept;

private:

	// Access locker
	static SouthSideAudioProcessor	mSouthSideAudioProcessor;
	static bool						mIsAllocated;

	// Pointer to synth engine
	AbstractSynthEngine*			mAbstractSynthEnginePtr	{ nullptr };

	Average<float>*					mAveragesPtrs [ TOTAL_NUM_CV_INPUTS ] { nullptr };

	bool							m1V_OctCalibrationValuesLoaded	{ false };
	float 							m1V_OctCalibrationValues 		[ cTotalNoOfNotes ] { 0.0f };

	bool							mCV_CalibrationValuesLoaded		{ false };
	tCV_CalibrationValues			mCV_CalibrationValues;

	void handleRequestSetupInfo ( uint32_t threadNo ) const noexcept;
	void handleADC_DataRequest  ( uint32_t threadNo ) const noexcept;

	void initSynthEngine () noexcept;

	void handleADC_CalibrationData 		( tNerveNetMessageHeader* headerPtr );
	void handle_1V_OCT_CalibrationData 	( tNerveNetMessageHeader* headerPtr );

};

} // namespace CasualNoises
