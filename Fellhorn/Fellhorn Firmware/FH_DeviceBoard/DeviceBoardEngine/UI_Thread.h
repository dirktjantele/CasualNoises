/*
  ==============================================================================

    UI_Thread.h
    Created: 15/02/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

//#include "NorthSideAudioProcessor.h"
#include <Drivers/NVM Drivers/NVM Driver/NVM_Driver.h>
#include <Threads/SPI_EncoderThread.h>
#include <Threads/SPI_LED_Thread.h>
#include <Drivers/OLED/SSD1309_Driver.h>
#include <NerveNet/NerveNetMessage.h>

namespace CasualNoises
{

//struct sNerveNetData;

// Arguments expected when starting the thread
typedef struct
{
	sNVM_DriverInitData			nvmDriverInitData;
	sSPI_EncoderThreadData		spiEncoderThreadData;
	sSPI_LED_ThreadData			spi_LED_ThreadData;
	sSSD1309_ConfigData			oledConfigData;
	sADC_MultiplexerThreadData	ADC_MultiplexerThreadData;
	void ( **nerveNetCallBackPtr ) ( uint32_t size, uint8_t* ptr );
} UI_ThreadData;

typedef struct
{
	uint32_t					version;
	uint32_t					count;
} UI_StateData;

// Start thread function
BaseType_t Start_UI_Thread(UI_ThreadData *argument);

} // namespace CasualNoises
