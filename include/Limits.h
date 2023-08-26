#pragma once
#include "xCommon.h"


CUSTOM_BEGIN

enum FloatRoundStyle
{
    Roundindeterminate      = -1, // Intermediate.
    RoundTowardZero         = 0,  // To zero.
    RoundToNearest          = 1,  // To the nearest representable value.
    RoundTowardInfinity     = 2,  // To infinity.
    RoundTowardNegInfinity  = 3   // To negative infinity.
};

enum FloatDenormStyle
{
    DenormIndeterminate = -1,  // Indeterminate at compile time whether denormalized values are allowed.
    DenormAbsent        = 0,   // The type does not allow denormalized values.
    DenormPresent       = 1    // The type allows denormalized values.
};

template<class Type>
struct NumericLimits
{
    // Static parameters

    /* This will be true for all fundamental types (which have
	specializations), and false for everything else.  */
    static constexpr bool IsSpecialized = false;

    static constexpr bool IsSigned = false;

    static constexpr bool IsInteger = false;

    /* True if the type uses an exact representation. All integer types are
	exact, but not all exact types are integer. For example, rational and
	fixed-exponent representations are exact but not integer. */
    static constexpr bool IsExact = false;

    /* The number of @c radix digits that be represented without change:  for
	integer types, the number of non-sign bits in the mantissa; for
	floating types, the number of @c radix digits in the mantissa.  */
    static constexpr int Digits = 0;

    /* The number of base 10 digits that can be represented without change. */
    static constexpr int Digits10 = 0;

    /* The number of base 10 digits required to ensure that values which
	differ are always differentiated.  */
    static constexpr int MaxDigits10 = 0;

    /* For integer types, specifies the base of the representation.  For
	floating types, specifies the base of the exponent representation.  */
    static constexpr int Radix = 0;

    /* The minimum negative integer such that @c radix raised to the power of
	(one less than that integer) is a normalized floating point number.  */
    static constexpr int MinExponent = 0;

    /* The minimum negative integer such that 10 raised to that power is in
	the range of normalized floating point numbers.  */
    static constexpr int MinExponent10 = 0;

    /* The maximum positive integer such that @c radix raised to the power of
	(one less than that integer) is a representable finite floating point
	number.  */
    static constexpr int MaxExponent = 0;

    /* The maximum positive integer such that 10 raised to that power is in
	the range of representable finite floating point numbers.  */
    static constexpr int MaxExponent10 = 0;

    /* True if-and-only-if the type adheres to the IEC 559 standard, also
	known as IEEE 754.  (Only makes sense for floating point types.)  */
    static constexpr bool Is_IEC559 = false;

    /* True if the set of values representable by the type is
	finite.  All built-in types are bounded, this member would be
	false for arbitrary precision types. */
    static constexpr bool IsBounded = false;

    /* True if the type is @e modulo. A type is modulo if, for any
	operation involving +, -, or * on values of that type whose
	result would fall outside the range [min(),max()], the value
	returned differs from the true value by an integer multiple of
	max() - min() + 1. On most machines, this is false for floating
	types, true for unsigned integers, and true for signed integers.
	See PR22200 about signed integers.  */
    static constexpr bool IsModulo = false;

    /* See std::float_denorm_style for more information.  */
    static constexpr FloatDenormStyle HasDenorm = DenormAbsent;

    /* True if loss of accuracy is detected as a denormalization loss,
	rather than as an inexact result. */
    static constexpr bool HasDenormLoss = false;

    /* True if the type has a representation for positive infinity.  */
    static constexpr bool HasInfinity = false;

    /* True if the type has a representation for a quiet (non-signaling)
	Not a Number.  */
    static constexpr bool HasQuietNaN = false;

    /* True if the type has a representation for a signaling
	Not a Number.  */
    static constexpr bool HasSignalingNaN = false;

    /* True if trapping is implemented for this type.  */
    static constexpr bool Traps = false;

    /* True if tininess is detected before rounding.  (see IEC 559)  */
    static constexpr bool TinynessBefore = false;

    /* This is only meaningful for floating types; integer types will all be
	RoundTowardZero.  */
    static constexpr FloatRoundStyle RoundStyle = RoundTowardZero;


    // Main functions

    /* The minimum finite value, or for floating types with
	denormalization, the minimum positive normalized value.  */
    static constexpr Type min() noexcept { 
        return Type();
    }

    /* The maximum finite value.  */
    static constexpr Type max() noexcept { 
        return Type();
    }

    /* A finite value x such that there is no other finite value y
    where y < x.  */
    static constexpr Type lowest() noexcept {
        return Type();
    }

    /* The @e machine @e epsilon:  the difference between 1 and the least
    value greater than 1 that is representable.  */
    static constexpr Type epsilon() noexcept {
        return Type();
    }

    /* The maximum rounding error measurement (see LIA-1).  */
    static constexpr Type round_error() noexcept {
        return Type();
    }

    /* The representation of positive infinity, if @c HasInfinity.  */
    static constexpr Type infinity() noexcept {
        return Type();
    }

    /* The representation of a quiet Not a Number,
    if @c HasQuietNaN. */
    static constexpr Type quiet_NaN() noexcept {
        return Type();
    }

