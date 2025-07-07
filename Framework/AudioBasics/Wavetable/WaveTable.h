/*
  ==============================================================================

    WaveTable.h
    Created: 01 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "stm32h7xx_hal.h"

namespace CasualNoises
{

static const uint32_t noOfSamplesInWave = 1024;

// Wavetable types
enum class eWaveTableType
{
	Bass,
	Lead,
	Pad,
	Synth,
	Bell,
	Plucked,
	Perc,
	Keys,
	Brass,
	Winds,
	Strings,
	Vocal,
	Seq,
	Sfx,
	User
};

// Structure of a wavetable header
typedef struct
{
	uint8_t				name[32];
	eWaveTableType		type;
	uint32_t			noOfWaves;
} sWaveTableHeader;

// Wavetable class
class WaveTable
{
public:
	 WaveTable() = delete;
	~WaveTable();

	WaveTable(uint16_t noOfWaves, eWaveTableType type, float* waveTableData);

	uint16_t		getNoOfWaves()			{ return mNoOfWaves; }
	eWaveTableType	getWaveTableType() 		{ return mWaveTableType; }
	float*			getWaveTableDataPtr()	{ return mWaveTableDataPtr; }

private:
	uint16_t		mNoOfWaves { 0 };
	eWaveTableType	mWaveTableType { eWaveTableType::User };
	float*			mWaveTableDataPtr { nullptr };

};

} // namespace CasualNoises
