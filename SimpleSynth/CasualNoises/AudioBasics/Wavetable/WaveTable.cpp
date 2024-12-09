/*
  ==============================================================================

    WaveTable.cpp
    Created: 1 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "WaveTable.h"

namespace CasualNoises
{

//==============================================================================
//          WaveTable()     /       ~WaveTable
//
// Note: a WaveTable object does not own the wave table data, allocation and release
// 			of the memory should be done by the creator of the object
//
//  CasualNoises    01/08/2023  First implementation
//==============================================================================
 WaveTable::WaveTable(uint16_t noOfWaves, eWaveTableType type, float* waveTableData)
 : mNoOfWaves (noOfWaves)
 , mWaveTableType (type)
 , mWaveTableDataPtr (waveTableData)
 {
 }

 WaveTable::~WaveTable()
 {
 }

} // namespace CasualNoises


