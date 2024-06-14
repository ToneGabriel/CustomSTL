#pragma once
#include "cFunctional.h"
#include "c_iterator.h"
#include "c_pair.h"


CUSTOM_BEGIN

#pragma region Non-modifying sequence operations
// all_of, any_of, none_of
template<class InputIt, class UnaryPredicate>
constexpr bool all_of(InputIt first, InputIt last, UnaryPredicate pred) {
    _verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first)
        if (!pred(*first))
            return false;

    return true;
}

template<class InputIt, class UnaryPredicate>
constexpr bool any_of(InputIt first, InputIt last, UnaryPredicate pred) {
    _verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first)
        if (pred(*first))
            return true;

    return false;
}

template<class InputIt, class UnaryPredicate>
constexpr bool none_of(InputIt first, InputIt last, UnaryPredicate pred) {
    return !any_of(first, last, pred);
}
// END all_of, any_of, none_of


// for_each, for_each_n
template<class InputIt, class UnaryFunction>
constexpr UnaryFunction for_each(InputIt first, InputIt last, UnaryFunction func) {
    _verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first)
        func(*first);

    return func; // implicit move
}

template<class InputIt, class Size, class UnaryFunction>
constexpr InputIt for_each_n(InputIt first, Size n, UnaryFunction func) {
    for (Size i = 0; i < n; ++first, (void)++i)
        func(*first);
 
    return first;
}
// END for_each, for_each_n


// count, count_if
template<class InputIt, class Type>
constexpr typename iterator_traits<InputIt>::DifferenceType count(InputIt first, InputIt last, const Type& value) {
    _verify_iteration_range(first, last);
    
    typename iterator_traits<InputIt>::DifferenceType ret = 0;

    for (/*Empty*/; first != last; ++first)
        if (*first == value)
            ++ret;

    return ret;
}

template<class InputIt, class UnaryPredicate>
constexpr typename iterator_traits<InputIt>::DifferenceType count_if(InputIt first, InputIt last, UnaryPredicate pred) {
    _verify_iteration_range(first, last);
    
    typename iterator_traits<InputIt>::DifferenceType ret = 0;

    for (/*Empty*/; first != last; ++first)
        if (pred(*first))
            ++ret;

    return ret;
}
// END count, count_if


// mismatch
template<class InputIt1, class InputIt2, class BinaryPredicate>
constexpr custom::pair<InputIt1, InputIt2> mismatch(InputIt1 first1, InputIt1 last1,
                                                    InputIt2 first2, BinaryPredicate pred) {
    
    _verify_iteration_range(first1, last1);
    for (/*Empty*/; first1 != last1 && pred(*first1, *first2); ++first1, ++first2) { /*do nothing*/ }
    return {first1, first2};
}

template<class InputIt1, class InputIt2>
constexpr custom::pair<InputIt1, InputIt2> mismatch(InputIt1 first1, InputIt1 last1, InputIt2 first2) {
    return custom::mismatch(first1, last1, first2, EqualTo<>{});
}

template<class InputIt1, class InputIt2, class BinaryPredicate>
constexpr custom::pair<InputIt1, InputIt2> mismatch(InputIt1 first1, InputIt1 last1,
                                                    InputIt2 first2, InputIt2 last2,
                                                    BinaryPredicate pred) {
    
    _verify_iteration_range(first1, last1);
    _verify_iteration_range(first2, last2);
    for (/*Empty*/; first1 != last1 && first2 != last2 && pred(*first1, *first2); ++first1, ++first2) { /*do nothing*/ }
    return {first1, first2};
}

template<class InputIt1, class InputIt2>
constexpr custom::pair<InputIt1, InputIt2> mismatch(InputIt1 first1, InputIt1 last1,
                                                    InputIt2 first2, InputIt2 last2) {

    return custom::mismatch(first1, last1, first2, last2, EqualTo<>{});
}
// END mismatch


