/*
  ==============================================================================

    NorthSideConnection.cpp

    Handle North Side slave messages

    Created: 21/01/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_NERVENET_THREAD

#include "NorthSideConnection.h"

#include "UI_Thread.h"

#include <NerveNet/NerveNetMessage.h>

namespace CasualNoises
{

//==============================================================================
//          processNerveNetData()
//
//  CasualNoises    21/01/2026  First implementation
//==============================================================================
void NorthSideConnection::processNerveNetData (
		 uint32_t count,
		 uint32_t size,
		 uint8_t* ptr )
{
	handleNerveNetCallBacks ( size, ptr );
}

} // namespace CasualNoises

#endif
