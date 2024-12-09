/*
  ==============================================================================

    maths.h
    Created: 4 dec 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

namespace CasualNoises
{

constexpr float pi  = 3.14159265358979323846f;

/*
 * Imported from the juce framework
 */

// Returns true if the two floating-point numbers are approximately equal.
template <typename Type>
constexpr bool approximatelyEqual (Type a, Type b)
{
    const auto diff = std::abs (a - b);
    return diff < 0.0000000001;
}

/** Re-maps a value from a source range to a target range. */
template <typename Type>
Type cn_map (Type sourceValue, Type sourceRangeMin, Type sourceRangeMax, Type targetRangeMin, Type targetRangeMax)
{
    assert (! approximatelyEqual (sourceRangeMax, sourceRangeMin)); // mapping from a range of zero will produce NaN!
    return targetRangeMin + ((targetRangeMax - targetRangeMin) * (sourceValue - sourceRangeMin)) / (sourceRangeMax - sourceRangeMin);
}

} // namespace CasualNoises
