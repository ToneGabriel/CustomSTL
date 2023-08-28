#pragma once
#include "xCommon.h"


CUSTOM_BEGIN

#pragma region heap
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
#pragma endregion heap

#pragma region fill_n
template<class OutputIt, class Size, class Type>
OutputIt fill_n(OutputIt first, Size count, const Type& value) {
    // TODO: implement
    return OutputIt();
}
#pragma endregion fill_n

CUSTOM_END