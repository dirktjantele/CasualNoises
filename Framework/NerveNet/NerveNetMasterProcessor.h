/*
  ==============================================================================

    NerveNetMasterProcessor.h

    Abstract class used to handle NerveNet master messages

    Created: 21/01/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#ifdef CASUALNOISES_NERVENET_THREAD

#include "main.h"

namespace CasualNoises
{

class NerveNetMasterProcessor
{
public:
	 NerveNetMasterProcessor() = default;
	~NerveNetMasterProcessor() = default;

	virtual void processNerveNetData ( uint32_t size, uint8_t* ptr ) = 0;

};

} // namespace CasualNoises

#endif
