/*
  ==============================================================================

    DeviceBoardConnection.h

    Handle NerveNet slave messages

    Created: 21/01/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

//#ifdef debugging

#pragma once

#ifdef CASUALNOISES_NERVENET_THREAD

#include <NerveNet/NerveNetSlaveProcessor.h>

#include "main.h"

namespace CasualNoises
{

class NerveNetSlaveThread;

class DeviceBoardConnection final : public NerveNetSlaveProcessor
{
public:
	 DeviceBoardConnection() = default;
	~DeviceBoardConnection() = default;

	void processNerveNetData ( uint32_t threadNo, uint32_t size, uint8_t* ptr ) override;

private:

	void handleRequestSetupInfo	( NerveNetSlaveThread* nerveNetThreadPtr );

};

} // namespace CasualNoises

//#endif

#endif
