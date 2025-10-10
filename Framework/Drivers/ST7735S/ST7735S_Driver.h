/*
  ==============================================================================

    ST7735S_Driver.h
    Created: 25 Sep 2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef USE_ST7735S_DRIVER

#pragma once

#include <math.h>
#include <string.h>

#include "../../Graphics/Fonts/Font.h"
#include "maths.h"

#include "main.h"

namespace CasualNoises
{

//==============================================================================
//          Command definition
//
//  CasualNoises    26/09/2025  First implementation
//==============================================================================

#define DELAY                 0x80

#define NOP                   0x00
#define SWRESET               0x01
#define RDDID                 0x04
#define RDDST                 0x09

#define SLPIN                 0x10
#define SLPOUT                0x11
#define PTLON                 0x12
#define NORON                 0x13

#define INVOFF                0x20
#define INVON                 0x21
#define DISPOFF               0x28
#define DISPON                0x29
#define RAMRD                 0x2E
#define CASET                 0x2A
#define RASET                 0x2B
#define RAMWR                 0x2C

#define PTLAR                 0x30
#define MADCTL                0x36
#define COLMOD                0x3A

#define FRMCTR1               0xB1
#define FRMCTR2               0xB2
#define FRMCTR3               0xB3
#define INVCTR                0xB4
#define DISSET5               0xB6

#define PWCTR1                0xC0
#define PWCTR2                0xC1
#define PWCTR3                0xC2
#define PWCTR4                0xC3
#define PWCTR5                0xC4
#define VMCTR1                0xC5

#define RDID1                 0xDA
#define RDID2                 0xDB
#define RDID3                 0xDC
#define RDID4                 0xDD

#define GMCTRP1               0xE0
#define GMCTRN1               0xE1

#define PWCTR6                0xFC

//==============================================================================
//          AREA definition
//
//  CasualNoises    26/09/2025  First implementation
//==============================================================================

#define MAX_X                 130               // max columns / MV = 0 in MADCTL
#define MAX_Y                 161               // max rows / MV = 0 in MADCTL
#define SIZE_X                MAX_X - 1         // columns max counter
#define SIZE_Y                MAX_Y - 1         // rows max counter
#define CACHE_SIZE_MEM        (MAX_X * MAX_Y)   // whole pixels

//==============================================================================
//          Colors
//
//  CasualNoises    26/09/2025  First implementation
//==============================================================================

#define BLACK                 0x0000
#define WHITE                 0xFFFF
#define RED                   0xF000

//==============================================================================
//          sST7735S_ConfigData
//
//  CasualNoises    25/09/2025  First implementation
//==============================================================================
typedef struct
{
	GPIO_TypeDef*		NRST_PORT;
	uint16_t			NRST_PIN;
	GPIO_TypeDef*		DC_PORT;
	uint16_t			DC_PIN;
	GPIO_TypeDef*		CS_PORT;
	uint16_t			CS_PIN;
	GPIO_TypeDef*		BL_PORT;					// ToDo: add back light pwm data
	uint16_t			BL_PIN;
	SPI_HandleTypeDef*	SPI;
} sST7735S_ConfigData;

//==============================================================================
//          ST7735S_Driver class
//
//  CasualNoises    25/09/2025  First implementation
//==============================================================================
class ST7735S_Driver
{
public:

	//==============================================================================
	//          ST7735S_Driver()
	//
	//  CasualNoises    25/09/2025  First implementation
	//==============================================================================
	ST7735S_Driver() = delete;
	ST7735S_Driver(sST7735S_ConfigData *configDataPtr)
	: mConfigDataPtr (configDataPtr)
	{
		initDisplay();
	}

	//==============================================================================
	//          initDisplay()
	//
	//  CasualNoises    26/09/2025  First implementation
	//==============================================================================
	void initDisplay()
	{
		hardResetDisplay();
		softResetDisplay();
		clearScreen();
	}

	//==============================================================================
	//          clearScreen()
	//
	//  CasualNoises    26/09/2025  First implementation
	//==============================================================================
	void clearScreen()
	{
		setWindow(0, 129, 0, 160);
		sendColor(0xffff, 40960);
		setWindow(1 + 2 + 0, 50 + 2 + 0, 1 + 1, 100 + 1);
		sendColor(0x0000, 5000 * 2);
		setWindow(1 + 3 + 0, 50 + 1 + 0, 1 + 2, 100 + 0);
		//		sendColor(0xffff, (4704 * 2) - (48 * 10));
		sendColor(0xffff, 4704);
/*
		  // set whole window
		  ST7735_SetWindow (lcd, 0, SIZE_X, 0, SIZE_Y);
		  // draw individual pixels
		  ST7735_SendColor565 (lcd, color, CACHE_SIZE_MEM);
*/
	}

