#pragma once
#include "xCommon.h"


CUSTOM_BEGIN

#pragma region Non-modifying sequence operations
// all_of, any_of, none_of
template<class InputIt, class UnaryPredicate>
constexpr bool all_of(InputIt first, InputIt last, UnaryPredicate pred) {
    return false;
}

template<class InputIt, class UnaryPredicate>
constexpr bool any_of(InputIt first, InputIt last, UnaryPredicate pred) {
    return false;
}

template<class InputIt, class UnaryPredicate>
constexpr bool none_of(InputIt first, InputIt last, UnaryPredicate pred) {
    return false;
}
// END all_of, any_of, none_of


// for_each, for_each_n
template<class InputIt, class UnaryFunction>    // TODO: check
constexpr UnaryFunction for_each(InputIt first, InputIt last, UnaryFunction func) {
    for (; first != last; ++first)
        func(*first);
 
    return func; // implicit move
}

template<class InputIt, class Size, class UnaryFunction>
constexpr InputIt for_each_n(InputIt first, Size n, UnaryFunction func) {
    for (Size i = 0; i < n; ++first, (void) ++i)
        func(*first);
 
    return first;
}
// END for_each, for_each_n


// count, count_if
// template<class InputIt, class Type>
// constexpr typename iterator_traits<InputIt>::difference_type count(InputIt first, InputIt last, const Type& value) {

// }

// template<class InputIt, class UnaryPredicate>
// constexpr typename iterator_traits<InputIt>::difference_type count_if(InputIt first, InputIt last, UnaryPredicate pred) {

// }
// END count, count_if


// mismatch
template<class InputIt1, class InputIt2>
constexpr custom::Pair<InputIt1, InputIt2> mismatch(InputIt1 first1,
                                                    InputIt1 last1,
                                                    InputIt2 first2) {
    return custom::Pair<InputIt1, InputIt2>();
}

template<class InputIt1, class InputIt2, class BinaryPredicate>
constexpr custom::Pair<InputIt1, InputIt2> mismatch(InputIt1 first1,
                                                    InputIt1 last1,
                                                    InputIt2 first2,
                                                    BinaryPredicate pred) {
    return custom::Pair<InputIt1, InputIt2> ();
}

template<class InputIt1, class InputIt2>
constexpr custom::Pair<InputIt1, InputIt2> mismatch(InputIt1 first1,
                                                    InputIt1 last1,
                                                    InputIt2 first2,
                                                    InputIt2 last2) {
    return custom::Pair<InputIt1, InputIt2> ();
}

template<class InputIt1, class InputIt2, class BinaryPredicate>
constexpr custom::Pair<InputIt1, InputIt2> mismatch(InputIt1 first1,
                                                    InputIt1 last1,
                                                    InputIt2 first2,
                                                    InputIt2 last2,
                                                    BinaryPredicate pred) {
    return custom::Pair<InputIt1, InputIt2> ();
}
// END mismatch



#pragma endregion Non-modifying sequence operations


#pragma region Modifying sequence operations
// TODO: implemet

// fill, fill_n
template<class ForwardIt, class Type>
constexpr void fill(ForwardIt first, ForwardIt last, const Type& value) {
    for (; first != last; ++first)  // TODO: check
        *first = value;
}

template<class OutputIt, class Size, class Type>
OutputIt fill_n(OutputIt first, Size count, const Type& value) {
    for (Size i = 0; i < count; ++i)
        *first++ = value;

    return first;   // TODO: check
}
// END fill, fill_n
#pragma endregion Modifying sequence operations


#pragma region Partitioning operations
// TODO: implement
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
#pragma endregion Minimum/maximum operations


#pragma region Comparison operations
// TODO: implement
#pragma endregion Comparison operations


#pragma region Permutation operations
// TODO: implement
#pragma endregion Permutation operations

CUSTOM_END