    /* The representation of a signaling Not a Number, if
    @c HasSignalingNaN. */
    static constexpr Type signaling_NaN() noexcept {
        return Type();
    }

    /* The minimum positive denormalized value.  For types where
    @c HasDenorm is false, this is the minimum positive normalized value.  */
    static constexpr Type denorm_min() noexcept {
        return Type();
    }
};

// ignore CV qualifiers
template<typename Type>
struct NumericLimits<const Type> : public NumericLimits<Type> { };

template<typename Type>
struct NumericLimits<volatile Type> : public NumericLimits<Type> { };

template<typename Type>
struct NumericLimits<const volatile Type> : public NumericLimits<Type> { };


// bool specialization
template<>
struct NumericLimits<bool>
{
    // Static parameters

    static constexpr bool IsSpecialized = true;
    static constexpr bool IsSigned      = false;
    static constexpr bool IsInteger     = true;
    static constexpr bool IsExact       = true;

    static constexpr int Digits         = 1;
    static constexpr int Digits10       = 0;
    static constexpr int MaxDigits10    = 0;
    static constexpr int Radix          = 2;

    static constexpr int MinExponent    = 0;
    static constexpr int MinExponent10  = 0;
    static constexpr int MaxExponent    = 0;
    static constexpr int MaxExponent10  = 0;

    static constexpr bool IsIEC559      = false;
    static constexpr bool IsBounded     = true;
    static constexpr bool IsModulo      = false;

    static constexpr FloatDenormStyle HasDenorm = DenormAbsent;
    static constexpr bool HasDenormLoss         = false;
    static constexpr bool HasInfinity           = false;
    static constexpr bool HasQuietNaN           = false;
    static constexpr bool HasSignalingNaN       = false;

    static constexpr bool Traps                 = true;
    static constexpr bool TinynessBefore        = false;
    static constexpr FloatRoundStyle RoundStyle = RoundTowardZero;

    // Main functions

    static constexpr bool min() noexcept {
        return false;
    }

    static constexpr bool max() noexcept {
        return true;
    }

    static constexpr bool lowest() noexcept {
        return min();
    }

    static constexpr bool epsilon() noexcept {
        return false;
    }

    static constexpr bool round_error() noexcept {
        return false;
    }

    static constexpr bool infinity() noexcept {
        return false;
    }

    static constexpr bool quiet_NaN() noexcept {
        return false;
    }

    static constexpr bool signaling_NaN() noexcept {
        return false;
    }

    static constexpr bool denorm_min() noexcept {
        return false;
    }
};  // END bool specialization


// // char specialization
// template<>
// struct NumericLimits<char>
// {
//     static constexpr bool IsSpecialized = true;

//     //static constexpr int Digits = __glibcxx_digits (char);
//     //static constexpr int Digits10 = __glibcxx_digits10 (char);
//     static constexpr int MaxDigits10 = 0;

//     //static constexpr bool IsSigned = __glibcxx_signed (char);
//     static constexpr bool IsInteger = true;
//     static constexpr bool IsExact = true;
//     static constexpr int Radix = 2;

//     static constexpr int MinExponent = 0;
//     static constexpr int MinExponent10 = 0;
//     static constexpr int MaxExponent = 0;
//     static constexpr int MaxExponent10 = 0;

//     static constexpr bool HasInfinity = false;
//     static constexpr bool HasQuietNaN = false;
//     static constexpr bool HasSignalingNaN = false;
//     static constexpr FloatDenormStyle HasDenorm = DenormAbsent;
//     static constexpr bool HasDenormLoss = false;

//     static constexpr bool IsIEC559 = false;
//     static constexpr bool IsBounded = true;
//     static constexpr bool IsModulo = !IsSigned;

//     static constexpr bool Traps = true;
//     static constexpr bool TinynessBefore = false;
//     static constexpr FloatRoundStyle RoundStyle = RoundTowardZero;

//     static constexpr char min() noexcept {
//         return 0; //__glibcxx_min(char);
//     }

//     static constexpr char max() noexcept {
//         return 0; //__glibcxx_max(char);
//     }

//     static constexpr char lowest() noexcept {
//         return min();
//     }

//     static constexpr char epsilon() noexcept {
//         return 0;
//     }

//     static constexpr char round_error() noexcept {
//         return 0;
//     }

//     static constexpr char infinity() noexcept {
//         return char();
//     }

//     static constexpr char quiet_NaN() noexcept {
//         return char();
//     }

//     static constexpr char signaling_NaN() noexcept {
//         return char();
//     }

//     static constexpr char denorm_min() noexcept {
//         return static_cast<char>(0);
//     }
// };


// // NumericLimits<signed char> specialization.
// template<>
// struct NumericLimits<signed char>
// {
//     static constexpr bool IsSpecialized = true;

//     static constexpr signed char
//     min() noexcept { return -__SCHAR_MAX__ - 1; }

//     static constexpr signed char
//     max() noexcept { return __SCHAR_MAX__; }

// #if __cplusplus >= 201103L
//     static constexpr signed char
//     lowest() noexcept { return min(); }
// #endif

