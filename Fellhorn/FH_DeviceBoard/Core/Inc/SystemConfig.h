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

constexpr uint32_t	DEFAULT_STACK_SIZE 					= 256;

constexpr uint32_t	NERVENET_THRAD_PRIORITY				= configMAX_PRIORITIES - 1;
constexpr uint32_t	UI_THREAD_PRIORITY					= 10;
//constexpr uint32_t	EVENT_THREAD_PRIORITY				= 10;
constexpr uint32_t	POT_THREAD_PRIORITY					= UI_THREAD_PRIORITY;
constexpr uint32_t	TRIGGER_THREAD_PRIORITY				= 1;

constexpr uint32_t	MAX_NO_OF_NERVENET_MASTER_THREADS 	= 1;			// Only 1 master thread is supported
constexpr uint32_t	MAX_NO_OF_NERVENET_SLAVE_THREADS  	= 0;
constexpr uint32_t	AUDIO_NERVENET_THREAD_NO		  	= 0;
constexpr uint32_t	NERVENET_DATA_SIZE				  	= 1024;

// Thread source identification, part of any event send from any thread to the UI thread
enum class eEventSourceID
{
	encoderThreadSourceID,
	potentiometerThreadSourceID,
	multiplexerADC_ThreadSourceID,
};

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

// Encoder & switch device numbers
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

// TLV tags
enum class eTLV_Tag
{
	UI_CurrentState		= 0x535f4955,			// 'UI_S'
	UI_PageStack		= 0x73696770,			// 'pgis'
	UI_PageStackPtr		= 0x74707370,			// 'pspt'
	UI_MainPageState	= 0x7473706d,			// 'mpst'
};


