/*
 * IIRFilterBase.cpp
 *
 *  Created on: Jul 28, 2025
 *      Author: dirktjantele
 */

#include "IIRFilterBase.h"

namespace CasualNoises {
namespace Filter {

void IIRFilterBase::updateFilterCoefficients(float cutoff, float q, float sampleRate, IIRType filterType)
{
	this->cutoff = cutoff;
	this->q = q;
	this->sampleRate = sampleRate;

	switch (filterType)
	{
	case IIRType::LPF:
		calculateLPF();
		break;
	case IIRType::HPF:
		calculateHPF();
		break;
	case IIRType::BPF:
		calculateBPF();
		break;
	case IIRType::BSF:
		calculateBSF();
		break;
	}
}

float IIRFilterBase::applyFiltering(float xn, ChannelFeed& c)
{
	float yn = a0 * xn + a1 * c.xn_1 + a2 * c.xn_2 - b1 * c.yn_1 - b2 * c.yn_2;

	c.xn_2 = c.xn_1;
	c.xn_1 = xn;

	c.yn_2 = c.yn_1;
	c.yn_1 = yn;

	return yn;
}

} // namespace Filter
} // namespace CasualNoises



