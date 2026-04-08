/*
  ==============================================================================

    NerveNetMessage.h
    Created: 09/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "SystemConfig.h"
#include <stdint.h>

namespace CasualNoises
{

constexpr uint32_t cNoOfTxMessageBuffers = 3;
constexpr uint32_t cNoOfRxMessageBuffers = 2;

enum class eNerveNetSourceId
{
	awaitingId,
	FellhornNorthSide,
	FellhornSouthSide,
	FellhornDeviceBoard,
	FellhornBothSides,
};
/*
typedef struct
{
	eNerveNetSourceId	messageSourceID;
	eNerveNetSourceId	messageDestinationID;
	uint32_t			messageNumber;
} sNerveNetHeader;
*/

typedef struct
{
	eNerveNetSourceId 	sourceID;			// ID of the source used for any reply on an event
	eNerveNetSourceId	destinationID;		// Used for forwarding messages
	uint32_t			messageNumber;		// Optional
	uint32_t			messageTag;			// Tags are implementation specific
	uint32_t			messageLength;		// The total length of the message in bytes
	// Any data to be send follows this header in memory as indicated by messageLength
} tNerveNetMessageHeader;

typedef struct
{
	float				audioData [ NUM_SAMPLES * NUM_CHANNELS ];
} sNerveNetAudio;

typedef struct
{
	uint32_t			size;
	uint8_t				data [ NERVENET_DATA_SIZE ];
} sNerveNetData;

typedef struct
{
	tNerveNetMessageHeader	header;
//#ifdef CASUALNOISES_NERVENET_SLAVE_AUDIO_SUPPORT
	sNerveNetAudio			audio;
//#endif
	sNerveNetData			data;
} sNerveNetMessage;

} // namespace CasualNoises
