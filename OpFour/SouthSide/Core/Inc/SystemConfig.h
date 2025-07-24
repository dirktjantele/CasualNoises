/*
  ==============================================================================

    SystemConfig.h
    Created: 23 oct 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "cmsis_os.h"

#include "CommonDefinitions.h"

// SouthSide has no codec, so the sample frequence should be set the same as the on the NorthSide!
constexpr uint32_t	SAMPLE_FREQUENCY				= 41666.0f;		// Real sample frequency

// 128 stereo samples at 32 bit/sample using double buffering
constexpr uint32_t 	FULL_AUDIO_BUFFER_SIZE			= NUM_SAMPLES * NUM_CHANNELS * 2;

constexpr uint32_t	DISPLY_WIDTH					= 320;
constexpr uint32_t	DISPLAY_HEIGHT					= 240;

constexpr uint32_t	PCF8574_ADRRESS_1				= 0x40;			// Shifted 1 bit to the left
constexpr uint32_t	PCF8574_ADRRESS_2				= 0x42;
constexpr uint32_t	PCF8574_COUNT					= 2;
constexpr uint32_t	ENCODER_COUNT					= 4;

constexpr uint32_t	DEFAULT_STACK_SIZE 				= 256;

constexpr uint32_t	AUDIO_THREAD_PRIORITY			= configMAX_PRIORITIES - 1;
constexpr uint32_t	NERVENET_THRAD_PRIORITY			= AUDIO_THREAD_PRIORITY - 10;
constexpr uint32_t	CV_IN_THREAD_PRIORITY			= AUDIO_THREAD_PRIORITY - 15;
constexpr uint32_t	TRIGGER_THREAD_PRIORITY			= 1;
constexpr uint32_t	NORTH_SOUTH_COM_THREAD_PRIORITY	= AUDIO_THREAD_PRIORITY - 10;

constexpr uint32_t	MAX_NO_OF_NERVENET_MASTER_THREADS = 1;
constexpr uint32_t	MAX_NO_OF_NERVENET_SLAVE_THREADS  = 2;
constexpr uint32_t	AUDIO_NERVENET_THREAD_NO		  = 0;
constexpr uint32_t	NERVENET_DATA_SIZE				  = 1024;