//     static constexpr int Digits = __glibcxx_digits (signed char);
//     static constexpr int Digits10
//     = __glibcxx_digits10 (signed char);
// #if __cplusplus >= 201103L
//     static constexpr int MaxDigits10 = 0;
// #endif
//     static constexpr bool IsSigned = true;
//     static constexpr bool IsInteger = true;
//     static constexpr bool IsExact = true;
//     static constexpr int Radix = 2;

//     static constexpr signed char
//     epsilon() noexcept { return 0; }

//     static constexpr signed char
//     round_error() noexcept { return 0; }

//     static constexpr int MinExponent = 0;
//     static constexpr int MinExponent10 = 0;
//     static constexpr int MaxExponent = 0;
//     static constexpr int MaxExponent10 = 0;

//     static constexpr bool HasInfinity = false;
//     static constexpr bool HasQuietNaN = false;
//     static constexpr bool HasSignalingNaN = false;
//     static constexpr FloatDenormStyle HasDenorm
//     = DenormAbsent;
//     static constexpr bool HasDenormLoss = false;

//     static constexpr signed char
//     infinity() noexcept { return static_cast<signed char>(0); }

//     static constexpr signed char
//     quiet_NaN() noexcept { return static_cast<signed char>(0); }

//     static constexpr signed char
//     signaling_NaN() noexcept
//     { return static_cast<signed char>(0); }

//     static constexpr signed char
//     denorm_min() noexcept
//     { return static_cast<signed char>(0); }

//     static constexpr bool IsIEC559 = false;
//     static constexpr bool IsBounded = true;
//     static constexpr bool IsModulo = false;

//     static constexpr bool Traps = true;
//     static constexpr bool TinynessBefore = false;
//     static constexpr FloatRoundStyle RoundStyle
//     = RoundTowardZero;
// };

// /// NumericLimits<unsigned char> specialization.
// template<>
// struct NumericLimits<unsigned char>
// {
//     static constexpr bool IsSpecialized = true;

//     static constexpr unsigned char
//     min() noexcept { return 0; }

//     static constexpr unsigned char
//     max() noexcept { return __SCHAR_MAX__ * 2U + 1; }

// #if __cplusplus >= 201103L
//     static constexpr unsigned char
//     lowest() noexcept { return min(); }
// #endif

//     static constexpr int Digits
//     = __glibcxx_digits (unsigned char);
//     static constexpr int Digits10
//     = __glibcxx_digits10 (unsigned char);
// #if __cplusplus >= 201103L
//     static constexpr int MaxDigits10 = 0;
// #endif
//     static constexpr bool IsSigned = false;
//     static constexpr bool IsInteger = true;
//     static constexpr bool IsExact = true;
//     static constexpr int Radix = 2;

//     static constexpr unsigned char
//     epsilon() noexcept { return 0; }

//     static constexpr unsigned char
//     round_error() noexcept { return 0; }

//     static constexpr int MinExponent = 0;
//     static constexpr int MinExponent10 = 0;
//     static constexpr int MaxExponent = 0;
//     static constexpr int MaxExponent10 = 0;

//     static constexpr bool HasInfinity = false;
//     static constexpr bool HasQuietNaN = false;
//     static constexpr bool HasSignalingNaN = false;
//     static constexpr FloatDenormStyle HasDenorm
//     = DenormAbsent;
//     static constexpr bool HasDenormLoss = false;

//     static constexpr unsigned char
//     infinity() noexcept
//     { return static_cast<unsigned char>(0); }

//     static constexpr unsigned char
//     quiet_NaN() noexcept
//     { return static_cast<unsigned char>(0); }

//     static constexpr unsigned char
//     signaling_NaN() noexcept
//     { return static_cast<unsigned char>(0); }

//     static constexpr unsigned char
//     denorm_min() noexcept
//     { return static_cast<unsigned char>(0); }

//     static constexpr bool IsIEC559 = false;
//     static constexpr bool IsBounded = true;
//     static constexpr bool IsModulo = true;

//     static constexpr bool Traps = true;
//     static constexpr bool TinynessBefore = false;
//     static constexpr FloatRoundStyle RoundStyle
//     = RoundTowardZero;
// };

// /// NumericLimits<wchar_t> specialization.
// template<>
// struct NumericLimits<wchar_t>
// {
//     static constexpr bool IsSpecialized = true;

//     static constexpr wchar_t
//     min() noexcept { return __glibcxx_min (wchar_t); }

//     static constexpr wchar_t
//     max() noexcept { return __glibcxx_max (wchar_t); }

// #if __cplusplus >= 201103L
//     static constexpr wchar_t
//     lowest() noexcept { return min(); }
// #endif

//     static constexpr int Digits = __glibcxx_digits (wchar_t);
//     static constexpr int Digits10
//     = __glibcxx_digits10 (wchar_t);
// #if __cplusplus >= 201103L
//     static constexpr int MaxDigits10 = 0;
// #endif
//     static constexpr bool IsSigned = __glibcxx_signed (wchar_t);
//     static constexpr bool IsInteger = true;
//     static constexpr bool IsExact = true;
//     static constexpr int Radix = 2;

//     static constexpr wchar_t
//     epsilon() noexcept { return 0; }

//     static constexpr wchar_t
//     round_error() noexcept { return 0; }