// search
template<class ForwardIt1, class ForwardIt2, class BinaryPredicate>
constexpr ForwardIt1 search(ForwardIt1 first, ForwardIt1 last,
                            ForwardIt2 seqFirst, ForwardIt2 seqLast,
                            BinaryPredicate pred) {

    _verify_iteration_range(first, last);
    
    for(;; ++first)
    {
        ForwardIt1 it = first;
        for (ForwardIt2 seqIt = seqFirst; /* Empty */ ; ++it, ++seqIt)
        {
            if (seqIt == seqLast)
                return first;
            if (it == last)
                return last;
            if (!pred(*it, *seqIt))
                break;
        }
    }
}

template<class ForwardIt1, class ForwardIt2>
constexpr ForwardIt1 search(ForwardIt1 first, ForwardIt1 last,
                            ForwardIt2 seqFirst, ForwardIt2 seqLast) {

    return custom::search(first, last, seqFirst, seqLast, EqualTo<>{});
}
// END search


// find, find_if, find_if_not
template<class InputIt, class Type>
constexpr InputIt find(InputIt first, InputIt last, const Type& value) {
    _verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first)
        if (*first == value)
            return first;
 
    return last;
}

template<class InputIt, class UnaryPredicate>
constexpr InputIt find_if(InputIt first, InputIt last, UnaryPredicate pred) {
    _verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first)
        if (pred(*first))
            return first;
 
    return last;
}

template<class InputIt, class UnaryPredicate>
constexpr InputIt find_if_not(InputIt first, InputIt last, UnaryPredicate pred) {
    _verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first)
        if (!pred(*first))
            return first;
 
    return last;
}
// END find, find_if, find_if_not
#pragma endregion Non-modifying sequence operations


#pragma region Modifying sequence operations
// copy, copy_if, copy_n, copy_backward
template<class InputIt, class OutputIt>
constexpr OutputIt copy(InputIt first, InputIt last, OutputIt destFirst) {
    _verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first, ++destFirst)
        *destFirst = *first;

    return destFirst;
}

template<class InputIt, class OutputIt, class UnaryPredicate>
constexpr OutputIt copy_if(InputIt first, InputIt last, OutputIt destFirst, UnaryPredicate pred) {
    _verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first)
        if (pred(*first))
        {
            *destFirst = *first;
            ++destFirst;
        }
 
    return destFirst;
}

template<class InputIt, class Size, class OutputIt>
constexpr OutputIt copy_n(InputIt first, Size count, OutputIt destFirst) {
    if (count > 0)
    {
        *destFirst = *first;
        ++destFirst;
        for (Size i = 1; i != count; ++i, ++destFirst)
            *destFirst = *++first;
    }
 
    return destFirst;
}

template<class BidirIt1, class BidirIt2>
constexpr BidirIt2 copy_backward(BidirIt1 first, BidirIt1 last, BidirIt2 destLast) {
    _verify_iteration_range(first, last);

    while (first != last)
        *(--destLast) = *(--last);

    return destLast;
}
// END copy, copy_if, copy_n, copy_backward


// move, move_backwards
template<class InputIt, class OutputIt>
constexpr OutputIt move(InputIt first, InputIt last, OutputIt destFirst) {
    _verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first, ++destFirst)
        *destFirst = custom::move(*first);

    return destFirst;
}

template<class BidirIt1, class BidirIt2>
constexpr BidirIt2 move_backward(BidirIt1 first, BidirIt1 last, BidirIt2 destLast) {
    _verify_iteration_range(first, last);

    while (first != last)
        *(--destLast) = custom::move(*(--last));

    return destLast;
}
// END move, move_backwards


// transform
template<class InputIt, class OutputIt, class UnaryFunction>
constexpr OutputIt transform( InputIt first, InputIt last,
                              OutputIt destFirst, UnaryFunction func) {

    _verify_iteration_range(first, last);

    while (first != last)
        *destFirst++ = func(*first++);
 
    return destFirst;
}

