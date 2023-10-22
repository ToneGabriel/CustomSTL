#pragma once
#include "Utility.h"
#include "Limits.h"

#include <cmath>

// TODO: check complex traits if they actualy need multiple implementations
CUSTOM_BEGIN

// complex traits implementation
template<class Type, EnableIf_t<IsFloatingPoint_v<Type>, bool> = true>
struct _ComplexTraitsBase
{
    using FloatType = Type;

    static constexpr FloatType epsilon_v() {
        return NumericLimits<FloatType>::epsilon();
    }

    static constexpr FloatType max_v() {
        return (NumericLimits<FloatType>::max)();
    }

    static constexpr FloatType norm_min_v() {
        return (NumericLimits<FloatType>::min)() > 0 ? (NumericLimits<FloatType>::min)() : 0;
    }

    static constexpr FloatType nan_v() {
        return NumericLimits<FloatType>::quiet_NaN();
    }

    static constexpr FloatType inf_v() {
        return NumericLimits<FloatType>::infinity();
    }

    static bool is_inf(FloatType val) {          // test for infinity
        return val == inf_v();
    }

    static constexpr bool is_nan(FloatType val) {
        return val == nan_v();
    }

    static bool signbit(FloatType val) {         // has 3 overloads for all floating-point types
        return ::signbit(val);
    }

    static FloatType abs_v(FloatType val) {
        return ::abs(val);
    }

    static FloatType sin(FloatType val) {
        return ::sin(val);
    }

    static FloatType cos(FloatType val) {
        return ::cos(val);
    }

    static FloatType tan(FloatType val) {
        return ::tan(val);
    }

    static FloatType atan(FloatType val) {
        return ::atan(val);
    }

};  // END _ComplexTraitsBase

struct _ComplexTraitsFloat : _ComplexTraitsBase<float>
{
    using FloatType = _ComplexTraitsBase<float>::FloatType;



};  // END _ComplexTraitsFloat

struct _ComplexTraitsDouble : _ComplexTraitsBase<double>
{
};  // END _ComplexTraitsDouble

struct _ComplexTraitsLongDouble : _ComplexTraitsBase<long double>
{
};  // END _ComplexTraitsLongDouble


// complex traits specialization
template<class Type>
struct ComplexTraits : _ComplexTraitsBase<Type> {};  // cannot use other than specializations below

template<>
struct ComplexTraits<float> : _ComplexTraitsFloat {};

template<>
struct ComplexTraits<double> : _ComplexTraitsDouble {};

template<>
struct ComplexTraits<long double> : _ComplexTraitsLongDouble {};


template<class Type, EnableIf_t<IsFloatingPoint_v<Type>, bool> = true>
class Complex
{
public:
    using TraitsType    = ComplexTraits<Type>;
    using ValueType     = Type;

private:
    enum RepParts
    {
        Real = 0,
        Imag = 1
    };

    ValueType _rep[2];

public:
    // Constructors

    constexpr Complex(  const ValueType& realVal = ValueType(),
                        const ValueType& imagVal = ValueType())
        : _rep{realVal, imagVal} { /*Empty*/ }

    template<class _Type>
    constexpr Complex(const Complex<_Type>& other)
        : _rep{ static_cast<ValueType>(other.real()), static_cast<ValueType>(other.imag()) } { /*Empty*/ }

public:
    // Operators

    // from this type
    constexpr Complex& operator=(const ValueType& val) {
        _rep[Real] = val;
        _rep[Imag] = 0;
        return *this;
    }

    constexpr Complex& operator+=(const ValueType& val) {
        _rep[Real] += val;
        return *this;
    }

    constexpr Complex& operator-=(const ValueType& val) {
        _rep[Real] -= val;
        return *this;
    }

    constexpr Complex& operator*=(const ValueType& val) {
        _rep[Real] *= val;
        _rep[Imag] *= val;
        return *this;
    }

    constexpr Complex& operator/=(const ValueType& val) {
        _rep[Real] /= val;
        _rep[Imag] /= val;
        return *this;
    }
    // END from this type

