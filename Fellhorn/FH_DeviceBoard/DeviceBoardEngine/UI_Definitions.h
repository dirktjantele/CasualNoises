/*
  ==============================================================================

    UI_Definitions.h
    Created: 02/01/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "main.h"

namespace CasualNoises
{

// Thread source identification, part of any event send from any thread to the UI thread
enum class eEventSourceID
{
	encoderThreadSourceID,
	potentiometerThreadSourceID,
	multiplexerADC_ThreadSourceID,
};

// Multiplexer signature
// The ADC multiplexer is controlled by 3 GPIO output pins
typedef struct
{
	GPIO_TypeDef*			SO_Port;
	uint16_t				SO_Pin;
	GPIO_TypeDef*			S1_Port;
	uint16_t				S1_Pin;
	GPIO_TypeDef*			S2_Port;
	uint16_t				S2_Pin;
	uint8_t					mask;
} sADC_MultiplexerSignature;

// Structure of a multiplexed ADC event
typedef struct
{
	eEventSourceID		eventSourceID;
	uint16_t	 		multiplexerNo;
	uint16_t			multiplexerChannelNo;
	uint32_t			value;
	int32_t				deviation;
} sMultiplexed_ADC_Event;

// Encoder signature
// A switch is a simplified encoder, device no's/pin no's for encoder switches set to 0xff
typedef struct
{
	uint8_t		encoderNo;
	uint8_t		switchDevNo;
	uint8_t		switchPinNo;
	uint8_t		enc_A_DevNo;
	uint8_t		enc_A_PinNo;
	uint8_t		enc_B_DevNo;
	uint8_t		enc_B_PinNo;
} sEncoderSignature;

// Encoder event types
enum class eEncoderEventType
{
	encoderSwitch,
	encoderCount
};

// Structure of an encoder event
typedef struct
{
	eEventSourceID		eventSourceID;
	eEncoderEventType 	eventType;
	uint16_t			encoderNo;
	int16_t				encoderCount;
	int32_t				switchBitMap;
} sEncoderEvent;

// Structure of incoming UI messages
typedef union
{
	sEncoderEvent				encoderEvent;
	sMultiplexed_ADC_Event		multiplexed_ADC_Event;
} sIncommingUI_Event;

} // namespace CasualNoises