template<class InputIt1, class InputIt2, class OutputIt, class BinaryFunction>
constexpr OutputIt transform(   InputIt1 first1, InputIt1 last1, 
                                InputIt2 first2, OutputIt destFirst,
                                BinaryFunction func) {

    _verify_iteration_range(first1, last1);

    while (first1 != last1)
        *destFirst++ = func(*first1++, *first2++);
 
    return destFirst;
}
// END transform


// generate, generate_n
template<class ForwardIt, class Generator>
constexpr void generate(ForwardIt first, ForwardIt last, Generator gen) {
    _verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first)
        *first = gen();
}

template<class OutputIt, class Size, class Generator>
constexpr OutputIt generate_n(OutputIt first, Size count, Generator gen) {
    for (Size i = 0; i < count; ++i, ++first)
        *first = gen();
 
    return first;
}
//END generate, generate_n


// fill, fill_n
template<class ForwardIt, class Type>
constexpr void fill(ForwardIt first, ForwardIt last, const Type& value) {
    _verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first)
        *first = value;
}

template<class OutputIt, class Size, class Type>
OutputIt fill_n(OutputIt first, Size count, const Type& value) {
    for (Size i = 0; i < count; ++i)
        *first++ = value;

    return first;
}
// END fill, fill_n


// iter_swap, swap_ranges
template<class ForwardIt1, class ForwardIt2>
constexpr void iter_swap(ForwardIt1 it1, ForwardIt2 it2) {
    custom::swap(*it1, *it2);
}

template<class ForwardIt1, class ForwardIt2>
constexpr ForwardIt2 swap_ranges(ForwardIt1 first1, ForwardIt1 last1, ForwardIt2 first2) {
    _verify_iteration_range(first1, last1);
    
    for (/*Empty*/; first1 != last1; ++first1, ++first2)
        custom::iter_swap(first1, first2);

    return first2;
}
// END iter_swap, swap_ranges
#pragma endregion Modifying sequence operations


#pragma region Partitioning operations
// partition, partition_point, is_partitioned, partition_copy
template<class ForwardIt, class UnaryPredicate>
constexpr ForwardIt partition(ForwardIt first, ForwardIt last, UnaryPredicate pred) {
    _verify_iteration_range(first, last);

    first = custom::find_if_not(first, last, pred);

    if (first == last)
        return first;

    for (ForwardIt it = custom::next(first); it != last; ++it)
        if (pred(*it))
        {
            custom::iter_swap(it, first);
            ++first;
        }

    return first;
}

template<class ForwardIt, class UnaryPredicate>
constexpr ForwardIt partition_point(ForwardIt first, ForwardIt last, UnaryPredicate pred) {
    _verify_iteration_range(first, last);

    for (auto length = custom::distance(first, last); 0 < length; )
    {
        auto half           = length / 2;
        ForwardIt middle    = custom::next(first, half);

        if (pred(*middle))
        {
            first   = custom::next(middle);
            length  -= (half + 1);
        }
        else
            length = half;
    }
 
    return first;
}

template<class InputIt, class UnaryPredicate>
constexpr bool is_partitioned(InputIt first, InputIt last, UnaryPredicate pred) {
    _verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first)
        if (!pred(*first))
            break;

    for (/*Empty*/; first != last; ++first)
        if (pred(*first))
            return false;

    return true;
}

template<class InputIt, class OutputIt1, class OutputIt2, class UnaryPredicate>
custom::pair<OutputIt1, OutputIt2> partition_copy(  InputIt first, InputIt last,
                                                    OutputIt1 destFirstTrue, OutputIt2 destFirstFalse,
                                                    UnaryPredicate pred) {

    _verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first)
        if (pred(*first))
        {
            *destFirstTrue = *first;
            ++destFirstTrue;
        }
        else
        {
            *destFirstFalse = *first;
            ++destFirstFalse;
        }
 
    return {destFirstTrue, destFirstFalse};
}
// END partition, partition_point, is_partitioned, partition_copy
#pragma endregion Partitioning operations