    // from this complex
    constexpr Complex& operator+=(const Complex& other) {
        _add(other);
        return *this;
    }

    constexpr Complex& operator-=(const Complex& other) {
        _sub(other);
        return *this;
    }

    constexpr Complex& operator*=(const Complex& other) {
        _mul(other);
        return *this;
    }

    constexpr Complex& operator/=(const Complex& other) {
        _div(other);
        return *this;
    }
    // END from this complex

    // from other complex type
    template<class _Type>
    constexpr Complex& operator=(const Complex<_Type>& other) {
        _rep[Real] = static_cast<ValueType>(other.real());
        _rep[Imag] = static_cast<ValueType>(other.imag());
        return *this;
    }

    template<class _Type>
    constexpr Complex& operator+=(const Complex<_Type>& other) {
        _add(other);
        return *this;
    }

    template<class _Type>
    constexpr Complex& operator-=(const Complex<_Type>& other) {
        _sub(other);
        return *this;
    }

    template<class _Type>
    constexpr Complex& operator*=(const Complex<_Type>& other) {
        _mul(other);
        return *this;
    }

    template<class _Type>
    constexpr Complex& operator/=(const Complex<_Type>& other) {
        _div(other);
        return *this;
    }
    // END from other comnplex type

public:
    // Main functions

    constexpr void real(const ValueType& val) { // set real component
        _rep[Real] = val;
    }

    constexpr void imag(const ValueType& val) { // set imaginary component
        _rep[Imag] = val;
    }

    constexpr ValueType real() const { // return real component
        return _rep[Real];
    }

    constexpr ValueType imag() const { // return imaginary component
        return _rep[Imag];
    }

private:
    // Helpers

    template<class _Type>
    constexpr void _add(const Complex<_Type>& other) {
        _rep[Real] += static_cast<ValueType>(other.real());
        _rep[Imag] += static_cast<ValueType>(other.imag());
    }

    template<class _Type>
    constexpr void _sub(const Complex<_Type>& other) {
        _rep[Real] -= static_cast<ValueType>(other.real());
        _rep[Imag] -= static_cast<ValueType>(other.imag());
    }

    template<class _Type>
    constexpr void _mul(const Complex<_Type>& other) {
        ValueType otherReal = static_cast<ValueType>(other.real());
        ValueType otherImag = static_cast<ValueType>(other.imag());

        ValueType temp      = _rep[Real] * otherReal - _rep[Imag] * otherImag;
        _rep[Imag]          = _rep[Real] * otherImag + _rep[Imag] * otherReal;
        _rep[Real]          = temp;
    }

    template<class _Type>
    constexpr void _div(const Complex<_Type>& other) {
        ValueType otherReal = static_cast<ValueType>(other.real());
        ValueType otherImag = static_cast<ValueType>(other.imag());

        if (TraitsType::is_nan(otherReal) || TraitsType::is_nan(otherImag))
        {
            // set NaN result
            _rep[Real] = TraitsType::nan_v();
            _rep[Imag] = _rep[Real];
        }
        else if (TraitsType::abs_v(otherImag) < TraitsType::abs_v(otherReal))
        {
            ValueType _Wr = otherImag / otherReal;
            ValueType _Wd = otherReal + _Wr * otherImag;

            if (TraitsType::is_nan(_Wd) || _Wd == 0)
            {
                // set NaN result
                _rep[Real] = TraitsType::nan_v();
                _rep[Imag] = _rep[Real];
            }
            else
            {
                // compute representable result
                ValueType temp  = (_rep[Real] + _rep[Imag] * _Wr) / _Wd;
                _rep[Imag]      = (_rep[Imag] - _rep[Real] * _Wr) / _Wd;
                _rep[Real]      = temp;
            }
        }
        else if (otherImag == 0) // other._rep[Real] == 0 && other._rep[Imag] == 0
        {
            _rep[Real] /= otherReal;
            _rep[Imag] /= otherReal;
        }
        else // 0 < |other._rep[Real]| <= |other._rep[Imag]|
        {
            ValueType _Wr = otherReal / otherImag;
            ValueType _Wd = otherImag + _Wr * otherReal;

            if (TraitsType::is_nan(_Wd) || _Wd == 0)
            {
                // set NaN result
                _rep[Real] = TraitsType::nan_v();
                _rep[Imag] = _rep[Real];
            }
            else
            {
                // compute representable result
                ValueType temp  = (_rep[Real] * _Wr + _rep[Imag]) / _Wd;
                _rep[Imag]      = (_rep[Imag] * _Wr - _rep[Real]) / _Wd;
                _rep[Real]      = temp;
            }
        }
    }
};  // END Complex common


