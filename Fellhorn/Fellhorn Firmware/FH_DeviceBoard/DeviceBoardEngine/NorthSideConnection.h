/*
  ==============================================================================

    NorthSideConnection.h

    Handle North Side slave messages

    Created: 21/01/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#ifdef CASUALNOISES_NERVENET_THREAD

#include <NerveNet/NerveNetMasterProcessor.h>

#include "main.h"

namespace CasualNoises
{

class NorthSideConnection : public NerveNetMasterProcessor
{
public:
	 NorthSideConnection() = default;
	~NorthSideConnection() = default;

	void processNerveNetData ( uint32_t size, uint8_t* ptr ) override;

};

} // namespace CasualNoises

#endif
