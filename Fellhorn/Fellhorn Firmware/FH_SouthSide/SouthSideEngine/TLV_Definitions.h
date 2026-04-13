/*
  ==============================================================================

    TLV_Definitions.h
    Created: 13/04/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

namespace CasualNoises
{

// TLV tags
enum class eTLV_Tag
{
	CV_CalibrationValues		= 0x76637663,			// 'cvcv'
};

typedef struct
{
	float						openInputValues 	[ NUM_CV_INPUTS ];
	float						min5V_InputValues 	[ NUM_CV_INPUTS ];
	float						plus5V_InputValues 	[ NUM_CV_INPUTS ];
} tCV_CalibrationValues;

} // namespace CasualNoises

