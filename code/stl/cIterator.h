#pragma once
#include "xIterator.h"


CUSTOM_BEGIN

// helpers for ReverseIterator operator->()
template<class Iterator, class Pointer, bool = IsPointer_v<RemoveCVRef_t<Iterator>>>
constexpr static bool _HasNothrowOperatorArrow = IsNothrowConvertible_v<Iterator, Pointer>;

template<class Iterator, class Pointer>
constexpr static bool _HasNothrowOperatorArrow<Iterator, Pointer, false> =
noexcept(_fake_copy_init<Pointer>(custom::declval<Iterator>().operator->()));

template<class Iterator>
class ReverseIterator                           // Adaptor for backwards iteration
{
public:
    using IteratorType      = Iterator;
    using IteratorCategory  = typename IteratorTraits<Iterator>::IteratorCategory;
    using ValueType         = typename IteratorTraits<Iterator>::ValueType;
    using DifferenceType    = typename IteratorTraits<Iterator>::DifferenceType;
    using Reference		    = typename IteratorTraits<Iterator>::Reference;
	using Pointer		    = typename IteratorTraits<Iterator>::Pointer;

    IteratorType Current;

public:

    constexpr ReverseIterator()                             = default;
    constexpr ReverseIterator(const ReverseIterator& other) = default;

    constexpr explicit ReverseIterator(IteratorType base)
    noexcept(IsNothrowCopyConstructible_v<IteratorType>)
        : Current(base) { /*Empty*/ }

    constexpr ReverseIterator& operator=(const ReverseIterator& other) = default;

    constexpr ReverseIterator& operator++() noexcept(noexcept(--Current)) {
        --Current;
        return *this;
    }

    constexpr ReverseIterator operator++(int)
    noexcept(IsNothrowCopyConstructible_v<IteratorType> && noexcept(--Current)) {
        ReverseIterator temp = *this;
        --Current;
        return temp;
    }

    constexpr ReverseIterator& operator--() noexcept(noexcept(++Current)) {
        ++Current;
        return *this;
    }

    constexpr ReverseIterator operator--(int)
    noexcept(IsNothrowCopyConstructible_v<IteratorType> && noexcept(++Current)) {
        ReverseIterator temp = *this;
        ++Current;
        return temp;
    }

    constexpr Pointer operator->() const
    noexcept(   IsNothrowCopyConstructible_v<IteratorType> &&
                noexcept(--(custom::declval<IteratorType&>())) &&
                _HasNothrowOperatorArrow<IteratorType&, Pointer>) {

        IteratorType temp = Current;
        --temp;

        if constexpr (IsPointer_v<IteratorType>)
            return temp;

        return temp.operator->();
    }

    constexpr Reference operator*() const
    noexcept(IsNothrowCopyConstructible_v<IteratorType> && noexcept(*--(custom::declval<IteratorType&>()))) {
        IteratorType temp = Current;
        return *(--temp);
    }

    constexpr IteratorType base() const
    noexcept(IsNothrowCopyConstructible_v<IteratorType>) {
        return Current;
    }

    constexpr const IteratorType& const_ref_base() const noexcept {
        return Current;
    }
}; // END ReverseIterator


// ReverseIterator binary operators
template<class Iterator1, class Iterator2>
constexpr bool operator==(const ReverseIterator<Iterator1>& left, const ReverseIterator<Iterator2>& right)
noexcept(noexcept(_fake_copy_init<bool>(left.const_ref_base() == right.const_ref_base()))) {
    return left.const_ref_base() == right.const_ref_base();
}

template<class Iterator1, class Iterator2>
constexpr bool operator!=(const ReverseIterator<Iterator1>& left, const ReverseIterator<Iterator2>& right)
noexcept(noexcept(_fake_copy_init<bool>(left.const_ref_base() != right.const_ref_base()))) {
    return left.const_ref_base() != right.const_ref_base();
}


// iterator functions
template<class InputIt>
constexpr typename IteratorTraits<InputIt>::DifferenceType distance(InputIt first, InputIt last) {
    _verify_iteration_range(first, last);
    
    if constexpr (IsRandomAccessIterator_v<InputIt>)
        return last - first; // assume the iterator will do debug checking
    else
    {
        typename IteratorTraits<InputIt>::DifferenceType dist = 0;

        for (/*Empty*/; first != last; ++first)
            ++dist;

        return dist;
    }
}

template<class InputIt, class Offset>
constexpr void advance(InputIt& first, Offset off) {
    if constexpr (IsRandomAccessIterator_v<InputIt>)
        first += off;
    else
    {
        if constexpr (IsSigned_v<Offset> && !IsBidirectionalIterator_v<InputIt>)
            CUSTOM_ASSERT(off >= 0, "Negative advance of non-bidirectional iterator!");

        for (/*Empty*/; off > 0; --off)  // happens for both input and bidirectional iterator
            ++first;

        if constexpr (IsSigned_v<Offset> && IsBidirectionalIterator_v<InputIt>)
            for (/*Empty*/; off < 0; ++off)
                --first;
    }
}

template<class InputIt>
constexpr InputIt next(InputIt first, typename custom::IteratorTraits<InputIt>::DifferenceType off = 1) {
    static_assert(IsInputIterator_v<InputIt>, "next requires input iterator!");

    custom::advance(first, off);
    return first;
}

template<class InputIt>
constexpr InputIt prev(InputIt first, typename custom::IteratorTraits<InputIt>::DifferenceType off = 1) {
    static_assert(IsBidirectionalIterator_v<InputIt>, "prev requires bidirectional iterator!");

    custom::advance(first, -off);
    return first;
}

CUSTOM_END