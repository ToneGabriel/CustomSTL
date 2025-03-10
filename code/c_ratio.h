#pragma once
#include <c_type_traits.h>

#include <cstdint>


CUSTOM_BEGIN

template<intmax_t Numerator, intmax_t Denominator = 1>
struct ratio;

template<class Type>
struct is_ratio;

template<class Type>
constexpr bool is_ratio_v = is_ratio<Type>::value;

CUSTOM_DETAIL_BEGIN     // helpers

constexpr intmax_t _abs(const intmax_t val) noexcept
{
    return val < 0 ? -val : val;
}

constexpr intmax_t _sign(const intmax_t val) noexcept
{
    return val < 0 ? -1 : 1;
}

constexpr intmax_t _gcd(intmax_t first, intmax_t second) noexcept   // computes GCD of abs(first) and abs(second)
{
    if (first == 0 && second == 0)
        return 1; // contrary to mathematical convention; avoids division by 0 in ratio_less

    first   = _abs(first);
    second  = _abs(second);

    while (second != 0)
    {
        intmax_t aux    = first;
        first           = second;
        second          = aux % second;
    }

    return first;
}

inline void _safe_add_integer_arithmetic_overflow_error() noexcept {}

constexpr intmax_t _safe_add(const intmax_t first, const intmax_t second) noexcept
{
    if (_sign(first) == _sign(second) && _abs(first) > INTMAX_MAX - _abs(second))
        _safe_add_integer_arithmetic_overflow_error();  // call to non-constexpr function (terminates)

    return first + second;
}

struct _Big_Uint128
{
    uint64_t Upper;
    uint64_t Lower;

    constexpr bool operator<(const _Big_Uint128 other) const noexcept
    {
        if (Upper != other.Upper)
            return Upper < other.Upper;

        return Lower < other.Lower;
    }
};  // END _Big_Uint128

