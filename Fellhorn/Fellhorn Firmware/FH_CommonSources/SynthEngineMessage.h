/*
  ==============================================================================

    SynthEngineMessage.h
    Created: 17/07/2025

    Structure of messages shared between North- and SouthSide (exchange using NerveNet)
    	NorthSide: change data using the UI
    	SouthSide: use data for synthesis

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "CommonDefinitions.h"

#include <NerveNet/NerveNetMessageHeader.h>

namespace CasualNoises
{

/***************************************** eSynthEngineMessageType *******************************/
enum class eSynthEngineMessageType
{
	initSynthEngine = 1,
	requestSetupInfo,
	setupInfoReply,
	setFrequency,
	potentiometerValue,
	ADC_Value,
	triggerEvent,
};

/***************************************** eSynthEngineInitType *******************************/
enum class eSynthEngineInitType
{
	hardInit = 1,
	softInit,
};

/***************************************** tInitMessageData *******************************/
typedef struct
{
//	tNerveNetMessageHeader		header;
	eSynthEngineInitType		initMessage;
} tInitMessage;

/***************************************** tRequestSetupInfoMessageData *******************************/
typedef struct
{
//	tNerveNetMessageHeader		header;
} tRequestSetupInfoMessageData;

/***************************************** tSetupInfoReplyMessageData *******************************/
typedef struct
{
	tNerveNetMessageHeader		header;
	char						setupName[16];
} tSetupInfoReplyMessageData;

/***************************************** tSetFrequencyMessage *******************************/
typedef struct
{
//	tNerveNetMessageHeader		header;
	float						frequency;
} tSetFrequencyMessage;

/***************************************** tPotValueMessage *******************************/
typedef struct
{
//	tNerveNetMessageHeader		header;
	uint32_t					potId;
	float						potValue;
} tPotValueMessage;

/***************************************** tADC_ValueMessage *******************************/
typedef struct
{
	tNerveNetMessageHeader		header;
	uint16_t 					data[NUM_CV_INPUTS];
} tADC_ValueMessage;

/***************************************** tTriggerMessageData *******************************/
typedef struct
{
	tNerveNetMessageHeader		header;
	uint32_t					beatNo;
} tTriggerMessage;

}	// namespace CasualNoises
