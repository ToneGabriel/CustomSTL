#pragma once
#include "stl_cpp/c_utility.h"
#include "stl_cpp/c_limits.h"
#include "stl_cpp/c_numbers.h"

#include <cmath>


CUSTOM_BEGIN

// Definitions
template<class Type>
class complex;

template<class Type>
struct complex_traits;

template<class Type>
Type abs(const complex<Type>& val); // return magnitude of complex

template<class Type>
constexpr Type norm(const complex<Type>& val);  // return squared magnitude

template<class Type>
Type arg(const complex<Type>& val); // return phase angle of complex as real

template<class Type>
constexpr complex<Type> conj(const complex<Type>& val); // return complex conjugate

template<class Type>
complex<Type> proj(const complex<Type>& val); // return complex projection

template<class Type>
complex<Type> polar(const Type& rho, const Type& theta = Type());

template<class Type>
complex<Type> exp(const complex<Type>& val);

template<class Type>
complex<Type> log(const complex<Type>& val);

template<class Type>
complex<Type> log10(const complex<Type>& val);

template<class Type>
complex<Type> sqrt(const complex<Type>& val);

template<class Type>
complex<Type> pow(const complex<Type>& base, const Type& ex);

template<class Type>
complex<Type> pow(const Type& base, const complex<Type>& ex);

template<class Type>
complex<Type> pow(const complex<Type>& base, const complex<Type>& ex);

template<class Type>
complex<Type> sin(const complex<Type>& val);

template<class Type>
complex<Type> asin(const complex<Type>& val);

template<class Type>
complex<Type> sinh(const complex<Type>& val);

template<class Type>
complex<Type> asinh(const complex<Type>& val);

template<class Type>
complex<Type> cos(const complex<Type>& val);

template<class Type>
complex<Type> acos(const complex<Type>& val);

template<class Type>
complex<Type> cosh(const complex<Type>& val);

template<class Type>
complex<Type> acosh(const complex<Type>& val);

template<class Type>
complex<Type> tan(const complex<Type>& val);

template<class Type>
complex<Type> atan(const complex<Type>& val);

template<class Type>
complex<Type> tanh(const complex<Type>& val);

template<class Type>
complex<Type> atanh(const complex<Type>& val);


CUSTOM_DETAIL_BEGIN

// complex traits implementation
template<class Type>
struct _Complex_Traits_Base
{
    static_assert(is_floating_point_v<Type>, "complex traits accepts only floating point data type");

    using float_type = Type;

    static constexpr float_type epsilon_v()
    {
        return numeric_limits<float_type>::epsilon();
    }

    static constexpr float_type max_v()
    {
        return (numeric_limits<float_type>::max)();
    }

    static constexpr float_type norm_min_v()
    {
        return (numeric_limits<float_type>::min)() > 0 ? (numeric_limits<float_type>::min)() : 0;
    }

    static constexpr float_type nan_v()
    {
        return numeric_limits<float_type>::quiet_NaN();
    }

    static constexpr float_type inf_v()
    {
        return numeric_limits<float_type>::infinity();
    }

    static constexpr bool is_nan(float_type val)
    {
        return val == nan_v();
    }

    static bool is_inf(float_type val)
    {
        return ::abs(val) == inf_v();
    }

    // has all floating point overloads
    static bool signbit(float_type val)
    {
        return std::signbit(val);
    }

    // has all floating point overloads
    static float_type abs_v(float_type val)
    {
        return ::abs(val);
    }

};  // END _Complex_Traits_Base

struct _Complex_Traits_Float : _Complex_Traits_Base<float>
{
    using float_type = typename _Complex_Traits_Base<float>::float_type; // float

    static float_type copysign(float_type magnitude, float_type sign)
    {
        return ::copysignf(magnitude, sign);
    }

    static float_type sin(float_type val)
    {
        return ::sinf(val);
    }

    static float_type sinh(float_type val)
    {
        return ::sinhf(val);
    }