private:

	sST7735S_ConfigData*	mConfigDataPtr	{ nullptr };

	//==============================================================================
	//          sendCommand() 1 byte command
	//
	//  CasualNoises    26/12/2024  First implementation
	//==============================================================================
	HAL_StatusTypeDef sendCommand(uint8_t cmd)
	{
		HAL_GPIO_WritePin(mConfigDataPtr->CS_PORT, mConfigDataPtr->CS_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(mConfigDataPtr->DC_PORT, mConfigDataPtr->DC_PIN, GPIO_PIN_RESET);
		HAL_StatusTypeDef res;
		res = HAL_SPI_Transmit(mConfigDataPtr->SPI, &cmd, 1, HAL_MAX_DELAY);
		HAL_GPIO_WritePin(mConfigDataPtr->CS_PORT, mConfigDataPtr->CS_PIN, GPIO_PIN_SET);
		return res;
	}

	//==============================================================================
	//          sendCommand() 1 byte command + 1 byte arg
	//
	//  CasualNoises    26/12/2024  First implementation
	//==============================================================================
	HAL_StatusTypeDef sendCommand(uint8_t cmd, uint8_t arg)
	{
		HAL_GPIO_WritePin(mConfigDataPtr->CS_PORT, mConfigDataPtr->CS_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(mConfigDataPtr->DC_PORT, mConfigDataPtr->DC_PIN, GPIO_PIN_RESET);
		HAL_StatusTypeDef res;
		uint16_t data = (cmd << 8) + arg;
		res = HAL_SPI_Transmit(mConfigDataPtr->SPI, (uint8_t*)&data, 2, HAL_MAX_DELAY);
		HAL_GPIO_WritePin(mConfigDataPtr->CS_PORT, mConfigDataPtr->CS_PIN, GPIO_PIN_SET);
		return res;
	}

	//==============================================================================
	//          sendData() 1 byte data
	//
	//  CasualNoises    26/12/2024  First implementation
	//==============================================================================
	HAL_StatusTypeDef sendData(uint8_t data)
	{
		HAL_GPIO_WritePin(mConfigDataPtr->CS_PORT, mConfigDataPtr->CS_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(mConfigDataPtr->DC_PORT, mConfigDataPtr->DC_PIN, GPIO_PIN_SET);
		HAL_StatusTypeDef res;
		res = HAL_SPI_Transmit(mConfigDataPtr->SPI, &data, 1, HAL_MAX_DELAY);
		HAL_GPIO_WritePin(mConfigDataPtr->CS_PORT, mConfigDataPtr->CS_PIN, GPIO_PIN_SET);
		return res;
	}

	//==============================================================================
	//          sendData() 2 byte data
	//
	//  CasualNoises    26/12/2024  First implementation
	//==============================================================================
	HAL_StatusTypeDef sendData(uint16_t data)
	{
		uint8_t buf[2];
		buf[0] = data >> 8;
		buf[1] = data & 0x00ff;

		HAL_GPIO_WritePin(mConfigDataPtr->CS_PORT, mConfigDataPtr->CS_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(mConfigDataPtr->DC_PORT, mConfigDataPtr->DC_PIN, GPIO_PIN_SET);
		HAL_StatusTypeDef res;
		res = HAL_SPI_Transmit(mConfigDataPtr->SPI, buf, 2, HAL_MAX_DELAY);
		HAL_GPIO_WritePin(mConfigDataPtr->CS_PORT, mConfigDataPtr->CS_PIN, GPIO_PIN_SET);
		return res;
	}

	//==============================================================================
	//          hardResetDisplay()
	//
	//  CasualNoises    25/09/2025  First implementation
	//==============================================================================
	void hardResetDisplay()
	{
		HAL_GPIO_WritePin(mConfigDataPtr->BL_PORT, mConfigDataPtr->BL_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(mConfigDataPtr->NRST_PORT, mConfigDataPtr->NRST_PIN, GPIO_PIN_RESET);
		vTaskDelay(pdMS_TO_TICKS(100));
		HAL_GPIO_WritePin(mConfigDataPtr->NRST_PORT, mConfigDataPtr->NRST_PIN, GPIO_PIN_SET);
		vTaskDelay(pdMS_TO_TICKS(200));			// Min 120 msec
		HAL_GPIO_WritePin(mConfigDataPtr->BL_PORT, mConfigDataPtr->BL_PIN, GPIO_PIN_SET);
	}

	//==============================================================================
	//          softResetDisplay()
	//
	//  CasualNoises    26/09/2025  First implementation
	//==============================================================================
	void softResetDisplay()
	{
		if (sendCommand(SWRESET) != HAL_OK) CN_ReportFault(eErrorCodes::runtimeError);			// SWRESET
		vTaskDelay(pdMS_TO_TICKS(200));			// Min 120 msec
		if (sendCommand(SLPOUT) != HAL_OK) CN_ReportFault(eErrorCodes::runtimeError);			// SLPOUT
		vTaskDelay(pdMS_TO_TICKS(200));			// Min 120 msec
//		if (sendCommand(COLMOD, 0x05) != HAL_OK) CN_ReportFault(eErrorCodes::runtimeError);		// COLMOD 16 bit
		if (sendCommand(COLMOD, 0x55) != HAL_OK) CN_ReportFault(eErrorCodes::runtimeError);		// COLMOD 16 bit
		// D7  D6  D5  D4  D3  D2  D1  D0
		// MY  MX  MV  ML RGB  MH   -   -
		// ------------------------------
		// ------------------------------
		// MV  MX  MY -> {MV (row / column exchange) MX (column address order), MY (row address order)}
		// ------------------------------
		//  0   0   0 -> begin left-up corner, end right-down corner
		//               left-right (normal view)
		//  0   0   1 -> begin left-down corner, end right-up corner
		//               left-right (Y-mirror)
		//  0   1   0 -> begin right-up corner, end left-down corner
		//               right-left (X-mirror)
		//  0   1   1 -> begin right-down corner, end left-up corner
		//               right-left (X-mirror, Y-mirror)
		//  1   0   0 -> begin left-up corner, end right-down corner
		//               up-down (X-Y exchange)
		//  1   0   1 -> begin left-down corner, end right-up corner
		//               down-up (X-Y exchange, Y-mirror)
		//  1   1   0 -> begin right-up corner, end left-down corner
		//               up-down (X-Y exchange, X-mirror)
		//  1   1   1 -> begin right-down corner, end left-up corner
		//               down-up (X-Y exchange, X-mirror, Y-mirror)
		// ------------------------------
		//  ML: vertical refresh order
		//      0 -> refresh top to bottom
		//      1 -> refresh bottom to top
		// ------------------------------
		// RGB: filter panel
		//      0 -> RGB
		//      1 -> BGR
		// ------------------------------
		//  MH: horizontal refresh order
		//      0 -> refresh left to right
		//      1 -> refresh right to left
		// 0xA0 = 1010 0000
		if (sendCommand(MADCTL, 0xA0) != HAL_OK) CN_ReportFault(eErrorCodes::runtimeError);		// MADCTL Memory Data Access Control
		if (sendCommand(DISPON) != HAL_OK) CN_ReportFault(eErrorCodes::runtimeError);			// DISPON Main screen turn on
		clearScreen();																			// Clear entire screen
	}

	//==============================================================================
	//          setWindow()
	//
	//  CasualNoises    26/09/2025  First implementation
	//==============================================================================
	void setWindow (uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1)
	{
/*
		// check if coordinates are out of range
		if ((x0 > x1)     ||
		  (x1 > SIZE_X) ||
		  (y0 > y1)     ||
		  (y1 > SIZE_Y)) {
		  CN_ReportFault(eErrorCodes::runtimeError);
		}
*/
		// column address set
		if (sendCommand(CASET) != HAL_OK) CN_ReportFault(eErrorCodes::runtimeError);
		if (sendData((uint16_t)(0x0000 | x0)) != HAL_OK) CN_ReportFault(eErrorCodes::runtimeError);
		if (sendData((uint16_t)(0x0000 | x1)) != HAL_OK) CN_ReportFault(eErrorCodes::runtimeError);

		// row address set
		if (sendCommand(RASET) != HAL_OK) CN_ReportFault(eErrorCodes::runtimeError);
		if (sendData((uint16_t)(0x0000 | y0)) != HAL_OK) CN_ReportFault(eErrorCodes::runtimeError);
		if (sendData((uint16_t)(0x0000 | y1)) != HAL_OK) CN_ReportFault(eErrorCodes::runtimeError);

	}

	//==============================================================================
	//          sendColor()
	//
	//  CasualNoises    26/09/2025  First implementation
	//==============================================================================
	void sendColor (uint16_t color, uint16_t count)
	{
		uint32_t cnt = 0;

		if (sendCommand(RAMWR) != HAL_OK) CN_ReportFault(eErrorCodes::runtimeError);
		while (count--) {
			sendData(color);
			++cnt;
		}
	}


};

} // namespace CasualNoises

#endif
