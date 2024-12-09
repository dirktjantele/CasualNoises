/*
  ==============================================================================

    SystemConfig.h
    Created: 23 oct 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "cmsis_os.h"

constexpr uint32_t	SAMPLE_FREQUENCY		= 44759;		// Real sample frequency
constexpr uint32_t 	NUM_SAMPLES 	 		= 128;
constexpr uint32_t 	NUM_CHANNELS 	 		= 2;

// 128 stereo samples at 32 bit/sample using double buffering
constexpr uint32_t 	FULL_AUDIO_BUFFER_SIZE	= NUM_SAMPLES * NUM_CHANNELS * 2;