    static float_type asinh(float_type val)
    {
        return ::asinhf(val);
    }

    static float_type cos(float_type val)
    {
        return ::cosf(val);
    }

    static float_type cosh(float_type val)
    {
        return ::coshf(val);
    }

    static float_type tan(float_type val)
    {
        return ::tanf(val);
    }

    static float_type tanh(float_type val)
    {
        return ::tanhf(val);
    }

    static float_type atan(float_type val)
    {
        return ::atanf(val);
    }

    static float_type atanh(float_type val)
    {
        return ::atanhf(val);
    }

    static float_type atan2(float_type yVal, float_type xVal)
    {
        return ::atan2f(yVal, xVal);
    }

    // return sqrt(left ^ 2 + right ^ 2)
    static float_type hypot(float_type left, float_type right)
    {
        return ::hypotf(left, right);
    }

    // return e ^ val
    static float_type exp(float_type val)
    {
        return ::expf(val);
    }

    // return val * 2 ^ exponent
    static float_type ldexp(float_type val, int exponent)
    {
        return ::ldexpf(val, exponent);
    }

    // return log_e (val)
    static float_type log(float_type val)
    {
        return ::logf(val);
    }

    // return log_e (1 + val)
    static float_type log1p(float_type val)
    {
        return ::log1pf(val);
    }

    static float_type pow(float_type base, float_type exponent)
    {
        return ::powf(base, exponent);
    }

    static float_type sqrt(float_type val)
    {
        return ::sqrtf(val);
    }
};  // END _Complex_Traits_Float

struct _Complex_Traits_Double : _Complex_Traits_Base<double>
{
    using float_type = typename _Complex_Traits_Base<double>::float_type; // double

    static float_type copysign(float_type magnitude, float_type sign)
    {
        return ::copysign(magnitude, sign);
    }

    static float_type sin(float_type val)
    {
        return ::sin(val);
    }

    static float_type sinh(float_type val)
    {
        return ::sinh(val);
    }

    static float_type asinh(float_type val)
    {
        return ::asinh(val);
    }

    static float_type cos(float_type val)
    {
        return ::cos(val);
    }

    static float_type cosh(float_type val)
    {
        return ::cosh(val);
    }

    static float_type tan(float_type val)
    {
        return ::tan(val);
    }

    static float_type tanh(float_type val)
    {
        return ::tanh(val);
    }

    static float_type atan(float_type val)
    {
        return ::atan(val);
    }

    static float_type atanh(float_type val)
    {
        return ::atanh(val);
    }

    static float_type atan2(float_type yVal, float_type xVal)
    {
        return ::atan2(yVal, xVal);
    }

    static float_type hypot(float_type left, float_type right)
    {
        return ::hypot(left, right);
    }

    static float_type exp(float_type val)
    {
        return ::exp(val);
    }

    static float_type ldexp(float_type val, int exponent)
    {
        return ::ldexp(val, exponent);
    }

    static float_type log(float_type val)
    {
        return ::log(val);
    }

    static float_type log1p(float_type val)
    {
        return ::log1p(val);
    }

    static float_type pow(float_type base, float_type exponent)
    {
        return ::pow(base, exponent);
    }

    static float_type sqrt(float_type val)
    {
        return ::sqrt(val);
    }
};  // END _Complex_Traits_Double

struct _Complex_Traits_Long_Double : _Complex_Traits_Base<long double>
{
    using float_type = typename _Complex_Traits_Base<long double>::float_type; // long double

    static float_type copysign(float_type magnitude, float_type sign)
    {
        return ::copysignl(magnitude, sign);
    }

    static float_type sin(float_type val)
    {
        return ::sinl(val);
    }

    static float_type sinh(float_type val)
    {
        return ::sinhl(val);
    }

    static float_type asinh(float_type val)
    {
        return ::asinhl(val);
    }

    static float_type cos(float_type val)
    {
        return ::cosl(val);
    }

