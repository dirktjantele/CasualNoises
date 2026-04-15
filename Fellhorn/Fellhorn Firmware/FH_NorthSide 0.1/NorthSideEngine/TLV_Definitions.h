/*
  ==============================================================================

    TLV_Definitions.h
    Created: 12/04/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "SynthEngineMessage.h"

namespace CasualNoises
{

// TLV tags
enum class eTLV_Tag
{
	CV_CalibrationValues		= 0x76637663,			// 'cvcv'
	_1V_OCT_CalibrationValues	= 0x4f2f5631,			// '1V/O'
};

typedef struct
{
	float						openInputValues 	[ NUM_CV_INPUTS ];
	float						min5V_InputValues 	[ NUM_CV_INPUTS ];
	float						plus5V_InputValues 	[ NUM_CV_INPUTS ];
} tCV_CalibrationValues;

typedef struct
{
	float						calibrationValues 	[ cTotalNoOfNotes ];
} t1V_OctCalibrationValues;

} // namespace CasualNoises

