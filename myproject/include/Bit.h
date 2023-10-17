#pragma once
#include "TypeTraits.h"
#include "Limits.h"


CUSTOM_BEGIN

// TODO: implement

enum class Endian
{
#if defined _MSC_VER
    little = 0,
    big    = 1,
    native = little
#elif defined __GNUG__
    little = __ORDER_LITTLE_ENDIAN__,
    big    = __ORDER_BIG_ENDIAN__,
    native = __BYTE_ORDER__
#endif
};


template<class To, class From,
EnableIf_t<Conjunction_v<   BoolConstant<sizeof(To) == sizeof(From)>,
                            IsTriviallyCopyable<To>,
                            IsTriviallyCopyable<From>>, bool> = true>
constexpr To bit_cast(const From& src) noexcept {
    return __builtin_bit_cast(To, src);
}


template<class Ty, EnableIf_t<IsUnsignedInteger_v<Ty>, bool> = true>
constexpr bool has_single_bit(const Ty val) noexcept {
    return val != 0 && (val & (val - 1)) == 0;
}


// Implementation of countl_zero without using specialized CPU instructions.
// Used at compile time and when said instructions are not supported.
// see "Hacker's Delight" section 5-3
template<class Ty, EnableIf_t<IsUnsignedInteger_v<Ty>, bool> = true>
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
template<class Ty, EnableIf_t<IsUnsignedInteger_v<Ty>, bool> = true>
constexpr int countr_zero(const Ty val) noexcept {
    constexpr int digits = NumericLimits<Ty>::Digits;
    return digits - countl_zero(static_cast<Ty>(static_cast<Ty>(~val) & static_cast<Ty>(val - 1)));
}


template<class Ty, EnableIf_t<IsUnsignedInteger_v<Ty>, bool> = true>
constexpr int countl_one(const Ty val) noexcept {
    return custom::countl_zero(static_cast<Ty>(~val));
}


template<class Ty, EnableIf_t<IsUnsignedInteger_v<Ty>, bool> = true>
constexpr int countr_one(const Ty val) noexcept {
    return custom::countr_zero(static_cast<Ty>(~val));
}


template<class Ty, EnableIf_t<IsUnsignedInteger_v<Ty>, bool> = true>
constexpr Ty bit_ceil(const Ty val) noexcept {
    if (val <= 1u)
        return Ty{1};

    const int shiftExponent = NumericLimits<Ty>::Digits - custom::countl_zero(static_cast<Ty>(val - 1));

    if constexpr (sizeof(Ty) < sizeof(unsigned int)) // for types subject to integral promotion
        if (custom::is_constant_evaluated())
        {
            // If the shift exponent equals number of digits then the correct result is not
            // representable as a value of Ty, and so the result is undefined.
            CUSTOM_ASSERT(shiftExponent != NumericLimits<Ty>::Digits, "Undefined result");
        }

    return static_cast<Ty>(Ty{1} << shiftExponent);
}


template<class Ty, EnableIf_t<IsUnsignedInteger_v<Ty>, bool> = true>
constexpr Ty bit_floor(const Ty val) noexcept {
    if (val == 0)
        return 0;

    return static_cast<Ty>(Ty{1} << (NumericLimits<Ty>::Digits - 1 - custom::countl_zero(val)));
}


template<class Ty, EnableIf_t<IsUnsignedInteger_v<Ty>, bool> = true>
constexpr int bit_width(const Ty val) noexcept {
    return NumericLimits<Ty>::Digits - custom::countl_zero(val);
}


template<class Ty, EnableIf_t<IsUnsignedInteger_v<Ty>, bool> = true>
constexpr Ty rotr(const Ty val, const int rotatio) noexcept; // not implemented


template<class Ty, EnableIf_t<IsUnsignedInteger_v<Ty>, bool> = true>
constexpr Ty rotl(const Ty val, const int rotatio) noexcept; // not implemented


template<class Ty, EnableIf_t<IsUnsignedInteger_v<Ty>, bool> = true>
constexpr int popcount(const Ty val) noexcept; // not implemented

CUSTOM_END