    static float_type cosh(float_type val)
    {
        return ::coshl(val);
    }

    static float_type tan(float_type val)
    {
        return ::tanl(val);
    }

    static float_type tanh(float_type val)
    {
        return ::tanhl(val);
    }

    static float_type atan(float_type val)
    {
        return ::atanl(val);
    }

    static float_type atanh(float_type val)
    {
        return ::atanhl(val);
    }

    static float_type atan2(float_type yVal, float_type xVal)
    {
        return ::atan2l(yVal, xVal);
    }

    static float_type hypot(float_type left, float_type right)
    {
        return ::hypotl(left, right);
    }

    static float_type exp(float_type val)
    {
        return ::expl(val);
    }

    static float_type ldexp(float_type val, int exponent)
    {
        return ::ldexpl(val, exponent);
    }

    static float_type log(float_type val)
    {
        return ::logl(val);
    }

    static float_type log1p(float_type val)
    {
        return ::log1pl(val);
    }

    static float_type pow(float_type base, float_type exponent)
    {
        return ::powl(base, exponent);
    }

    static float_type sqrt(float_type val)
    {
        return ::sqrtl(val);
    }
};  // END _Complex_Traits_Long_Double


template<class Type>    // helper for polar
complex<Type> _polar_positive_nan_inf_zero_rho(const Type& rho, const Type& theta)  // rho is +NaN/+Inf/+0
{
    if (complex_traits<Type>::is_nan(theta) || complex_traits<Type>::is_inf(theta))
    {
        if (complex_traits<Type>::is_inf(rho))
            return complex<Type>(rho, complex_traits<Type>::sin(theta)); // (Inf, NaN/Inf)
        else
            return complex<Type>(rho, complex_traits<Type>::copysign(rho, theta)); // (NaN/0, NaN/Inf)
    }
    else if (theta == Type())
        return complex<Type>(rho, theta); // (NaN/Inf/0, 0)
    else // theta is finite non-zero
        return complex<Type>(rho * complex_traits<Type>::cos(theta),
            rho * complex_traits<Type>::sin(theta)); // (NaN/Inf/0, finite non-zero)
}

CUSTOM_DETAIL_END


// complex traits specialization
template<class Type>
struct complex_traits : detail::_Complex_Traits_Base<Type> {};  // cannot use other than specializations below

template<>
struct complex_traits<float> : detail::_Complex_Traits_Float {};

template<>
struct complex_traits<double> : detail::_Complex_Traits_Double {};

template<>
struct complex_traits<long double> : detail::_Complex_Traits_Long_Double {};


template<class Type>
class complex
{
public:
    static_assert(is_floating_point_v<Type>, "complex class accepts only floating point data type");

    using traits_type   = complex_traits<Type>;
    using value_type    = Type;

private:
    enum _Rep_Parts : uint16_t
    {
        _Real = 0,
        _Imag = 1
    };

    value_type _rep[2];  // values stored in array where 0-real / 1-imag

public:
    // Constructors

    constexpr complex(  const value_type& realVal = value_type(),
                        const value_type& imagVal = value_type())
        : _rep{ realVal, imagVal } { /*Empty*/ }

    template<class _Type>
    constexpr complex(const complex<_Type>& other)
        : _rep{ static_cast<value_type>(other.real()), static_cast<value_type>(other.imag()) } { /*Empty*/ }

public:
    // Operators

    // from this type
    constexpr complex& operator=(const value_type& val)
    {
        _rep[_Real] = val;
        _rep[_Imag] = 0;
        return *this;
    }

    constexpr complex& operator+=(const value_type& val)
    {
        _rep[_Real] += val;
        return *this;
    }

    constexpr complex& operator-=(const value_type& val)
    {
        _rep[_Real] -= val;
        return *this;
    }

    constexpr complex& operator*=(const value_type& val)
    {
        _rep[_Real] *= val;
        _rep[_Imag] *= val;
        return *this;
    }

