/*
  ==============================================================================

    SystemConfig.h
    Created: 11/12/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "cmsis_os.h"

#include "CommonDefinitions.h"

constexpr uint32_t	DISPLY_WIDTH						= 128;
constexpr uint32_t	DISPLAY_HEIGHT						= 64;

constexpr uint32_t	DEFAULT_STACK_SIZE 					= 512;
constexpr uint32_t	UI_THREAD_STACK_SIZE 				= DEFAULT_STACK_SIZE * 16;

constexpr uint32_t	NERVENET_THREAD_PRIORITY			= configMAX_PRIORITIES - 1;
constexpr uint32_t	UI_THREAD_PRIORITY					= configMAX_PRIORITIES - 10;
//constexpr uint32_t	EVENT_THREAD_PRIORITY				= 10;
constexpr uint32_t	ENCODER_THREAD_PRIORITY				= configMAX_PRIORITIES - 15;
constexpr uint32_t	POT_THREAD_PRIORITY					= configMAX_PRIORITIES - 15;
constexpr uint32_t	TRIGGER_THREAD_PRIORITY				= 1;

constexpr uint32_t	MAX_NO_OF_NERVENET_MASTER_THREADS 	= 1;			// Only 1 master thread is supported
constexpr uint32_t	MAX_NO_OF_NERVENET_SLAVE_THREADS  	= 0;
constexpr uint32_t	AUDIO_NERVENET_THREAD_NO		  	= 0;
constexpr uint32_t	NERVENET_DATA_SIZE				  	= 1024;

/*
// Thread source identification, part of any event send from any thread to the UI thread
enum class eEventSourceID
{
	encoderThreadSourceID,
	potentiometerThreadSourceID,
	multiplexerADC_ThreadSourceID,
};
*/

// Bit pattern for led's (see schematic for shift register connections)
// Note, all bit numbers are shifted 8 bit to the right to avoid compilation errors!
enum class eLED_BitNums
{
	FADER_5		 		= 17,
	FADER_4				= 18,
	FADER_3				= 19,
	FADER_2				= 20,
	FADER_1				= 21,
	FADER_6				= 22,
	FADER_7				= 23,
	FADER_8				= 24,
	SWITCH_2			= 25,
	SWITCH_6			= 26,
	SWITCH_5			= 27,
	SWITCH_4			= 28,
	SWITCH_3			= 29,
	SWITCH_1			= 30,
	EXIT_SWITCH			= 31,
};

// Encoder switch &  other switch device numbers
enum class eEncoderNums
{
	MAIN_ENCODER		= 0,
};

// Encoder switch &  other switch device numbers
enum class eSwitchNums
{
	ENCODER_SWITCH		= 0,
	EXIT_SWITCH			= 7,
	ALT_SWITCH			= 1,
	LEFT_ARROW_SWITCH	= 2,
	RIGTH_ARROW_SWITCH	= 3,
	SETUP_SWITCH		= 4,
	LOAD_SWITCH			= 5,
	SAVE_SWITCH			= 6,
};

// Encoder switch bitmap positions
enum class eSwitchBitmapPos
{
	ALT_SWITCH			= 1,
	SAVE_SWITCH			= 11,
};

// No of ADC multiplexers and multiplexer channels
constexpr uint32_t	NO_OF_ADC_MULTIPLEXERS		= 2;
constexpr uint32_t	NO_OF_ADC_MULTI_CHANNELS	= 8;

// ADC multiplexer channel num's for each multiplexer
enum class eMultiplexerChannel_0
{
	P_1					= 2,
	P_2					= 6,
	P_3					= 1,
	P_4					= 4,
	P_NC_1				= 3,
	P_NC_2				= 5,
	P_NC_3				= 7,
	P_NC_4				= 0,
};

enum class eMultiplexerChannel_1
{
	F_1					= 2,
	F_2					= 1,
	F_3					= 4,
	F_4					= 6,
	F_5					= 0,
	F_6					= 3,
	F_7					= 5,
	F_8					= 7,
};

// TLV tags
enum class eTLV_Tag
{
	UI_CurrentState		= 0x535f4955,			// 'UI_S'
	UI_PageStack		= 0x73696770,			// 'pgis'
	UI_PageStackPtr		= 0x74707370,			// 'pspt'
	UI_MainPageState	= 0x7473706d,			// 'mpst'
	CalibrationValues	= 0x766c6370,			// 'pclv'
};


