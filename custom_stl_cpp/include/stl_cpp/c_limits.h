#pragma once
#include "stl_cpp/c_type_traits.h"

#include <float.h>



#if defined _MSC_VER

// integral
#ifndef _CUSTOM_INTEGRAL_TRAPS
#define _CUSTOM_INTEGRAL_TRAPS false
#endif

// float
#ifndef _CUSTOM_FLOAT_HAS_DENORM_LOSS
#define _CUSTOM_FLOAT_HAS_DENORM_LOSS false
#endif

#ifndef _CUSTOM_FLOAT_TRAPS
#define _CUSTOM_FLOAT_TRAPS false
#endif

#ifndef _CUSTOM_FLOAT_TINYNESS_BEFORE
#define _CUSTOM_FLOAT_TINYNESS_BEFORE false
#endif

// double
#ifndef _CUSTOM_DOUBLE_HAS_DENORM_LOSS
#define _CUSTOM_DOUBLE_HAS_DENORM_LOSS false
#endif

#ifndef _CUSTOM_DOUBLE_TRAPS
#define _CUSTOM_DOUBLE_TRAPS false
#endif

#ifndef _CUSTOM_DOUBLE_TINYNESS_BEFORE
#define _CUSTOM_DOUBLE_TINYNESS_BEFORE false
#endif

// long double
#ifndef _CUSTOM_LONG_DOUBLE_HAS_DENORM_LOSS
#define _CUSTOM_LONG_DOUBLE_HAS_DENORM_LOSS false
#endif

#ifndef _CUSTOM_LONG_DOUBLE_TRAPS
#define _CUSTOM_LONG_DOUBLE_TRAPS false
#endif

#ifndef _CUSTOM_LONG_DOUBLE_TINYNESS_BEFORE
#define _CUSTOM_LONG_DOUBLE_TINYNESS_BEFORE false
#endif

#elif defined __GNUG__

// integral
#ifndef _CUSTOM_INTEGRAL_TRAPS
#define _CUSTOM_INTEGRAL_TRAPS true
#endif

// float
#ifndef _CUSTOM_FLOAT_HAS_DENORM_LOSS
#define _CUSTOM_FLOAT_HAS_DENORM_LOSS false
#endif

#ifndef _CUSTOM_FLOAT_TRAPS
#define _CUSTOM_FLOAT_TRAPS false
#endif

#ifndef _CUSTOM_FLOAT_TINYNESS_BEFORE
#define _CUSTOM_FLOAT_TINYNESS_BEFORE false
#endif

// double
#ifndef _CUSTOM_DOUBLE_HAS_DENORM_LOSS
#define _CUSTOM_DOUBLE_HAS_DENORM_LOSS false
#endif

#ifndef _CUSTOM_DOUBLE_TRAPS
#define _CUSTOM_DOUBLE_TRAPS false
#endif

#ifndef _CUSTOM_DOUBLE_TINYNESS_BEFORE
#define _CUSTOM_DOUBLE_TINYNESS_BEFORE false
#endif

// long double
#ifndef _CUSTOM_LONG_DOUBLE_HAS_DENORM_LOSS
#define _CUSTOM_LONG_DOUBLE_HAS_DENORM_LOSS false
#endif

#ifndef _CUSTOM_LONG_DOUBLE_TRAPS
#define _CUSTOM_LONG_DOUBLE_TRAPS false
#endif

#ifndef _CUSTOM_LONG_DOUBLE_TINYNESS_BEFORE
#define _CUSTOM_LONG_DOUBLE_TINYNESS_BEFORE false
#endif

// Fixup for double radix
#ifndef DBL_RADIX
#define DBL_RADIX FLT_RADIX
#endif

// Fixup for long double radix
#ifndef LDBL_RADIX
#define LDBL_RADIX FLT_RADIX
#endif

#endif


// Helpers
#define _CUSTOM_SIGNED_B(T,B)       ((T)(-1) < 0)
#define _CUSTOM_DIGITS_B(T,B)	    (B - _CUSTOM_SIGNED_B (T,B))
#define _CUSTOM_DIGITS10_B(T,B)     (_CUSTOM_DIGITS_B (T,B) * 643L / 2136)


// CHAR_BIT Specialization Helpers
#define _CUSTOM_SIGNED(T)           _CUSTOM_SIGNED_B (T, sizeof(T) * CHAR_BIT)
#define _CUSTOM_DIGITS(T)           _CUSTOM_DIGITS_B (T, sizeof(T) * CHAR_BIT)
#define _CUSTOM_DIGITS10(T)         _CUSTOM_DIGITS10_B (T, sizeof(T) * CHAR_BIT)
#define _CUSTOM_MAX_DIGITS10(T)     (2 + (T) * 643L / 2136)
// The fraction 643/2136 approximates log10(2) to 7 significant digits

CUSTOM_BEGIN

