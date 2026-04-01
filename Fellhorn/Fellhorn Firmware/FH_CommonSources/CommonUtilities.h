/*
  ==============================================================================

    CommonUtilities.h
    Created: 14/03/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

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

