/*
  ==============================================================================

    NerveNetSlaveProcessor.h

    Abstract class used to handle NerveNet slave messages

    Created: 21/01/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#ifdef CASUALNOISES_NERVENET_THREAD

#include "main.h"

namespace CasualNoises
{

class NerveNetSlaveProcessor
{
public:
	 NerveNetSlaveProcessor() = default;
	~NerveNetSlaveProcessor() = default;

	virtual void processNerveNetData ( uint32_t threadNo, uint32_t size, uint8_t* ptr ) = 0;

};

} // namespace CasualNoises

#endif
