/*
  ==============================================================================

    LinkwitzRileyFilter.h
    Created: 28 jul 2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

namespace CasualNoises {
namespace Filter {

class LinkwitzRileyFilter final : public IIRFilterBase
{

private:
	void calculateLPF() override;
	void calculateHPF() override;
	void calculateBPF() {};
	void calculateBSF() {};

};

} // namespace Filter
} // namespace CasualNoises
