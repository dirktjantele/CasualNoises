/*
  ==============================================================================

    UI_Thread.h
    Created: 15/02/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include <UI_DisplayHandlers/DisplayHandlerRoot.h>
#include "NorthSideAudioProcessor.h"

namespace CasualNoises
{

// Arguments expected when starting the thread
typedef struct
{
	sNVM_DriverInitData			nvmDriverInitData;
	sSPI_EncoderThreadData		spiEncoderThreadData;
	sSPI_LED_ThreadData			spi_LED_ThreadData;
	sSSD1309_ConfigData			oledConfigData;
	sPotentiometerThreadData	potentiometerThreadData;
	sADC_ThreadData				ADC_ThreadData;
//	sSynthesiserParams*			synthesiserParamsPtr;
	void (**nerveNetCallBackPtr)(CasualNoises::sNerveNetData*);
} UI_ThreadData;

// Structure of incoming messages
typedef union
{
	sEncoderEvent				encoderEvent;
	sPotentiometerEventStruct	potentiometerEvent;
} sIncommingUI_Event;


// Start thread function
BaseType_t Start_UI_Thread(UI_ThreadData *argument);

} // namespace CasualNoises
