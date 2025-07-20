/*
  ==============================================================================

    MainDisplayHandler.cpp

    Display the main GUI page and handles events

    Created: 20/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#include <UI_DisplayHandlers/MainDisplayHandler.h>

namespace CasualNoises
{

void MainDisplayHandler::openDisplayPage(CasualNoises::SSD1309_Driver* oledDriverPtr)
{
	oledDriverPtr->clearDisplay();
}

} // namespace CasualNoises


