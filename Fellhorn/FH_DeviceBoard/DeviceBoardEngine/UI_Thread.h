/*
  ==============================================================================

    UI_Thread.h
    Created: 15/02/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

//#include "NorthSideAudioProcessor.h"
#include "../CasualNoises/Drivers/NVM Drivers/NVM Driver/NVM_Driver.h"
#include "../CasualNoises/Threads/SPI_EncoderThread.h"
#include "../CasualNoises/Threads/SPI_LED_Thread.h"
#include "../CasualNoises/Drivers/OLED/SSD1309_Driver.h"

namespace CasualNoises
{

// Arguments expected when starting the thread
typedef struct
{
	sNVM_DriverInitData			nvmDriverInitData;
	sSPI_EncoderThreadData		spiEncoderThreadData;
	sSPI_LED_ThreadData			spi_LED_ThreadData;
	sSSD1309_ConfigData			oledConfigData;
	sADC_MultiplexerThreadData	ADC_MultiplexerThreadData;
//	void (**nerveNetCallBackPtr)(CasualNoises::sNerveNetData*);
} UI_ThreadData;

// Structure of incoming messages
typedef union
{
	sEncoderEvent				encoderEvent;
	sMultiplexed_ADC_Event		multiplexed_ADC_Event;
} sIncommingUI_Event;

typedef struct
{
	uint32_t					version;
} UI_StateData;

// Start thread function
BaseType_t Start_UI_Thread(UI_ThreadData *argument);

} // namespace CasualNoises