//     static constexpr int MinExponent = 0;
//     static constexpr int MinExponent10 = 0;
//     static constexpr int MaxExponent = 0;
//     static constexpr int MaxExponent10 = 0;

//     static constexpr bool HasInfinity = false;
//     static constexpr bool HasQuietNaN = false;
//     static constexpr bool HasSignalingNaN = false;
//     static constexpr FloatDenormStyle HasDenorm
//     = DenormAbsent;
//     static constexpr bool HasDenormLoss = false;

//     static constexpr wchar_t
//     infinity() noexcept { return wchar_t(); }

//     static constexpr wchar_t
//     quiet_NaN() noexcept { return wchar_t(); }

//     static constexpr wchar_t
//     signaling_NaN() noexcept { return wchar_t(); }

//     static constexpr wchar_t
//     denorm_min() noexcept { return wchar_t(); }

//     static constexpr bool IsIEC559 = false;
//     static constexpr bool IsBounded = true;
//     static constexpr bool IsModulo = !IsSigned;

//     static constexpr bool Traps = true;
//     static constexpr bool TinynessBefore = false;
//     static constexpr FloatRoundStyle RoundStyle
//     = RoundTowardZero;
// };

// #if _GLIBCXX_USE_CHAR8_T
// /// NumericLimits<char8_t> specialization.
// template<>
// struct NumericLimits<char8_t>
// {
//     static constexpr bool IsSpecialized = true;

//     static constexpr char8_t
//     min() noexcept { return __glibcxx_min (char8_t); }

//     static constexpr char8_t
//     max() noexcept { return __glibcxx_max (char8_t); }

//     static constexpr char8_t
//     lowest() noexcept { return min(); }

//     static constexpr int Digits = __glibcxx_digits (char8_t);
//     static constexpr int Digits10 = __glibcxx_digits10 (char8_t);
//     static constexpr int MaxDigits10 = 0;
//     static constexpr bool IsSigned = __glibcxx_signed (char8_t);
//     static constexpr bool IsInteger = true;
//     static constexpr bool IsExact = true;
//     static constexpr int Radix = 2;

//     static constexpr char8_t
//     epsilon() noexcept { return 0; }

//     static constexpr char8_t
//     round_error() noexcept { return 0; }

//     static constexpr int MinExponent = 0;
//     static constexpr int MinExponent10 = 0;
//     static constexpr int MaxExponent = 0;
//     static constexpr int MaxExponent10 = 0;

//     static constexpr bool HasInfinity = false;
//     static constexpr bool HasQuietNaN = false;
//     static constexpr bool HasSignalingNaN = false;
//     static constexpr FloatDenormStyle HasDenorm
// = DenormAbsent;
//     static constexpr bool HasDenormLoss = false;

//     static constexpr char8_t
//     infinity() noexcept { return char8_t(); }

//     static constexpr char8_t
//     quiet_NaN() noexcept { return char8_t(); }

//     static constexpr char8_t
//     signaling_NaN() noexcept { return char8_t(); }

//     static constexpr char8_t
//     denorm_min() noexcept { return char8_t(); }

//     static constexpr bool IsIEC559 = false;
//     static constexpr bool IsBounded = true;
//     static constexpr bool IsModulo = !IsSigned;

//     static constexpr bool Traps = true;
//     static constexpr bool TinynessBefore = false;
//     static constexpr FloatRoundStyle RoundStyle
// = RoundTowardZero;
// };
// #endif

// #if __cplusplus >= 201103L
// /// NumericLimits<char16_t> specialization.
// template<>
// struct NumericLimits<char16_t>
// {
//     static constexpr bool IsSpecialized = true;

//     static constexpr char16_t
//     min() noexcept { return __glibcxx_min (char16_t); }

//     static constexpr char16_t
//     max() noexcept { return __glibcxx_max (char16_t); }

//     static constexpr char16_t
//     lowest() noexcept { return min(); }

//     static constexpr int Digits = __glibcxx_digits (char16_t);
//     static constexpr int Digits10 = __glibcxx_digits10 (char16_t);
//     static constexpr int MaxDigits10 = 0;
//     static constexpr bool IsSigned = __glibcxx_signed (char16_t);
//     static constexpr bool IsInteger = true;
//     static constexpr bool IsExact = true;
//     static constexpr int Radix = 2;

//     static constexpr char16_t
//     epsilon() noexcept { return 0; }

//     static constexpr char16_t
//     round_error() noexcept { return 0; }

//     static constexpr int MinExponent = 0;
//     static constexpr int MinExponent10 = 0;
//     static constexpr int MaxExponent = 0;
//     static constexpr int MaxExponent10 = 0;

//     static constexpr bool HasInfinity = false;
//     static constexpr bool HasQuietNaN = false;
//     static constexpr bool HasSignalingNaN = false;
//     static constexpr FloatDenormStyle HasDenorm = DenormAbsent;
//     static constexpr bool HasDenormLoss = false;

//     static constexpr char16_t
//     infinity() noexcept { return char16_t(); }

//     static constexpr char16_t
//     quiet_NaN() noexcept { return char16_t(); }

//     static constexpr char16_t
//     signaling_NaN() noexcept { return char16_t(); }

