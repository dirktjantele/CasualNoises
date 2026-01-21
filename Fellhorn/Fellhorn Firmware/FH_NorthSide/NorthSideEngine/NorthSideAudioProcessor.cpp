/*
  ==============================================================================

    NorthSideAudioProcessor.cpp
    Created: 10/03/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "NorthSideAudioProcessor.h"

#include "SystemConfig.h"

#include "CasualNoises.h"

#include "DSP/Filter/IIRFilters.h"								// ToDo: do we need this?

#include "SynthEngineMessage.h"

namespace CasualNoises
{

// Space for statics
NorthSideAudioProcessor	NorthSideAudioProcessor::mNorthSideAudioProcessor;
bool					NorthSideAudioProcessor::mIsAllocated = false;

//==============================================================================
//          prepareToPlay
//
// this method is called before the first call to processBlock()
//
//  CasualNoises    04/12/2024  First implementation
//  CasualNoises    14/12/2024  Passing synthesiser params structure
//	CasualNoises    10/03/2025  Adapted for Fellhorn
//==============================================================================
void NorthSideAudioProcessor::prepareToPlay (
		float sampleRate,
		uint32_t maximumExpectedSamplesPerBlock,
		void* synthParams)
{

	// store settings
	mSampleRate 					= sampleRate;
	mMaximumExpectedSamplesPerBlock = maximumExpectedSamplesPerBlock;

}

//==============================================================================
//          releaseResources
//
// This method is called after the last call to processBlock()
//
//  CasualNoises    04/12/2024  First implementation
//	CasualNoises    10/03/2025  Adapted for Fellhorn
//==============================================================================
void NorthSideAudioProcessor::releaseResources()
{
	CN_ReportFault(eErrorCodes::runtimeError);			// Not implemented yet
}

//==============================================================================
//          processNerveNetData
//
// Process incoming NerveNet data
//
//  CasualNoises    21/01/2026  First implementation
//==============================================================================
void NorthSideAudioProcessor::processNerveNetData(uint32_t threadNo, uint32_t size, uint8_t* ptr)
{
/*
	tSE_Message* dataPtr = (tSE_Message*)ptr;
	UNUSED(dataPtr);
*/
}

//==============================================================================
//          processBlock
//
// This method is called each time a new audio block needs to be processed
//
//  CasualNoises    04/12/2024  First implementation
//	CasualNoises    10/03/2025  Adapted for Fellhorn
//	CasualNoises    13/07/2025  NerveNet support added
//==============================================================================
void NorthSideAudioProcessor::processBlock (AudioBuffer& buffer, AudioBuffer& NN_buffer)
{
	static CasualNoises::Filter::IIRFilters filter(2);

	CasualNoises::Filter::ChannelFeed cf0 = filter.getChannelFeed(0);
	UNUSED ( cf0 );
	CasualNoises::Filter::ChannelFeed cf1 = filter.getChannelFeed(1);
	UNUSED ( cf1 );

	buffer.copyAudio(NN_buffer);

}

//==============================================================================
//          handle_ADC_Data
//
// Handle new adc data:
//	1 V/OCT
//	CV1 - CV3
//
//	CasualNoises    27/07/2025  NerveNet support added
//==============================================================================
void NorthSideAudioProcessor::handle_ADC_Data(uint32_t noOfEntries, uint16_t* adcDataPtr)
{
	// ToDo: process adc data
}

} // namespace CasualNoises



