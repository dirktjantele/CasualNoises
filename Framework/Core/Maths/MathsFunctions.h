/*
  ==============================================================================

    MathsFunctions.h
    Created: 30 mar 2024
    Author:  Dirk Tjantele

    Used Juce as source for this stuff!!

  ==============================================================================
*/

/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2022 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

#pragma once

#include <arm_math.h>

namespace CasualNoises
{

/** Commonly used mathematical constants */
template <typename FloatType>
struct MathConstants
{
    /** A predefined value for Pi */
    static constexpr FloatType pi = static_cast<FloatType> (3.141592653589793238L);

    /** A predefined value for 2 * Pi */
    static constexpr FloatType twoPi = static_cast<FloatType> (2 * 3.141592653589793238L);

    /** A predefined value for Pi / 2 */
    static constexpr FloatType halfPi = static_cast<FloatType> (3.141592653589793238L / 2);

    /** A predefined value for Euler's number */
    static constexpr FloatType euler = static_cast<FloatType> (2.71828182845904523536L);

    /** A predefined value for sqrt (2) */
    static constexpr FloatType sqrt2 = static_cast<FloatType> (1.4142135623730950488L);
};

/** Converts an angle in degrees to radians. */
template <typename FloatType>
constexpr FloatType degreesToRadians (FloatType degrees) noexcept     { return degrees * (MathConstants<FloatType>::pi / FloatType (180)); }

/** Converts an angle in radians to degrees. */
template <typename FloatType>
constexpr FloatType radiansToDegrees (FloatType radians) noexcept     { return radians * (FloatType (180) / MathConstants<FloatType>::pi); }

/** Returns the larger of two values. */
template <typename Type>
constexpr Type jmax (Type a, Type b)                                   { return a < b ? b : a; }

/** Returns the larger of three values. */
template <typename Type>
constexpr Type jmax (Type a, Type b, Type c)                           { return a < b ? (b < c ? c : b) : (a < c ? c : a); }

/** Returns the larger of four values. */
template <typename Type>
constexpr Type jmax (Type a, Type b, Type c, Type d)                   { return jmax (a, jmax (b, c, d)); }

/** Returns the smaller of two values. */
template <typename Type>
constexpr Type jmin (Type a, Type b)                                   { return b < a ? b : a; }

/** Returns the smaller of three values. */
template <typename Type>
constexpr Type jmin (Type a, Type b, Type c)                           { return b < a ? (c < b ? c : b) : (c < a ? c : a); }

/** Returns the smaller of four values. */
template <typename Type>
constexpr Type jmin (Type a, Type b, Type c, Type d)                   { return jmin (a, jmin (b, c, d)); }

/** Remaps a normalised value (between 0 and 1) to a target range.
    This effectively returns (targetRangeMin + value0To1 * (targetRangeMax - targetRangeMin)).
*/
template <typename Type>
constexpr Type jmap (Type value0To1, Type targetRangeMin, Type targetRangeMax)
{
    return targetRangeMin + value0To1 * (targetRangeMax - targetRangeMin);
}

/** Remaps a value from a source range to a target range. */
template <typename Type>
Type jmap (Type sourceValue, Type sourceRangeMin, Type sourceRangeMax, Type targetRangeMin, Type targetRangeMax)
{
//    assert (! approximatelyEqual (sourceRangeMax, sourceRangeMin)); // mapping from a range of zero will produce NaN!
	assert (sourceRangeMax != sourceRangeMin);
    return targetRangeMin + ((targetRangeMax - targetRangeMin) * (sourceValue - sourceRangeMin)) / (sourceRangeMax - sourceRangeMin);
}

/** Remaps a normalised value (between 0 and 1) to a logarithmic target range.

    The entire target range must be greater than zero.

    @see mapFromLog10

    @code
    mapToLog10 (0.5, 0.4, 40.0) == 4.0
    @endcode
*/
template <typename Type>
Type mapToLog10 (Type value0To1, Type logRangeMin, Type logRangeMax)
{
    assert (logRangeMin > 0);
    assert (logRangeMax > 0);

    auto logMin = std::log10 (logRangeMin);
    auto logMax = std::log10 (logRangeMax);

    return std::pow ((Type) 10.0, value0To1 * (logMax - logMin) + logMin);
}

/** Remaps a logarithmic value in a target range to a normalised value (between 0 and 1).

    The entire target range must be greater than zero.

    @see mapToLog10

    @code
    mapFromLog10 (4.0, 0.4, 40.0) == 0.5
    @endcode
*/
template <typename Type>
Type mapFromLog10 (Type valueInLogRange, Type logRangeMin, Type logRangeMax)
{
    assert (logRangeMin > 0);
    assert (logRangeMax > 0);

    auto logMin = std::log10 (logRangeMin);
    auto logMax = std::log10 (logRangeMax);

    return (std::log10 (valueInLogRange) - logMin) / (logMax - logMin);
}

/** Scans an array of values, returning the minimum value that it contains. */
template <typename Type, typename Size>
Type findMinimum (const Type* data, Size numValues)
{
    if (numValues <= 0)
        return Type (0);

    auto result = *data++;

    while (--numValues > 0) // (> 0 rather than >= 0 because we've already taken the first sample)
    {
        auto v = *data++;

        if (v < result)
            result = v;
    }

    return result;
}

/** Scans an array of values, returning the maximum value that it contains. */
template <typename Type, typename Size>
Type findMaximum (const Type* values, Size numValues)
{
    if (numValues <= 0)
        return Type (0);

    auto result = *values++;

    while (--numValues > 0) // (> 0 rather than >= 0 because we've already taken the first sample)
    {
        auto v = *values++;

        if (result < v)
            result = v;
    }

    return result;
}

/** Scans an array of values, returning the minimum and maximum values that it contains. */
template <typename Type>
void findMinAndMax (const Type* values, int numValues, Type& lowest, Type& highest)
{
    if (numValues <= 0)
    {
        lowest = Type (0);
        highest = Type (0);
    }
    else
    {
        auto mn = *values++;
        auto mx = mn;

        while (--numValues > 0) // (> 0 rather than >= 0 because we've already taken the first sample)
        {
            auto v = *values++;

            if (mx < v)  mx = v;
            if (v < mn)  mn = v;
        }

        lowest = mn;
        highest = mx;
    }
}

/** Constrains a value to keep it within a given range.

    This will check that the specified value lies between the lower and upper bounds
    specified, and if not, will return the nearest value that would be in-range. Effectively,
    it's like calling jmax (lowerLimit, jmin (upperLimit, value)).

    Note that it expects that lowerLimit <= upperLimit. If this isn't true,
    the results will be unpredictable.

    @param lowerLimit           the minimum value to return
    @param upperLimit           the maximum value to return
    @param valueToConstrain     the value to try to return
    @returns    the closest value to valueToConstrain which lies between lowerLimit
                and upperLimit (inclusive)
    @see jmin, jmax, jmap
*/
template <typename Type>
Type jlimit (Type lowerLimit,
             Type upperLimit,
             Type valueToConstrain) noexcept
{
    assert (lowerLimit <= upperLimit); // if these are in the wrong order, results are unpredictable..

    return valueToConstrain < lowerLimit ? lowerLimit
                                         : (upperLimit < valueToConstrain ? upperLimit
                                                                          : valueToConstrain);
}

/** Returns true if a value is at least zero, and also below a specified upper limit.
    This is basically a quicker way to write:
    @code valueToTest >= 0 && valueToTest < upperLimit
    @endcode
*/
template <typename Type1, typename Type2>
bool isPositiveAndBelow (Type1 valueToTest, Type2 upperLimit) noexcept
{
    assert (Type1() <= static_cast<Type1> (upperLimit)); // makes no sense to call this if the upper limit is itself below zero..
    return Type1() <= valueToTest && valueToTest < static_cast<Type1> (upperLimit);
}

template <typename Type>
bool isPositiveAndBelow (int valueToTest, Type upperLimit) noexcept
{
    assert (upperLimit >= 0); // makes no sense to call this if the upper limit is itself below zero..
    return static_cast<unsigned int> (valueToTest) < static_cast<unsigned int> (upperLimit);
}

/** Returns true if a value is at least zero, and also less than or equal to a specified upper limit.
    This is basically a quicker way to write:
    @code valueToTest >= 0 && valueToTest <= upperLimit
    @endcode
*/
template <typename Type1, typename Type2>
bool isPositiveAndNotGreaterThan (Type1 valueToTest, Type2 upperLimit) noexcept
{
    assert (Type1() <= static_cast<Type1> (upperLimit)); // makes no sense to call this if the upper limit is itself below zero..
    return Type1() <= valueToTest && valueToTest <= static_cast<Type1> (upperLimit);
}

template <typename Type>
bool isPositiveAndNotGreaterThan (int valueToTest, Type upperLimit) noexcept
{
    assert (upperLimit >= 0); // makes no sense to call this if the upper limit is itself below zero..
    return static_cast<unsigned int> (valueToTest) <= static_cast<unsigned int> (upperLimit);
}

/** Computes the absolute difference between two values and returns true if it is less than or equal
    to a given tolerance, otherwise it returns false.
*/
template <typename Type>
bool isWithin (Type a, Type b, Type tolerance) noexcept
{
    return std::abs (a - b) <= tolerance;
}

/** Fast floating-point-to-integer conversion.

    This is faster than using the normal c++ cast to convert a float to an int, and
    it will round the value to the nearest integer, rather than rounding it down
    like the normal cast does.

    Note that this routine gets its speed at the expense of some accuracy, and when
    rounding values whose floating point component is exactly 0.5, odd numbers and
    even numbers will be rounded up or down differently.
*/
template <typename FloatType>
int roundToInt (const FloatType value) noexcept
{
    union { int asInt[2]; double asDouble; } n;
    n.asDouble = ((double) value) + 6755399441055744.0;

   #if BIG_ENDIAN
    return n.asInt [1];
   #else
    return n.asInt [0];
   #endif
}

inline int roundToInt (int value) noexcept
{
    return value;
}

/** Fast floating-point-to-integer conversion.

    This is a slightly slower and slightly more accurate version of roundToInt(). It works
    fine for values above zero, but negative numbers are rounded the wrong way.
*/
inline int roundToIntAccurate (double value) noexcept
{
    return roundToInt (value + 1.5e-8);
}

/** Truncates a positive floating-point number to an unsigned int.

    This is generally faster than static_cast<unsigned int> (std::floor (x))
    but it only works for positive numbers small enough to be represented as an
    unsigned int.
*/
template <typename FloatType>
unsigned int truncatePositiveToUnsignedInt (FloatType value) noexcept
{
    assert (value >= static_cast<FloatType> (0));
 //    assert (static_cast<FloatType> (value)
 //            <= static_cast<FloatType> (std::numeric_limits<unsigned int>::max()));
    return static_cast<unsigned int> (value);
}

/** Returns true if the specified integer is a power-of-two. */
template <typename IntegerType>
constexpr bool isPowerOfTwo (IntegerType value)
{
   return (value & (value - 1)) == 0;
}

/** Returns the smallest power-of-two which is equal to or greater than the given integer. */
inline int nextPowerOfTwo (int n) noexcept
{
    --n;
    n |= (n >> 1);
    n |= (n >> 2);
    n |= (n >> 4);
    n |= (n >> 8);
    n |= (n >> 16);
    return n + 1;
}

/** Returns the index of the highest set bit in a (non-zero) number.
    So for n=3 this would return 1, for n=7 it returns 2, etc.
    An input value of 0 is illegal!
*/
int findHighestSetBit (uint32_t n) noexcept;

/** Returns the number of bits in a 32-bit integer. */
constexpr int countNumberOfBits (uint32_t n) noexcept
{
    n -= ((n >> 1) & 0x55555555);
    n =  (((n >> 2) & 0x33333333) + (n & 0x33333333));
    n =  (((n >> 4) + n) & 0x0f0f0f0f);
    n += (n >> 8);
    n += (n >> 16);
    return (int) (n & 0x3f);
}

/** Returns the number of bits in a 64-bit integer. */
constexpr int countNumberOfBits (uint64_t n) noexcept
{
    return countNumberOfBits ((uint32_t) n) + countNumberOfBits ((uint32_t) (n >> 32));
}

/** Performs a modulo operation, but can cope with the dividend being negative.
    The divisor must be greater than zero.
*/
template <typename IntegerType>
IntegerType negativeAwareModulo (IntegerType dividend, const IntegerType divisor) noexcept
{
    jassert (divisor > 0);
    dividend %= divisor;
    return (dividend < 0) ? (dividend + divisor) : dividend;
}

/** Returns the square of its argument. */
template <typename NumericType>
inline constexpr NumericType square (NumericType n) noexcept
{
    return n * n;
}

}
