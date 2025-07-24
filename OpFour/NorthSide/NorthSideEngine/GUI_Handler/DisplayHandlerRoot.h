/*
  ==============================================================================

    DisplayHandlerRoot.h
    Created: 20/07/2025

    Abstract class where all page handlers derive from

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "CasualNoises.h"

namespace CasualNoises
{

class DisplayHandlerRoot
{
public:

	 DisplayHandlerRoot() = default;
	~DisplayHandlerRoot() = default;

	virtual void openDisplayPage(CasualNoises::SSD1309_Driver* oledDriverPtr) = 0;

private:

};

} // namespace CasualNoises