    constexpr complex& operator/=(const value_type& val)
    {
        _rep[_Real] /= val;
        _rep[_Imag] /= val;
        return *this;
    }
    // END from this type

    // from this complex
    constexpr complex& operator+=(const complex& other)
    {
        _add(other);
        return *this;
    }

    constexpr complex& operator-=(const complex& other)
    {
        _sub(other);
        return *this;
    }

    constexpr complex& operator*=(const complex& other)
    {
        _mul(other);
        return *this;
    }

    constexpr complex& operator/=(const complex& other)
    {
        _div(other);
        return *this;
    }
    // END from this complex

    // from other complex type
    template<class _Type>
    constexpr complex& operator=(const complex<_Type>& other)
    {
        _rep[_Real] = static_cast<value_type>(other.real());
        _rep[_Imag] = static_cast<value_type>(other.imag());
        return *this;
    }

    template<class _Type>
    constexpr complex& operator+=(const complex<_Type>& other)
    {
        _add(other);
        return *this;
    }

    template<class _Type>
    constexpr complex& operator-=(const complex<_Type>& other)
    {
        _sub(other);
        return *this;
    }

    template<class _Type>
    constexpr complex& operator*=(const complex<_Type>& other)
    {
        _mul(other);
        return *this;
    }

    template<class _Type>
    constexpr complex& operator/=(const complex<_Type>& other)
    {
        _div(other);
        return *this;
    }
    // END from other comnplex type

public:
    // Main functions

    constexpr void real(const value_type& val)  // set real component
    {
        _rep[_Real] = val;
    }

    constexpr void imag(const value_type& val)  // set imaginary component
    {
        _rep[_Imag] = val;
    }

    constexpr value_type real() const           // return real component
    {
        return _rep[_Real];
    }

    constexpr value_type imag() const           // return imaginary component
    {
        return _rep[_Imag];
    }

private:
    // Helpers

    template<class _Type>
    constexpr void _add(const complex<_Type>& other)
    {
        _rep[_Real] += static_cast<value_type>(other.real());
        _rep[_Imag] += static_cast<value_type>(other.imag());
    }

    template<class _Type>
    constexpr void _sub(const complex<_Type>& other)
    {
        _rep[_Real] -= static_cast<value_type>(other.real());
        _rep[_Imag] -= static_cast<value_type>(other.imag());
    }

    template<class _Type>
    constexpr void _mul(const complex<_Type>& other)
    {
        value_type otherReal = static_cast<value_type>(other.real());
        value_type otherImag = static_cast<value_type>(other.imag());

        value_type temp      = _rep[_Real] * otherReal - _rep[_Imag] * otherImag;
        _rep[_Imag]          = _rep[_Real] * otherImag + _rep[_Imag] * otherReal;
        _rep[_Real]          = temp;
    }

    template<class _Type>
    constexpr void _div(const complex<_Type>& other)
    {
        value_type otherReal = static_cast<value_type>(other.real());
        value_type otherImag = static_cast<value_type>(other.imag());

        if (traits_type::is_nan(otherReal) || traits_type::is_nan(otherImag))
        {
            // set NaN result
            _rep[_Real] = traits_type::nan_v();
            _rep[_Imag] = _rep[_Real];
        }
        else if (traits_type::abs_v(otherImag) < traits_type::abs_v(otherReal))
        {
            value_type wr = otherImag / otherReal;
            value_type wd = otherReal + wr * otherImag;

            if (traits_type::is_nan(wd) || wd == 0)
            {
                // set NaN result
                _rep[_Real] = traits_type::nan_v();
                _rep[_Imag] = _rep[_Real];
            }
            else
            {
                // compute representable result
                value_type temp  = (_rep[_Real] + _rep[_Imag] * wr) / wd;
                _rep[_Imag]      = (_rep[_Imag] - _rep[_Real] * wr) / wd;
                _rep[_Real]      = temp;
            }
        }
        else if (otherImag == 0) // other._rep[_Real] == 0 && other._rep[_Imag] == 0
        {
            _rep[_Real] /= otherReal;
            _rep[_Imag] /= otherReal;
        }
        else // 0 < |other._rep[_Real]| <= |other._rep[_Imag]|
        {
            value_type wr = otherReal / otherImag;
            value_type wd = otherImag + wr * otherReal;

            if (traits_type::is_nan(wd) || wd == 0)
            {
                // set NaN result
                _rep[_Real] = traits_type::nan_v();
                _rep[_Imag] = _rep[_Real];
            }
            else
            {
                // compute representable result
                value_type temp  = (_rep[_Real] * wr + _rep[_Imag]) / wd;
                _rep[_Imag]      = (_rep[_Imag] * wr - _rep[_Real]) / wd;
                _rep[_Real]      = temp;
            }
        }
    }
};  // END complex


