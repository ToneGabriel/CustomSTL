#pragma once
#include "cTypeTraits.h"

CUSTOM_BEGIN

// iterator tags
struct InputIteratorTag {};
struct OutputIteratorTag {};
struct ForwardIteratorTag       : InputIteratorTag {};
struct BidirectionalIteratorTag : ForwardIteratorTag {};
struct RandomAccessIteratorTag  : BidirectionalIteratorTag {};

// normal iterator traits helpers
template<class Iterator, class = void>
struct _IteratorTraits {};

template<class Iterator>
struct _IteratorTraits<Iterator, Void_t<    typename Iterator::IteratorCategory,
                                            typename Iterator::ValueType,
                                            typename Iterator::DifferenceType,
                                            typename Iterator::Pointer,
                                            typename Iterator::Reference>>
{
    using IteratorCategory = typename Iterator::IteratorCategory;
    using ValueType        = typename Iterator::ValueType;
    using DifferenceType   = typename Iterator::DifferenceType;
    using Pointer          = typename Iterator::Pointer;
    using Reference        = typename Iterator::Reference;
};

// pointer iterator traits helpers
template<class Type, bool = IsObject_v<Type>>
struct _IteratorTraitsPtr
{
    using IteratorCategory = RandomAccessIteratorTag;
    using ValueType        = RemoveCV_t<Type>;
    using DifferenceType   = ptrdiff_t;
    using Pointer          = Type*;
    using Reference        = Type&;
};

template<class Type>
struct _IteratorTraitsPtr<Type, false> {};

// iterator traits
template<class Iterator>
struct IteratorTraits : _IteratorTraits<Iterator> {};

template<class Type>
struct IteratorTraits<Type*> : _IteratorTraitsPtr<Type> {};

// is iterator
template<class Iter, class = void>
constexpr bool IsIterator_v = false;

template<class Iter>
constexpr bool IsIterator_v<Iter, Void_t<typename IteratorTraits<Iter>::IteratorCategory>> = true;

template<class Iter>
struct IsIterator : BoolConstant<IsIterator_v<Iter>> {};

// is input iterator
template<class Iter>
constexpr bool IsInputIterator_v = IsConvertible_v<typename IteratorTraits<Iter>::IteratorCategory, InputIteratorTag>;

// is output iterator
template<class Iter>
constexpr bool IsOutputIterator_v = IsConvertible_v<typename IteratorTraits<Iter>::IteratorCategory, OutputIteratorTag>;

// is forward iterator
template<class Iter>
constexpr bool IsForwardIterator_v = IsConvertible_v<typename IteratorTraits<Iter>::IteratorCategory, ForwardIteratorTag>;

// is bidirectional iterator
template<class Iter>
constexpr bool IsBidirectionalIterator_v = IsConvertible_v<typename IteratorTraits<Iter>::IteratorCategory, BidirectionalIteratorTag>;

// is random access iterator
template<class Iter>
constexpr bool IsRandomAccessIterator_v = IsConvertible_v<typename IteratorTraits<Iter>::IteratorCategory, RandomAccessIteratorTag>;


// helpers
template<class Type>
constexpr void _verify_range(const Type* const first, const Type* const last) noexcept {
    // ALL containers have this function implemented as friend in their iterators
    // This is a specialization for raw pointers
    CUSTOM_ASSERT(first <= last, "Transposed pointer range.");
}

template<class Iter1, class Iter2>
constexpr void _verify_iteration_range(const Iter1& first, const Iter2& last) {
    // check that [first, last) forms an iterator range
    _verify_range(first, last);
}

template<class OtherIter, class InputIt, class BinaryPredicate>
constexpr void _verify_iteration_order(InputIt first, InputIt last, BinaryPredicate&& pred) {
    // test if range is ordered by predicate
    // and the value types match for the 2 iterator types

    using OtherValue    = typename IteratorTraits<OtherIter>::ValueType;
    using IterValue     = typename IteratorTraits<InputIt>::ValueType;

    if constexpr (IsSame_v<OtherValue, IterValue> && IsForwardIterator_v<InputIt>)
        if (first != last)
            for (auto next = first; ++next != last; first = next)
                CUSTOM_ASSERT(!static_cast<bool>(pred(*next, *first)), "Sequence not ordered");
}

template<class BinaryPredicate, class Type1, class Type2,
EnableIf_t<IsSame_v<RemoveCVRef_t<Type1>, RemoveCVRef_t<Type2>>, bool> = true>
constexpr bool _verify_predicate_order(BinaryPredicate&& pred, Type1&& left, Type2&& right)
noexcept(noexcept(pred(left, right)) && noexcept(pred(right, left))) {
    // test if pred is strict weak ordering
    // when the arguments are the cv-same-type

    const bool res = static_cast<bool>(pred(left, right));

    if (res)
        CUSTOM_ASSERT(!pred(right, left), "Invalid comparator");

    return res;
}

CUSTOM_END