enum float_round_style
{
    RoundIndeterminate      = -1, // Indeterminate.
    RoundTowardZero         = 0,  // To zero.
    RoundToNearest          = 1,  // To the nearest representable value.
    RoundTowardInfinity     = 2,  // To infinity.
    RoundTowardNegInfinity  = 3   // To negative infinity.
};

enum float_denorm_style
{
    DenormIndeterminate     = -1,  // Indeterminate at compile time whether denormalized values are allowed.
    DenormAbsent            = 0,   // The type does not allow denormalized values.
    DenormPresent           = 1    // The type allows denormalized values.
};


template<class Type>
struct numeric_limits
{
    // Static parameters

    /* This is only meaningful for floating types; integer types will all be
	RoundTowardZero. */
    static constexpr float_round_style round_style = RoundTowardZero;

    /* See std::float_denorm_style for more information. */
    static constexpr float_denorm_style has_denorm = DenormAbsent;

    /* True if loss of accuracy is detected as a denormalization loss,
    rather than as an inexact result. */
    static constexpr bool has_denorm_loss = false;

    /* True if the type has a representation for positive infinity. */
    static constexpr bool has_infinity = false;

    /* True if the type has a representation for a quiet (non-signaling)
    Not a Number. */
    static constexpr bool has_quiet_NaN = false;

    /* True if the type has a representation for a signaling
    Not a Number. */
    static constexpr bool has_signaling_NaN = false;

    /* This will be true for all fundamental types (which have
	specializations), and false for everything else. */
    static constexpr bool is_specialized = false;

    /* True if the type is signed. */
    static constexpr bool is_signed = false;

    /* True if the type is integer. */
    static constexpr bool is_integer = false;

    /* True if the type uses an exact representation. All integer types are
	exact, but not all exact types are integer. For example, rational and
	fixed-exponent representations are exact but not integer. */
    static constexpr bool is_exact = false;

    /* True if-and-only-if the type adheres to the IEC 559 standard, also
    known as IEEE 754.  (Only makes sense for floating point types.) */
    static constexpr bool is_iec559 = false;

    /* True if the set of values representable by the type is
    finite.  All built-in types are bounded, this member would be
    false for arbitrary precision types. */
    static constexpr bool is_bounded = false;

    /* True if the type is modulo. A type is modulo if, for any
    operation involving +, -, or * on values of that type whose
    result would fall outside the range [min(),max()], the value
    returned differs from the true value by an integer multiple of
    max() - min() + 1. On most machines, this is false for floating
    types, true for unsigned integers, and true for signed integers.
    See PR22200 about signed integers. */
    static constexpr bool is_modulo = false;

    /* True if trapping is implemented for this type. */
    static constexpr bool traps = false;

    /* True if tininess is detected before rounding.  (see IEC 559) */
    static constexpr bool tinyness_before = false;

    /* The number of radix digits that be represented without change:  for
	integer types, the number of non-sign bits in the mantissa; for
	floating types, the number of radix digits in the mantissa. */
    static constexpr int digits = 0;

    /* The number of base 10 digits that can be represented without change. */
    static constexpr int digits10 = 0;

    /* The number of base 10 digits required to ensure that values which
	differ are always differentiated. */
    static constexpr int max_digits10 = 0;

    /* For integer types, specifies the base of the representation.  For
	floating types, specifies the base of the exponent representation. */
    static constexpr int radix = 0;

    /* The minimum negative integer such that radix raised to the power of
	(one less than that integer) is a normalized floating point number. */
    static constexpr int min_exponent = 0;

    /* The minimum negative integer such that 10 raised to that power is in
	the range of normalized floating point numbers. */
    static constexpr int min_exponent10 = 0;

    /* The maximum positive integer such that radix raised to the power of
	(one less than that integer) is a representable finite floating point
	number. */
    static constexpr int max_exponent = 0;

    /* The maximum positive integer such that 10 raised to that power is in
	the range of representable finite floating point numbers. */
    static constexpr int max_exponent10 = 0;


    // Main functions

    /* The minimum finite value, or for floating types with
	denormalization, the minimum positive normalized value. */
    static constexpr Type (min)() noexcept
	{
        return Type();
    }

    /* The maximum finite value. */
    static constexpr Type (max)() noexcept
	{
        return Type();
    }

    /* A finite value x such that there is no other finite value y
    where y < x. */
    static constexpr Type lowest() noexcept
	{
        return Type();
    }

    /* The machine epsilon:  the difference between 1 and the least
    value greater than 1 that is representable. */
    static constexpr Type epsilon() noexcept
	{
        return Type();
    }

    /* The maximum rounding error measurement (see LIA-1). */
    static constexpr Type round_error() noexcept
	{
        return Type();
    }

    /* The representation of positive infinity, if has_infinity. */
    static constexpr Type infinity() noexcept
	{
        return Type();
    }

