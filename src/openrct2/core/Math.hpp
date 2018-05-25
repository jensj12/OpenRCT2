#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include "../common.h"

#include <algorithm>
#include <limits>
#include <cmath>
#include <type_traits>

/**
 * Common mathematical functions.
 */
namespace Math
{
    template<typename T>
    static T Min(T a, T b)
    {
        return (std::min)(a, b);
    }

    template<typename T>
    static T Max(T a, T b)
    {
        return (std::max)(a, b);
    }

    template<typename T>
    static T Clamp(T low, T x, T high)
    {
        return (std::min)((std::max)(low, x), high);
    }

    template<typename T>
    static T Sign(T x)
    {
        if (x < 0) return -1;
        if (x > 0) return 1;
        return 0;
    }

    // Taken from http://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
    template<class T>
    typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type AlmostEqual(T x, T y, sint32 ulp = 20)
    {
        // the machine epsilon has to be scaled to the magnitude of the values used
        // and multiplied by the desired precision in ULPs (units in the last place)
        return std::abs(x - y) <= std::numeric_limits<T>::epsilon() * std::abs(x + y) * ulp
            // unless the result is subnormal
            || std::abs(x - y) < (std::numeric_limits<T>::min)(); // TODO: Remove parentheses around min once the macro is removed
    }
} // namespace Math
