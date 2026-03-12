/*
  ==============================================================================

    CommonDefinitions.h
    Created: 19/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

constexpr uint32_t 	NUM_SAMPLES 		= 256;
constexpr uint32_t 	NUM_CHANNELS		= 2;

constexpr uint32_t	NUM_CV_INPUTS		= 7;		// Control voltage inputs from ADC
constexpr uint32_t	TOTAL_NUM_CV_INPUTS	= 9;		// Includes 1V/OCT inputs

enum class ePotentioMeterId
{
	pot_1		= 0x02,
	pot_2		= 0x06,
	pot_3		= 0x01,
	pot_4		= 0x04,
	slider_1	= 0x12,
	slider_2	= 0x11,
	slider_3	= 0x14,
	slider_4	= 0x16,
	slider_5	= 0x10,
	slider_6	= 0x13,
	slider_7	= 0x15,
	slider_8	= 0x17
};

constexpr uint32_t	CV_INPUT_1		= 0;
constexpr uint32_t	CV_INPUT_2		= 1;
constexpr uint32_t	CV_INPUT_3		= 2;
constexpr uint32_t	CV_INPUT_4		= 3;
constexpr uint32_t	CV_INPUT_5		= 4;
constexpr uint32_t	CV_INPUT_6		= 5;
constexpr uint32_t	CV_INPUT_7		= 6;