    /* The representation of a quiet Not a Number,
    if has_quiet_NaN. */
    static constexpr Type quiet_NaN() noexcept
	{
        return Type();
    }

    /* The representation of a signaling Not a Number, if
    has_signaling_NaN. */
    static constexpr Type signaling_NaN() noexcept
	{
        return Type();
    }

    /* The minimum positive denormalized value.  For types where
    has_denorm is false, this is the minimum positive normalized value. */
    static constexpr Type denorm_min() noexcept
	{
        return Type();
    }
};  // END numeric_limits

// ignore CV qualifiers
template<typename Type>
struct numeric_limits<const Type> : public numeric_limits<Type> { };

template<typename Type>
struct numeric_limits<volatile Type> : public numeric_limits<Type> { };

template<typename Type>
struct numeric_limits<const volatile Type> : public numeric_limits<Type> { };


// bool specialization
template<>
struct numeric_limits<bool>
{
    static constexpr float_round_style round_style = RoundTowardZero;
    static constexpr float_denorm_style has_denorm = DenormAbsent;

    static constexpr bool has_denorm_loss = false;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;

    static constexpr bool traps = _CUSTOM_INTEGRAL_TRAPS;
    static constexpr bool tinyness_before = false;

    static constexpr int digits = 1;
    static constexpr int digits10 = 0;
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;


    static constexpr bool (min)() noexcept
	{
        return false;
    }

    static constexpr bool (max)() noexcept
	{
        return true;
    }

    static constexpr bool lowest() noexcept
	{
        return (min)();
    }

    static constexpr bool epsilon() noexcept
	{
        return false;
    }

    static constexpr bool round_error() noexcept
	{
        return false;
    }

    static constexpr bool infinity() noexcept
	{
        return false;
    }

    static constexpr bool quiet_NaN() noexcept
	{
        return false;
    }

    static constexpr bool signaling_NaN() noexcept
	{
        return false;
    }

    static constexpr bool denorm_min() noexcept
	{
        return false;
    }
};  // END bool specialization


// char specialization
template<>
struct numeric_limits<char>
{
    static constexpr float_round_style round_style = RoundTowardZero;
    static constexpr float_denorm_style has_denorm = DenormAbsent;

    static constexpr bool has_denorm_loss = false;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = _CUSTOM_SIGNED(char);
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = !is_signed;

    static constexpr bool traps = _CUSTOM_INTEGRAL_TRAPS;
    static constexpr bool tinyness_before = false;

    static constexpr int digits = _CUSTOM_DIGITS(char);
    static constexpr int digits10 = _CUSTOM_DIGITS10(char);
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;


    static constexpr char (min)() noexcept
	{
        return CHAR_MIN;
    }

    static constexpr char (max)() noexcept
	{
        return CHAR_MAX;
    }

    static constexpr char lowest() noexcept
	{
        return (min)();
    }

    static constexpr char epsilon() noexcept
	{
        return 0;
    }

    static constexpr char round_error() noexcept
	{
        return 0;
    }

    static constexpr char infinity() noexcept
	{
        return 0;
    }

    static constexpr char quiet_NaN() noexcept
	{
        return 0;
    }

    static constexpr char signaling_NaN() noexcept
	{
        return 0;
    }

    static constexpr char denorm_min() noexcept
	{
        return 0;
    }
};  // END char specialization


// signed char specialization
template<>
struct numeric_limits<signed char>
{
    static constexpr float_round_style round_style = RoundTowardZero;
    static constexpr float_denorm_style has_denorm = DenormAbsent;

    static constexpr bool has_denorm_loss = false;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;

    static constexpr bool traps = _CUSTOM_INTEGRAL_TRAPS;
    static constexpr bool tinyness_before = false;

    static constexpr int digits = _CUSTOM_DIGITS(signed char);
    static constexpr int digits10 = _CUSTOM_DIGITS10(signed char);
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;


    static constexpr signed char (min)() noexcept
	{
        return SCHAR_MIN;
    }

    static constexpr signed char (max)() noexcept
	{
        return SCHAR_MAX;
    }

    static constexpr signed char lowest() noexcept
	{
        return (min)();
    }

    static constexpr signed char epsilon() noexcept
	{
        return 0;
    }

    static constexpr signed char round_error() noexcept
	{
        return 0;
    }

    static constexpr signed char infinity() noexcept
	{
        return 0;
    }

    static constexpr signed char quiet_NaN() noexcept
	{
        return 0;
    }

    static constexpr signed char signaling_NaN() noexcept
	{
        return 0;
    }

    static constexpr signed char denorm_min() noexcept
	{
        return 0;
    }
};  // END signed char specialization


// unsigned char specialization
template<>
struct numeric_limits<unsigned char>
{
    static constexpr float_round_style round_style = RoundTowardZero;
    static constexpr float_denorm_style has_denorm = DenormAbsent;

