/*
  ==============================================================================

    TriggerThread.cpp
    Created: 2 dec 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_TRIGGER_THREAD

#include "TriggerThread.h"

#include "main.h"
#include "SystemConfig.h"

#include "CasualNoises.h"

//==============================================================================
//          TriggerThread
//
// Just loop around and toggle trigger pin
//
//  CasualNoises    02/12/2024  First implementation
//==============================================================================
void CasualNoises::TriggerThread(void* pvParameters)
{
	while ( true )
	{
		toggleTrigger_1();
	}

}

#endif
