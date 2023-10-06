#pragma once
#include "Utility.h"
#include "Iterator.h"


CUSTOM_BEGIN

// iota
template<class ForwardIt, class Type>
constexpr void iota(ForwardIt first, ForwardIt last, Type value) {
    _verify_iteration_range(first, last);

    while (first != last)
    {
        *first++ = value;
        ++value;
    }
}
// END iota


// accumulate
template<class InputIt, class Type, class BinaryOperation>
constexpr Type accumulate(InputIt first, InputIt last, Type init, BinaryOperation op) {
    _verify_iteration_range(first, last);

    while (first != last)
    {
        init = op(custom::move(init), *first);
        ++first;
    }
 
    return init;
}

template<class InputIt, class Type>
constexpr Type accumulate(InputIt first, InputIt last, Type init) {
    return custom::accumulate(first, last, init, Plus<>{});
}
// END accumulate


// inner_product
template<class InputIt1, class InputIt2, class Type, class BinaryOperation1, class BinaryOperation2>
constexpr Type inner_product(   InputIt1 first1, InputIt1 last1,
                                InputIt2 first2, Type init, 
                                BinaryOperation1 op1, BinaryOperation2 op2) {

    _verify_iteration_range(first1, last1);

    while (first1 != last1)
    {
        init = op1(custom::move(init), op2(*first1, *first2));
        ++first1;
        ++first2;
    }

    return init;
}

template<class InputIt1, class InputIt2, class Type>
constexpr Type inner_product(InputIt1 first1, InputIt1 last1, InputIt2 first2, Type init) {
    return custom::inner_product(first1, last1, first2, custom::move(init), Plus<>{}, Multiplies<>{});
}
// END inner_product


// adjacent_difference
template<class InputIt, class OutputIt, class BinaryOperation>
constexpr OutputIt adjacent_difference(InputIt first, InputIt last, OutputIt destFirst, BinaryOperation op) {
    _verify_iteration_range(first, last);
    
    if (first != last)
    {
        using ValueType = typename IteratorTraits<InputIt>::ValueType;

        ValueType acc   = *first;
        *destFirst      = acc;
    
        while (++first != last)
        {
            ValueType val   = *first;
            *++destFirst    = op(val, custom::move(acc));
            acc             = custom::move(val);
        }

        ++destFirst;
    }
 
    return destFirst;
}

template<class InputIt, class OutputIt>
constexpr OutputIt adjacent_difference(InputIt first, InputIt last, OutputIt destFirst) {
    return custom::adjacent_difference(first, last, destFirst, Minus<>{});
}
// END adjacent_difference