#pragma region Sorting operations
// TODO: implement
#pragma endregion Sorting operations


#pragma region Binary search operations // (on sorted ranges)
// TODO: implement
#pragma endregion Binary search operations


#pragma region Other operations on sorted ranges
// TODO: implement
#pragma endregion Other operations on sorted ranges


#pragma region Set operations // (on sorted ranges)
// TODO: implement
// includes, set_intersection
template<class InputIt1, class InputIt2, class BinaryPredicate>
constexpr bool includes(InputIt1 first1, InputIt1 last1,
                        InputIt2 first2, InputIt2 last2,
                        BinaryPredicate pred) {

    // test if every element in sorted [first2, last2) is in sorted [first1, last1)

    _verify_iteration_range(first1, last1);
    _verify_iteration_range(first2, last2);
    
    _verify_iteration_order<InputIt2>(first1, last1, pred);
    _verify_iteration_order<InputIt1>(first2, last2, pred);

    for (/*Empty*/; first1 != last1 && first2 != last2; ++first1)
    {
        if (_verify_predicate_order(pred, *first2, *first1))
            return false;

        if (!pred(*first1, *first2))
            ++first2;
    }

    return first2 == last2;
}

template<class InputIt1, class InputIt2>
constexpr bool includes(InputIt1 first1, InputIt1 last1,
                        InputIt2 first2, InputIt2 last2) {
    return custom::includes(first1, last1, first2, last2, Less<>{});
}

template<class InputIt1, class InputIt2, class OutputIt, class BinaryPredicate>
constexpr OutputIt set_intersection(InputIt1 first1, InputIt1 last1,
                                    InputIt2 first2, InputIt2 last2,
                                    OutputIt destFirst, BinaryPredicate pred) {

    _verify_iteration_range(first1, last1);
    _verify_iteration_range(first2, last2);
    
    _verify_iteration_order<InputIt2>(first1, last1, pred);
    _verify_iteration_order<InputIt1>(first2, last2, pred);

    while (first1 != last1 && first2 != last2)
    {
        if (_verify_predicate_order(pred, *first1, *first2))
            ++first1;
        else if (pred(*first2, *first1))
            ++first2;
        else
        {
            *destFirst = *first1;
            ++destFirst;
            ++first1;
            ++first2;
        }
    }

    return destFirst;
}

template<class InputIt1, class InputIt2, class OutputIt>
constexpr OutputIt set_intersection(InputIt1 first1, InputIt1 last1,
                                    InputIt2 first2, InputIt2 last2) {
    return custom::set_intersection(first1, last1, first2, last2, Less<>{});
}
#pragma endregion Set operations


#pragma region Heap operations
template<class RandomIt>
constexpr void make_heap(RandomIt first, RandomIt last) {
    // TODO: implement
}

template<class RandomIt, class Compare>
constexpr void make_heap(RandomIt first, RandomIt last, Compare comp) {
    // TODO: implement
}

template<class RandomIt>
constexpr void push_heap(RandomIt first, RandomIt last) {
    // TODO: implement
}

template<class RandomIt, class Compare>
constexpr void push_heap(RandomIt first, RandomIt last, Compare comp) {
    // TODO: implement
}

template<class RandomIt>
constexpr void pop_heap(RandomIt first, RandomIt last) {
    // TODO: implement
}

template<class RandomIt, class Compare>
constexpr void pop_heap(RandomIt first, RandomIt last, Compare comp) {
    // TODO: implement
}

template<class RandomIt>
constexpr void sort_heap(RandomIt first, RandomIt last) {
    // TODO: implement
}

template<class RandomIt, class Compare>
constexpr void sort_heap(RandomIt first, RandomIt last, Compare comp) {
    // TODO: implement
}

template<class RandomIt>
constexpr bool is_heap(RandomIt first, RandomIt last) {
    // TODO: implement
    return false;
}

template<class RandomIt, class Compare>
constexpr bool is_heap(RandomIt first, RandomIt last, Compare comp) {
    // TODO: implement
    return false;
}

