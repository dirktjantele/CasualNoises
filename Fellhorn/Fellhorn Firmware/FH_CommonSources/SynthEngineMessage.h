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

#include "NerveNet/NerveNetMessage.h"

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
	ADC_DataReply,
	ADC_CalibrationData,
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
	tNerveNetMessageHeader		header;
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
	bool						altSwitchState;
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

/************************************* tCV_InputCalibrationSettings***************************/
// Used to send CV input calibration data to the North- and SouthSide
typedef struct
{
	tNerveNetMessageHeader		header;
	float						openInputValues 	[ NUM_CV_INPUTS ];
	float						min5V_InputValues 	[ NUM_CV_INPUTS ];
	float						plus5V_InputValues 	[ NUM_CV_INPUTS ];
} tCV_InputCalibrationSettings;

/***************************************** tSetFrequencyMessage *******************************/
typedef struct
{
	tNerveNetMessageHeader		header;
	float						frequency;
} tSetFrequencyMessage;

}	// namespace CasualNoises

//#endif
