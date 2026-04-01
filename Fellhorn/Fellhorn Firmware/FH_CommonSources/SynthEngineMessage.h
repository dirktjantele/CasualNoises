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

//#ifdef debugging

#pragma once

#include "CommonDefinitions.h"

#include "NerveNet/NerveNetMessageHeader.h"

namespace CasualNoises
{

/***************************************** eSynthEngineMessageType *******************************/
enum class eSynthEngineMessageType
{
	initSynthEngine = 1,
	requestSetupInfo,
	setupInfoReply,
	potentiometerValue,
	ADC_DataRequest,
	triggerEvent,
	setFrequency,
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

typedef struct
{
//	tNerveNetMessageHeader		header;
	uint32_t					version;
} tRequestSetupInfoReplyData;

/***************************************** tPotValueMessage *******************************/
typedef struct
{
	tNerveNetMessageHeader		header;
	uint16_t					multiplexerNo;
	uint16_t					multiplexerChannelNo;
	float						potValue;
	float 						deviation;
} tPotValueMessage;

/***************************************** tRequestADC_Data *******************************/
typedef struct
{
	tNerveNetMessageHeader		header;
} tRequestADC_Data;

typedef struct
{
	tNerveNetMessageHeader		header;
	float						data [ TOTAL_NUM_CV_INPUTS ];
} tRequestADC_ReplyData;

/***************************************** tTriggerMessageData *******************************/
typedef struct
{
	tNerveNetMessageHeader		header;
	uint32_t					beatNo;
} tTriggerMessage;

/***************************************** tSetupInfoReplyMessageData *******************************/
typedef struct
{
	tNerveNetMessageHeader		header;
	char						setupName[16];
} tSetupInfoReplyMessageData;

/***************************************** tSetFrequencyMessage *******************************/
typedef struct
{
	tNerveNetMessageHeader		header;
	float						frequency;
} tSetFrequencyMessage;

}	// namespace CasualNoises

//#endif
