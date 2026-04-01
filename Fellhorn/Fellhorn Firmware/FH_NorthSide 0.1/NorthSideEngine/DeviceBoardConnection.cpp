/*
  ==============================================================================

    DeviceBoardConnection.cpp

    Handle NerveNet slave messages

    Created: 21/01/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

//#ifdef debugging

#ifdef CASUALNOISES_NERVENET_THREAD

#include "DeviceBoardConnection.h"

#include "EffectEngines/AbstractEffectEngine.h"

#include <SynthEngineMessage.h>

#include <NerveNet/NerveNetSlaveThread.h>
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
	NerveNetSlaveThread* nerveNetThreadPtr = gNerveNetSlaveThreadPtr [ threadNo ];
	if ( nerveNetThreadPtr == nullptr )
		CN_ReportFault ( eErrorCodes::NerveNetThread_Error );

	// Dispatch all incoming messages
	while ( size > 0 )
	{

		// Handle event
		tInitMessage* messagePtr = ( tInitMessage* ) ptr;

		if ( messagePtr->header.destinationID == eNerveNetSourceId::FellhornNorthSide )
		{

			eSynthEngineMessageType type = ( eSynthEngineMessageType ) messagePtr->header.messageTag;
			switch ( type )
			{
				case eSynthEngineMessageType::initSynthEngine:
																												// ToDo: handle this event
					break;
				case eSynthEngineMessageType::requestSetupInfo:
					handleRequestSetupInfo ( nerveNetThreadPtr );
					break;
				case eSynthEngineMessageType::potentiometerValue:
				{																								// ToDo: handle this event
					tPotValueMessage* _ptr = (tPotValueMessage*) messagePtr;
					float value = _ptr->potValue;
					UNUSED ( value );
				}
					break;
				case eSynthEngineMessageType::ADC_DataRequest:
					break;
				default:
					CN_ReportFault ( eErrorCodes::NerveNetThread_Error );
			}

			// ToDo forward event to the NorthSideAudioProcessor
			extern AbstractEffectEngine* gAbstractEffectEnginePtr;
			if ( gAbstractEffectEnginePtr != nullptr )
			{
				gAbstractEffectEnginePtr->processNerveNetMessage ( messagePtr );
			}

		} else
		{

			// Forward event to the South Side
			gNerveNetMasterThreadPtr [ 0 ]->sendMessage( messagePtr, messagePtr->header.messageLength, true);

		}

		// Goto next event
		size -= messagePtr->header.messageLength;
		ptr  += messagePtr->header.messageLength;

	}

}

//==============================================================================
//          handleRequestSetupInfo()
//
//  CasualNoises    21/01/2026  First implementation
//==============================================================================
void DeviceBoardConnection::handleRequestSetupInfo ( NerveNetSlaveThread* nerveNetThreadPtr )
{
	tRequestSetupInfoReplyData reply;
	reply.header.sourceID 		= eNerveNetSourceId::FellhornNorthSide;
	reply.header.destinationID	= eNerveNetSourceId::FellhornDeviceBoard;
	reply.header.messageTag 	= (uint32_t)eSynthEngineMessageType::requestSetupInfo;
	reply.header.messageLength 	= sizeof ( tRequestSetupInfoReplyData );
	reply.version				= 0x3130534e;				// 'NS01'
	nerveNetThreadPtr->sendMessage( &reply, reply.header.messageLength );			// ToDo remove multiple events
}

} // namespace CasualNoises

#endif

//#endif
