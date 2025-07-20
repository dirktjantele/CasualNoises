/*
  ==============================================================================

    CommunicationPackageDefinitions.h

    Handles communication with SouthSide

    Created: 10/02/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "CasualNoises.h"

namespace CasualNoises
{

enum class ePackageClass
{
	dataPackage,
	cmdPackage
};

enum class ePackageType
{
	emptyPackageType,
	setupPackageType,
	SS_StatPackageType,
};

// Empty package used to poll South Side for pending data
typedef struct
{
	uint32_t			size;
	ePackageType		type;
} tEmptyPackage;

// First package send to the SouthSide after power-on
typedef struct
{
	uint32_t			size;
	ePackageType		type;
	bool				mute;
	uint32_t			sampleFreq;
	uint32_t			bufferSize;
	uint32_t			NumChannels;
} tSetupPackage;

// South Side status info
typedef struct
{
	uint32_t			size;
	ePackageType		type;
	uint32_t			noOfFlashDevices;
	uint32_t			flashDeviceSize;
	uint32_t			largestFreeTLV_Size;
	uint32_t			totalNoOfTLV_FreeBlocks;

} tSS_StatusPackage;

typedef union
{
	tEmptyPackage		emptyPackage;
	tSetupPackage		setupPackage;
	tSS_StatusPackage	SouthSideStatus;
} tCmdPackage;

typedef struct
{
	uint32_t			packageNumber;
	ePackageClass		packageClass;
	uint32_t			noOfPackages;
} tPackageArrayHeader;

constexpr uint32_t 	cTotalCmdPackageArrayCount = 10;

typedef struct
{
	tPackageArrayHeader header;
	tCmdPackage			cmds[cTotalCmdPackageArrayCount];
} tCmdPackageArray;

typedef struct
{
	tPackageArrayHeader header;
	float				audioData[NUM_SAMPLES];
} tAudioPackage;

typedef union
{
	tCmdPackageArray	cmdPackageArray;
	tAudioPackage		audioPackage;
} tTransmitReceiveBuffer;

} // namespace CasualNoises
