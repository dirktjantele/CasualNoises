/*
  ==============================================================================

    Colour.h
    Created: 19 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

namespace CasualNoises
{

enum class colour
{
	BLACK        = 0x0000,
	NAVY         = 0x000F,
	DARKGREEN    = 0x03E0,
	DARKCYAN     = 0x03EF,
	MAROON       = 0x7800,
	PURPLE       = 0x780F,
	OLIVE        = 0x7BE0,
	LIGHTGREY    = 0xC618,
	DARKGREY     = 0x7BEF,
	BLUE         = 0x001F,
	GREEN        = 0x07E0,
	CYAN         = 0x07FF,
	RED          = 0xF800,
	MAGENTA      = 0xF81F,
	YELLOW       = 0xFFE0,
	WHITE        = 0xFFFF,
	ORANGE       = 0xFD20,
	GREENYELLOW  = 0xAFE5,
	PINK         = 0xF81F
};

class Colour
{
public:
	Colour() = delete;
};

} // namespace CasualNoises
