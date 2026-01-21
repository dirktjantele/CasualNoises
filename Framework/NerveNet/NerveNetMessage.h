/*
  ==============================================================================

    NerveNetMessage.h
    Created: 09/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "main.h"
#include "SystemConfig.h"

namespace CasualNoises
{

constexpr uint32_t cNoOfTxMessageBuffers = 3;
constexpr uint32_t cNoOfRxMessageBuffers = 2;

enum class eNerveNetSourceId
{
	eAwaitingId,
	eFellhornNortSide,
	eFellhornSouthSide,
	eFellhornDeviceBoard,
};

typedef struct
{
	eNerveNetSourceId	messageSourceID;
	uint32_t			messageNumber;
} sNerveNetHeader;

typedef struct
{
	float				audioData [ NUM_SAMPLES * NUM_CHANNELS ];
} sNerveNetAudio;

typedef struct
{
	uint32_t			size;
	uint8_t				data[NERVENET_DATA_SIZE];
} sNerveNetData;

typedef struct
{
	sNerveNetHeader	header;
#ifdef CASUALNOISES_NERVENET_AUDIO_SUPPORT
	sNerveNetAudio	audio;
#endif
	sNerveNetData	data;
} sNerveNetMessage;

} // namespace CasualNoises