// complex unary operators

template<class Type>
constexpr complex<Type> operator+(const complex<Type>& right)
{
    return right;
}

template<class Type>
constexpr complex<Type> operator-(const complex<Type>& right)
{
    return complex<Type>(-right.real(), -right.imag());
}

// complex binary operators

// ostream
template<class Type>
std::ostream& operator<<(std::ostream& os, const complex<Type>& complex)
{
	os << '(' << complex.real() << ',' << complex.imag() << ')';
	return os;
}

// add
template<class Type>
constexpr complex<Type> operator+(const complex<Type>& left, const complex<Type>& right)
{
    complex<Type> temp(left);
    temp += right;
    return temp;
}

template<class Type>
constexpr complex<Type> operator+(const complex<Type>& left, const Type& val)
{
    complex<Type> temp(left);
    temp += val;
    return temp;
}

template<class Type>
constexpr complex<Type> operator+(const Type& val, const complex<Type>& right)
{
    complex<Type> temp(right);
    temp += val;
    return temp;
}

// sub
template<class Type>
constexpr complex<Type> operator-(const complex<Type>& left, const complex<Type>& right)
{
    complex<Type> temp(left);
    temp -= right;
    return temp;
}

template<class Type>
constexpr complex<Type> operator-(const complex<Type>& left, const Type& val)
{
    complex<Type> temp(left);
    temp -= val;
    return temp;
}

template<class Type>
constexpr complex<Type> operator-(const Type& val, const complex<Type>& right)
{
    complex<Type> temp(val);
    temp -= right;
    return temp;
}

// mul
template<class Type>
constexpr complex<Type> operator*(const complex<Type>& left, const complex<Type>& right)
{
    complex<Type> temp(left);
    temp *= right;
    return temp;
}

template<class Type>
constexpr complex<Type> operator*(const complex<Type>& left, const Type& val)
{
    complex<Type> temp(left);
    temp *= val;
    return temp;
}

template<class Type>
constexpr complex<Type> operator*(const Type& val, const complex<Type>& right)
{
    complex<Type> temp(right);
    temp *= val;
    return temp;
}

// div
template<class Type>
constexpr complex<Type> operator/(const complex<Type>& left, const complex<Type>& right)
{
    complex<Type> temp(left);
    temp /= right;
    return temp;
}

template<class Type>
constexpr complex<Type> operator/(const complex<Type>& left, const Type& val)
{
    complex<Type> temp(left);
    temp /= val;
    return temp;
}

template<class Type>
constexpr complex<Type> operator/(const Type& val, const complex<Type>& right)
{
    complex<Type> temp(val);
    temp /= right;
    return temp;
}

// equal
template<class Type>
constexpr bool operator==(const complex<Type>& left, const complex<Type>& right)
{
    return left.real() == right.real() && left.imag() == right.imag();
}

template<class Type>
constexpr bool operator!=(const complex<Type>& left, const complex<Type>& right)
{
    return !(left == right);
}

