#pragma once
#include "Utility.h"
#include "Limits.h"

#include <cmath>

// TODO: implement
CUSTOM_BEGIN

// complex traits implementation
template<class Type>
class _ComplexTraits
{
public:

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
};  // END _ComplexTraits common

class _ComplexTraitsFloat : public _ComplexTraits<float>
{
public:

};  // END _ComplexTraitsFloat

class _ComplexTraitsDouble : public _ComplexTraits<double>
{
public:

};  // END _ComplexTraitsDouble

class _ComplexTraitsLongDouble : public _ComplexTraits<long double>
{
public:

};  // END _ComplexTraitsLongDouble


// complex traits specialization
template<class Type>
class ComplexTraits : _ComplexTraits<Type> {};

template<>
class ComplexTraits<float> : _ComplexTraitsFloat {};

template<>
class ComplexTraits<double> : _ComplexTraitsDouble {};

template<>
class ComplexTraits<long double> : _ComplexTraitsLongDouble {};


template<class Type>
class Complex
{
public:
    using TraitsType    = ComplexTraits<Type>;
    using ValueType     = Type;

    ValueType _rep[2];
private:

    enum RepParts
    {
        Real = 0,
        Imag = 1
    };

public:
    // Constructors

    constexpr Complex(  const ValueType& realVal = ValueType(),
                        const ValueType& imagVal = ValueType())
        : _rep{realVal, imagVal} { /*Empty*/ }

public:
    // Operators

public:
    // Main functions

    constexpr void real(const ValueType& val) { // set real component
        _rep[Real] = val;
    }

    constexpr void imag(const ValueType& val) { // set imaginary component
        _rep[Imag] = val;
    }

};  // END Complex common

CUSTOM_END