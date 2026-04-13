/*
  ==============================================================================

    SouthSideConnection.cpp

    Handle South Side slave messages

    Created: 07/04/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_NERVENET_THREAD

#include "SouthSideConnection.h"

#include "Utilities/ReportFault.h"

#include "NerveNet/NerveNetSlaveThread.h"
#include "NerveNet/NerveNetMessage.h"

namespace CasualNoises
{

//==============================================================================
//          processNerveNetData()
//
//  CasualNoises    07/04/2026  First implementation
//==============================================================================
//extern NerveNetSlaveThread* gNerveNetSlaveThreadPtr;
void SouthSideConnection::processNerveNetData (
		 uint32_t /*count*/,
		 uint32_t size,
		 uint8_t* ptr )
{
	while ( size > 0 )
	{

		// Destination = DeviceBoard?
		tNerveNetMessageHeader* messagePtr = ( tNerveNetMessageHeader* ) ptr;
		if ( messagePtr->destinationID == eNerveNetSourceId::FellhornDeviceBoard )
		{
			for ( uint32_t i = 0; i < MAX_NO_OF_NERVENET_SLAVE_THREADS; ++i )
				gNerveNetSlaveThreadPtr [ i ]->sendMessage( messagePtr, messagePtr->messageLength );
		} else

		// Destination = NothSide?
		if ( messagePtr->destinationID == eNerveNetSourceId::FellhornDeviceBoard )
		{
		} else

		// Invalid destination
		CN_ReportFault( eErrorCodes::NerveNetThread_Error );

		// Get next message
		ptr += messagePtr->messageLength;
		size -= messagePtr->messageLength;

	}

}

} // namespace CasualNoises

#endif
