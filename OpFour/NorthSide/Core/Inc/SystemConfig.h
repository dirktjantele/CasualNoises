/*
  ==============================================================================

    SystemConfig.h
    Created: 06/02/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "cmsis_os.h"

#include "CommonDefinitions.h"

constexpr float		SAMPLE_FREQUENCY				= 41666.0f;		// Real sample frequency

// 256 stereo samples at 32 bit/sample using double buffering
constexpr uint32_t 	FULL_AUDIO_BUFFER_SIZE			= NUM_SAMPLES * NUM_CHANNELS * 2;

constexpr uint32_t	DISPLY_WIDTH					= 128;
constexpr uint32_t	DISPLAY_HEIGHT					= 64;

constexpr uint32_t	DEFAULT_STACK_SIZE 				= 256;

constexpr uint32_t	AUDIO_THREAD_PRIORITY			= configMAX_PRIORITIES - 1;
constexpr uint32_t	NERVENET_THRAD_PRIORITY			= AUDIO_THREAD_PRIORITY - 10;
constexpr uint32_t	UI_THREAD_PRIORITY				= 10;
constexpr uint32_t	EVENT_THREAD_PRIORITY			= 10;
constexpr uint32_t	POT_THREAD_PRIORITY				= UI_THREAD_PRIORITY;
constexpr uint32_t	CV_IN_THREAD_PRIORITY			= 2;
constexpr uint32_t	TRIGGER_THREAD_PRIORITY			= 1;

constexpr uint32_t	MAX_NO_OF_NERVENET_MASTER_THREADS = 1;			// Only 1 master thread is supported
constexpr uint32_t	MAX_NO_OF_NERVENET_SLAVE_THREADS  = 0;
constexpr uint32_t	AUDIO_NERVENET_THREAD_NO		  = 0;
constexpr uint32_t	NERVENET_DATA_SIZE				  = 1024;

// Thread source identification, part of any event send from any thread to the UI thread
enum class eEventSourceID
{
	encoderThreadSourceID,
	potentiometerThreadSourceID,
};

// Bit pattern for led's (see schematic for shift register connections)
// Note, all bit numbers are shifted 8 bit to the right to avoid compilation errors!
enum class eLED_BitNums
{
 	  LED_1_RED		= 0x00000800,
	  LED_1_BLUE	= 0x00000400,
	  LED_1_GREEN	= 0x00000200,
	  LED_2_RED		= 0x00000002,
	  LED_2_BLUE	= 0x00000004,
	  LED_2_GREEN	= 0x00000008,
	  LED_3_RED		= 0x00004000,
	  LED_3_BLUE	= 0x00002000,
	  LED_3_GREEN	= 0x00001000,
	  LED_4_RED		= 0x00000010,
	  LED_4_BLUE	= 0x00000020,
	  LED_4_GREEN	= 0x00000040,
	  LED_5			= 0x00000100,
	  SWITCH_1_LED	= 0x00080000,
	  SWITCH_2_LED	= 0x00800000,
	  SWITCH_3_LED	= 0x00100000,
	  SWITCH_4_LED	= 0x00400000
};

