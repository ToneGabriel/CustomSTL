#pragma once
#include "custom/utility.h"
#include "custom/iterator.h"
#include "custom/bit.h"


CUSTOM_BEGIN

// iota
template<class ForwardIt, class Type>
constexpr void iota(ForwardIt first, ForwardIt last, Type value)
{
    detail::_verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first, ++value)
        *first = value;
}
// END iota


// accumulate
template<class InputIt, class Type, class BinaryOperation>
constexpr Type accumulate(InputIt first, InputIt last, Type init, BinaryOperation op)
{
    detail::_verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first)
        init = op(custom::move(init), *first);
 
    return init;
}

template<class InputIt, class Type>
constexpr Type accumulate(InputIt first, InputIt last, Type init)
{
    return custom::accumulate(first, last, init, plus<>{});
}
// END accumulate


// inner_product
template<class InputIt1, class InputIt2, class Type, class BinaryOperation1, class BinaryOperation2>
constexpr Type inner_product(   InputIt1 first1, InputIt1 last1,
                                InputIt2 first2, Type init, 
                                BinaryOperation1 op1, BinaryOperation2 op2)
{
    detail::_verify_iteration_range(first1, last1);

    for (/*Empty*/; first1 != last1; ++first1, ++first2)
        init = op1(custom::move(init), op2(*first1, *first2));

    return init;
}

template<class InputIt1, class InputIt2, class Type>
constexpr Type inner_product(InputIt1 first1, InputIt1 last1, InputIt2 first2, Type init)
{
    return custom::inner_product(first1, last1, first2, custom::move(init), plus<>{}, multiplies<>{});
}
// END inner_product


// adjacent_difference
template<class InputIt, class OutputIt, class BinaryOperation>
constexpr OutputIt adjacent_difference(InputIt first, InputIt last, OutputIt destFirst, BinaryOperation op)
{
    detail::_verify_iteration_range(first, last);
    
    if (first != last)
    {
        using value_type = typename iterator_traits<InputIt>::value_type;

        value_type acc   = *first;
        *destFirst      = acc;
    
        while (++first != last)
        {
            value_type val   = *first;
            *++destFirst    = op(val, custom::move(acc));
            acc             = custom::move(val);
        }

        ++destFirst;
    }
 
    return destFirst;
}

template<class InputIt, class OutputIt>
constexpr OutputIt adjacent_difference(InputIt first, InputIt last, OutputIt destFirst)
{
    return custom::adjacent_difference(first, last, destFirst, minus<>{});
}
// END adjacent_difference


// partial_sum
template<class InputIt, class OutputIt, class BinaryOperation>
constexpr OutputIt partial_sum(InputIt first, InputIt last, OutputIt destFirst, BinaryOperation op)
{
    detail::_verify_iteration_range(first, last);
    
    if (first != last)
    {
        using value_type = typename iterator_traits<InputIt>::value_type;

        value_type acc       = *first;
        *destFirst          = acc;
    
        while (++first != last)
        {
            acc             = op(custom::move(acc), *first);
            *++destFirst    = acc;
        }

        ++destFirst;
    }
    
    return destFirst;
}

template<class InputIt, class OutputIt>
constexpr OutputIt partial_sum(InputIt first, InputIt last, OutputIt destFirst)
{
    return partial_sum(first, last, destFirst, plus<>{});
}
// END partial_sum


// reduce
template<class InputIt, class Type, class BinaryOperation>
constexpr Type reduce(InputIt first, InputIt last, Type init, BinaryOperation op)
{
    detail::_verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first)
        init = op(custom::move(init), *first);

    return init;
}

template<class InputIt, class Type>
constexpr Type reduce(InputIt first, InputIt last, Type init)
{
    return custom::reduce(first, last, custom::move(init), plus<>{});
}

template<class InputIt>
constexpr typename iterator_traits<InputIt>::value_type reduce(InputIt first, InputIt last)
{
    return custom::reduce(first, last, typename iterator_traits<InputIt>::value_type{}, plus<>{});
}
// END reduce


// exclusive_scan
template<class InputIt, class OutputIt, class Type, class BinaryOperation>
constexpr OutputIt exclusive_scan(  InputIt first, InputIt last,
                                    OutputIt destFirst, Type init,
                                    BinaryOperation op)
{
    detail::_verify_iteration_range(first, last);

    if (first != last)
    {
        for(;;)
        {
            Type temp(op(init, *first));
            *destFirst++ = init;
            ++first;

            if (first == last)
                break;

            init = custom::move(temp);
        }
    }

    return destFirst;
}

