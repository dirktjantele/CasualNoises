/*
LinkwitzRileyFilter *
 *  Created on: Jul 28, 2025
 *      Author: dirktjantele
 */

#include "IIRFilters.h"

#include <arm_math.h>
#include "../../maths.h"

#include "IIRFilters.h"

namespace CasualNoises {
namespace Filter {

//#include "Core/Maths/MathsFunctions.h"

void LinkwitzRileyFilter::calculateLPF()
{
	float teta = pi * cutoff / sampleRate;
	float omega = pi * cutoff;
	float k = omega / std::tan(teta);
	float k_square = std::pow(k, 2.0f);
	float omega_square = std::pow(omega, 2.0f);
	float sigma = k_square + omega_square + 2.0f * k * omega;

	a0 = omega_square / sigma;
	a1 = 2.0f * a0;
	a2 = a0;
	b1 = (-2.0f * k_square + 2.0f * omega_square) / sigma;
	b2 = (-2.0f * k * omega + k_square + omega_square) / sigma;
}

void LinkwitzRileyFilter::calculateHPF()
{
	float teta = pi * cutoff / sampleRate;
	float omega = pi * cutoff;
	float k = omega / std::tan(teta);
	float k_square = std::pow(k, 2.0f);
	float omega_square = std::pow(omega, 2.0f);
	float sigma = k_square + omega_square + 2.0f * k * omega;

	a0 = k_square / sigma;
	a1 = -2.0f * a0;
	a2 = a0;
	b1 = (-2.0f * k_square + 2.0f * omega_square) / sigma;
	b2 = (-2.0f * k * omega + k_square + omega_square) / sigma;
}

} // namespace Filter
} // namespace CasualNoises
