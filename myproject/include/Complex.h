#pragma once
#include "Utility.h"
#include "Limits.h"

#include <cmath>

// TODO: implement
CUSTOM_BEGIN

// complex traits implementation
template<class Type, EnableIf_t<IsFloatingPoint_v<Type>, bool> = true>
struct _ComplexTraitsBase
{
    using FloatType = Type;

    static constexpr Type _epsilon() {
        return NumericLimits<Type>::epsilon();
    }

    static constexpr Type _max() {
        return (NumericLimits<Type>::max)();
    }

    static constexpr Type _norm_min() {
        return (NumericLimits<Type>::min)() > 0 ? (NumericLimits<Type>::min)() : 0;
    }

    static constexpr Type _inf() {
        return NumericLimits<Type>::infinity();
    }
};  // END _ComplexTraitsBase

struct _ComplexTraitsFloat : _ComplexTraitsBase<float>
{
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
    enum RepParts : int
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
        : _rep{ static_cast<ValueType>(other._rep[Real]),
                static_cast<ValueType>(other._rep[Imag]) } { /*Empty*/ }

public:
    // Operators

    template<class _Type>
    constexpr Complex& operator=(const Complex<_Type>& other) {
        _rep[Real] = static_cast<ValueType>(other._rep[Real]);
        _rep[Imag] = static_cast<ValueType>(other._rep[Imag]);
        return *this;
    }

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
        _rep[Real] += static_cast<ValueType>(other._rep[Real]);
        _rep[Imag] += static_cast<ValueType>(other._rep[Imag]);
    }

    template<class _Type>
    constexpr void _sub(const Complex<_Type>& other) {
        _rep[Real] -= static_cast<ValueType>(other._rep[Real]);
        _rep[Imag] -= static_cast<ValueType>(other._rep[Imag]);
    }

    template<class _Type>
    constexpr void _mul(const Complex<_Type>& other) {
        ValueType otherReal = static_cast<ValueType>(other._rep[Real]);
        ValueType otherImag = static_cast<ValueType>(other._rep[Imag]);

        ValueType temp      = _rep[Real] * otherReal - _rep[Imag] * otherImag;
        _rep[Imag]          = _rep[Real] * otherImag + _rep[Imag] * otherReal;
        _rep[Real]          = temp;
    }

    template<class _Type>
    constexpr void _div(const Complex<_Type>& other) {
        ValueType otherReal = static_cast<ValueType>(other._rep[Real]);
        ValueType otherImag = static_cast<ValueType>(other._rep[Imag]);
        // TODO: implement
    }

};  // END Complex common

CUSTOM_END