//     static constexpr char16_t
//     denorm_min() noexcept { return char16_t(); }

//     static constexpr bool IsIEC559 = false;
//     static constexpr bool IsBounded = true;
//     static constexpr bool IsModulo = !IsSigned;

//     static constexpr bool Traps = true;
//     static constexpr bool TinynessBefore = false;
//     static constexpr FloatRoundStyle RoundStyle = RoundTowardZero;
// };

// /// NumericLimits<char32_t> specialization.
// template<>
// struct NumericLimits<char32_t>
// {
//     static constexpr bool IsSpecialized = true;

//     static constexpr char32_t
//     min() noexcept { return __glibcxx_min (char32_t); }

//     static constexpr char32_t
//     max() noexcept { return __glibcxx_max (char32_t); }

//     static constexpr char32_t
//     lowest() noexcept { return min(); }

//     static constexpr int Digits = __glibcxx_digits (char32_t);
//     static constexpr int Digits10 = __glibcxx_digits10 (char32_t);
//     static constexpr int MaxDigits10 = 0;
//     static constexpr bool IsSigned = __glibcxx_signed (char32_t);
//     static constexpr bool IsInteger = true;
//     static constexpr bool IsExact = true;
//     static constexpr int Radix = 2;

//     static constexpr char32_t
//     epsilon() noexcept { return 0; }

//     static constexpr char32_t
//     round_error() noexcept { return 0; }

//     static constexpr int MinExponent = 0;
//     static constexpr int MinExponent10 = 0;
//     static constexpr int MaxExponent = 0;
//     static constexpr int MaxExponent10 = 0;

//     static constexpr bool HasInfinity = false;
//     static constexpr bool HasQuietNaN = false;
//     static constexpr bool HasSignalingNaN = false;
//     static constexpr FloatDenormStyle HasDenorm = DenormAbsent;
//     static constexpr bool HasDenormLoss = false;

//     static constexpr char32_t
//     infinity() noexcept { return char32_t(); }

//     static constexpr char32_t
//     quiet_NaN() noexcept { return char32_t(); }

//     static constexpr char32_t
//     signaling_NaN() noexcept { return char32_t(); }

//     static constexpr char32_t
//     denorm_min() noexcept { return char32_t(); }

//     static constexpr bool IsIEC559 = false;
//     static constexpr bool IsBounded = true;
//     static constexpr bool IsModulo = !IsSigned;

//     static constexpr bool Traps = true;
//     static constexpr bool TinynessBefore = false;
//     static constexpr FloatRoundStyle RoundStyle = RoundTowardZero;
// };
// #endif

// /// NumericLimits<short> specialization.
// template<>
// struct NumericLimits<short>
// {
//     static constexpr bool IsSpecialized = true;

//     static constexpr short
//     min() noexcept { return -__SHRT_MAX__ - 1; }

//     static constexpr short
//     max() noexcept { return __SHRT_MAX__; }

// #if __cplusplus >= 201103L
//     static constexpr short
//     lowest() noexcept { return min(); }
// #endif

//     static constexpr int Digits = __glibcxx_digits (short);
//     static constexpr int Digits10 = __glibcxx_digits10 (short);
// #if __cplusplus >= 201103L
//     static constexpr int MaxDigits10 = 0;
// #endif
//     static constexpr bool IsSigned = true;
//     static constexpr bool IsInteger = true;
//     static constexpr bool IsExact = true;
//     static constexpr int Radix = 2;

//     static constexpr short
//     epsilon() noexcept { return 0; }

//     static constexpr short
//     round_error() noexcept { return 0; }

//     static constexpr int MinExponent = 0;
//     static constexpr int MinExponent10 = 0;
//     static constexpr int MaxExponent = 0;
//     static constexpr int MaxExponent10 = 0;

//     static constexpr bool HasInfinity = false;
//     static constexpr bool HasQuietNaN = false;
//     static constexpr bool HasSignalingNaN = false;
//     static constexpr FloatDenormStyle HasDenorm
//     = DenormAbsent;
//     static constexpr bool HasDenormLoss = false;

//     static constexpr short
//     infinity() noexcept { return short(); }

//     static constexpr short
//     quiet_NaN() noexcept { return short(); }

//     static constexpr short
//     signaling_NaN() noexcept { return short(); }

//     static constexpr short
//     denorm_min() noexcept { return short(); }

//     static constexpr bool IsIEC559 = false;
//     static constexpr bool IsBounded = true;
//     static constexpr bool IsModulo = false;

//     static constexpr bool Traps = true;
//     static constexpr bool TinynessBefore = false;
//     static constexpr FloatRoundStyle RoundStyle
//     = RoundTowardZero;
// };

// /// NumericLimits<unsigned short> specialization.
// template<>
// struct NumericLimits<unsigned short>
// {
//     static constexpr bool IsSpecialized = true;

//     static constexpr unsigned short
//     min() noexcept { return 0; }

//     static constexpr unsigned short
//     max() noexcept { return __SHRT_MAX__ * 2U + 1; }

// #if __cplusplus >= 201103L
//     static constexpr unsigned short
//     lowest() noexcept { return min(); }
// #endif

