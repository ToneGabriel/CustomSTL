#pragma once
#include "TypeTraits.h"

#include <cstdint>


CUSTOM_BEGIN

// ratio
template<intmax_t N, intmax_t D = 1>
struct Ratio    // holds the Ratio of N to D
{
    static_assert(D != 0, "zero denominator");
    static_assert(-INTMAX_MAX <= N, "numerator too negative");
    static_assert(-INTMAX_MAX <= D, "denominator too negative");

    static constexpr intmax_t Num = _Sign_of(N) * _Sign_of(D) * _Abs(N) / _Gcd(N, D);
    static constexpr intmax_t Den = _Abs(D) / _Gcd(N, D);

    using Type = Ratio<Num, Den>;
};

// is ratio
template<class Type>
constexpr bool IsRatio_v = false;

template<intmax_t Rat1, intmax_t Rat2>
constexpr bool IsRatio_v<Ratio<Rat1, Rat2>> = true;

// ration multiply
template<class Rat1, class Rat2>
struct _RatioMultiply  // multiply two ratios
{
    static_assert(IsRatio_v<Rat1> && IsRatio_v<Rat2>, "RatioMultiply<R1, R2> requires R1 and R2 to be ratios.");

    static constexpr intmax_t Num1 = Rat1::Num;
    static constexpr intmax_t Den1 = Rat1::Den;
    static constexpr intmax_t Num2 = Rat2::Num;
    static constexpr intmax_t Den2 = Rat2::Den;

    static constexpr intmax_t _Gx = _Gcd(Num1, Den2);
    static constexpr intmax_t _Gy = _Gcd(Num2, Den1);

    using Num = _Safe_mult<Num1 / _Gx, Num2 / _Gy, true>;
    using Den = _Safe_mult<Den1 / _Gy, Den2 / _Gx, true>;
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

    static constexpr intmax_t Num2 = Rat2::Num;
    static constexpr intmax_t Den2 = Rat2::Den;

    using Rat2Inverse = Ratio<Den2, Num2>;
};

template<class Rat1, class Rat2, bool Sfinae = true>
using _RatioDivideSfinae = typename _RatioMultiplySfinae<Rat1, typename _RatioDivide<Rat1, Rat2>::Rat2Inverse, Sfinae>::Type;

template<class Rat1, class Rat2>
using RatioDivide = _RatioDivideSfinae<Rat1, Rat2, false>;

// ratios
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