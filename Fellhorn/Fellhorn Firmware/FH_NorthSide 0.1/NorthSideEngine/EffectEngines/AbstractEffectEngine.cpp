/*
  ==============================================================================

    AbstractEffectEngine.cpp

    Handle device board NerveNet messages

    Created: 13/03/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include "AbstractEffectEngine.h"

#include <CommonUtilities.h>

namespace CasualNoises
{

//==============================================================================
//          processNerveNetMessage
//
// Filter out potentiometer events, pre-process the data and apply data
//
//  CasualNoises    13/03/2026  First implementation
//==============================================================================
void AbstractEffectEngine::processNerveNetMessage ( tInitMessage* messagePtr )
{

	eSynthEngineMessageType type = (eSynthEngineMessageType)messagePtr->header.messageTag;
	switch ( type )
	{
	case eSynthEngineMessageType::potentiometerValue:
		{
			tPotValueMessage* ptr 			= (tPotValueMessage*) messagePtr;
			uint16_t multiplexerNo 			= ptr->multiplexerNo;
			uint16_t multiplexerChannelNo 	= ptr->multiplexerChannelNo;
			ePotentioMeterId potId  		= ( ePotentioMeterId )( ( multiplexerNo << 4	) + multiplexerChannelNo );
			float potValue 					= ptr->potValue;
			applyPotentiometerValue ( potId, potValue );
		}
		break;
	default:
		break;
	}

};

} // namespace CasualNoises
