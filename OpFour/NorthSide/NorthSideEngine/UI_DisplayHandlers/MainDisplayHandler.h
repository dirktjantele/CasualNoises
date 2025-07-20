/*
  ==============================================================================

    MainDispalyHandler.h
    Created: 20/07/2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "DisplayHandlerRoot.h"

namespace CasualNoises
{

class MainDisplayHandler : public DisplayHandlerRoot
{
public:

	 MainDisplayHandler() = default;
	~MainDisplayHandler() = default;

	void openDisplayPage(CasualNoises::SSD1309_Driver* oledDriverPtr) override;

private:

};

} // namespace CasualNoises