// multiply two 64-bit integers into a 128-bit integer, Knuth's algorithm M
constexpr _Big_Uint128 _big_multiply(const uint64_t left, const uint64_t right) noexcept
{
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

// ratio less
constexpr bool _ratio_less( const int64_t num1, const int64_t den1,
                            const int64_t num2, const int64_t den2) noexcept
{                                
    if (num1 >= 0 && num2 >= 0)
        return  detail::_big_multiply(static_cast<uint64_t>(num1), static_cast<uint64_t>(den2)) <
                detail::_big_multiply(static_cast<uint64_t>(num2), static_cast<uint64_t>(den1));

    if (num1 < 0 && num2 < 0)
        return  detail::_big_multiply(static_cast<uint64_t>(-num2), static_cast<uint64_t>(den1)) <
                detail::_big_multiply(static_cast<uint64_t>(-num1), static_cast<uint64_t>(den2));

    return num1 < num2;
}

// safe multiply
template<intmax_t first, intmax_t second, bool Sfinae = false,
bool Good = detail::_abs(first) <= INTMAX_MAX / (second == 0 ? 1 : detail::_abs(second))>
struct _Safe_Multiply : integral_constant<intmax_t, first * second> {}; // computes first * second without overflow

template<intmax_t first, intmax_t second, bool Sfinae>
struct _Safe_Multiply<first, second, Sfinae, false>
{
    static_assert(Sfinae, "integer arithmetic overflow");   // first * second would overflow
};

// ratio add
template<class Rat1, class Rat2>
struct _Ratio_Add
{
    static_assert(is_ratio_v<Rat1> && is_ratio_v<Rat2>,
                    "ratio_add<R1, R2> requires R1 and R2 to be ratios.");

private:
    static constexpr intmax_t _gcd = detail::_gcd(Rat1::den, Rat2::den);

public:
    using type = typename ratio<detail::_safe_add(  _Safe_Multiply<Rat1::num, Rat2::den / _gcd>::value,
                                                    _Safe_Multiply<Rat2::num, Rat1::den / _gcd>::value),
                                _Safe_Multiply<Rat1::den, Rat2::den / _gcd>::value>::type;
};  // END _Ratio_Add

// ratio subtract
template<class Rat1, class Rat2>
struct _Ratio_Subtract
{
    static_assert(is_ratio_v<Rat1> && is_ratio_v<Rat2>,
                    "ratio_subtract<R1, R2> requires R1 and R2 to be ratios.");

    using type = _Ratio_Add<Rat1, ratio<-Rat2::num, Rat2::den>>::type;
};  // END _Ratio_Subtract

// ratio multiply
template<class Rat1, class Rat2>
struct _Ratio_Multiply  // multiply two ratios
{
    static_assert(is_ratio_v<Rat1> && is_ratio_v<Rat2>,
                    "ratio_multiply<R1, R2> requires R1 and R2 to be ratios.");

private:
    static constexpr intmax_t _gcd1 = detail::_gcd(Rat1::num, Rat2::den);
    static constexpr intmax_t _gcd2 = detail::_gcd(Rat2::num, Rat1::den);

public:
    using num = _Safe_Multiply< Rat1::num / _gcd1,
                                Rat2::num / _gcd2, true>;

    using den = _Safe_Multiply< Rat1::den / _gcd2,
                                Rat2::den / _gcd1, true>;
};  // END _Ratio_Multiply

template<class Rat1, class Rat2, bool Sfinae = true, class = void>
struct _Ratio_Multiply_Sfinae   // detect overflow during multiplication
{
    static_assert(Sfinae, "integer arithmetic overflow");
};

template<class Rat1, class Rat2, bool Sfinae>
struct _Ratio_Multiply_Sfinae<Rat1, Rat2, Sfinae,
                            void_t< typename _Ratio_Multiply<Rat1, Rat2>::num::type,
                                    typename _Ratio_Multiply<Rat1, Rat2>::den::type>>
{
    using type = ratio<_Ratio_Multiply<Rat1, Rat2>::num::value, _Ratio_Multiply<Rat1, Rat2>::den::value>;
};

// ratio divide
template<class Rat1, class Rat2>
struct _Ratio_Divide    // divide two ratios
{
    static_assert(is_ratio_v<Rat1> && is_ratio_v<Rat2>,
                    "ratio_divide<R1, R2> requires R1 and R2 to be ratios.");

    using _Rat2Inverse = ratio<Rat2::den, Rat2::num>;
};  // END _Ratio_Divide

template<class Rat1, class Rat2, bool Sfinae = true>
using _Ratio_Divide_Sfinae = _Ratio_Multiply_Sfinae<Rat1, typename _Ratio_Divide<Rat1, Rat2>::_Rat2Inverse, Sfinae>;

CUSTOM_DETAIL_END     // helpers


// ratio
template<intmax_t Numerator, intmax_t Denominator>
struct ratio    // holds the ratio of Numerator to Denominator
{
    static_assert(Denominator != 0, "zero denominator");
    static_assert(-INTMAX_MAX <= Numerator, "numerator too negative");
    static_assert(-INTMAX_MAX <= Denominator, "denominator too negative");

    static constexpr intmax_t num = detail::_sign(Numerator) * detail::_sign(Denominator) * detail::_abs(Numerator) / detail::_gcd(Numerator, Denominator);
    static constexpr intmax_t den = detail::_abs(Denominator) / detail::_gcd(Numerator, Denominator);

    using type = ratio<num, den>;
};  // END ratio

template<class Type>
struct is_ratio : false_type {};

template<intmax_t Num, intmax_t Den>
struct is_ratio<ratio<Num, Den>> : true_type {};

template<class Rat1, class Rat2>
using ratio_add = typename detail::_Ratio_Add<Rat1, Rat2>::type;

template<class Rat1, class Rat2>
using ratio_subtract = typename detail::_Ratio_Subtract<Rat1, Rat2>::type;

template<class Rat1, class Rat2>
using ratio_multiply = typename detail::_Ratio_Multiply_Sfinae<Rat1, Rat2, false>::type;

template<class Rat1, class Rat2>
using ratio_divide = typename detail::_Ratio_Divide_Sfinae<Rat1, Rat2, false>::type;

// ratio equal
template<class Rat1, class Rat2>
struct ratio_equal : bool_constant<Rat1::num == Rat2::num && Rat1::den == Rat2::den>
{
    static_assert(is_ratio_v<Rat1> && is_ratio_v<Rat2>,
                    "ratio_equal<R1, R2> requires R1 and R2 to be ratios.");
};

template<class Rat1, class Rat2>
constexpr bool ratio_equal_v = ratio_equal<Rat1, Rat2>::value;

// ratio not equal
template<class Rat1, class Rat2>
struct ratio_not_equal : bool_constant<!ratio_equal_v<Rat1, Rat2>>
{
    static_assert(is_ratio_v<Rat1> && is_ratio_v<Rat2>,
                    "ratio_not_equal<R1, R2> requires R1 and R2 to be ratios.");
};

template<class Rat1, class Rat2>
constexpr bool ratio_not_equal_v = ratio_not_equal<Rat1, Rat2>::value;

template<class Rat1, class Rat2>
struct ratio_less : bool_constant<detail::_ratio_less(Rat1::num, Rat1::den, Rat2::num, Rat2::den)>
{
    static_assert(is_ratio_v<Rat1> && is_ratio_v<Rat2>,
                    "ratio_less<R1, R2> requires R1 and R2 to be ratios.");
};

template<class Rat1, class Rat2>
constexpr bool ratio_less_v = ratio_less<Rat1, Rat2>::value;

// ratio less equal
template<class Rat1, class Rat2>
struct ratio_less_equal : bool_constant<!ratio_less_v<Rat2, Rat1>>
{
    static_assert(is_ratio_v<Rat1> && is_ratio_v<Rat2>,
                    "ratio_less_equal<R1, R2> requires R1 and R2 to be ratios.");
};

template<class Rat1, class Rat2>
constexpr bool ratio_less_equal_v = ratio_less_equal<Rat1, Rat2>::value;

// ratio greater
template<class Rat1, class Rat2>
struct ratio_greater : ratio_less<Rat2, Rat1>::type
{
    static_assert(is_ratio_v<Rat1> && is_ratio_v<Rat2>,
                    "ratio_greater<R1, R2> requires R1 and R2 to be ratios.");
};

template<class Rat1, class Rat2>
constexpr bool ratio_greater_v = ratio_greater<Rat1, Rat2>::value;

// ratio greater equal
template<class Rat1, class Rat2>
struct ratio_greater_equal : bool_constant<!ratio_less_v<Rat1, Rat2>>
{
    static_assert(is_ratio_v<Rat1> && is_ratio_v<Rat2>,
                    "ratio_greater_equal<R1, R2> requires R1 and R2 to be ratios.");
};

template<class Rat1, class Rat2>
constexpr bool ratio_greater_equal_v = ratio_greater_equal<Rat1, Rat2>::value;


// constant ratios
using atto  = ratio<1, 1000000000000000000LL>;
using femto = ratio<1, 1000000000000000LL>;
using pico  = ratio<1, 1000000000000LL>;
using nano  = ratio<1, 1000000000>;
using micro = ratio<1, 1000000>;
using milli = ratio<1, 1000>;
using centi = ratio<1, 100>;
using deci  = ratio<1, 10>;
using deca  = ratio<10, 1>;
using hecto = ratio<100, 1>;
using kilo  = ratio<1000, 1>;
using mega  = ratio<1000000, 1>;
using giga  = ratio<1000000000, 1>;
using tera  = ratio<1000000000000LL, 1>;
using peta  = ratio<1000000000000000LL, 1>;
using exa   = ratio<1000000000000000000LL, 1>;

CUSTOM_END