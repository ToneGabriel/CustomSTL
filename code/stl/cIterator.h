#pragma once
#include "xIterator.h"


CUSTOM_BEGIN

#pragma region BackInsertIterator
// helpers
template<class Container, class = void>
constexpr bool _HasPushBackFunctionMember_v = false;

template<class Container>
constexpr bool _HasPushBackFunctionMember_v<Container,
Void_t< typename Container::ValueType,
        decltype(custom::declval<Container>().push_back(custom::declval<typename Container::ValueType>())),
        decltype(custom::declval<Container>().push_back(custom::move(custom::declval<typename Container::ValueType>())))>> = true;

template<class Container>
class BackInsertIterator
{
public:
    static_assert(_HasPushBackFunctionMember_v<Container>,
                    "BackInsertIterator require a container with push_back function member!");

    using ContainerType     = Container;
    using IteratorCategory  = OutputIteratorTag;
    using ValueType         = void;
    using DifferenceType    = void;
    using Reference		    = void;
	using Pointer		    = void;

protected:
    Container* _container;

public:

    constexpr explicit BackInsertIterator(Container& cont) noexcept
        : _container(&cont) { /*Empty*/ }

    constexpr BackInsertIterator& operator=(const typename Container::ValueType& val) {
        _container->push_back(val);
        return *this;
    }

    constexpr BackInsertIterator& operator=(typename Container::ValueType&& val) {
        _container->push_back(custom::move(val));
        return *this;
    }

    constexpr BackInsertIterator& operator*() noexcept {
        return *this;
    }

    constexpr BackInsertIterator& operator++() noexcept {
        return *this;
    }

    constexpr BackInsertIterator operator++(int) noexcept {
        return *this;
    }
};

template<class Container>
constexpr BackInsertIterator<Container> back_inserter(Container& cont) noexcept {
    return BackInsertIterator<Container>(cont);
}
#pragma endregion BackInsertIterator

#pragma region FrontInsertIterator
// helpers
template<class Container, class = void>
constexpr bool _HasPushFrontFunctionMember_v = false;

template<class Container>
constexpr bool _HasPushFrontFunctionMember_v<Container,
Void_t< typename Container::ValueType,
        decltype(custom::declval<Container>().push_front(custom::declval<typename Container::ValueType>())),
        decltype(custom::declval<Container>().push_front(custom::move(custom::declval<typename Container::ValueType>())))>> = true;

template<class Container>
class FrontInsertIterator
{
public:
    static_assert(_HasPushFrontFunctionMember_v<Container>,
                    "FrontInsertIterator require a container with push_front function member!");

    using ContainerType     = Container;
    using IteratorCategory  = OutputIteratorTag;
    using ValueType         = void;
    using DifferenceType    = void;
    using Reference		    = void;
	using Pointer		    = void;

protected:
    Container* _container;

public:

    constexpr explicit FrontInsertIterator(Container& cont) noexcept
        : _container(&cont) { /*Empty*/ }

    constexpr FrontInsertIterator& operator=(const typename Container::ValueType& val) {
        _container->push_front(val);
        return *this;
    }

    constexpr FrontInsertIterator& operator=(typename Container::ValueType&& val) {
        _container->push_front(custom::move(val));
        return *this;
    }

    constexpr FrontInsertIterator& operator*() noexcept {
        return *this;
    }

    constexpr FrontInsertIterator& operator++() noexcept {
        return *this;
    }

    constexpr FrontInsertIterator operator++(int) noexcept {
        return *this;
    }
};

template<class Container>
constexpr FrontInsertIterator<Container> front_inserter(Container& cont) noexcept {
    return FrontInsertIterator<Container>(cont);
}
#pragma endregion FrontInsertIterator

#pragma region InsertIterator
// helpers
template<class Container, class = void>
constexpr bool _HasInsertFunctionMember_v = false;

template<class Container>
constexpr bool _HasInsertFunctionMember_v<Container,
Void_t< typename Container::ValueType,
        typename Container::Iterator,
        decltype(custom::declval<Container>().insert(   custom::declval<typename Container::Iterator>(),
                                                        custom::declval<typename Container::ValueType>())),
        decltype(custom::declval<Container>().insert(   custom::declval<typename Container::Iterator>(),
                                                        custom::move(custom::declval<typename Container::ValueType>())))>> = true;

template<class Container>
class InsertIterator
{
public:
    static_assert(_HasInsertFunctionMember_v<Container>,
                    "InsertIterator require a container with insert function member!");

    using ContainerType     = Container;
    using IteratorType      = typename Container::Iterator;
    using IteratorCategory  = OutputIteratorTag;
    using ValueType         = void;
    using DifferenceType    = void;
    using Reference		    = void;
	using Pointer		    = void;

protected:
    Container* _container;
    IteratorType iter;

public:

    constexpr InsertIterator(Container& cont, IteratorType iter)
        : _container(&cont), iter(iter) { /*Empty*/ }

    constexpr InsertIterator& operator=(const typename Container::ValueType& val) {
        iter = _container->insert(iter, val);
        ++iter;
        return *this;
    }

    constexpr InsertIterator& operator=(typename Container::ValueType&& val) {
        iter = _container->insert(iter, custom::move(val));
        ++iter;
        return *this;
    }

    constexpr InsertIterator& operator*() {
        return *this;
    }

    constexpr InsertIterator& operator++() {
        return *this;
    }

    constexpr InsertIterator& operator++(int) {
        return *this;
    }
};

template<class Container>
constexpr InsertIterator<Container> inserter(Container& cont, typename Container::Iterator iter) {
    return InsertIterator<Container>(cont, iter);
}
#pragma endregion InsertIterator

#pragma region ReverseIterator
// helpers for ReverseIterator operator->()
template<class Iterator, class Pointer, bool = IsPointer_v<RemoveCVRef_t<Iterator>>>
constexpr bool _HasNothrowOperatorArrow_v = IsNothrowConvertible_v<Iterator, Pointer>;

template<class Iterator, class Pointer>
constexpr bool _HasNothrowOperatorArrow_v<Iterator, Pointer, false> =
noexcept(_fake_copy_init<Pointer>(custom::declval<Iterator>().operator->()));

template<class Iterator>
class ReverseIterator                           // Adaptor for backwards iteration
{
public:
    static_assert(IsBidirectionalIterator_v<Iterator>,
                    "ReverseIterator require Bidirectional Iterator type!");

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
                _HasNothrowOperatorArrow_v<IteratorType&, Pointer>) {

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

template<class Iterator>
constexpr ReverseIterator<Iterator> make_reverse_iterator(Iterator base)
noexcept(IsNothrowCopyConstructible_v<Iterator>) {
    return ReverseIterator<Iterator>(base);
}
#pragma endregion ReverseIterator

#pragma region Iterator Functions
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
#pragma endregion Iterator Functions

CUSTOM_END