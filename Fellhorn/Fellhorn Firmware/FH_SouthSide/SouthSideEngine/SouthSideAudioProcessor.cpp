/*
  ==============================================================================

    SouthSideAudioProcessor.cpp
    Created: 13/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

//#include <stdio.h>

#include "SystemConfig.h"

#include "SouthSideAudioProcessor.h"

#include "SynthEngines/PulsarSynthEngine.h"

#include "NerveNet/NerveNetSlaveThread.h"
#include "NerveNet/NerveNetMessageHeader.h"
#include "NerveNet/NerveNetMessage.h"

#include "AudioBasics/Buffers/AudioBuffer.h"

#include "Core/Maths/Average.h"
#include "maths.h"

#include "SynthEngineMessage.h"
//#include "Synthesizer/Basics/ADSR.h"

#include <CommonUtilities.h>

#include "string.h"

namespace CasualNoises
{

// Space for statics
SouthSideAudioProcessor	SouthSideAudioProcessor::mSouthSideAudioProcessor;
bool					SouthSideAudioProcessor::mIsAllocated = false;

//==============================================================================
//          prepareToPlay
//
// this method is called before the first call to processBlock()
//
//  CasualNoises    13/07/2025  First implementation
//==============================================================================
void SouthSideAudioProcessor::prepareToPlay (
		float sampleRate,
		uint32_t maximumExpectedSamplesPerBlock,
		void* inSynthParams)
{

	mAbstractSynthEnginePtr = new PulsarSynthEngine;													// ToDo create the right kind of engine here
	mAbstractSynthEnginePtr->prepareToPlay(sampleRate, maximumExpectedSamplesPerBlock, inSynthParams);
/*
	mADSR_Ptr = new ADSR(sampleRate);
	tADSR_Settings adsrSettings;
	adsrSettings.type 		  = eADSR_Type::oneShot;
	adsrSettings.holdTime1	  = 0.0f;
	adsrSettings.attackTime	  = 0.05f;
	adsrSettings.holdTime2	  = 0.02f;
	adsrSettings.decayTime	  = 0.05f;
	adsrSettings.sustainLevel = 0.1f;
	adsrSettings.releaseTime  = 0.0f;
	adsrSettings.cycleTime	  = 0.1f;
	mADSR_Ptr->updateSettings(&adsrSettings);
*/
}

//==============================================================================
//          releaseResources
//
// This method is called after the last call to processBlock()
//
//  CasualNoises    13/07/2025  First implementation
//==============================================================================
void SouthSideAudioProcessor::releaseResources()
{
	mAbstractSynthEnginePtr->releaseResources();
}

//==============================================================================
//          processNerveNetData
//
// Process incoming NerveNet data
//
//  CasualNoises    20/07/2025  First implementation
//==============================================================================
void SouthSideAudioProcessor::processNerveNetData (
		uint32_t threadNo,
		uint32_t size,
		uint8_t* ptr )
{

	// Handle all events
	while (size > 0)
	{

		tNerveNetMessageHeader* headerPtr	  = (tNerveNetMessageHeader*)ptr;
		eSynthEngineMessageType messageType   = (eSynthEngineMessageType)headerPtr->messageTag;
		uint32_t				messageLength = headerPtr->messageLength;

		switch ( messageType )
		{
		case eSynthEngineMessageType::initSynthEngine:		// Ignore initSynthEngine messages
			break;
		case eSynthEngineMessageType::requestSetupInfo:		// Set-up info request
		case eSynthEngineMessageType::setFrequency:			// Set oscillator frequency
		case eSynthEngineMessageType::potentiometerValue:	// Update potentiometer values
		case eSynthEngineMessageType::triggerEvent:			// Trigger events
			mAbstractSynthEnginePtr->processNerveNetData ( threadNo, messageLength, ptr );
			break;
		case eSynthEngineMessageType::ADC_DataRequest:
			break;
		default:
			CN_ReportFault(eErrorCodes::NerveNetThread_Error);
		}

		size -= messageLength;
		ptr  += messageLength;

	}

}

//==============================================================================
//          processBlock
//
// This method is called each time a new audio block needs to be processed
//
//  CasualNoises    13/07/2025  First implementation
//==============================================================================
void SouthSideAudioProcessor::processBlock ( AudioBuffer* buffer, AudioBuffer* NN_buffer )
{
//	setTimeMarker_4();

	mAbstractSynthEnginePtr->processBlock ( buffer, NN_buffer );

/* ToDo: handle ADSR functionality in the synthesizer engine
	uint32_t numSamples = NN_buffer.getNumSamples ();
	const float* rptr0 = NN_buffer.getReadPointer ( 0 );
	const float* rptr1 = NN_buffer.getReadPointer ( 1 );
	float* wptr0 = NN_buffer.getWritePointer ( 0 );
	float* wptr1 = NN_buffer.getWritePointer ( 1 );
	for ( uint32_t i = 0; i < numSamples; ++i )
	{
		float gain = mADSR_Ptr->nextValue();
		float gain = 1.0f;
		*wptr0++ = *rptr0++ * gain;
		*wptr1++ = *rptr1++ * gain;
	}
*/
//	resetTimeMarker_4();
}
/*
//==============================================================================
//          normalize1V_OCT
//
// Convert 1V/OCT ADC values into a voltage											ToDo: use calibration settings
//
//	CasualNoises    11/03/2026  First implementation
//==============================================================================
inline float normalize1V_OCT ( uint16_t value )
{
	float volts = ( (float)value * 10) / 65535.0f;
	return volts;
}

//==============================================================================
//          normalizeCV_Input
//
// Convert a CV ADC values into a range from -1.0 to 1.0							ToDo: use calibration settings
//
//	CasualNoises    11/03/2026  First implementation
//==============================================================================
inline float normalizeCV_Input ( uint16_t value )
{
	float volts = ( (float)value * 2) / 65535.0f;
	return 1.0f - volts;
}
*/
//==============================================================================
//          handle_ADC_Data
//
// Handle new adc data
//
//	CasualNoises    02/02/2026  First implementation
//==============================================================================
void SouthSideAudioProcessor::handle_ADC_Data ( uint32_t noOfEntries, uint16_t* adcDataPtr )
{
	static sControlVoltages controlVoltages;

	controlVoltages._1V_OCT_1 = normalize1V_OCT( adcDataPtr[0] );
	controlVoltages._1V_OCT_2 = normalize1V_OCT( adcDataPtr[1] );

	for (uint32_t i = 0; i < NUM_CV_INPUTS; ++i)
	{
		controlVoltages.CV_Inputs [ i ] = normalizeCV_Input ( adcDataPtr[i + 2] );
	}

	mAbstractSynthEnginePtr->applyControlVoltages ( &controlVoltages );

}

} // namespace CasualNoises
