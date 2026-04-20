/*
  ==============================================================================

    CommonUtilities.h
    Created: 14/03/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "Core/Maths/MathsFunctions.h"

#include "SynthEngineMessage.h"

#include "TLV_Definitions.h"

namespace CasualNoises
{

//==============================================================================
//          normalize1V_OCT
//
// Convert 1V/OCT ADC values into a voltage
// In: 	0 <= value <= 65535.0f
// Out: 0 <= out   <= 10.0f
//
//	CasualNoises    11/03/2026  First implementation
//==============================================================================
inline float normalize1V_OCT (
		float value,
		bool calibrationLoaded,
		float ( &calibration ) [ cTotalNoOfNotes ] )
{

	float volts = 0.0f;
	if ( calibrationLoaded )
	{
		if ( value > 0)
		{
			uint32_t index = jmap ( value, 0.0f, 65535.0f, 0.0f, ( float ) ( cTotalNoOfNotes ) ) + 0.8f;
			volts = ( calibration [ index ] * 10) / 65535.0f;
		}
	} else
	{
		volts = ( (float) value * 10) / 65535.0f;
	}

	return volts;

}

//==============================================================================
//          normalizeCV_Input
//
// Convert a CV ADC values into a range from -1.0 to 1.0
//
//	CasualNoises    11/03/2026  First implementation
//==============================================================================
inline float normalizeCV_Input (
		uint16_t value,
		uint32_t index,
		bool calibrationLoaded,
		tCV_CalibrationValues& calibration )
{

	float volts = 1.0f - ( ( (float) value * 2 ) / 65535.0f );
	if ( calibrationLoaded )
	{
		if ( volts <= calibration.openInputValues [ index ] )
		{
			volts /= calibration.min5V_InputValues [ index ] * -1.0f;
		} else
		{
			volts /= calibration.plus5V_InputValues [ index ];
		}
	}

	return volts;
}


}	// namespace CasualNoises
