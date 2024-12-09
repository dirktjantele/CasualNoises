/*
  ==============================================================================

    WaveView.cpp
    Created: 18 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

// 23/1/2024 updated in order to be able to use the ILI9341 in parallel mode (8080 mode)
#ifdef USE_ILI9341_SERIAL_MODE

#include "WaveView.h"

namespace CasualNoises
{

//==============================================================================
//          paint()
//
//  CasualNoises    19/08/2023  First implementation
//==============================================================================
void WaveView::paint(Graphics& g)
{
	g.setColour(colour::WHITE);
	g.fillRect(mX, mY, mWidth, mHeight);
}

} // namespace CasualNoises

#endif
