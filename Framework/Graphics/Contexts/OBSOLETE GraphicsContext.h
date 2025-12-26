/*
  ==============================================================================

    GraphicsContext.h
    Created: 19 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

// 23/1/2024 updated in order to be able to use the ILI9341 in parallel mode (8080 mode)
#ifdef USE_ILI9341_SERIAL_MODE

#pragma once

#include "ili9341.h"

#include "../Colour/Colour.h"

namespace CasualNoises
{

class Graphics final
{
public:
	 Graphics() = delete;
	~Graphics() = default;

	Graphics(ili9341_t* screen);

	void setColour(	colour 	newColour ) { mColor = newColour; }

	void fillAll() const;
	void fillRect(int x, int y, int width, int height) const;

private:

	ili9341_t* 	mScreen 	{ nullptr };
	colour		mColor		{ colour::LIGHTGREY };

};

} // namespace CasualNoises

#endif