template<class Type>
constexpr bool operator==(const complex<Type>& left, const Type& val)
{
    return left.real() == val && left.imag() == 0;
}

template<class Type>
constexpr bool operator!=(const complex<Type>& left, const Type& val)
{
    return !(left == val);
}

template<class Type>
constexpr bool operator==(const Type& val, const complex<Type>& right)
{
    return val == right.real() && 0 == right.imag();
}

template<class Type>
constexpr bool operator!=(const Type& val, const complex<Type>& right)
{
    return !(val == right);
}

// Other complex operations

template<class Type>
Type abs(const complex<Type>& val)  // return magnitude of complex
{
    return complex_traits<Type>::hypot(val.real(), val.imag());
}

template<class Type>
constexpr Type norm(const complex<Type>& val)   // return squared magnitude
{
    return val.real() * val.real() + val.imag() * val.imag();
}

template<class Type>
Type arg(const complex<Type>& val)  // return phase angle of complex as real
{
    return complex_traits<Type>::atan2(val.imag(), val.real());
}

template<class Type>
constexpr complex<Type> conj(const complex<Type>& val)  // return complex conjugate
{
    return complex<Type>(val.real(), -val.imag());
}

template<class Type>
complex<Type> proj(const complex<Type>& val)    // return complex projection
{
    if (complex_traits<Type>::is_inf(val.real()) || complex_traits<Type>::is_inf(val.imag()))
        return complex<Type>(   complex_traits<Type>::inf_v(),
                                complex_traits<Type>::copysign(Type(), val.imag()));

    return val;
}

template<class Type>
complex<Type> polar(const Type& rho, const Type& theta)
{
    if (complex_traits<Type>::signbit(rho))
        return -detail::_polar_positive_nan_inf_zero_rho(-rho, theta);

    return detail::_polar_positive_nan_inf_zero_rho(rho, theta);
}

template<class Type>
complex<Type> exp(const complex<Type>& val)
{
    return custom::polar(complex_traits<Type>::exp(val.real()), val.imag());
}

template<class Type>
complex<Type> log(const complex<Type>& val)
{
    return complex<Type>(complex_traits<Type>::log(custom::abs(val)), custom::arg(val));
}

template<class Type>
complex<Type> log10(const complex<Type>& val)
{
    return custom::log(val) / complex_traits<Type>::log(Type(10.0));
}

template<class Type>
complex<Type> sqrt(const complex<Type>& val)
{
    const Type realVal  = val.real();
    const Type imagVal  = val.imag();
    const Type zeroVal  = Type();

    if (realVal == zeroVal)
    {
        Type temp = complex_traits<Type>::sqrt(complex_traits<Type>::abs_v(imagVal) / Type(2.0));
        return complex<Type>(temp, imagVal < zeroVal ? -temp : temp);
    }
    else
    {
        Type temp       = complex_traits<Type>::sqrt(Type(2.0) * (custom::abs(val) + complex_traits<Type>::abs_v(realVal)));
        Type tempBy2    = temp / Type(2.0);
        
        return realVal > zeroVal ?  complex<Type>(tempBy2, imagVal / temp) :
                                    complex<Type>(  complex_traits<Type>::abs_v(imagVal) / temp,
                                                    imagVal < zeroVal ? -tempBy2 : tempBy2);
    }
}

template<class Type>
complex<Type> pow(const complex<Type>& base, const Type& ex)
{
    if (base.imag() == Type() && base.real() > Type())
        return complex_traits<Type>::pow(base.real(), ex);

    complex<Type> temp = custom::log(base);
    return custom::polar<Type>(complex_traits<Type>::exp(ex * temp.real()), ex * temp.imag());
}

template<class Type>
complex<Type> pow(const Type& base, const complex<Type>& ex)
{
    if (base > Type())
        return custom::polar<Type>( complex_traits<Type>::pow(base, ex.real()),
                                    ex.imag() * complex_traits<Type>::log(base));

    return custom::pow(complex<Type>(base), ex);
}

