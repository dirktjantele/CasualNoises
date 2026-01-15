/*
  ==============================================================================

    SSD1309_Driver.h
    Created: 25 Dec 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_SSD1309_DRIVER

#pragma once

#include <math.h>

#include "CasualNoises.h"
#include <Core/Text/String.h>
#include <Graphics/Fonts/Font.h>
#include <Graphics/Geometry/Rectangle.h>
#include "maths.h"

#include "main.h"

namespace CasualNoises
{

//==============================================================================
//          SSD1309_ConfigData
//
//  CasualNoises    25/12/2024  First implementation
//==============================================================================
typedef struct
{
	GPIO_TypeDef*		NRST_PORT;
	uint16_t			NRST_PIN;
	GPIO_TypeDef*		DC_PORT;
	uint16_t			DC_PIN;
	GPIO_TypeDef*		CS_PORT;
	uint16_t			CS_PIN;
	SPI_HandleTypeDef*	SPI;
} sSSD1309_ConfigData;

//==============================================================================
//          eBitOperations
//
//  CasualNoises    25/12/2024  First implementation
//==============================================================================
enum class eBitOperations
{
	SetBitOp,
	ClearBitOp,
	InvertBitOp
};

//==============================================================================
//          SSD1309_Driver class
//
//  CasualNoises    25/12/2024  First implementation
//  CasualNoises    13/12/2025  Made it more generic for Fellhorn rev 2
//==============================================================================
class SSD1309_Driver final
{
public:

	//==============================================================================
	//          SSD1309_Driver()
	//
	//  CasualNoises    25/12/2024  First implementation
	//==============================================================================
	SSD1309_Driver() = delete;
	SSD1309_Driver(sSSD1309_ConfigData *configDataPtr)
	: mConfigDataPtr (configDataPtr)
	{
		resetDisplay();
		displayOn();
		clearDisplay();
	}

	//==============================================================================
	//          resetDisplay()
	//
	//  CasualNoises    25/12/2024  First implementation
	//==============================================================================
	void resetDisplay()
	{
		  HAL_GPIO_WritePin(mConfigDataPtr->NRST_PORT, mConfigDataPtr->NRST_PIN, GPIO_PIN_RESET);
		  vTaskDelay(pdMS_TO_TICKS(100));
		  HAL_GPIO_WritePin(mConfigDataPtr->NRST_PORT, mConfigDataPtr->NRST_PIN, GPIO_PIN_SET);
	}

	//==============================================================================
	//          displayOn()
	//
	//  CasualNoises    25/12/2024  First implementation
	//==============================================================================
	HAL_StatusTypeDef displayOn()
	{
		HAL_StatusTypeDef res;
		res = sendCommand(0x20, 0x00);		// Set horizontal addressing mode
		if (res != HAL_OK) return res;
		res = sendCommand(0xa4);			// Entire display on
		if (res != HAL_OK) return res;
		res = sendCommand(0xaf);			// Set display on
		return res;
	}

	//==============================================================================
	//          clearDisplay()
	//
	//  CasualNoises    25/12/2024  First implementation
	//==============================================================================
	void clearDisplay()
	{
		for (uint32_t y = 0; y < cDisplayHeight; ++y)
			for (uint32_t x = 0; x < cDisplayWidth; ++x)
				mBitMap[x][y] = 0x00;
	}

	//==============================================================================
	//          refreshDisplay()
	//
	//  CasualNoises    25/12/2024  First implementation
	//==============================================================================
	HAL_StatusTypeDef refreshDisplay()
	{

		// Fill bitmap buffer with pixel data from the source bitmap
		uint32_t x = 0;
		uint32_t y = 0;
		for (uint32_t i = 0; i < cBitMapBufferSize; ++i)
		{
			uint8_t byte = 0x00;
			for (uint32_t j = 0; j < 8; ++j)
			{
				byte >>= 1;
				if (mBitMap[x][y + j])
					byte |= 0x80;
			}
			mBitMapBuffer[i] = byte;
			x += 1;
			if (x >= cDisplayWidth)
			{
				x = 0;
				y += 8;
			}
		}

		// Send bitmap buffer to the display
		HAL_GPIO_WritePin(mConfigDataPtr->CS_PORT, mConfigDataPtr->CS_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(mConfigDataPtr->DC_PORT, mConfigDataPtr->DC_PIN, GPIO_PIN_SET);
		HAL_StatusTypeDef res;
		res = HAL_SPI_Transmit(mConfigDataPtr->SPI, mBitMapBuffer, cBitMapBufferSize, HAL_MAX_DELAY);
// ToDo: use dma for the transfert
//		res = HAL_SPI_Transmit_DMA(mConfigDataPtr->SPI, mBitMapBuffer, cBitMapBufferSize);
		HAL_GPIO_WritePin(mConfigDataPtr->CS_PORT, mConfigDataPtr->CS_PIN, GPIO_PIN_SET);
		return res;

	}

	//==============================================================================
	//          drawPixel()
	//
	//  CasualNoises    25/12/2024  First implementation
	//==============================================================================
	void drawPixel(int32_t x, int32_t y, eBitOperations op = eBitOperations::SetBitOp)
	{
		if ((x < 0) || (x >= cDisplayWidth) ||
			(y < 0) || (y >= cDisplayHeight))
			return;
		if (op == eBitOperations::SetBitOp)
			mBitMap[x][y] = 0xff;
		if (op == eBitOperations::ClearBitOp)
			mBitMap[x][y] = 0x00;
		if (op == eBitOperations::InvertBitOp)
			mBitMap[x][y] = 0xff - mBitMap[x][y];
	}

	//==============================================================================
	//          drawRect()
	//
	//  CasualNoises    27/12/2024  First implementation
	//==============================================================================
	void drawRect(int16_t x, int16_t y, uint16_t w, uint16_t h,
			eBitOperations op = eBitOperations::SetBitOp)
	{
	  drawLine( x,     y,     x + w, y    , op );
	  drawLine( x,     y + h, x + w, y + h, op );
	  drawLine( x,     y,     x,     y + h, op );
	  drawLine( x + w, y,     x + w, y + h, op );
	}

	//==============================================================================
	//          drawRect()
	//
	//  CasualNoises    04/01/2026  First implementation
	//==============================================================================
	void drawRect(Rectangle<int> rect,
			eBitOperations op = eBitOperations::SetBitOp)
	{
		drawRect(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight(), op);
	}

	//==============================================================================
	//          fillRect()
	//
	//  CasualNoises    27/12/2024  First implementation
	//==============================================================================
	void fillRect ( int32_t x, int32_t y, int32_t w, int32_t h,
			eBitOperations op = eBitOperations::SetBitOp )
	{

		// Verify we have something within screen dimensions to be drawn
		if ( ! clipRect ( &x, &y, &w, &h ) )
			return;

		// Fill the rectangle line by line
		for ( int32_t l = 0; l < h; ++l )
		{
			drawLine ( x, y + l, x + w - 1, y + l, op );
		}

	}

	//==============================================================================
	//          drawRect()
	//
	//  CasualNoises    04/01/2026  First implementation
	//==============================================================================
	void fillRect(Rectangle<int> rect,
			eBitOperations op = eBitOperations::SetBitOp)
	{
		fillRect(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight(), op);
	}

	//==============================================================================
	//          drawLine()
	//
	//  CasualNoises    26/12/2024  First implementation
	//==============================================================================
	void drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, eBitOperations op = eBitOperations::SetBitOp)
	{
		float dx = x1 - x0;
		float dy = y1 - y0;

		// vertical line?
		if (dx == 0)
		{
			if (dy == 0)
				return;
			for (int32_t i = abs(dy); i >= 0; --i)
			{
				drawPixel(x0, y0 + i, op);
			}
		return;
		}

		// horizontal line?
		else if (dy == 0)
		{
			if (dx == 0)
				return;
			for (int32_t i = abs(dx); i >= 0; --i)
			{
				drawPixel(x0 + i, y0, op);
			}
			return;
		}

		// Quadrant #1
		if (dx > dy)
		{
			if (dx < 0)
			{
				swap(&x0, &x1);
				swap(&y0, &y1);
				dx = x1 - x0;
				dy = y1 - y0;
			}
			float step = dy / dx;
			float y = y0;
			for (int32_t x = x0; x <= x1; ++x)
			{
				drawPixel(x, y, op);
				y += step;
			}
		} else

		// Quadrant #2
		{
			if (dy < 0)
			{
				swap(&x0, &x1);
				swap(&y0, &y1);
				dx = x1 - x0;
				dy = y1 - y0;
			}
			float step = dx / dy;
			float x = x0;
			for (int32_t y = y0; y <= y1; ++y)
			{
				drawPixel(x, y, op);
				x += step;
			}
		}

	}

	//==============================================================================
	//          drawCircle()
	//
	//  CasualNoises    27/12/2024  First implementation
	//==============================================================================
	void drawCircle(int16_t x, int16_t y, int16_t r, eBitOperations op = eBitOperations::SetBitOp)
	{
	  int16_t f = 1 - r;
	  int16_t fx = 1;
	  int16_t fy = -2 * r;
	  int16_t ix = 0;
	  int16_t iy = r;

	  drawPixel(x, y + r, op);
	  drawPixel(x, y - r, op);
	  drawPixel(x + r, y, op);
	  drawPixel(x - r, y, op);

	  while (ix < iy) {

	    if (f >= 0) {
	      iy--;
	      fy += 2;
	      f += fy;
	    }

	    ix++;
	    fx += 2;
	    f += fx;

	    drawPixel(x + ix, y + iy, op);
	    drawPixel(x - ix, y + iy, op);
	    drawPixel(x + ix, y - iy, op);
	    drawPixel(x - ix, y - iy, op);
	    drawPixel(x + iy, y + ix, op);
	    drawPixel(x - iy, y + ix, op);
	    drawPixel(x + iy, y - ix, op);
	    drawPixel(x - iy, y - ix, op);
	  }
	}

	//==============================================================================
	//          drawCharacter()
	//
	//  CasualNoises    27/12/2024  First implementation
	//==============================================================================
	void drawCharacter(int32_t x, int32_t y, unsigned char c, const sFont* fontPtr,
							eBitOperations op = eBitOperations::SetBitOp)
	{
		if (c < ' ')
			c = ' ';
		if (c > '~')
			c = ' ';
		c -= ' ';

		uint32_t width  = fontPtr->width;
		uint32_t height = fontPtr->height;
		const uint16_t* pt = fontPtr->glyph + (height * c);
		for (uint32_t dy = 0; dy < height; ++dy)
		{
			uint16_t word = *pt++;
			for (uint32_t dx = 0; dx < width; ++dx)
			{
				if (word & 0x8000)
					drawPixel(x + dx, y, op);
				word <<= 1;
			}
			y += 1;
		}

	}

	//==============================================================================
	//          drawText()
	//
	//  CasualNoises    27/12/2024  First implementation
	//==============================================================================
	void drawText(int32_t x, int32_t y, const char* ptr, const sFont* fontPtr,
							eBitOperations op = eBitOperations::SetBitOp)
	{

		uint32_t len = strlen(ptr);
		uint32_t w = fontPtr->width;

		for (uint32_t i = 0; i < len; ++i)
		{
			drawCharacter(x, y, ptr[i], fontPtr, op);
			x += w;
		}

	}

	//==============================================================================
	//          drawString()
	//
	//  CasualNoises    29/12/2025  First implementation
	//==============================================================================
/*	void drawString(int32_t x, int32_t y, const String& string, const sFont* fontPtr,
							eBitOperations op = eBitOperations::SetBitOp)
	{

		uint16_t len = string.length();
		uint32_t w = fontPtr->width;

		for (uint32_t i = 0; i < len; ++i)
		{
//			drawCharacter(x, y, string[i], fontPtr, op);
			x += w;
		}

	}
*/
	//==============================================================================
	//          drawProgressBar()
	//
	//  CasualNoises    27/12/2024  First implementation
	//==============================================================================
	void drawProgressBar(float progress)
	{

		// Position and dimension
		uint32_t x = 10;
		uint32_t w = cDisplayWidth - (2 * x);
		uint32_t h = 18;
		uint32_t y = (cDisplayHeight / 2) - (h / 2);

		// Paint outer box
		fillRect(x, y, w, h);

		// Paint inner box
		x += 1;	y += 1;	w -= 2;	h -= 2;
		fillRect(x, y, w, h, eBitOperations::InvertBitOp);
		x += 1;	y += 1;	w -= 2; h -= 2;
		w = (w * progress) / 100.0f;
		fillRect(x, y, w, h, eBitOperations::InvertBitOp);

		// Paint progress text
		uint32_t p = progress;
		w = 24;
		if (p >= 10)
			w += 12;
		if (p >= 100)
			w += 12;
		x = (cDisplayWidth / 2) - (w / 2);
		if (p >= 100)
		{
			drawCharacter(x, y, '1', &CasualNoises::font_11x18, eBitOperations::InvertBitOp);
			x += 12;
			drawCharacter(x, y, '0', &CasualNoises::font_11x18, eBitOperations::InvertBitOp);
			x += 12;
			drawCharacter(x, y, '0', &CasualNoises::font_11x18, eBitOperations::InvertBitOp);
			x += 12;
		}
		else if (p >= 10)
		{
			uint32_t d = p / 10;
			drawCharacter(x, y, '0' + d, &CasualNoises::font_11x18, eBitOperations::InvertBitOp);
			x += 12;
			drawCharacter(x, y, '0' + p - (d * 10), &CasualNoises::font_11x18, eBitOperations::InvertBitOp);
			x += 12;
		}
		else
		{
			drawCharacter(x, y, '0' + p, &CasualNoises::font_11x18, eBitOperations::InvertBitOp);
			x += 12;
		}
		drawCharacter(x, y, '%', &CasualNoises::font_11x18, eBitOperations::InvertBitOp);

	}

	//==============================================================================
	//          drawWave()
	//
	// Draw a wave onto the display
	// No of samples in the wave should be equal to the width of the display
	//
	//  CasualNoises    27/12/2024  First implementation
	//==============================================================================
	void drawWave(const float* wavePtr)
	{
		for (uint32_t x = 0; x < cDisplayWidth; ++x)
		{
			float sample = wavePtr[x];
			uint32_t y = cn_map (sample, -1.0f, 1.0f, (float)cDisplayHeight, 0.0f);
			drawPixel(x, y);
		}
	}

	//==============================================================================
	//          drawWave()
	//
	// Draw a wave onto the display
	// No of samples in the wave should be equal to the width of the display
	//
	//  CasualNoises    28/12/2024  First implementation
	//==============================================================================