    static constexpr bool has_denorm_loss = false;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = true;

    static constexpr bool traps = _CUSTOM_INTEGRAL_TRAPS;
    static constexpr bool tinyness_before = false;

    static constexpr int digits = _CUSTOM_DIGITS(unsigned char);
    static constexpr int digits10 = _CUSTOM_DIGITS10(unsigned char);
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;


    static constexpr unsigned char (min)() noexcept
	{
        return 0;
    }

    static constexpr unsigned char (max)() noexcept
	{
        return UCHAR_MAX;
    }

    static constexpr unsigned char lowest() noexcept
	{
        return (min)();
    }

    static constexpr unsigned char epsilon() noexcept
	{
        return 0;
    }

    static constexpr unsigned char round_error() noexcept
	{
        return 0;
    }

    static constexpr unsigned char infinity() noexcept
	{
        return 0;
    }

    static constexpr unsigned char quiet_NaN() noexcept
	{
        return 0;
    }

    static constexpr unsigned char signaling_NaN() noexcept
	{
        return 0;
    }

    static constexpr unsigned char denorm_min() noexcept
	{
        return 0;
    }
};  // END unsigned char specialization


// wchar_t specialization
template<>
struct numeric_limits<wchar_t>
{
    static constexpr float_round_style round_style = RoundTowardZero;
    static constexpr float_denorm_style has_denorm = DenormAbsent;

    static constexpr bool has_denorm_loss = false;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = _CUSTOM_SIGNED(wchar_t);
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = !is_signed;

    static constexpr bool traps = _CUSTOM_INTEGRAL_TRAPS;
    static constexpr bool tinyness_before = false;

    static constexpr int digits = _CUSTOM_DIGITS(wchar_t);
    static constexpr int digits10 = _CUSTOM_DIGITS10(wchar_t);
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;


    static constexpr wchar_t (min)() noexcept
	{
        return WCHAR_MIN;
    }

    static constexpr wchar_t (max)() noexcept
	{
        return WCHAR_MAX;
    }

    static constexpr wchar_t lowest() noexcept
	{
        return (min)();
    }

    static constexpr wchar_t epsilon() noexcept
	{
        return 0;
    }

    static constexpr wchar_t round_error() noexcept
	{
        return 0;
    }

    static constexpr wchar_t infinity() noexcept
	{
        return 0;
    }

    static constexpr wchar_t quiet_NaN() noexcept
	{
        return 0;
    }

    static constexpr wchar_t signaling_NaN() noexcept
	{
        return 0;
    }

    static constexpr wchar_t denorm_min() noexcept
	{
        return 0;
    }
};  // END wchar_t specialization


// char8_t specialization
#ifdef __cpp_char8_t
template<>
struct numeric_limits<char8_t>
{
    static constexpr float_round_style round_style = RoundTowardZero;
    static constexpr float_denorm_style has_denorm = DenormAbsent;

    static constexpr bool has_denorm_loss = false;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = _CUSTOM_SIGNED(char8_t);
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = !is_signed;

    static constexpr bool traps = _CUSTOM_INTEGRAL_TRAPS;
    static constexpr bool tinyness_before = false;

    static constexpr int digits = _CUSTOM_DIGITS(char8_t);
    static constexpr int digits10 = _CUSTOM_DIGITS10(char8_t);
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;


    static constexpr char8_t (min)() noexcept
	{
        return 0;
    }

    static constexpr char8_t (max)() noexcept
	{
        return UCHAR_MAX;
    }

    static constexpr char8_t lowest() noexcept
	{
        return (min)();
    }

    static constexpr char8_t epsilon() noexcept
	{
        return 0;
    }

    static constexpr char8_t round_error() noexcept
	{
        return 0;
    }

    static constexpr char8_t infinity() noexcept
	{
        return 0;
    }

    static constexpr char8_t quiet_NaN() noexcept
	{
        return 0;
    }

    static constexpr char8_t signaling_NaN() noexcept
	{
        return 0;
    }

    static constexpr char8_t denorm_min() noexcept
	{
        return 0;
    }
};  // END char8_t specialization
#endif  // char8_t defined


// char16_t specialization
template<>
struct numeric_limits<char16_t>
{
    static constexpr float_round_style round_style = RoundTowardZero;
    static constexpr float_denorm_style has_denorm = DenormAbsent;

    static constexpr bool has_denorm_loss = false;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = _CUSTOM_SIGNED(char16_t);
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = !is_signed;

    static constexpr bool traps = _CUSTOM_INTEGRAL_TRAPS;
    static constexpr bool tinyness_before = false;

    static constexpr int digits = _CUSTOM_DIGITS(char16_t);
    static constexpr int digits10 = _CUSTOM_DIGITS10(char16_t);
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;


    static constexpr char16_t (min)() noexcept
	{
        return 0;
    }

    static constexpr char16_t (max)() noexcept
	{
        return USHRT_MAX;
    }