template<class Type>
complex<Type> pow(const complex<Type>& base, const complex<Type>& ex)
{
    return base == Type() ? Type() : custom::exp(ex * custom::log(base));
}

template<class Type>
complex<Type> sin(const complex<Type>& val)
{
    return complex<Type>(   complex_traits<Type>::cosh(val.imag()) * complex_traits<Type>::sin(val.real()),
                            complex_traits<Type>::sinh(val.imag()) * complex_traits<Type>::cos(val.real()));
}

template<class Type>
complex<Type> asin(const complex<Type>& val)
{
    complex<Type> asinhTemp = custom::asinh(complex<Type>(-val.imag(), val.real()));
    return complex<Type>(asinhTemp.imag(), -asinhTemp.real());
}

template<class Type>
complex<Type> sinh(const complex<Type>& val)
{
    return complex<Type>(   complex_traits<Type>::sinh(val.real()) * complex_traits<Type>::cos(val.imag()),
                            complex_traits<Type>::cosh(val.real()) * complex_traits<Type>::sin(val.imag()));
}

template<class Type>
complex<Type> asinh(const complex<Type>& val)
{
    // Kahan's formula -> asinh(z) = ln(z + sqrt(z^2 + 1))
    complex<Type> temp((val.real() - val.imag()) * (val.real() + val.imag()) + Type(1.0),
                        Type(2.0) * val.real() * val.imag());

    return custom::log(custom::sqrt(temp) + val);
}

template<class Type>
complex<Type> cos(const complex<Type>& val)
{
    return complex<Type>(   complex_traits<Type>::cosh(val.imag()) * complex_traits<Type>::cos(val.real()),
                            -complex_traits<Type>::sinh(val.imag()) * complex_traits<Type>::sin(val.real()));
}

template<class Type>
complex<Type> acos(const complex<Type>& val)
{
    const complex<Type> asinTemp = custom::asin(val);
    return complex<Type>(custom::numbers::pi_by_2_v<Type> - asinTemp.real(), -asinTemp.imag());
}

template<class Type>
complex<Type> cosh(const complex<Type>& val)
{
    return complex<Type>(   complex_traits<Type>::cosh(val.real()) * complex_traits<Type>::cos(val.imag()),
                            complex_traits<Type>::sinh(val.real()) * complex_traits<Type>::sin(val.imag()));
}

template<class Type>
complex<Type> acosh(const complex<Type>& val)
{
    // Kahan's formula -> acosh(z) = ln(z + sqrt(z^2 - 1))
    return Type(2.0) * custom::log( custom::sqrt(Type(0.5) * (val + Type(1.0))) +
                                    custom::sqrt(Type(0.5) * (val - Type(1.0))));
}

template<class Type>
complex<Type> tan(const complex<Type>& val)
{
    return custom::sin(val) / custom::cos(val);
}

template<class Type>
complex<Type> atan(const complex<Type>& val)
{
    complex<Type> atanhTemp = custom::atanh(complex<Type>(-val.imag(), val.real()));
    return complex<Type>(atanhTemp.imag(), -atanhTemp.real());
}

template<class Type>
complex<Type> tanh(const complex<Type>& val)
{
    return custom::sinh(val) / custom::cosh(val);
}

template<class Type>
complex<Type> atanh(const complex<Type>& val)
{
    const Type imag2    = val.imag() * val.imag();
    const Type temp     = Type(1.0) - imag2 - val.real() * val.real();

    Type num = Type(1.0) + val.real();
    Type den = Type(1.0) - val.real();

    num = imag2 + num * num;
    den = imag2 + den * den;

    return complex<Type>(   Type(0.25) * (complex_traits<Type>::log(num) - complex_traits<Type>::log(den)),
			                Type(0.5) * complex_traits<Type>::atan2(Type(2.0) * val.imag(), temp));
}

CUSTOM_END