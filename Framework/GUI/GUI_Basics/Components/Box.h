/*
  ==============================================================================

    Box.h
    Created: 29/12/2025

    Paints a box around other components

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include "Component.h"

namespace CasualNoises
{

class Box final : public Component
{
public:
	 Box() = delete;
	 Box(String name);
	~Box() = default;

	void paint(Graphics& g) noexcept override;

	bool handleUI_event(sIncommingUI_Event* uiEvent, bool altState, Graphics& g) override { return false; }

private:

};

} // namespace CasualNoises
