#pragma once
#include "TypeTraits.h"


CUSTOM_BEGIN

template<class Type>
constexpr void _verify_range(const Type* const first, const Type* const last) noexcept {
    CUSTOM_ASSERT(first <= last, "Transposed pointer range...");
}

template<class Iter1, class Iter2>
constexpr void _verify_iteration_range(const Iter1& first, const Iter2& last) {
    // check that [first, last) forms an iterator range
    _verify_range(first, last);
}

CUSTOM_END