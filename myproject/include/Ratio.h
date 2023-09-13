#pragma once
#include "TypeTraits.h"

#include <cstdint>


CUSTOM_BEGIN

// helpers
constexpr intmax_t _abs(const intmax_t val) noexcept {
    return val < 0 ? -val : val;
}

constexpr intmax_t _sign(const intmax_t val) noexcept {
    return val < 0 ? -1 : 1;
}

constexpr intmax_t _gcd(intmax_t first, intmax_t second) noexcept {  // computes GCD of abs(first) and abs(second)
    if (first == 0 && second == 0)
        return 1; // contrary to mathematical convention; avoids division by 0 in RatioLess

    first   = _abs(first);
    second  = _abs(second);

    while (second != 0)
    {
        auto aux    = first;
        first       = second;
        second      = aux % second;
    }

    return first;
}

inline void _safe_add_integer_arithmetic_overflow_error() noexcept {}   // TODO: what is this?

constexpr intmax_t _safe_add(const intmax_t first, const intmax_t second) noexcept {
    if (_sign(first) == _sign(second) && _abs(first) > INTMAX_MAX - _abs(second))
        _safe_add_integer_arithmetic_overflow_error();

    return first + second;
}

struct _BigUint128
{
    uint64_t Upper;
    uint64_t Lower;

    constexpr bool operator<(const _BigUint128 other) const noexcept {
        if (Upper != other.Upper)
            return Upper < other.Upper;

        return Lower < other.Lower;
    }
};

// multiply two 64-bit integers into a 128-bit integer, Knuth's algorithm M
constexpr _BigUint128 _big_multiply(const uint64_t left, const uint64_t right) noexcept {
    const uint64_t leftLow      = left & 0xFFFF'FFFFULL;
    const uint64_t leftHigh     = left >> 32;
    const uint64_t rightLow     = right & 0xFFFF'FFFFULL;
    const uint64_t rightHigh    = right >> 32;

    uint64_t aux                = leftLow * rightLow;
    uint64_t carry              = aux >> 32;
    const uint64_t lower32      = aux & 0xFFFF'FFFFULL;

    aux                         = leftLow * rightHigh + carry;
    const uint64_t midLower     = aux & 0xFFFF'FFFFULL;
    const uint64_t midUpper     = aux >> 32;

    aux                         = leftHigh * rightLow + midLower;
    carry                       = aux >> 32;

    return { leftHigh * rightHigh + midUpper + carry, (aux << 32) + lower32 };
}

// ratio
template<intmax_t Numerator, intmax_t Denominator = 1>
struct Ratio    // holds the Ratio of Numerator to Denominator
{
    static_assert(Denominator != 0, "zero denominator");
    static_assert(-INTMAX_MAX <= Numerator, "numerator too negative");
    static_assert(-INTMAX_MAX <= Denominator, "denominator too negative");

    static constexpr intmax_t Num = _sign(Numerator) * _sign(Denominator) * _abs(Numerator) / _gcd(Numerator, Denominator);
    static constexpr intmax_t Den = _abs(Denominator) / _gcd(Numerator, Denominator);

    using Type = Ratio<Num, Den>;
};

// is ratio
template<class Type>
constexpr bool IsRatio_v = false;

template<intmax_t Rat1, intmax_t Rat2>
constexpr bool IsRatio_v<Ratio<Rat1, Rat2>> = true;

// safe multiply
template<intmax_t First, intmax_t Second, bool Sfinae = false,
bool Good = _abs(First) <= INTMAX_MAX / (Second == 0 ? 1 : _abs(Second))>
struct _SafeMultiply : IntegralConstant<intmax_t, First * Second> {}; // computes First * Second without overflow

template<intmax_t First, intmax_t Second, bool Sfinae>
struct _SafeMultiply<First, Second, Sfinae, false>
{
    static_assert(Sfinae, "integer arithmetic overflow");   // First * Second would overflow
};

