/*
  ==============================================================================

    WaveView.h
    Created: 18 aug 2023
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

// 23/1/2024 updated in order to be able to use the ILI9341 in parallel mode (8080 mode)
#ifdef USE_ILI9341_SERIAL_MODE

#include "../GUI_Basics/Components/Component.h"

namespace CasualNoises
{

class WaveView final : public Component
{
public:
	 WaveView() = default;
	~WaveView() = default;

	void	paint(Graphics& g) override;

private:

};

} // namespace CasualNoises

#endif