    static constexpr char16_t lowest() noexcept
	{
        return (min)();
    }

    static constexpr char16_t epsilon() noexcept
	{
        return 0;
    }

    static constexpr char16_t round_error() noexcept
	{
        return 0;
    }

    static constexpr char16_t infinity() noexcept
	{
        return 0;
    }

    static constexpr char16_t quiet_NaN() noexcept
	{
        return 0;
    }

    static constexpr char16_t signaling_NaN() noexcept
	{
        return 0;
    }

    static constexpr char16_t denorm_min() noexcept
	{
        return 0;
    }
};  // END char16_t specialization


// char32_t specialization
template<>
struct numeric_limits<char32_t>
{
    static constexpr float_round_style round_style = RoundTowardZero;
    static constexpr float_denorm_style has_denorm = DenormAbsent;

    static constexpr bool has_denorm_loss = false;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = _CUSTOM_SIGNED(char32_t);
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = !is_signed;

    static constexpr bool traps = _CUSTOM_INTEGRAL_TRAPS;
    static constexpr bool tinyness_before = false;

    static constexpr int digits = _CUSTOM_DIGITS(char32_t);
    static constexpr int digits10 = _CUSTOM_DIGITS10(char32_t);
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;


    static constexpr char32_t (min)() noexcept
	{
        return 0;
    }

    static constexpr char32_t (max)() noexcept
	{
        return UINT_MAX;
    }

    static constexpr char32_t lowest() noexcept
	{
        return (min)();
    }

    static constexpr char32_t epsilon() noexcept
	{
        return 0;
    }

    static constexpr char32_t round_error() noexcept
	{
        return 0;
    }

    static constexpr char32_t infinity() noexcept
	{
        return 0;
    }

    static constexpr char32_t quiet_NaN() noexcept
	{
        return 0;
    }

    static constexpr char32_t signaling_NaN() noexcept
	{
        return 0;
    }

    static constexpr char32_t denorm_min() noexcept
	{
        return 0;
    }
};  // END char32_t specialization


// short specialization
template<>
struct numeric_limits<short>
{
    static constexpr float_round_style round_style = RoundTowardZero;
    static constexpr float_denorm_style has_denorm = DenormAbsent;

    static constexpr bool has_denorm_loss = false;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;

    static constexpr bool traps = _CUSTOM_INTEGRAL_TRAPS;
    static constexpr bool tinyness_before = false;

    static constexpr int digits = _CUSTOM_DIGITS(short);
    static constexpr int digits10 = _CUSTOM_DIGITS10(short);
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;


    static constexpr short (min)() noexcept
	{
        return SHRT_MIN;
    }

    static constexpr short (max)() noexcept
	{
        return SHRT_MAX;
    }

    static constexpr short lowest() noexcept
	{
        return (min)();
    }

    static constexpr short epsilon() noexcept
	{
        return 0;
    }

    static constexpr short round_error() noexcept
	{
        return 0;
    }

    static constexpr short infinity() noexcept
	{
        return 0;
    }

    static constexpr short quiet_NaN() noexcept
	{
        return 0;
    }

    static constexpr short signaling_NaN() noexcept
	{
        return 0;
    }

    static constexpr short denorm_min() noexcept
	{
        return 0;
    }
};  // END short specialization


// unsigned short specialization
template<>
struct numeric_limits<unsigned short>
{
    static constexpr float_round_style round_style = RoundTowardZero;
    static constexpr float_denorm_style has_denorm = DenormAbsent;

    static constexpr bool has_denorm_loss = false;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = true;

    static constexpr bool traps = _CUSTOM_INTEGRAL_TRAPS;
    static constexpr bool tinyness_before = false;

    static constexpr int digits = _CUSTOM_DIGITS(unsigned short);
    static constexpr int digits10 = _CUSTOM_DIGITS10(unsigned short);
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;


    static constexpr unsigned short (min)() noexcept
	{
        return 0;
    }

    static constexpr unsigned short (max)() noexcept
	{
        return USHRT_MAX;
    }

    static constexpr unsigned short lowest() noexcept
	{
        return (min)();
    }

    static constexpr unsigned short epsilon() noexcept
	{
        return 0;
    }

    static constexpr unsigned short round_error() noexcept
	{
        return 0;
    }

    static constexpr unsigned short infinity() noexcept
	{
        return 0;
    }

    static constexpr unsigned short quiet_NaN() noexcept
	{
        return 0;
    }

    static constexpr unsigned short signaling_NaN() noexcept
	{
        return 0;
    }

    static constexpr unsigned short denorm_min() noexcept
	{
        return 0;
    }
};  // END unsigned short specialization


// int specialization
template<>
struct numeric_limits<int>
{
    static constexpr float_round_style round_style = RoundTowardZero;
    static constexpr float_denorm_style has_denorm = DenormAbsent;

