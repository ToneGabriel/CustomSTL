#pragma once
#include "TypeTraits.h"


CUSTOM_BEGIN

template<class Type>
constexpr void _verify_range(const Type* const first, const Type* const last) noexcept {
    CUSTOM_ASSERT(first <= last, "Transposed pointer range...");
}

//template<class Iter, class Sentinel>
//constexpr void _adl_verify_range(const Iter& first, const Sentinel& last) {
//    // check that [first, last) forms an iterator range
//    _verify_range(first, last);
//    //if (_Range_verifiable_v<Iter, Sentinel>) {
//    //}
//}

CUSTOM_END