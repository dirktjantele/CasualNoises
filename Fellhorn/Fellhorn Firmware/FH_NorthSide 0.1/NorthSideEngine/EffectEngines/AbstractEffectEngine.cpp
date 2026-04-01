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

//#ifdef debugging

namespace CasualNoises
{

//==============================================================================
//          applyControlVoltages
//
// Filter out potentiometer events, pre-process the data and apply data
//
//  CasualNoises    14/03/2026  First implementation
//==============================================================================
void AbstractEffectEngine::applyControlVoltages (  uint32_t noOfEntries, uint16_t* adcDataPtr  )
{

	static sControlVoltages controlVoltages;

	controlVoltages._1V_OCT_1 = normalize1V_OCT( adcDataPtr[0] );
	controlVoltages._1V_OCT_2 = normalize1V_OCT( adcDataPtr[1] );

	for (uint32_t i = 0; i < NUM_CV_INPUTS; ++i)
	{
		controlVoltages.CV_Inputs [ i ] = normalizeCV_Input ( adcDataPtr[i + 2] );
	}

	applyControlVoltages ( &controlVoltages );
};

//==============================================================================
//          processNerveNetMessage
//
// Filter out potentiometer events, pre-process the data and apply data
//
//  CasualNoises    13/03/2026  First implementation
//==============================================================================
void AbstractEffectEngine::processNerveNetMessage ( tInitMessage* messagePtr )
{
	if ( messagePtr->header.messageTag == (uint32_t)eSynthEngineMessageType::potentiometerValue )
	{
		tPotValueMessage* ptr 			= (tPotValueMessage*) messagePtr;
		uint16_t multiplexerNo 			= ptr->multiplexerNo;
		uint16_t multiplexerChannelNo 	= ptr->multiplexerChannelNo;
		ePotentioMeterId potId  		= ( ePotentioMeterId )( ( multiplexerNo << 4	) + multiplexerChannelNo );
		float potValue 					= ptr->potValue;
		applyPotentiometerValue ( potId, potValue );
	}
};

} // namespace CasualNoises

//#endif
