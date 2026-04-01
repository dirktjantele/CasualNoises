/*
  ==============================================================================

    NerveNetMessageHeader.h
    Created: 17/07/2025

    Structure of messages shared between North- and SouthSide (exchange using NerveNet)
    	NorthSide: change data using the UI
    	SouthSide: use data for synthesis

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#ifdef CASUALNOISES_NERVENET_THREAD

#include "NerveNetMessage.h"

namespace CasualNoises
{

typedef struct
{
	eNerveNetSourceId 	sourceID;			// ID of the source used for any reply on an event
	eNerveNetSourceId	destinationID;		// Used for forwarding messages
	uint32_t			messageTag;			// Tags are implementation specific
	uint32_t			messageLength;		// The total length of the message in bytes
	// Any data to be send follows this header in memory as indicated by messageLength
} tNerveNetMessageHeader;

}	// namespace CasualNoises

#endif
