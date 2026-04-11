/*
  ==============================================================================

    enumClassUtilities.h
    Created: 10/04/2026

    Author:  Dirk Tjantele

  ==============================================================================
*/

#pragma once

// Get next element in an enum class object,
//   go to the first element when the end of the enum is reached
// Last entry in the enum should be 'Count'
template <typename E>
constexpr E next(E e) {
    static_assert(std::is_enum_v<E>);
    using T = std::underlying_type_t<E>;

    T value = static_cast<T>(e);
    value++;
    if (value == static_cast<T>(E::Count))
        value = 0;

    return static_cast<E>(value);
}

