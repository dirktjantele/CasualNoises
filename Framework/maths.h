/*
  ==============================================================================

    maths.h
    Created: 4 dec 2024
    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

#include <limits>

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
// NOTE: try to avoid cn_map, it takes a lot of cpu time!!!
template <typename Type>
Type cn_map (Type sourceValue, Type sourceRangeMin, Type sourceRangeMax, Type targetRangeMin, Type targetRangeMax)
{
    assert (! approximatelyEqual (sourceRangeMax, sourceRangeMin)); // mapping from a range of zero will produce NaN!
    return targetRangeMin + ((targetRangeMax - targetRangeMin) * (sourceValue - sourceRangeMin)) / (sourceRangeMax - sourceRangeMin);
}

/** Limits a value to a given range. */
template <typename Type>
Type cn_limit (Type sourceValue, Type rangeMin, Type rangeMax)
{
	if (sourceValue < rangeMin)
		sourceValue = rangeMin;
	else if (sourceValue > rangeMax)
		sourceValue = rangeMax;
	return sourceValue;
}

/** Equivalent to operator== */
template <typename Type>
constexpr bool exactlyEqual (Type a, Type b)
{
//    JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wfloat-equal")
    return a == b;
//    JUCE_END_IGNORE_WARNINGS_GCC_LIKE
}

/** The isfinite() method seems to vary between platforms, so this is a
    platform-independent function for it.
*/
/*
template <typename NumericType>
bool isfinite (NumericType value) noexcept
{
    if constexpr (std::numeric_limits<NumericType>::has_infinity
                  || std::numeric_limits<NumericType>::has_quiet_NaN
                  || std::numeric_limits<NumericType>::has_signaling_NaN)
    {
        return std::isfinite (value);
    }
    else
    {
        UNUSED(value);
        return true;
    }
}
*/

} // namespace CasualNoises
