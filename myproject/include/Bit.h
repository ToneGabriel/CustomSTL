#pragma once
#include "TypeTraits.h"
#include "Limits.h"


CUSTOM_BEGIN

// TODO: implement

// Implementation of countl_zero without using specialized CPU instructions.
// Used at compile time and when said instructions are not supported.
// see "Hacker's Delight" section 5-3
template<class Ty>
constexpr int countl_zero(Ty val) noexcept {
    Ty yy = 0;

    unsigned int nn = NumericLimits<Ty>::Digits;
    unsigned int cc = NumericLimits<Ty>::Digits / 2;

    do
    {
        yy = static_cast<Ty>(val >> cc);
        
        if (yy != 0)
        {
            nn -= cc;
            val = yy;
        }

        cc >>= 1;
    } while (cc != 0);

    return static_cast<int>(nn) - static_cast<int>(val);
}

// Implementation of countr_zero without using specialized CPU instructions.
// Used at compile time and when said instructions are not supported.
// see "Hacker's Delight" section 5-4
template<class Ty>
constexpr int countr_zero(const Ty val) noexcept {
    constexpr int digits = NumericLimits<Ty>::Digits;
    return digits - countl_zero(static_cast<Ty>(static_cast<Ty>(~val) & static_cast<Ty>(val - 1)));
}

CUSTOM_END