// ratio add
template<class Rat1, class Rat2>
struct _RatioAdd
{
    static_assert(IsRatio_v<Rat1> && IsRatio_v<Rat2>, "RatioAdd<R1, R2> requires R1 and R2 to be ratios.");

private:
    static constexpr intmax_t _gcd1 = _gcd(Rat1::Den, Rat2::Den);

public:
    using Type = typename Ratio<_safe_add(  _SafeMultiply<Rat1::Num, Rat2::Den / _gcd1>::Value,
                                            _SafeMultiply<Rat2::Num, Rat1::Den / _gcd1>::Value),
                                _SafeMultiply<Rat1::Den, Rat2::Den / _gcd1>::Value>::Type;
};

template<class Rat1, class Rat2>
using RatioAdd = typename _RatioAdd<Rat1, Rat2>::Type;

// ratio subtract
template<class Rat1, class Rat2>
struct _RatioSubtract
{
    static_assert(IsRatio_v<Rat1> && IsRatio_v<Rat2>, "RatioSubtract<R1, R2> requires R1 and R2 to be ratios.");

    using Type = RatioAdd<Rat1, Ratio<-Rat2::Num, Rat2::Den>>;
};

template<class Rat1, class Rat2>
using RatioSubtract = typename _RatioSubtract<Rat1, Rat2>::Type;

// ratio multiply
template<class Rat1, class Rat2>
struct _RatioMultiply  // multiply two ratios
{
    static_assert(IsRatio_v<Rat1> && IsRatio_v<Rat2>, "RatioMultiply<R1, R2> requires R1 and R2 to be ratios.");

private:
    static constexpr intmax_t _gcd1 = _gcd(Rat1::Num, Rat2::Den);
    static constexpr intmax_t _gcd2 = _gcd(Rat2::Num, Rat1::Den);

public:
    using Num = _SafeMultiply<  Rat1::Num / _gcd1,
                                Rat2::Num / _gcd2, true>;

    using Den = _SafeMultiply<  Rat1::Den / _gcd2,
                                Rat2::Den / _gcd1, true>;
};

template<class Rat1, class Rat2, bool Sfinae = true, class = void>
struct _RatioMultiplySfinae   // detect overflow during multiplication
{
    static_assert(Sfinae, "integer arithmetic overflow");
};

template<class Rat1, class Rat2, bool Sfinae>
struct _RatioMultiplySfinae<Rat1, Rat2, Sfinae,
                            Void_t< typename _RatioMultiply<Rat1, Rat2>::Num::Type,
                                    typename _RatioMultiply<Rat1, Rat2>::Den::Type>>
{
    using Type = Ratio<_RatioMultiply<Rat1, Rat2>::Num::Value, _RatioMultiply<Rat1, Rat2>::Den::Value>;
};

template<class Rat1, class Rat2>
using RatioMultiply = typename _RatioMultiplySfinae<Rat1, Rat2, false>::Type;

// ratio divide
template<class Rat1, class Rat2>
struct _RatioDivide    // divide two ratios
{
    static_assert(IsRatio_v<Rat1> && IsRatio_v<Rat2>, "RatioDivide<R1, R2> requires R1 and R2 to be ratios.");

    using Rat2Inverse = Ratio<Rat2::Den, Rat2::Num>;
};

template<class Rat1, class Rat2, bool Sfinae = true>
using _RatioDivideSfinae = typename _RatioMultiplySfinae<Rat1, typename _RatioDivide<Rat1, Rat2>::Rat2Inverse, Sfinae>::Type;

template<class Rat1, class Rat2>
using RatioDivide = _RatioDivideSfinae<Rat1, Rat2, false>;

// ratio equal
template<class Rat1, class Rat2>
struct RatioEqual : BoolConstant<Rat1::Num == Rat2::Num && Rat1::Den == Rat2::Den>
{
    static_assert(IsRatio_v<Rat1> && IsRatio_v<Rat2>, "RatioEqual<R1, R2> requires R1 and R2 to be ratios.");
};

template<class Rat1, class Rat2>
constexpr bool RatioEqual_v = RatioEqual<Rat1, Rat2>::Value;

