/*
  ==============================================================================

    ILI9341_Driver.cpp
    Created: 20 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

// 23/1/2024 updated in order to be able to use the ILI9341 in parallel mode (8080 mode)
#ifdef USE_ILI9341_SERIAL_MODE

#include "ILI9341_Driver.h"
#include "ILI9341_Registers.h"

namespace CasualNoises
{

namespace ILI9341_Driver
{

static void resetDevice(ILI9341_Config* configPtr)
{
	HAL_GPIO_WritePin(configPtr->reset_port, configPtr->reset_pin, GPIO_PIN_SET);
	HAL_Delay(200);
	HAL_GPIO_WritePin(configPtr->reset_port, configPtr->reset_pin, GPIO_PIN_RESET);
	HAL_Delay(200);
	HAL_GPIO_WritePin(configPtr->reset_port, configPtr->reset_pin, GPIO_PIN_SET);
	HAL_Delay(200);
}

static HAL_StatusTypeDef writeCommand(ILI9341_Config* configPtr, uint8_t command)
{
	HAL_GPIO_WritePin(configPtr->select_port, configPtr->select_pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(configPtr->dc_port, configPtr->dc_pin, GPIO_PIN_RESET);
	HAL_StatusTypeDef res = HAL_SPI_Transmit(configPtr->SPI_HandlePtr, &command, sizeof(command), HAL_MAX_DELAY);
	HAL_GPIO_WritePin(configPtr->dc_port, configPtr->dc_pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(configPtr->select_port, configPtr->select_pin, GPIO_PIN_SET);
	return res;
}

static HAL_StatusTypeDef writeData(ILI9341_Config* configPtr, uint8_t data[], uint16_t size)
{
	HAL_GPIO_WritePin(configPtr->select_port, configPtr->select_pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(configPtr->dc_port, configPtr->dc_pin, GPIO_PIN_SET);
	HAL_StatusTypeDef res = HAL_SPI_Transmit(configPtr->SPI_HandlePtr, data, size, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(configPtr->select_port, configPtr->select_pin, GPIO_PIN_SET);
	return res;
}

static HAL_StatusTypeDef writeCommandData(ILI9341_Config* configPtr, uint8_t command, uint8_t data[], uint16_t size)
{
	HAL_StatusTypeDef res = writeCommand(configPtr, command);
	if (res != HAL_OK) return res;
	res = writeData(configPtr, data, size);
	return res;
}

static HAL_StatusTypeDef configDevice(ILI9341_Config* configPtr)
{

//	uint8_t params[15];
	HAL_StatusTypeDef res;

	res = writeCommand(configPtr, ILI9341_SWRESET);
	if (res != HAL_OK) return res;

//	res = writeCommandData(configPtr, ILI9341_POWERB, (uint8_t[]){ 0x00, 0xd9, 0x30 }, 3);
/*
//	writeCommand(ILI9341_SWRESET);
	lcd_write_cmd(ILI9341_POWERB);
	params[0] = 0x00;
	params[1] = 0xD9;
	params[2] = 0x30;
	lcd_write_data(params, 3);

	lcd_write_cmd(ILI9341_POWER_SEQ);
	params[0]= 0x64;
	params[1]= 0x03;
	params[2]= 0X12;
	params[3]= 0X81;
	lcd_write_data(params, 4);

	lcd_write_cmd(ILI9341_DTCA);
	params[0]= 0x85;
	params[1]= 0x10;
	params[2]= 0x7A;
	lcd_write_data(params, 3);

	lcd_write_cmd(ILI9341_POWERA);
	params[0]= 0x39;
	params[1]= 0x2C;
	params[2]= 0x00;
	params[3]= 0x34;
	params[4]= 0x02;
	lcd_write_data(params, 5);

	lcd_write_cmd(ILI9341_PRC);
	params[0]= 0x20;
	lcd_write_data(params, 1);

	lcd_write_cmd(ILI9341_DTCB);
	params[0]= 0x00;
	params[1]= 0x00;
	lcd_write_data(params, 2);

	lcd_write_cmd(ILI9341_POWER1);
	params[0]= 0x1B;
	lcd_write_data(params, 1);

	lcd_write_cmd(ILI9341_POWER2);
	params[0]= 0x12;
	lcd_write_data(params, 1);

	lcd_write_cmd(ILI9341_VCOM1);
	params[0]= 0x08;
	params[1]= 0x26;
	lcd_write_data(params, 2);

	lcd_write_cmd(ILI9341_VCOM2);
	params[0]= 0XB7;
	lcd_write_data(params, 1);


	lcd_write_cmd(ILI9341_PIXEL_FORMAT);
	params[0]= 0x55; //select RGB565
	lcd_write_data(params, 1);

	lcd_write_cmd(ILI9341_FRMCTR1);
	params[0]= 0x00;
	params[1]= 0x1B;//frame rate = 70
	lcd_write_data(params, 2);

	lcd_write_cmd(ILI9341_DFC);    // Display Function Control
	params[0]= 0x0A;
	params[1]= 0xA2;
	lcd_write_data(params, 2);

	lcd_write_cmd(ILI9341_3GAMMA_EN);    // 3Gamma Function Disable
	params[0]= 0x02; //LCD_WR_DATA(0x00);
	lcd_write_data(params, 1);

	lcd_write_cmd(ILI9341_GAMMA);
	params[0]= 0x01;
	lcd_write_data(params, 1);

	lcd_write_cmd(ILI9341_PGAMMA);    //Set Gamma
	params[0]= 0x0F;
	params[1]= 0x1D;
	params[2]= 0x1A;
	params[3]= 0x0A;
	params[4]= 0x0D;
	params[5]= 0x07;
	params[6]= 0x49;
	params[7]= 0X66;
	params[8]= 0x3B;
	params[9]= 0x07;
	params[10]= 0x11;
	params[11]= 0x01;
	params[12]= 0x09;
	params[13]= 0x05;
	params[14]= 0x04;
	lcd_write_data(params, 15);

	lcd_write_cmd(ILI9341_NGAMMA);
	params[0]= 0x00;
	params[1]= 0x18;
	params[2]= 0x1D;
	params[3]= 0x02;
	params[4]= 0x0F;
	params[5]= 0x04;
	params[6]= 0x36;
	params[7]= 0x13;
	params[8]= 0x4C;
	params[9]= 0x07;
	params[10]= 0x13;
	params[11]= 0x0F;
	params[12]= 0x2E;
	params[13]= 0x2F;
	params[14]= 0x05;
	lcd_write_data(params, 15);

	lcd_write_cmd(ILI9341_SLEEP_OUT); //Exit Sleep
	delay_50ms();
	delay_50ms();
	lcd_write_cmd(ILI9341_DISPLAY_ON); //display on
*/

	return res;

}


} // namespace ILI9341_Driver

} // namespace CasualNoises

#endif
