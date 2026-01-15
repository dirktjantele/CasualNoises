/*
  ==============================================================================

    NorthSideEngine.h

    Handles all kind of events, like internal clock timer, NerveNet, ...

    Created: 10/01/2026
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "cmsis_os.h"
#include "stm32h7xx_hal.h"

#include <Drivers/NVM Drivers/NVM Driver/NVM_Driver.h>
#include <NerveNet/NerveNetMessage.h>

namespace CasualNoises
{

typedef struct
{
	sNVM_DriverInitData			nvmDriverInitData;

	void ( **nerveNetCallBackPtr ) ( CasualNoises::sNerveNetData* );
} sNorthSideEngineParams;

// Start thread function
BaseType_t StartNorthSideEngineThread ( sNorthSideEngineParams* params );

} // namespace CasualNoises