//     static constexpr int Digits
//     = __glibcxx_digits (unsigned short);
//     static constexpr int Digits10
//     = __glibcxx_digits10 (unsigned short);
// #if __cplusplus >= 201103L
//     static constexpr int MaxDigits10 = 0;
// #endif
//     static constexpr bool IsSigned = false;
//     static constexpr bool IsInteger = true;
//     static constexpr bool IsExact = true;
//     static constexpr int Radix = 2;

//     static constexpr unsigned short
//     epsilon() noexcept { return 0; }

//     static constexpr unsigned short
//     round_error() noexcept { return 0; }

//     static constexpr int MinExponent = 0;
//     static constexpr int MinExponent10 = 0;
//     static constexpr int MaxExponent = 0;
//     static constexpr int MaxExponent10 = 0;

//     static constexpr bool HasInfinity = false;
//     static constexpr bool HasQuietNaN = false;
//     static constexpr bool HasSignalingNaN = false;
//     static constexpr FloatDenormStyle HasDenorm
//     = DenormAbsent;
//     static constexpr bool HasDenormLoss = false;

//     static constexpr unsigned short
//     infinity() noexcept
//     { return static_cast<unsigned short>(0); }

//     static constexpr unsigned short
//     quiet_NaN() noexcept
//     { return static_cast<unsigned short>(0); }

//     static constexpr unsigned short
//     signaling_NaN() noexcept
//     { return static_cast<unsigned short>(0); }

//     static constexpr unsigned short
//     denorm_min() noexcept
//     { return static_cast<unsigned short>(0); }

//     static constexpr bool IsIEC559 = false;
//     static constexpr bool IsBounded = true;
//     static constexpr bool IsModulo = true;

//     static constexpr bool Traps = true;
//     static constexpr bool TinynessBefore = false;
//     static constexpr FloatRoundStyle RoundStyle
//     = RoundTowardZero;
// };

// /// NumericLimits<int> specialization.
// template<>
// struct NumericLimits<int>
// {
//     static constexpr bool IsSpecialized = true;

//     static constexpr int
//     min() noexcept { return -__INT_MAX__ - 1; }

//     static constexpr int
//     max() noexcept { return __INT_MAX__; }

// #if __cplusplus >= 201103L
//     static constexpr int
//     lowest() noexcept { return min(); }
// #endif

//     static constexpr int Digits = __glibcxx_digits (int);
//     static constexpr int Digits10 = __glibcxx_digits10 (int);
// #if __cplusplus >= 201103L
//     static constexpr int MaxDigits10 = 0;
// #endif
//     static constexpr bool IsSigned = true;
//     static constexpr bool IsInteger = true;
//     static constexpr bool IsExact = true;
//     static constexpr int Radix = 2;

//     static constexpr int
//     epsilon() noexcept { return 0; }

//     static constexpr int
//     round_error() noexcept { return 0; }

//     static constexpr int MinExponent = 0;
//     static constexpr int MinExponent10 = 0;
//     static constexpr int MaxExponent = 0;
//     static constexpr int MaxExponent10 = 0;

//     static constexpr bool HasInfinity = false;
//     static constexpr bool HasQuietNaN = false;
//     static constexpr bool HasSignalingNaN = false;
//     static constexpr FloatDenormStyle HasDenorm
//     = DenormAbsent;
//     static constexpr bool HasDenormLoss = false;

//     static constexpr int
//     infinity() noexcept { return static_cast<int>(0); }

//     static constexpr int
//     quiet_NaN() noexcept { return static_cast<int>(0); }

//     static constexpr int
//     signaling_NaN() noexcept { return static_cast<int>(0); }

//     static constexpr int
//     denorm_min() noexcept { return static_cast<int>(0); }

//     static constexpr bool IsIEC559 = false;
//     static constexpr bool IsBounded = true;
//     static constexpr bool IsModulo = false;

//     static constexpr bool Traps = true;
//     static constexpr bool TinynessBefore = false;
//     static constexpr FloatRoundStyle RoundStyle
//     = RoundTowardZero;
// };

// /// NumericLimits<unsigned int> specialization.
// template<>
// struct NumericLimits<unsigned int>
// {
//     static constexpr bool IsSpecialized = true;

//     static constexpr unsigned int
//     min() noexcept { return 0; }

//     static constexpr unsigned int
//     max() noexcept { return __INT_MAX__ * 2U + 1; }

// #if __cplusplus >= 201103L
//     static constexpr unsigned int
//     lowest() noexcept { return min(); }
// #endif

//     static constexpr int Digits
//     = __glibcxx_digits (unsigned int);
//     static constexpr int Digits10
//     = __glibcxx_digits10 (unsigned int);
// #if __cplusplus >= 201103L
//     static constexpr int MaxDigits10 = 0;
// #endif
//     static constexpr bool IsSigned = false;
//     static constexpr bool IsInteger = true;
//     static constexpr bool IsExact = true;
//     static constexpr int Radix = 2;

//     static constexpr unsigned int
//     epsilon() noexcept { return 0; }

//     static constexpr unsigned int
//     round_error() noexcept { return 0; }

//     static constexpr int MinExponent = 0;
//     static constexpr int MinExponent10 = 0;
//     static constexpr int MaxExponent = 0;
//     static constexpr int MaxExponent10 = 0;

