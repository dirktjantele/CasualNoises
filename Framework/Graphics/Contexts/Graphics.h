/*
  ==============================================================================

    Graphics.h
    Created: 26/12/2025

    Author:  Dirk Tjantele

  ==============================================================================
*/

#ifdef CASUALNOISES_DISPLAY_DRIVER

#pragma once

#include "../../Drivers/OLED/SSD1309_Driver.h"

#include "../Colour/Colour.h"

namespace CasualNoises
{

class SSD1309_Driver;

class Graphics final
{
public:

	 Graphics() = delete;
	~Graphics() = default;

	Graphics(SSD1309_Driver* screen);
	SSD1309_Driver* getScreen() noexcept { return mScreen; }

	void setColour (colour 	newColour ) { mColor = newColour; }
	void refreshDisplay()				{ mScreen->refreshDisplay(); }

	void fillAll ( eBitOperations op = eBitOperations::ClearBitOp )  noexcept;
	void fillRect ( int x, int y, int width, int height, eBitOperations op = eBitOperations::SetBitOp ) noexcept;
	void fillRect ( Rectangle<int> rect, eBitOperations op = eBitOperations::SetBitOp ) noexcept;

private:

	SSD1309_Driver* 	mScreen 	{ nullptr };
	colour				mColor		{ colour::LIGHTGREY };

};

} // namespace CasualNoises

#endif
