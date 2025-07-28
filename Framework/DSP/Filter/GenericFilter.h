/*
  ==============================================================================

    GenericFilter.h
    Created: 28 jul 2025
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

namespace CasualNoises {
namespace Filter {

class GenericFilter: public IIRFilterBase
{

private:
	void calculateLPF() override;
	void calculateHPF() override;
	void calculateBPF() override;
	void calculateBSF() override;
};

} // namespace Filter
} // namespace CasualNoises