//     static constexpr bool HasInfinity = false;
//     static constexpr bool HasQuietNaN = false;
//     static constexpr bool HasSignalingNaN = false;
//     static constexpr FloatDenormStyle HasDenorm
//     = DenormAbsent;
//     static constexpr bool HasDenormLoss = false;

//     static constexpr unsigned int
//     infinity() noexcept { return static_cast<unsigned int>(0); }

//     static constexpr unsigned int
//     quiet_NaN() noexcept
//     { return static_cast<unsigned int>(0); }

//     static constexpr unsigned int
//     signaling_NaN() noexcept
//     { return static_cast<unsigned int>(0); }

//     static constexpr unsigned int
//     denorm_min() noexcept
//     { return static_cast<unsigned int>(0); }

//     static constexpr bool IsIEC559 = false;
//     static constexpr bool IsBounded = true;
//     static constexpr bool IsModulo = true;

//     static constexpr bool Traps = true;
//     static constexpr bool TinynessBefore = false;
//     static constexpr FloatRoundStyle RoundStyle
//     = RoundTowardZero;
// };

// /// NumericLimits<long> specialization.
// template<>
// struct NumericLimits<long>
// {
//     static constexpr bool IsSpecialized = true;

//     static constexpr long
//     min() noexcept { return -__LONG_MAX__ - 1; }

//     static constexpr long
//     max() noexcept { return __LONG_MAX__; }

// #if __cplusplus >= 201103L
//     static constexpr long
//     lowest() noexcept { return min(); }
// #endif

//     static constexpr int Digits = __glibcxx_digits (long);
//     static constexpr int Digits10 = __glibcxx_digits10 (long);
// #if __cplusplus >= 201103L
//     static constexpr int MaxDigits10 = 0;
// #endif
//     static constexpr bool IsSigned = true;
//     static constexpr bool IsInteger = true;
//     static constexpr bool IsExact = true;
//     static constexpr int Radix = 2;

//     static constexpr long
//     epsilon() noexcept { return 0; }

//     static constexpr long
//     round_error() noexcept { return 0; }

//     static constexpr int MinExponent = 0;
//     static constexpr int MinExponent10 = 0;
//     static constexpr int MaxExponent = 0;
//     static constexpr int MaxExponent10 = 0;

//     static constexpr bool HasInfinity = false;
//     static constexpr bool HasQuietNaN = false;
//     static constexpr bool HasSignalingNaN = false;
//     static constexpr FloatDenormStyle HasDenorm
//     = DenormAbsent;
//     static constexpr bool HasDenormLoss = false;

//     static constexpr long
//     infinity() noexcept { return static_cast<long>(0); }

//     static constexpr long
//     quiet_NaN() noexcept { return static_cast<long>(0); }

//     static constexpr long
//     signaling_NaN() noexcept { return static_cast<long>(0); }

//     static constexpr long
//     denorm_min() noexcept { return static_cast<long>(0); }

//     static constexpr bool IsIEC559 = false;
//     static constexpr bool IsBounded = true;
//     static constexpr bool IsModulo = false;

//     static constexpr bool Traps = true;
//     static constexpr bool TinynessBefore = false;
//     static constexpr FloatRoundStyle RoundStyle
//     = RoundTowardZero;
// };

// /// NumericLimits<unsigned long> specialization.
// template<>
// struct NumericLimits<unsigned long>
// {
//     static constexpr bool IsSpecialized = true;

//     static constexpr unsigned long
//     min() noexcept { return 0; }

//     static constexpr unsigned long
//     max() noexcept { return __LONG_MAX__ * 2UL + 1; }

// #if __cplusplus >= 201103L
//     static constexpr unsigned long
//     lowest() noexcept { return min(); }
// #endif

//     static constexpr int Digits
//     = __glibcxx_digits (unsigned long);
//     static constexpr int Digits10
//     = __glibcxx_digits10 (unsigned long);
// #if __cplusplus >= 201103L
//     static constexpr int MaxDigits10 = 0;
// #endif
//     static constexpr bool IsSigned = false;
//     static constexpr bool IsInteger = true;
//     static constexpr bool IsExact = true;
//     static constexpr int Radix = 2;

//     static constexpr unsigned long
//     epsilon() noexcept { return 0; }

//     static constexpr unsigned long
//     round_error() noexcept { return 0; }

//     static constexpr int MinExponent = 0;
//     static constexpr int MinExponent10 = 0;
//     static constexpr int MaxExponent = 0;
//     static constexpr int MaxExponent10 = 0;

//     static constexpr bool HasInfinity = false;
//     static constexpr bool HasQuietNaN = false;
//     static constexpr bool HasSignalingNaN = false;
//     static constexpr FloatDenormStyle HasDenorm
//     = DenormAbsent;
//     static constexpr bool HasDenormLoss = false;

//     static constexpr unsigned long
//     infinity() noexcept
//     { return static_cast<unsigned long>(0); }

//     static constexpr unsigned long
//     quiet_NaN() noexcept
//     { return static_cast<unsigned long>(0); }