// Complex unary operators

template<class Type>
constexpr Complex<Type> operator+(const Complex<Type>& left) {
    return left;
}

template<class Type>
constexpr Complex<Type> operator-(const Complex<Type>& left) {
    return Complex<Type>(-left.real(), -left.imag());
}

// Complex binary operators

// add
template<class Type>
constexpr Complex<Type> operator+(const Complex<Type>& left, const Complex<Type>& right) {
    Complex<Type> temp(left);
    temp += right;
    return temp;
}

template<class Type>
constexpr Complex<Type> operator+(const Complex<Type>& left, const Type& val) {
    Complex<Type> temp(left);
    temp += val;
    return temp;
}

template<class Type>
constexpr Complex<Type> operator+(const Type& val, const Complex<Type>& right) {
    Complex<Type> temp(right);
    temp += val;
    return temp;
}

// sub
template<class Type>
constexpr Complex<Type> operator-(const Complex<Type>& left, const Complex<Type>& right) {
    Complex<Type> temp(left);
    temp -= right;
    return temp;
}

template<class Type>
constexpr Complex<Type> operator-(const Complex<Type>& left, const Type& val) {
    Complex<Type> temp(left);
    temp -= val;
    return temp;
}

template<class Type>
constexpr Complex<Type> operator-(const Type& val, const Complex<Type>& right) {
    Complex<Type> temp(val);
    temp -= right;
    return temp;
}

// mul
template<class Type>
constexpr Complex<Type> operator*(const Complex<Type>& left, const Complex<Type>& right) {
    Complex<Type> temp(left);
    temp *= right;
    return temp;
}

template<class Type>
constexpr Complex<Type> operator*(const Complex<Type>& left, const Type& val) {
    Complex<Type> temp(left);
    temp *= val;
    return temp;
}

template<class Type>
constexpr Complex<Type> operator*(const Type& val, const Complex<Type>& right) {
    Complex<Type> temp(right);
    temp *= val;
    return temp;
}

// div
template<class Type>
constexpr Complex<Type> operator/(const Complex<Type>& left, const Complex<Type>& right) {
    Complex<Type> temp(left);
    temp /= right;
    return temp;
}

template<class Type>
constexpr Complex<Type> operator/(const Complex<Type>& left, const Type& val) {
    Complex<Type> temp(left);
    temp /= val;
    return temp;
}

template<class Type>
constexpr Complex<Type> operator/(const Type& val, const Complex<Type>& right) {
    Complex<Type> temp(val);
    temp /= right;
    return temp;
}

// equal
template<class Type>
constexpr bool operator==(const Complex<Type>& left, const Complex<Type>& right) {
    return left.real() == right.real() && left.imag() == right.imag();
}

template<class Type>
constexpr bool operator!=(const Complex<Type>& left, const Complex<Type>& right) {
    return !(left == right);
}

template<class Type>
constexpr bool operator==(const Complex<Type>& left, const Type& val) {
    return left.real() == val && left.imag() == 0;
}

template<class Type>
constexpr bool operator!=(const Complex<Type>& left, const Type& val) {
    return !(left == val);
}

template<class Type>
constexpr bool operator==(const Type& val, const Complex<Type>& right) {
    return val == right.real() && 0 == right.imag();
}

template<class Type>
constexpr bool operator!=(const Type& val, const Complex<Type>& right) {
    return !(val == right);
}

CUSTOM_END