    static constexpr bool has_denorm_loss = false;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;

    static constexpr bool traps = _CUSTOM_INTEGRAL_TRAPS;
    static constexpr bool tinyness_before = false;

    static constexpr int digits = _CUSTOM_DIGITS(int);
    static constexpr int digits10 = _CUSTOM_DIGITS10(int);
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;


    static constexpr int (min)() noexcept
	{
        return INT_MIN;
    }

    static constexpr int (max)() noexcept
	{
        return INT_MAX;
    }

    static constexpr int lowest() noexcept
	{
        return (min)();
    }

    static constexpr int epsilon() noexcept
	{
        return 0;
    }

    static constexpr int round_error() noexcept
	{
        return 0;
    }

    static constexpr int infinity() noexcept
	{
        return 0;
    }

    static constexpr int quiet_NaN() noexcept
	{
        return 0;
    }

    static constexpr int signaling_NaN() noexcept
	{
        return 0;
    }

    static constexpr int denorm_min() noexcept
	{
        return 0;
    }
};  // END int specialization


// unsigned int specialization
template<>
struct numeric_limits<unsigned int>
{
    static constexpr float_round_style round_style = RoundTowardZero;
    static constexpr float_denorm_style has_denorm = DenormAbsent;

    static constexpr bool has_denorm_loss = false;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = true;

    static constexpr bool traps = _CUSTOM_INTEGRAL_TRAPS;
    static constexpr bool tinyness_before = false;

    static constexpr int digits = _CUSTOM_DIGITS(unsigned int);
    static constexpr int digits10 = _CUSTOM_DIGITS10(unsigned int);
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;


    static constexpr unsigned int (min)() noexcept
	{
        return 0;
    }

    static constexpr unsigned int (max)() noexcept
	{
        return UINT_MAX;
    }

    static constexpr unsigned int lowest() noexcept
	{
        return (min)();
    }

    static constexpr unsigned int epsilon() noexcept
	{
        return 0;
    }

    static constexpr unsigned int round_error() noexcept
	{
        return 0;
    }

    static constexpr unsigned int infinity() noexcept
	{
        return 0;
    }

    static constexpr unsigned int quiet_NaN() noexcept
	{
        return 0;
    }

    static constexpr unsigned int signaling_NaN() noexcept
	{
        return 0;
    }

    static constexpr unsigned int denorm_min() noexcept
	{
        return 0;
    }
};  // END unsigned int specialization


// long specialization
template<>
struct numeric_limits<long>
{
    static constexpr float_round_style round_style = RoundTowardZero;
    static constexpr float_denorm_style has_denorm = DenormAbsent;

    static constexpr bool has_denorm_loss = false;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;

    static constexpr bool traps = _CUSTOM_INTEGRAL_TRAPS;
    static constexpr bool tinyness_before = false;

    static constexpr int digits = _CUSTOM_DIGITS(long);
    static constexpr int digits10 = _CUSTOM_DIGITS10(long);
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;


    static constexpr long (min)() noexcept
	{
        return LONG_MIN;
    }

    static constexpr long (max)() noexcept
	{
        return LONG_MAX;
    }

    static constexpr long lowest() noexcept
	{
        return (min)();
    }

    static constexpr long epsilon() noexcept
	{
        return 0;
    }

    static constexpr long round_error() noexcept
	{
        return 0;
    }

    static constexpr long infinity() noexcept
	{
        return 0;
    }

    static constexpr long quiet_NaN() noexcept
	{
        return 0;
    }

    static constexpr long signaling_NaN() noexcept
	{
        return 0;
    }

    static constexpr long denorm_min() noexcept
	{
        return 0;
    }
};  // END long specialization


// unsigned long specialization
template<>
struct numeric_limits<unsigned long>
{
    static constexpr float_round_style round_style = RoundTowardZero;
    static constexpr float_denorm_style has_denorm = DenormAbsent;

    static constexpr bool has_denorm_loss = false;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = true;

    static constexpr bool traps = _CUSTOM_INTEGRAL_TRAPS;
    static constexpr bool tinyness_before = false;

    static constexpr int digits = _CUSTOM_DIGITS(unsigned long);
    static constexpr int digits10 = _CUSTOM_DIGITS10(unsigned long);
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;


    static constexpr unsigned long (min)() noexcept
	{
        return 0;
    }

    static constexpr unsigned long (max)() noexcept
	{
        return ULONG_MAX;
    }

    static constexpr unsigned long lowest() noexcept
	{
        return (min)();
    }

    static constexpr unsigned long epsilon() noexcept
	{
        return 0;
    }

    static constexpr unsigned long round_error() noexcept
	{
        return 0;
    }

    static constexpr unsigned long infinity() noexcept
	{
        return 0;
    }

    static constexpr unsigned long quiet_NaN() noexcept
	{
        return 0;
    }

