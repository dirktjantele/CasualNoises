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

//#include "NerveNet/NerveNetMessageHeader.h"

namespace DeviceBoard
{

// Thread source identification, part of any event send from any thread to the UI thread
enum class eEventSourceID
{
	encoderThreadSourceID,
	multiplexerADC_ThreadSourceID,
	nerveNetSourceID,
	UI_ThreadSourceId,
};

// Multiplexer signature, one structure used for each multiplexer
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

// Logical potentiometer id's
enum class eLogicalPotId
{
	pot_1	= 0,
	pot_2,
	pot_3,
	pot_4,
	slider_1,
	slider_2,
	slider_3,
	slider_4,
	slider_5,
	slider_6,
	slider_7,
	slider_8,
	unconnected,			// Should be last
};

// Multiplexer signature, one structure used for each multiplexer
// This struct is used by the Multiplexed ADC thread to do the mapping from mult coordinates to eLogicalPotId's
typedef struct
{
	eLogicalPotId logicalPotIds [ NO_OF_ADC_MULTI_CHANNELS ];
} sLogicalPotChannelNums;

// Structure of a multiplexed ADC event
typedef struct
{
	eEventSourceID				eventSourceID;
	uint16_t	 				multiplexerNo;
	uint16_t					multiplexerChannelNo;
	eLogicalPotId				logicalPotId;
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
	bool		allChanges;
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
	eEventSourceID				eventSourceID;
	eEncoderEventType 			eventType;
	uint16_t					encoderNo;
	eSwitchNums					encoderSwitchNum;
	int16_t						encoderCount;
	bool						newState;
} sEncoderEvent;

// Structure of a NerveNet event
typedef struct
{
	eEventSourceID				eventSourceID;
	uint32_t					eventLength;
	uint8_t*					eventdataPtr;
} sNerveNetEvent;

// Structure used to let the UI thread start a new preset performance
constexpr uint32_t cPerformanceNameLength = 24;
typedef struct
{
	eEventSourceID				eventSourceID;
	uint32_t					performanceId;
	char						performanceName [ cPerformanceNameLength + 1 ];
} sLaunchPerformanceEvent;

// Structure of incoming UI messages
typedef union
{
	sEncoderEvent				encoderEvent;
	sMultiplexed_ADC_Event		multiplexed_ADC_Event;
	sNerveNetEvent				nerveNetEvent;
	sLaunchPerformanceEvent		launchPerformanceEvent;
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

} // namespace DeviceBoard

namespace CasualNoises
{
using DeviceBoard::eEventSourceID;
using DeviceBoard::sADC_MultiplexerSignature;
using DeviceBoard::eLogicalPotId;
using DeviceBoard::sLogicalPotChannelNums;
using DeviceBoard::sMultiplexed_ADC_Event;
using DeviceBoard::sEncoderSignature;
using DeviceBoard::eEncoderEventType;
using DeviceBoard::sEncoderEvent;
using DeviceBoard::sNerveNetEvent;
using DeviceBoard::sIncommingUI_Event;
using DeviceBoard::sCalibrationData;
using DeviceBoard::sSystemSettings;
}
