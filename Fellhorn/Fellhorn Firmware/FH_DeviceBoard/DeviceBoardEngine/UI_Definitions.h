/*
  ==============================================================================

    UI_Definitions.h
    Created: 02/01/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "main.h"
#include "SystemConfig.h"

#include "NerveNet/NerveNetMessageHeader.h"

namespace CasualNoises
{

// Thread source identification, part of any event send from any thread to the UI thread
enum class eEventSourceDestinationID
{
	encoderThreadSourceID,
	multiplexerADC_ThreadSourceID,
	nerveNetNorthSideSourceID,
	nerveNetSouthSideSourceID,
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
	eEventSourceDestinationID	eventSourceID;
	uint16_t	 				multiplexerNo;
	uint16_t					multiplexerChannelNo;
	uint32_t					value;
	int32_t						deviation;
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
	eEventSourceDestinationID	eventSourceID;
	eEncoderEventType 			eventType;
	uint16_t					encoderNo;
	int16_t						encoderCount;
	int32_t						switchBitMap;
} sEncoderEvent;

// Structure of a NerveNet event
typedef struct
{
	eEventSourceDestinationID	eventSourceID;
	uint32_t					eventLength;
	uint8_t*					eventdataPtr;
} sNerveNetEvent;

// Structure of incoming UI messages
typedef union
{
	sEncoderEvent				encoderEvent;
	sMultiplexed_ADC_Event		multiplexed_ADC_Event;
	sNerveNetEvent				nerveNetEvent;
} sIncommingUI_Event;

// Potentiometer/slider calibration values
typedef struct
{
	uint8_t		reveserved_1;
	uint8_t		isCalibrated;
	uint16_t	reveserved_2;
	uint32_t	minValues [NO_OF_ADC_MULTIPLEXERS] [NO_OF_ADC_MULTI_CHANNELS];
	uint32_t	maxValues [NO_OF_ADC_MULTIPLEXERS] [NO_OF_ADC_MULTI_CHANNELS];
	bool		maskFlags [NO_OF_ADC_MULTIPLEXERS] [NO_OF_ADC_MULTI_CHANNELS];
} sCalibrationData;

// System settings
typedef struct
{
	sCalibrationData 		calibrationData;
} sSystemSettings;

} // namespace CasualNoises
