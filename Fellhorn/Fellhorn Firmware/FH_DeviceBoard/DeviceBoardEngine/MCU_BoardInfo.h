/*
  ==============================================================================

    MCU_BoardInfo.h

    Usefull for communication between threads

    Created: 19/04/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include <stdint.h>

namespace CasualNoises
{

typedef struct
{
	uint32_t		version;

} tNorthSideInfo;

typedef struct
{
	uint32_t		version;

} tSouthSideInfo;

typedef struct
{
	tNorthSideInfo	NorthSide;
	tSouthSideInfo	SouthSide;
} tMCU_BoardInfo;

extern tMCU_BoardInfo	gMCU_BoardInfo;

} // namespace CasualNoises
