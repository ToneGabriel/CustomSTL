#pragma once
#include "Utility.h"


CUSTOM_BEGIN

// iterator tags
struct InputIteratorTag {};
struct OutputIteratorTag {};
struct ForwardIteratorTag : InputIteratorTag {};
struct BidirectionalIteratorTag : ForwardIteratorTag {};
struct RandomAccessIteratorTag : BidirectionalIteratorTag {};

// normal iterator traits helpers
template<class Iterator, class = void>
struct _IteratorTraits {};

template<class Iterator>
struct _IteratorTraits<Iterator, Void_t<    typename Iterator::IteratorCategory,
                                            typename Iterator::ValueType,
                                            /*typename Iterator::DifferenceType,*/
                                            typename Iterator::Pointer,
                                            typename Iterator::Reference>>
{
    using IteratorCategory = typename Iterator::IteratorCategory;
    using ValueType        = typename Iterator::ValueType;
    //using DifferenceType   = typename Iterator::DifferenceType;
    using Pointer           = typename Iterator::Pointer;
    using Reference         = typename Iterator::Reference;
};

// pointer iterator traits helpers
template<class Type, bool = IsObject_v<Type>>
struct _IteratorTraitsPtr
{
    using IteratorCategory = RandomAccessIteratorTag;
    using ValueType        = RemoveCV_t<Type>;
    //using DifferenceType   = ptrdiff_t;
    using Pointer           = Type*;
    using Reference         = Type&;
};

template<class Type>
struct _IteratorTraitsPtr<Type, false> {};

// iterator traits
template<class Iterator>
struct IteratorTraits : _IteratorTraits<Iterator> {};

template<class Type>
struct IteratorTraits<Type*> : _IteratorTraitsPtr<Type> {};

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
    //using DifferenceType   = typename IteratorTraits<Iterator>::DifferenceType;
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


CUSTOM_END