/*
 * Font.h
 *
 *  Created on: Dec 27, 2024
 *      Author: dirktjantele
 */

#pragma once

#include "main.h"

namespace CasualNoises
{

typedef struct
{
    const uint8_t width;
    const uint8_t height;
    const uint16_t glyph[];
}
sFont;

extern sFont const font_7x10;
extern sFont const font_11x18;
extern sFont const font_16x26;

}; // namespace CasualNoises