// partial_sum
template<class InputIt, class OutputIt, class BinaryOperation>
constexpr OutputIt partial_sum(InputIt first, InputIt last, OutputIt destFirst, BinaryOperation op) {
    _verify_iteration_range(first, last);
    
    if (first != last)
    {
        using ValueType = typename IteratorTraits<InputIt>::ValueType;

        ValueType acc       = *first;
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
constexpr OutputIt partial_sum(InputIt first, InputIt last, OutputIt destFirst) {
    return partial_sum(first, last, destFirst, Plus<>{});
}
// END partial_sum


// reduce
template<class InputIt, class Type, class BinaryOperation>
constexpr Type reduce(InputIt first, InputIt last, Type init, BinaryOperation op) {
    _verify_iteration_range(first, last);

    while (first != last)
    {
        init = op(custom::move(init), *first);
        ++first;
    }

    return init;
}

template<class InputIt, class Type>
constexpr Type reduce(InputIt first, InputIt last, Type init) {
    return custom::reduce(first, last, custom::move(init), Plus<>{});
}

template<class InputIt>
constexpr typename IteratorTraits<InputIt>::ValueType reduce(InputIt first, InputIt last) {
    return custom::reduce(first, last, typename IteratorTraits<InputIt>::ValueType{}, Plus<>{});
}
// END reduce


// exclusive_scan
template<class InputIt, class OutputIt, class Type, class BinaryOperation>
constexpr OutputIt exclusive_scan(  InputIt first, InputIt last,
                                    OutputIt destFirst, Type init,
                                    BinaryOperation op) {

    _verify_iteration_range(first, last);

    if (first != last)
    {
        for(;;)
        {
            Type temp(op(init, *first));
            *destFirst = init;
            ++destFirst;
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
                                    OutputIt destFirst, Type init) {

    return custom::exclusive_scan(first, last, destFirst, custom::move(init), Plus<>{});
}
// END exclusive_scan


// inclusive_scan
template<class InputIt, class OutputIt, class BinaryOperation, class Type>
constexpr OutputIt inclusive_scan( InputIt first, InputIt last, OutputIt destFirst,
                                   BinaryOperation op, Type init) {

    _verify_iteration_range(first, last);
    
    for (; first != last; ++first)
    {
        init = op(custom::move(init), *first);
        *destFirst = init;
        ++destFirst;
    }

    return destFirst;
}

template<class InputIt, class OutputIt, class BinaryOperation>
constexpr OutputIt inclusive_scan(  InputIt first, InputIt last,
                                    OutputIt destFirst, BinaryOperation op) {
                                   
    _verify_iteration_range(first, last);

    if (first != last)
    {
        using ValueType = typename IteratorTraits<InputIt>::ValueType;

        ValueType init = *first;
        for (;;)
        {
            *destFirst = init;
            ++destFirst;
            ++first;
            if (first == last)
                break;

            init = op(custom::move(init), *first);
        }
    }

    return destFirst;
}

template<class InputIt, class OutputIt>
constexpr OutputIt inclusive_scan(InputIt first, InputIt last, OutputIt destFirst) {
    return custom::inclusive_scan(first, last, destFirst, Plus<>{});
}
// END inclusive_scan


// transform_reduce
template<class InputIt1, class InputIt2, class Type>
constexpr Type transform_reduce(InputIt1 first1, InputIt1 last1, InputIt2 first2, Type init) {
    // TODO: implement
    return Type();
}
// END transform_reduce


// transform_exclusive_scan
template<class InputIt, class OutputIt, class Type, class BinaryOperation, class UnaryOperation>
constexpr OutputIt transform_exclusive_scan(InputIt first, InputIt last,
                                            OutputIt destFirst, Type init,
                                            BinaryOperation bop, UnaryOperation uop) {
    
    _verify_iteration_range(first, last);

    if (first != last)
    {
        for(;;)
        {
            Type temp(bop(init, uop(*first)));
            *destFirst = init;
            ++destFirst;
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
template<class InputIt, class OutputIt, class BinaryOperation, class UnaryOperation>
constexpr OutputIt transform_inclusive_scan(InputIt first, InputIt last, OutputIt destFirst,
                                            BinaryOperation bop, UnaryOperation uop) {
    // TODO: implement
    return OutputIt();
}
// END transform_inclusive_scan


// gcd
template<class MType, class NType>
constexpr CommonType_t<MType, NType> gcd(MType mval, NType nval) noexcept {
    // TODO: implement
    return 0;
}
// END gcd


// lcm
template<class MType, class NType>
constexpr CommonType_t<MType, NType> lcm(MType mval, NType nval) noexcept {
    // TODO: implement
    return 0;
}
// END lcm


// midpoint
template<class Type>
constexpr Type midpoint(Type val1, Type val2) noexcept {
    // TODO: implement
    return 0;
}

template<class Type, EnableIf_t<IsObject_v<Type>, bool> = true>
constexpr Type* midpoint(Type* const ptr1, Type* const ptr2) noexcept {
    if (ptr1 > ptr2)
        return ptr1 - ((ptr1 - ptr2) >> 1); // shift for codegen
    else
        return ptr1 + ((ptr2 - ptr1) >> 1); // shift for codegen
}
// END midpoint

CUSTOM_END