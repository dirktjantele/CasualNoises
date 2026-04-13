/*
  ==============================================================================

    SouthSideConnection.h

    Handle South Side slave messages

    Created: 07/04/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#ifdef CASUALNOISES_NERVENET_THREAD

#include <NerveNet/NerveNetMasterProcessor.h>

#include "main.h"

namespace CasualNoises
{

class SouthSideConnection final : public NerveNetMasterProcessor
{
public:
	 SouthSideConnection() = default;
	~SouthSideConnection() = default;

	void processNerveNetData ( uint32_t count, uint32_t size, uint8_t* ptr ) override;

};

} // namespace CasualNoises

#endif