// ratio not equal
template<class Rat1, class Rat2>
struct RatioNotEqual : BoolConstant<!RatioEqual_v<Rat1, Rat2>>
{
    static_assert(IsRatio_v<Rat1> && IsRatio_v<Rat2>, "RatioNotEqual<R1, R2> requires R1 and R2 to be ratios.");
};

template<class Rat1, class Rat2>
constexpr bool RatioNotEqual_v = RatioNotEqual<Rat1, Rat2>::Value;

// ratio less
constexpr bool _ratio_less( const int64_t num1, const int64_t den1,
                            const int64_t num2, const int64_t den2) noexcept {
                                
    if (num1 >= 0 && num2 >= 0)
        return  _big_multiply(static_cast<uint64_t>(num1), static_cast<uint64_t>(den2)) <
                _big_multiply(static_cast<uint64_t>(num2), static_cast<uint64_t>(den1));

    if (num1 < 0 && num2 < 0)
        return  _big_multiply(static_cast<uint64_t>(-num2), static_cast<uint64_t>(den1)) <
                _big_multiply(static_cast<uint64_t>(-num1), static_cast<uint64_t>(den2));

    return num1 < num2;
}

template<class Rat1, class Rat2>
struct RatioLess : BoolConstant<_ratio_less(Rat1::Num, Rat1::Den, Rat2::Num, Rat2::Den)>
{
    static_assert(IsRatio_v<Rat1> && IsRatio_v<Rat2>, "RatioLess<R1, R2> requires R1 and R2 to be ratios.");
};

template<class Rat1, class Rat2>
constexpr bool RatioLess_v = RatioLess<Rat1, Rat2>::Value;

// ratio less equal
template<class Rat1, class Rat2>
struct RatioLessEqual : BoolConstant<!RatioLess_v<Rat2, Rat1>>
{
    static_assert(IsRatio_v<Rat1> && IsRatio_v<Rat2>, "RatioLessEqual<R1, R2> requires R1 and R2 to be ratios.");
};

template<class Rat1, class Rat2>
constexpr bool RatioLessEqual_v = RatioLessEqual<Rat1, Rat2>::Value;

// ratio greater
template<class Rat1, class Rat2>
struct RatioGreater : RatioLess<Rat2, Rat1>::Type
{
    static_assert(IsRatio_v<Rat1> && IsRatio_v<Rat2>, "RatioGreater<R1, R2> requires R1 and R2 to be ratios.");
};

template<class Rat1, class Rat2>
constexpr bool RatioGreater_v = RatioGreater<Rat1, Rat2>::Value;

// ratio greater equal
template<class Rat1, class Rat2>
struct RatioGreaterEqual : BoolConstant<!RatioLess_v<Rat1, Rat2>>
{
    static_assert(IsRatio_v<Rat1> && IsRatio_v<Rat2>, "RatioGreaterEqual<R1, R2> requires R1 and R2 to be ratios.");
};

template<class Rat1, class Rat2>
constexpr bool RatioGreaterEqual_v = RatioGreaterEqual<Rat1, Rat2>::Value;


// constant ratios
using Atto  = Ratio<1, 1000000000000000000LL>;
using Femto = Ratio<1, 1000000000000000LL>;
using Pico  = Ratio<1, 1000000000000LL>;
using Nano  = Ratio<1, 1000000000>;
using Micro = Ratio<1, 1000000>;
using Milli = Ratio<1, 1000>;
using Centi = Ratio<1, 100>;
using Deci  = Ratio<1, 10>;
using Deca  = Ratio<10, 1>;
using Hecto = Ratio<100, 1>;
using Kilo  = Ratio<1000, 1>;
using Mega  = Ratio<1000000, 1>;
using Giga  = Ratio<1000000000, 1>;
using Tera  = Ratio<1000000000000LL, 1>;
using Peta  = Ratio<1000000000000000LL, 1>;
using Exa   = Ratio<1000000000000000000LL, 1>;

CUSTOM_END