#ifdef USE_AUDIO_BUFFER
	void drawWave(const AudioBuffer& audioBuffer)
	{
		const float* rpt = audioBuffer.getReadPointer(0);
		drawWave(rpt);
	}
#endif

	//==============================================================================
	//          getDisplayWidth()
	//
	//  CasualNoises    27/12/2024  First implementation
	//==============================================================================
	uint32_t getDisplayWidth()
	{
		return cDisplayWidth;
	}

	//==============================================================================
	//          getDisplayHeight()
	//
	//  CasualNoises    27/12/2024  First implementation
	//==============================================================================
	uint32_t getDisplayHeight()
	{
		return cDisplayHeight;
	}

private:
	static constexpr int32_t	cDisplayWidth 		= 128;
	static constexpr int32_t	cDisplayHeight		= 64;
	static constexpr int32_t	cBitMapSize			= cDisplayWidth * cDisplayHeight;
	static constexpr int32_t	cBitMapBufferSize	= cBitMapSize / 8;

	sSSD1309_ConfigData*	mConfigDataPtr	{ nullptr };
	uint8_t					mBitMap[cDisplayWidth][cDisplayHeight];
	uint8_t					mBitMapBuffer[cBitMapBufferSize];

	//==============================================================================
	//          clipRect()
	//
	// Return true if there is a clipped result available
	//
	//  CasualNoises    27/12/2024  First implementation
	//==============================================================================
	bool clipRect(int32_t *x, int32_t *y, int32_t *w, int32_t *h)
	{

		//  1. rect origin beyond screen dimensions, nothing to draw
		if ((*x >= cDisplayWidth) || (*y >= cDisplayHeight))
			return false;

		//  2. rect width or height is 0, nothing to draw
		if ((*w == 0) || (*h == 0))
			return false;

		// 3. rect origin has negative component, adjust origin and dimensions
		if (*x < 0)
		{
			*w += *x;
			*x = 0;
		}
		if (*y < 0)
		{
			*h += *y;
			*y = 0;
		}

	    //  4. rect width beyond screen width, reduce rect width
	    if ((*x + *w - 1) >= cDisplayWidth)
	    {
	    	*w = cDisplayWidth - *x;
	    }

	    //  5. rect height beyond screen height, reduce rect height
	    if ((*y + *h - 1) >= cDisplayHeight)
	    {
	    	*h = cDisplayHeight - *y;
	    }

	    return true;

	}

	//==============================================================================
	//          swap()
	//
	//  CasualNoises    25/12/2024  First implementation
	//==============================================================================
	void swap(int32_t* p1, int32_t* p2)
	{
		int32_t tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
	}

	//==============================================================================
	//          abs()
	//
	//  CasualNoises    25/12/2024  First implementation
	//==============================================================================
	int32_t abs(int32_t v)
	{
		if (v > 0)
			return v;
		else
			return 0 - v;
	}

	//==============================================================================
	//          sendCommand() 1 byte command
	//
	//  CasualNoises    25/12/2024  First implementation
	//==============================================================================
	HAL_StatusTypeDef sendCommand(uint8_t cmd)
	{
		  HAL_GPIO_WritePin(mConfigDataPtr->CS_PORT, mConfigDataPtr->CS_PIN, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(mConfigDataPtr->DC_PORT, mConfigDataPtr->DC_PIN, GPIO_PIN_RESET);
		  HAL_StatusTypeDef res;
		  res = HAL_SPI_Transmit(mConfigDataPtr->SPI, &cmd, 1, HAL_MAX_DELAY);
		  HAL_GPIO_WritePin(mConfigDataPtr->DC_PORT, mConfigDataPtr->DC_PIN, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(mConfigDataPtr->CS_PORT, mConfigDataPtr->CS_PIN, GPIO_PIN_SET);
		  return res;
	}

	//==============================================================================
	//          sendCommand() 2 byte command
	//
	//  CasualNoises    25/12/2024  First implementation
	//==============================================================================
	HAL_StatusTypeDef sendCommand(uint8_t cmd1, uint8_t cmd2)
	{
		  HAL_GPIO_WritePin(mConfigDataPtr->CS_PORT, mConfigDataPtr->CS_PIN, GPIO_PIN_RESET);
		  HAL_GPIO_WritePin(mConfigDataPtr->DC_PORT, mConfigDataPtr->DC_PIN, GPIO_PIN_RESET);
		  HAL_StatusTypeDef res;
		  res = HAL_SPI_Transmit(mConfigDataPtr->SPI, &cmd1, 1, HAL_MAX_DELAY);
		  res = HAL_SPI_Transmit(mConfigDataPtr->SPI, &cmd2, 1, HAL_MAX_DELAY);
		  HAL_GPIO_WritePin(mConfigDataPtr->DC_PORT, mConfigDataPtr->DC_PIN, GPIO_PIN_SET);
		  HAL_GPIO_WritePin(mConfigDataPtr->CS_PORT, mConfigDataPtr->CS_PIN, GPIO_PIN_SET);
		  return res;
	}

};

} // namespace CasualNoises

#endif
