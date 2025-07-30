#pragma once
#include "custom/type_traits.h"
#include "custom/limits.h"


CUSTOM_BEGIN

enum class endian
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
enable_if_t<conjunction_v<  bool_constant<sizeof(To) == sizeof(From)>,
                            is_trivially_copyable<To>,
                            is_trivially_copyable<From>>, bool> = true>
constexpr To bit_cast(const From& src) noexcept
{
    return __builtin_bit_cast(To, src);
}


template<class Ty, enable_if_t<is_unsigned_integer_v<Ty>, bool> = true>
constexpr bool has_single_bit(Ty val) noexcept
{
    return val != 0 && (val & (val - 1)) == 0;
}


// Implementation of countl_zero without using specialized CPU instructions.
// Used at compile time and when said instructions are not supported.
// see "Hacker's Delight" section 5-3
template<class Ty, enable_if_t<is_unsigned_integer_v<Ty>, bool> = true>
constexpr int countl_zero(Ty val) noexcept
{
    Ty yy = 0;

    unsigned int nn = numeric_limits<Ty>::digits;
    unsigned int cc = numeric_limits<Ty>::digits / 2;

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
template<class Ty, enable_if_t<is_unsigned_integer_v<Ty>, bool> = true>
constexpr int countr_zero(Ty val) noexcept
{
    constexpr int digits = numeric_limits<Ty>::digits;
    return digits - countl_zero(static_cast<Ty>(static_cast<Ty>(~val) & static_cast<Ty>(val - 1)));
}


template<class Ty, enable_if_t<is_unsigned_integer_v<Ty>, bool> = true>
constexpr int countl_one(Ty val) noexcept
{
    return custom::countl_zero(static_cast<Ty>(~val));
}


template<class Ty, enable_if_t<is_unsigned_integer_v<Ty>, bool> = true>
constexpr int countr_one(Ty val) noexcept
{
    return custom::countr_zero(static_cast<Ty>(~val));
}


template<class Ty, enable_if_t<is_unsigned_integer_v<Ty>, bool> = true>
constexpr Ty bit_ceil(Ty val) noexcept
{
    if (val <= 1u)
        return Ty{1};

    const int shiftExponent = numeric_limits<Ty>::digits - custom::countl_zero(static_cast<Ty>(val - 1));

    if constexpr (sizeof(Ty) < sizeof(unsigned int)) // for types subject to integral promotion
        if (custom::is_constant_evaluated())
        {
            // If the shift exponent equals number of digits then the correct result is not
            // representable as a value of Ty, and so the result is undefined.
            CUSTOM_ASSERT(shiftExponent != numeric_limits<Ty>::digits, "Undefined result");
        }

    return static_cast<Ty>(Ty{1} << shiftExponent);
}


template<class Ty, enable_if_t<is_unsigned_integer_v<Ty>, bool> = true>
constexpr Ty bit_floor(Ty val) noexcept
{
    if (val == 0)
        return 0;

    return static_cast<Ty>(Ty{1} << (numeric_limits<Ty>::digits - 1 - custom::countl_zero(val)));
}


template<class Ty, enable_if_t<is_unsigned_integer_v<Ty>, bool> = true>
constexpr int bit_width(Ty val) noexcept
{
    return numeric_limits<Ty>::digits - custom::countl_zero(val);
}


template<class Ty, enable_if_t<is_unsigned_integer_v<Ty>, bool> = true>
constexpr Ty rotl(Ty val, int rotation) noexcept;


template<class Ty, enable_if_t<is_unsigned_integer_v<Ty>, bool> = true>
constexpr Ty rotr(Ty val, int rotation) noexcept
{
    constexpr int digits    = numeric_limits<Ty>::digits;
    const int remainder     = rotation % digits;

    if (remainder == 0)
        return val;
    else if (remainder > 0)
        return static_cast<Ty>(static_cast<Ty>(val >> remainder) | static_cast<Ty>(val << (digits - remainder)));
    else
        return custom::rotl(val, -remainder);
}


template<class Ty, enable_if_t<is_unsigned_integer_v<Ty>, bool>>
constexpr Ty rotl(Ty val, int rotation) noexcept
{
    constexpr int digits    = numeric_limits<Ty>::digits;
    const int remainder     = rotation % digits;

    if (remainder == 0)
        return val;
    else if (remainder > 0)
        return static_cast<Ty>(static_cast<Ty>(val << remainder) | static_cast<Ty>(val >> (digits - remainder)));
    else
        return custom::rotr(val, -remainder);
}


template<class Ty, enable_if_t<is_unsigned_integer_v<Ty>, bool> = true>
constexpr int popcount(Ty val) noexcept
{
#ifdef __GNUG__
    constexpr int digits = numeric_limits<Ty>::digits;

    if constexpr (digits <= numeric_limits<unsigned>::digits)
        return __builtin_popcount(val);
    else if constexpr (digits <= numeric_limits<unsigned long>::digits)
        return __builtin_popcountl(val);
    else // (digits <= numeric_limits<unsigned long long>::digits)
        return __builtin_popcountll(val);
#else // __GNUG__
    int count = 0;

    while (val)
    {
        count += val & 1;
        val >>= 1;
    }

    return count;
#endif // __GNUG__
}

CUSTOM_END