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

#include <NerveNet/NerveNetMessageHeader.h>

namespace CasualNoises
{

/***************************************** eSynthEngineMessageType *******************************/
enum class eSynthEngineMessageType
{
	initSynthEngine = 1,
	requestSetupInfo,
	setupInfoReply,
	potentiometerValue,
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
	tNerveNetMessageHeader		header;
	eSynthEngineInitType		initMessage;
} tInitMessage;

/***************************************** tRequestSetupInfoMessageData *******************************/
typedef struct
{
	tNerveNetMessageHeader		header;
} tRequestSetupInfoMessageData;

/***************************************** tSetupInfoReplyMessageData *******************************/
typedef struct
{
	tNerveNetMessageHeader		header;
	char						setupName[16];
} tSetupInfoReplyMessageData;

/***************************************** tPotValueMessageData *******************************/
typedef struct
{
	tNerveNetMessageHeader		header;
	uint32_t					potId;
	float						potValue;
} tPotValueMessage;

}	// namespace CasualNoises