    static constexpr unsigned long signaling_NaN() noexcept
	{
        return 0;
    }

    static constexpr unsigned long denorm_min() noexcept
	{
        return 0;
    }
};  // END unsigned long specialization


// long long specialization
template<>
struct numeric_limits<long long>
{
    static constexpr float_round_style round_style = RoundTowardZero;
    static constexpr float_denorm_style has_denorm = DenormAbsent;

    static constexpr bool has_denorm_loss = false;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;

    static constexpr bool traps = _CUSTOM_INTEGRAL_TRAPS;
    static constexpr bool tinyness_before = false;

    static constexpr int digits = _CUSTOM_DIGITS(long long);
    static constexpr int digits10 = _CUSTOM_DIGITS10(long long);
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;


    static constexpr long long (min)() noexcept
	{
        return LLONG_MIN;
    }

    static constexpr long long (max)() noexcept
	{
        return LLONG_MAX;
    }

    static constexpr long long lowest() noexcept
	{
        return (min)();
    }

    static constexpr long long epsilon() noexcept
	{
        return 0;
    }

    static constexpr long long round_error() noexcept
	{
        return 0;
    }

    static constexpr long long infinity() noexcept
	{
        return 0;
    }

    static constexpr long long quiet_NaN() noexcept
	{
        return 0;
    }

    static constexpr long long signaling_NaN() noexcept
	{
        return 0;
    }

    static constexpr long long denorm_min() noexcept
	{
        return 0;
    }
};  // END long long specialization


// unsigned long long specialization
template<>
struct numeric_limits<unsigned long long>
{
    static constexpr float_round_style round_style = RoundTowardZero;
    static constexpr float_denorm_style has_denorm = DenormAbsent;

    static constexpr bool has_denorm_loss = false;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = false;
    static constexpr bool is_integer = true;
    static constexpr bool is_exact = true;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = true;

    static constexpr bool traps = _CUSTOM_INTEGRAL_TRAPS;
    static constexpr bool tinyness_before = false;

    static constexpr int digits = _CUSTOM_DIGITS(unsigned long long);
    static constexpr int digits10 = _CUSTOM_DIGITS10(unsigned long long);
    static constexpr int max_digits10 = 0;
    static constexpr int radix = 2;

    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;


    static constexpr unsigned long long (min)() noexcept
	{
        return 0;
    }

    static constexpr unsigned long long (max)() noexcept
	{
        return ULLONG_MAX;
    }

    static constexpr unsigned long long lowest() noexcept
	{
        return (min)();
    }

    static constexpr unsigned long long epsilon() noexcept
	{
        return 0;
    }

    static constexpr unsigned long long round_error() noexcept
	{
        return 0;
    }

    static constexpr unsigned long long infinity() noexcept
	{
        return 0;
    }

    static constexpr unsigned long long quiet_NaN() noexcept
	{
        return 0;
    }

    static constexpr unsigned long long signaling_NaN() noexcept
	{
        return 0;
    }

    static constexpr unsigned long long denorm_min() noexcept
	{
        return 0;
    }
};  // END unsigned long long specialization


// float specialization
template<>
struct numeric_limits<float>
{
    static constexpr float_round_style round_style = RoundToNearest;
    static constexpr float_denorm_style has_denorm = DenormPresent;

    static constexpr bool has_denorm_loss = _CUSTOM_FLOAT_HAS_DENORM_LOSS;
    static constexpr bool has_infinity = true;
    static constexpr bool has_quiet_NaN = true;
    static constexpr bool has_signaling_NaN = true;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = false;
    static constexpr bool is_iec559 = true;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;

    static constexpr bool traps = _CUSTOM_FLOAT_TRAPS;
    static constexpr bool tinyness_before = _CUSTOM_FLOAT_TINYNESS_BEFORE;

    static constexpr int digits = FLT_MANT_DIG;
    static constexpr int digits10 = FLT_DIG;
    static constexpr int max_digits10 = _CUSTOM_MAX_DIGITS10(FLT_MANT_DIG);
    static constexpr int radix = FLT_RADIX;

    static constexpr int min_exponent = FLT_MIN_EXP;
    static constexpr int min_exponent10 = FLT_MIN_10_EXP;
    static constexpr int max_exponent = FLT_MAX_EXP;
    static constexpr int max_exponent10 = FLT_MAX_10_EXP;


    static constexpr float (min)() noexcept
	{
        return FLT_MIN;
    }

    static constexpr float (max)() noexcept
	{
        return FLT_MAX;
    }

    static constexpr float lowest() noexcept
	{
        return -(max)();
    }

    static constexpr float epsilon() noexcept
	{
        return FLT_EPSILON;
    }

    static constexpr float round_error() noexcept
	{
        return 0.5f;
    }

    static constexpr float infinity() noexcept
	{
        return __builtin_huge_valf();
    }

    static constexpr float quiet_NaN() noexcept
	{
        return __builtin_nanf("");
    }

