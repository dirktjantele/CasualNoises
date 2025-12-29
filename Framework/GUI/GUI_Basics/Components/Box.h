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

	void paint(Graphics& g) noexcept;

private:

};

} // namespace CasualNoises