template<class InputIt, class OutputIt, class Type>
constexpr OutputIt exclusive_scan(  InputIt first, InputIt last,
                                    OutputIt destFirst, Type init)
{
    return custom::exclusive_scan(first, last, destFirst, custom::move(init), plus<>{});
}
// END exclusive_scan


// inclusive_scan
template<class InputIt, class OutputIt, class BinaryOperation, class Type>
constexpr OutputIt inclusive_scan( InputIt first, InputIt last, OutputIt destFirst,
                                   BinaryOperation op, Type init)
{
    detail::_verify_iteration_range(first, last);
    
    for (/*Empty*/; first != last; ++first)
    {
        init = op(custom::move(init), *first);
        *destFirst++ = init;
    }

    return destFirst;
}

template<class InputIt, class OutputIt, class BinaryOperation>
constexpr OutputIt inclusive_scan(  InputIt first, InputIt last,
                                    OutputIt destFirst, BinaryOperation op)
{
    detail::_verify_iteration_range(first, last);

    if (first != last)
    {
        using value_type = typename iterator_traits<InputIt>::value_type;

        value_type init = *first;

        for (;;)
        {
            *destFirst++ = init;
            ++first;

            if (first == last)
                break;

            init = op(custom::move(init), *first);
        }
    }

    return destFirst;
}

template<class InputIt, class OutputIt>
constexpr OutputIt inclusive_scan(InputIt first, InputIt last, OutputIt destFirst)
{
    return custom::inclusive_scan(first, last, destFirst, plus<>{});
}
// END inclusive_scan


// transform_reduce
template<class InputIt1, class InputIt2, class Type, class BinaryOperation1, class BinaryOperation2>
constexpr Type transform_reduce(InputIt1 first1, InputIt1 last1,
                                InputIt2 first2, Type init,
                                BinaryOperation1 bop1, BinaryOperation2 bop2)
{
    detail::_verify_iteration_range(first1, last1);

    for (/*Empty*/; first1 != last1; ++first1, ++first2)
        init = bop1(custom::move(init), bop2(*first1, *first2));

    return init;
}

template<class InputIt1, class InputIt2, class Type>
constexpr Type transform_reduce(InputIt1 first1, InputIt1 last1, InputIt2 first2, Type init)
{
    return custom::transform_reduce(first1, last1, first2, custom::move(init), plus<>{}, multiplies<>{});
}

template<class InputIt1, class Type, class BinaryOperation, class UnaryOperation>
constexpr Type transform_reduce(InputIt1 first, InputIt1 last,
                                Type init, BinaryOperation bop, UnaryOperation uop)
{
    detail::_verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first)
        init = bop(custom::move(init), uop(*first));

    return init;
}
// END transform_reduce


// transform_exclusive_scan
template<class InputIt, class OutputIt, class Type, class BinaryOperation, class UnaryOperation>
constexpr OutputIt transform_exclusive_scan(InputIt first, InputIt last,
                                            OutputIt destFirst, Type init,
                                            BinaryOperation bop, UnaryOperation uop)
{
    detail::_verify_iteration_range(first, last);

    if (first != last)
    {
        for(;;)
        {
            Type temp(bop(init, uop(*first)));
            *destFirst++ = init;
            ++first;

            if (first == last)
                break;

            init = custom::move(temp);
        }
    }

    return destFirst;
}
// END transform_exclusive_scan


// transform_inclusive_scan
template<class InputIt, class OutputIt, class BinaryOperation, class UnaryOperation, class Type>
constexpr OutputIt transform_inclusive_scan(InputIt first, InputIt last, OutputIt destFirst,
                                            BinaryOperation bop, UnaryOperation uop, Type init)
{
    detail::_verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first)
    {
        init = bop(custom::move(init), uop(*first));
        *destFirst++ = init;
    }

    return destFirst;
}

template<class InputIt, class OutputIt, class BinaryOperation, class UnaryOperation>
constexpr OutputIt transform_inclusive_scan(InputIt first, InputIt last,
                                            OutputIt destFirst, BinaryOperation bop, UnaryOperation uop)
{
    detail::_verify_iteration_range(first, last);

    if (first != last)
    {
        auto init = uop(*first);

        for (;;)
        {
            *destFirst++ = init;
            ++first;

            if (first == last)
                break;

            init = bop(custom::move(init), uop(*first));
        }
    }

    return destFirst;
}
// END transform_inclusive_scan


