#pragma once
#include "TypeTraits.h"


CUSTOM_BEGIN

template<class Ty = void>
struct EqualTo
{
    constexpr bool operator()(const Ty& left, const Ty& right) const
    noexcept(noexcept(_fake_copy_init<bool>(left == right))) {
        return left == right;
    }
};

template<>
struct EqualTo<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(static_cast<Ty1&&>(left) == static_cast<Ty2&&>(right)))
    -> decltype(static_cast<Ty1&&>(left) == static_cast<Ty2&&>(right)) {
        return static_cast<Ty1&&>(left) == static_cast<Ty2&&>(right);
    }

    using IsTransparent = int;
};

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

template<class Iter1, class Iter2, class Predicate>
constexpr bool equal(const Iter1 first1, const Iter1 last1, const Iter2 first2, Predicate pred) {
    // compare [first1, last1) to [first2, ...)

    //_Adl_verify_range(first1, last1);
    //auto _UFirst1 = _Get_unwrapped(first1);
    //const auto _ULast1 = _Get_unwrapped(last1);
    //auto _UFirst2 = _Get_unwrapped_n(first2, _Idl_distance<Iter1>(_UFirst1, _ULast1));

    while (first1 != last1)
    {
        if (!pred(*first1, *first2))
            return false;

        ++first1;
        ++first2;
    }

    return true;
}

template<class Iter1, class Iter2>
constexpr bool equal(const Iter1 first1, const Iter1 last1, const Iter2 first2) {
    // compare [first1, last1) to [first2, ...)

    return custom::equal(first1, last1, first2, EqualTo<>{});
}

CUSTOM_END