template<class RandomIt>
constexpr RandomIt is_heap_until(RandomIt first, RandomIt last) {
    // TODO: implement
    return RandomIt();
}

template<class RandomIt, class Compare>
constexpr RandomIt is_heap_until(RandomIt first, RandomIt last, Compare comp) {
    // TODO: implement
    return RandomIt();
}
#pragma endregion Heap operations


#pragma region Minimum/maximum operations
// max
template<class Type>
constexpr const Type& (max)(const Type& a, const Type& b) {
    return (a > b) ? a : b;
}

template<class Type, class Compare>
constexpr const Type& (max)(const Type& a, const Type& b, Compare comp) {
    return (comp(a, b)) ? a : b;
}
// END max

// max_element
template<class ForwardIt, class Compare>
ForwardIt max_element(ForwardIt first, ForwardIt last, Compare comp) {
    _verify_iteration_range(first, last);
    
    if (first == last)
        return last;
 
    ForwardIt largest = first;
    ++first;
 
    for (/*Empty*/; first != last; ++first)
        if (comp(*largest, *first))
            largest = first;
 
    return largest;
}

template<class ForwardIt>
ForwardIt max_element(ForwardIt first, ForwardIt last) {
    return custom::max_element(first, last, Less<>{});
}
// END max_element

// min
template<class Type>
constexpr const Type& (min)(const Type& a, const Type& b) {
    return (a < b) ? a : b;
}

template<class Type, class Compare>
constexpr const Type& (min)(const Type& a, const Type& b, Compare comp) {
    return (comp(a, b)) ? a : b;
}
// END min

// min_element
template<class ForwardIt, class Compare>
ForwardIt min_element(ForwardIt first, ForwardIt last, Compare comp) {
    _verify_iteration_range(first, last);

    if (first == last)
        return last;
 
    ForwardIt smallest = first;
    ++first;
 
    for (/*Empty*/; first != last; ++first)
        if (comp(*first, *smallest))
            smallest = first;
 
    return smallest;
}

template<class ForwardIt>
ForwardIt min_element(ForwardIt first, ForwardIt last) {
    return custom::min_element(first, last, Less<>{});
}
// END min_element
#pragma endregion Minimum/maximum operations


#pragma region Comparison operations
// equal
template<class InputIt1, class InputIt2, class BinaryPredicate>
constexpr bool equal(   InputIt1 first1, InputIt1 last1,
                        InputIt2 first2, BinaryPredicate pred) {    // compare [first1, last1) to [first2, ...)

    _verify_iteration_range(first1, last1);

    for (/*Empty*/; first1 != last1; ++first1, ++first2)
        if (!pred(*first1, *first2))
            return false;

    return true;
}

template<class InputIt1, class InputIt2>
constexpr bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2) {
    return custom::equal(first1, last1, first2, EqualTo<>{});
}
// END equal


// lexicographical_compare
template<class InputIt1, class InputIt2, class Compare>
constexpr bool lexicographical_compare( InputIt1 first1, InputIt1 last1,
                                        InputIt2 first2, InputIt2 last2, Compare comp) {

    _verify_iteration_range(first1, last1);
    _verify_iteration_range(first2, last2);

    for (/*Empty*/; (first1 != last1) && (first2 != last2); ++first1, ++first2)
    {
        if (comp(*first1, *first2))
            return true;
        if (comp(*first2, *first1))
            return false;
    }
 
    return (first1 == last1) && (first2 != last2);
}

template<class InputIt1, class InputIt2>
constexpr bool lexicographical_compare( InputIt1 first1, InputIt1 last1,
                                        InputIt2 first2, InputIt2 last2) {

    return custom::lexicographical_compare(first1, last1, first2, last2, Less<>{});
}
// END lexicographical_compare
#pragma endregion Comparison operations


#pragma region Permutation operations
// TODO: implement
#pragma endregion Permutation operations

CUSTOM_END