    static constexpr float signaling_NaN() noexcept
	{
        return __builtin_nansf("");
    }

    static constexpr float denorm_min() noexcept
	{
        return FLT_TRUE_MIN;
    }
};  // END float specialization


// double specialization
template<>
struct numeric_limits<double>
{
    static constexpr float_round_style round_style = RoundToNearest;
    static constexpr float_denorm_style has_denorm = DenormPresent;

    static constexpr bool has_denorm_loss = _CUSTOM_DOUBLE_HAS_DENORM_LOSS;
    static constexpr bool has_infinity = true;
    static constexpr bool has_quiet_NaN = true;
    static constexpr bool has_signaling_NaN = true;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = false;
    static constexpr bool is_iec559 = true;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;

    static constexpr bool traps = _CUSTOM_DOUBLE_TRAPS;
    static constexpr bool tinyness_before = _CUSTOM_DOUBLE_TINYNESS_BEFORE;

    static constexpr int digits = DBL_MANT_DIG;
    static constexpr int digits10 = DBL_DIG;
    static constexpr int max_digits10 = _CUSTOM_MAX_DIGITS10(DBL_MANT_DIG);
    static constexpr int radix = DBL_RADIX;

    static constexpr int min_exponent = DBL_MIN_EXP;
    static constexpr int min_exponent10 = DBL_MIN_10_EXP;
    static constexpr int max_exponent = DBL_MAX_EXP;
    static constexpr int max_exponent10 = DBL_MAX_10_EXP;


    static constexpr double (min)() noexcept
	{
        return DBL_MIN;
    }

    static constexpr double (max)() noexcept
	{
        return DBL_MAX;
    }

    static constexpr double lowest() noexcept
	{
        return -(max)();
    }

    static constexpr double epsilon() noexcept
	{
        return DBL_EPSILON;
    }

    static constexpr double round_error() noexcept
	{
        return 0.5;
    }

    static constexpr double infinity() noexcept
	{
        return __builtin_huge_val();
    }

    static constexpr double quiet_NaN() noexcept
	{
        return __builtin_nan("");
    }

    static constexpr double signaling_NaN() noexcept
	{
        return __builtin_nans("");
    }

    static constexpr double denorm_min() noexcept
	{
        return DBL_TRUE_MIN;
    }
};  // END double specialization


// long double specialization
template<>
struct numeric_limits<long double>
{
    static constexpr float_round_style round_style = RoundToNearest;
    static constexpr float_denorm_style has_denorm = DenormPresent;

    static constexpr bool has_denorm_loss = _CUSTOM_LONG_DOUBLE_HAS_DENORM_LOSS;
    static constexpr bool has_infinity = true;
    static constexpr bool has_quiet_NaN = true;
    static constexpr bool has_signaling_NaN = true;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = false;
    static constexpr bool is_iec559 = true;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;

    static constexpr bool traps = _CUSTOM_LONG_DOUBLE_TRAPS;
    static constexpr bool tinyness_before = _CUSTOM_LONG_DOUBLE_TINYNESS_BEFORE;

    static constexpr int digits = LDBL_MANT_DIG;
    static constexpr int digits10 = LDBL_DIG;
    static constexpr int max_digits10 = _CUSTOM_MAX_DIGITS10(LDBL_MANT_DIG);
    static constexpr int radix = LDBL_RADIX;

    static constexpr int min_exponent = LDBL_MIN_EXP;
    static constexpr int min_exponent10 = LDBL_MIN_10_EXP;
    static constexpr int max_exponent = LDBL_MAX_EXP;
    static constexpr int max_exponent10 = LDBL_MAX_10_EXP;


    static constexpr long double (min)() noexcept
	{
        return LDBL_MIN;
    }

    static constexpr long double (max)() noexcept
	{
        return LDBL_MAX;
    }

    static constexpr long double lowest() noexcept
	{
        return -(max)();
    }

    static constexpr long double epsilon() noexcept
	{
        return LDBL_EPSILON;
    }

    static constexpr long double round_error() noexcept
	{
        return 0.5L;
    }

    static constexpr long double infinity() noexcept
	{
        return __builtin_huge_val();
    }

    static constexpr long double quiet_NaN() noexcept
	{
        return __builtin_nan("");
    }

    static constexpr long double signaling_NaN() noexcept
	{
        return __builtin_nans("");
    }

    static constexpr long double denorm_min() noexcept
	{
        return LDBL_TRUE_MIN;
    }
};  // END long double specialization

CUSTOM_END


#undef _CUSTOM_SIGNED_B
#undef _CUSTOM_DIGITS_B
#undef _CUSTOM_DIGITS10_B

#undef _CUSTOM_SIGNED
#undef _CUSTOM_DIGITS
#undef _CUSTOM_DIGITS10
#undef _CUSTOM_MAX_DIGITS10