template<class Integral>
constexpr auto abs_unsigned(const Integral val) noexcept
{
    // computes absolute value of val (converting to an unsigned integer type if necessary to avoid overflow
    // representing the negation of the minimum value)
    static_assert(is_integral_v<Integral>);

    if constexpr (is_signed_v<Integral>)
    {
        using _Unsigned = make_unsigned_t<Integral>;

        // note static_cast to _Unsigned such that Integral == short returns unsigned short rather than int
        if (val < 0)
            return static_cast<_Unsigned>(_Unsigned{0} - static_cast<_Unsigned>(val));

        return static_cast<_Unsigned>(val);
    }
    else
        return val;
}


// greatest common divizor
template<class first, class second>
constexpr common_type_t<first, second> gcd(first val1, second val2) noexcept
{
    static_assert(is_nonbool_integral_v<first> && is_nonbool_integral_v<second>, "GCD requires nonbool integral types");

    using _Common           = common_type_t<first, second>;
    using _CommonUnsigned   = make_unsigned_t<_Common>;

    _CommonUnsigned valAbs1 = abs_unsigned(val1);
    _CommonUnsigned valAbs2 = abs_unsigned(val2);

    if (valAbs1 == 0)
        return static_cast<_Common>(valAbs2);

    if (valAbs2 == 0)
        return static_cast<_Common>(valAbs1);

    auto valTrailingZeros1      = static_cast<unsigned long>(countr_zero(valAbs1));
    auto valTrailingZeros2      = static_cast<unsigned long>(countr_zero(valAbs2));
    auto commonFactorsOf2       = (custom::min)(valTrailingZeros1, valTrailingZeros2);

    valAbs1 >>= valTrailingZeros1;
    valAbs2 >>= valTrailingZeros2;

    for (;;)
    {
        if (valAbs1 > valAbs2)  // swap
        {
            _CommonUnsigned temp    = valAbs1;
            valAbs1                 = valAbs2;
            valAbs2                 = temp;
        }

        valAbs2 -= valAbs1;

        if (valAbs2 == 0)
            return static_cast<_Common>(valAbs1 << commonFactorsOf2);

        valAbs2 >>= static_cast<unsigned long>(countr_zero(valAbs2));
    }
}
// END gcd


//least common multiple
template<class first, class second>
constexpr common_type_t<first, second> lcm(const first val1, const second val2) noexcept
{
    static_assert(is_nonbool_integral_v<first> && is_nonbool_integral_v<second>, "LCM requires nonbool integral types");
    
    using _Common           = common_type_t<first, second>;
    using _CommonUnsigned   = make_unsigned_t<_Common>;

    _CommonUnsigned valAbs1 = abs_unsigned(val1);
    _CommonUnsigned valAbs2 = abs_unsigned(val2);
    
    if (valAbs1 == 0 || valAbs2 == 0)
        return 0;

    return static_cast<_Common>((valAbs1 / custom::gcd(valAbs1, valAbs2)) * valAbs2);
}
// END lcm


// midpoint
template<class Type>
constexpr Type midpoint(Type val1, Type val2) noexcept
{
    if constexpr (is_integral_v<Type>)
    {
        using _Unsigned = make_unsigned_t<Type>;

        const auto valUnsigned1 = static_cast<_Unsigned>(val1);
        const auto valUnsigned2 = static_cast<_Unsigned>(val2);

        if (val1 > val2)
            return static_cast<Type>(val1 - static_cast<Type>(static_cast<_Unsigned>(valUnsigned1 - valUnsigned2) >> 1));
        else
            return static_cast<Type>(val1 + static_cast<Type>(static_cast<_Unsigned>(valUnsigned2 - valUnsigned1) >> 1));
    }
    else // IsFloating_v
    {
        constexpr Type lowLimit     = (numeric_limits<Type>::min)() * 2;
        constexpr Type highLimit    = (numeric_limits<Type>::max)() / 2;

        const Type valAbs1 = val1 < 0 ? -val1 : val1;
        const Type valAbs2 = val2 < 0 ? -val2 : val2;

        if (valAbs1 <= highLimit && valAbs2 <= highLimit)
            return (val1 + val2) / 2;   // always correctly rounded

        if (valAbs1 < lowLimit)         // not safe to halve val1
            return val1 + val2 / 2;

        if (valAbs2 < lowLimit)         // not safe to halve val2
            return val1 / 2 + val2;

        return val1 / 2 + val2 / 2;     // otherwise correctly rounded
    }

    return 0;
}

template<class Type, enable_if_t<is_object_v<Type>, bool> = true>
constexpr Type* midpoint(Type* const ptr1, Type* const ptr2) noexcept
{
    static_assert(sizeof(Type) != 0, "type must be complete");

    if (ptr1 > ptr2)
        return ptr1 - ((ptr1 - ptr2) >> 1);
    else
        return ptr1 + ((ptr2 - ptr1) >> 1);
}
// END midpoint

CUSTOM_END