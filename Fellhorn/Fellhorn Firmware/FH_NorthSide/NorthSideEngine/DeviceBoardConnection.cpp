/*
  ==============================================================================

    DeviceBoardConnection.cpp

    Handle NerveNet slave messages

    Created: 21/01/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_NERVENET_THREAD

#include "DeviceBoardConnection.h"

#include <SynthEngineMessage.h>

#include <NerveNet/NerveNetMasterThread.h>
#include <NerveNet/NerveNetMessage.h>

#include <Utilities/ReportFault.h>

namespace CasualNoises
{

//==============================================================================
//          processNerveNetData()
//
//  CasualNoises    21/01/2026  First implementation
//==============================================================================
void DeviceBoardConnection::processNerveNetData ( uint32_t threadNo, uint32_t size, uint8_t* ptr )
{

	// NerveNet should be up and running right now
	NerveNetMasterThread* nerveNetThreadPtr = gNerveNetMasterThreadPtr [ threadNo ];
	if ( nerveNetThreadPtr == nullptr )
		CN_ReportFault ( eErrorCodes::NerveNetThread_Error );

	// Dispatch all incoming messages
	while ( size > 0 )
	{
		tInitMessage* messagePtr = ( tInitMessage* ) ptr;
		eSynthEngineMessageType type = ( eSynthEngineMessageType ) messagePtr->header.messageTag;
		switch ( type )
		{
			case eSynthEngineMessageType::initSynthEngine:
																											// ToDo: handle this event
				break;
			case eSynthEngineMessageType::requestSetupInfo:

				break;
			default:
				CN_ReportFault ( eErrorCodes::NerveNetThread_Error );
		}
		size -= messagePtr->header.messageLength;
		ptr  += messagePtr->header.messageLength;
	}

}

} // namespace CasualNoises

#endif
