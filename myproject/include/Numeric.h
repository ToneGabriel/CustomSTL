#pragma once
#include "Utility.h"
#include "Iterator.h"


CUSTOM_BEGIN
// TODO: implement

// iota
template<class ForwardIt, class Type>
constexpr void iota(ForwardIt first, ForwardIt last, Type value) {
    while (first != last)
    {
        *first++ = value;
        ++value;
    }
}
// END iota


// accumulate
template<class InputIt, class Type>
constexpr Type accumulate(InputIt first, InputIt last, Type init) {
    for (; first != last; ++first)
        init = custom::move(init) + *first;
 
    return init;
}

template<class InputIt, class Type, class BinaryOperation>
constexpr Type accumulate(InputIt first, InputIt last, Type init, BinaryOperation op) {
    for (; first != last; ++first)
        init = op(custom::move(init), *first);
 
    return init;
}
// END accumulate


// inner_product
template<class InputIt1, class InputIt2, class Type>
constexpr Type inner_product(InputIt1 first1, InputIt1 last1, InputIt2 first2, Type init) {
    while (first1 != last1)
    {
        init = custom::move(init) + (*first1) * (*first2);
        ++first1;
        ++first2;
    }
 
    return init;
}

template<class InputIt1, class InputIt2, class Type, class BinaryOperation1, class BinaryOperation2>
constexpr Type inner_product(   InputIt1 first1,
                                InputIt1 last1,
                                InputIt2 first2,
                                Type init,
                                BinaryOperation1 op1,
                                BinaryOperation2 op2) {

    while (first1 != last1)
    {
        init = op1(custom::move(init), op2(*first1, *first2));
        ++first1;
        ++first2;
    }

    return init;
}
// END inner_product


// adjacent_difference
template<class InputIt, class OutputIt>
constexpr OutputIt adjacent_difference(InputIt first, InputIt last, OutputIt d_first) {
    if (first == last)
        return d_first;
 
    typedef typename std::iterator_traits<InputIt>::value_type value_t;
    value_t acc = *first;
    *d_first = acc;
 
    while (++first != last)
    {
        value_t val = *first;
        *++d_first = val - custom::move(acc);
        acc = custom::move(val);
    }
 
    return ++d_first;
}

template<class InputIt, class OutputIt, class BinaryOperation>
constexpr OutputIt adjacent_difference(InputIt first, InputIt last, OutputIt d_first, BinaryOperation op) {
    if (first == last)
        return d_first;
 
    typedef typename std::iterator_traits<InputIt>::value_type value_t;
    value_t acc = *first;
    *d_first = acc;
 
    while (++first != last)
    {
        value_t val = *first;
        *++d_first = op(val, custom::move(acc));
        acc = custom::move(val);
    }
 
    return ++d_first;
}
// END adjacent_difference


// partial_sum
template<class InputIt, class OutputIt>
constexpr OutputIt partial_sum(InputIt first, InputIt last, OutputIt d_first) {
    if (first == last)
        return d_first;
 
    typename std::iterator_traits<InputIt>::value_type sum = *first;
    *d_first = sum;
 
    while (++first != last)
    {
        sum = custom::move(sum) + *first;
        *++d_first = sum;
    }

    return ++d_first;
}

template<class InputIt, class OutputIt, class BinaryOperation>
constexpr OutputIt partial_sum(InputIt first, InputIt last, OutputIt d_first, BinaryOperation op) {
    if (first == last)
        return d_first;
 
    typename std::iterator_traits<InputIt>::value_type acc = *first;
    *d_first = acc;
 
    while (++first != last)
    {
        acc = op(custom::move(acc), *first);
        *++d_first = acc;
    }
 
    return ++d_first;
}
// END partial_sum


// reduce
template<class InputIt>
constexpr typename std::iterator_traits<InputIt>::value_type reduce(InputIt first, InputIt last) {
    return typename std::iterator_traits<InputIt>::value_type();
}
// END reduce


// exclusive_scan
template<class InputIt, class OutputIt, class Type>
constexpr OutputIt exclusive_scan(InputIt first, InputIt last, OutputIt d_first, Type init) {
    return OutputIt();
}
// END exclusive_scan


// inclusive_scan
template<class InputIt, class OutputIt>
constexpr OutputIt inclusive_scan(InputIt first, InputIt last, OutputIt d_first) {
    return OutputIt();
}
// END inclusive_scan


// transform_reduce
template<class InputIt1, class InputIt2, class Type>
constexpr Type transform_reduce(InputIt1 first1, InputIt1 last1, InputIt2 first2, Type init) {
    return Type();
}
// END transform_reduce


// transform_exclusive_scan
template<class InputIt, class OutputIt, class T, class BinaryOperation, class UnaryOperation>
constexpr OutputIt transform_exclusive_scan(InputIt first,
                                            InputIt last,
                                            OutputIt d_first,
                                            T init,
                                            BinaryOperation binary_op,
                                            UnaryOperation unary_op) {
    return OutputIt();
}
// END transform_exclusive_scan


// transform_inclusive_scan
template<class InputIt, class OutputIt, class BinaryOperation, class UnaryOperation>
constexpr OutputIt transform_inclusive_scan(InputIt first,
                                            InputIt last,
                                            OutputIt d_first,
                                            BinaryOperation binary_op,
                                            UnaryOperation unary_op) {
    return OutputIt();
}
// END transform_inclusive_scan

CUSTOM_END