//     static constexpr unsigned long
//     signaling_NaN() noexcept
//     { return static_cast<unsigned long>(0); }

//     static constexpr unsigned long
//     denorm_min() noexcept
//     { return static_cast<unsigned long>(0); }

//     static constexpr bool IsIEC559 = false;
//     static constexpr bool IsBounded = true;
//     static constexpr bool IsModulo = true;

//     static constexpr bool Traps = true;
//     static constexpr bool TinynessBefore = false;
//     static constexpr FloatRoundStyle RoundStyle
//     = RoundTowardZero;
// };

// /// NumericLimits<long long> specialization.
// template<>
// struct NumericLimits<long long>
// {
//     static constexpr bool IsSpecialized = true;

//     static constexpr long long
//     min() noexcept { return -__LONG_LONG_MAX__ - 1; }

//     static constexpr long long
//     max() noexcept { return __LONG_LONG_MAX__; }

// #if __cplusplus >= 201103L
//     static constexpr long long
//     lowest() noexcept { return min(); }
// #endif

//     static constexpr int Digits
//     = __glibcxx_digits (long long);
//     static constexpr int Digits10
//     = __glibcxx_digits10 (long long);
// #if __cplusplus >= 201103L
//     static constexpr int MaxDigits10 = 0;
// #endif
//     static constexpr bool IsSigned = true;
//     static constexpr bool IsInteger = true;
//     static constexpr bool IsExact = true;
//     static constexpr int Radix = 2;

//     static constexpr long long
//     epsilon() noexcept { return 0; }

//     static constexpr long long
//     round_error() noexcept { return 0; }

//     static constexpr int MinExponent = 0;
//     static constexpr int MinExponent10 = 0;
//     static constexpr int MaxExponent = 0;
//     static constexpr int MaxExponent10 = 0;

//     static constexpr bool HasInfinity = false;
//     static constexpr bool HasQuietNaN = false;
//     static constexpr bool HasSignalingNaN = false;
//     static constexpr FloatDenormStyle HasDenorm
//     = DenormAbsent;
//     static constexpr bool HasDenormLoss = false;

//     static constexpr long long
//     infinity() noexcept { return static_cast<long long>(0); }

//     static constexpr long long
//     quiet_NaN() noexcept { return static_cast<long long>(0); }

//     static constexpr long long
//     signaling_NaN() noexcept
//     { return static_cast<long long>(0); }

//     static constexpr long long
//     denorm_min() noexcept { return static_cast<long long>(0); }

//     static constexpr bool IsIEC559 = false;
//     static constexpr bool IsBounded = true;
//     static constexpr bool IsModulo = false;

//     static constexpr bool Traps = true;
//     static constexpr bool TinynessBefore = false;
//     static constexpr FloatRoundStyle RoundStyle
//     = RoundTowardZero;
// };

// /// NumericLimits<unsigned long long> specialization.
// template<>
// struct NumericLimits<unsigned long long>
// {
//     static constexpr bool IsSpecialized = true;

//     static constexpr unsigned long long
//     min() noexcept { return 0; }

//     static constexpr unsigned long long
//     max() noexcept { return __LONG_LONG_MAX__ * 2ULL + 1; }

// #if __cplusplus >= 201103L
//     static constexpr unsigned long long
//     lowest() noexcept { return min(); }
// #endif

//     static constexpr int Digits
//     = __glibcxx_digits (unsigned long long);
//     static constexpr int Digits10
//     = __glibcxx_digits10 (unsigned long long);
// #if __cplusplus >= 201103L
//     static constexpr int MaxDigits10 = 0;
// #endif
//     static constexpr bool IsSigned = false;
//     static constexpr bool IsInteger = true;
//     static constexpr bool IsExact = true;
//     static constexpr int Radix = 2;

//     static constexpr unsigned long long
//     epsilon() noexcept { return 0; }

//     static constexpr unsigned long long
//     round_error() noexcept { return 0; }

//     static constexpr int MinExponent = 0;
//     static constexpr int MinExponent10 = 0;
//     static constexpr int MaxExponent = 0;
//     static constexpr int MaxExponent10 = 0;

//     static constexpr bool HasInfinity = false;
//     static constexpr bool HasQuietNaN = false;
//     static constexpr bool HasSignalingNaN = false;
//     static constexpr FloatDenormStyle HasDenorm
//     = DenormAbsent;
//     static constexpr bool HasDenormLoss = false;

//     static constexpr unsigned long long
//     infinity() noexcept
//     { return static_cast<unsigned long long>(0); }

//     static constexpr unsigned long long
//     quiet_NaN() noexcept
//     { return static_cast<unsigned long long>(0); }

//     static constexpr unsigned long long
//     signaling_NaN() noexcept
//     { return static_cast<unsigned long long>(0); }

//     static constexpr unsigned long long
//     denorm_min() noexcept
//     { return static_cast<unsigned long long>(0); }

//     static constexpr bool IsIEC559 = false;
//     static constexpr bool IsBounded = true;
//     static constexpr bool IsModulo = true;

//     static constexpr bool Traps = true;
//     static constexpr bool TinynessBefore = false;
//     static constexpr FloatRoundStyle RoundStyle
//     = RoundTowardZero;
// };

CUSTOM_END