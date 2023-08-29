#pragma once
#include "Utility.h"


CUSTOM_BEGIN

#pragma region Numeric operations
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
#pragma endregion Numeric operations

CUSTOM_END