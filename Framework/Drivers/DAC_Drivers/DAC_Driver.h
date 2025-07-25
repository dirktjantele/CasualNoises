/*
  ==============================================================================

    DAC_Driver.h
    Created: 18 oct 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include  "cmsis_os.h"

#include "CasualNoises.h"

namespace CasualNoises
{

class DAC_Driver
{
public:
	 DAC_Driver() = default;
	~DAC_Driver() = default;

	//==============================================================================
	//          startDriver
	//
	// Start the driver
	// size: number of uint32_t words to send to the device
	//			may be different from the actual of words send by the driver
	//
	//  CasualNoises    19/10/2023  First implementation
	//==============================================================================
	virtual void startDriver(uint32_t noOfChannels, uint32_t noOfSamples) = 0;

	//==============================================================================
	//          sendAudio
	//
	// Send a block of audio data to the DAC using ping-pong buffering
	// sourcePointer: points to an array of float's with half the length of buffer size
	// transmitCompleted: this indicates which half of the buffer is to be filled
	//
	//  CasualNoises    19/10/2023  First implementation
	//==============================================================================
	virtual void sendAudio(float** sourcePointer, bool transmitCompleted) = 0;

private:

};

} // namespace CasualNoises
