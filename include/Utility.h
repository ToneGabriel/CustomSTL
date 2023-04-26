#pragma once
#include "TypeTraits.h"


CUSTOM_BEGIN

// move
template<class Ty>
constexpr RemoveReference_t<Ty>&& move(Ty&& val) noexcept { 
    return static_cast<RemoveReference_t<Ty>&&>(val);
}

// forward
template<class Ty>
constexpr Ty&& forward(RemoveReference_t<Ty>& val) noexcept {
    return static_cast<Ty&&>(val);
}

template<class Ty>
constexpr Ty&& forward(RemoveReference_t<Ty>&& val) noexcept {
    static_assert(!IsLvalueReference_v<Ty>, "bad forward call");
    return static_cast<Ty&&>(val);
}

template<class Iterator>
class ReverseIterator                           // Adaptor for backwards iteration
{
public:
    using ValueType     = typename Iterator::ValueType;
    using Reference		= typename Iterator::Reference;
	using Pointer		= typename Iterator::Pointer;

    Iterator _Base;

public:

    explicit ReverseIterator(const Iterator& base)
        : _Base(base) { /*Empty*/ }

    ReverseIterator& operator++() {
        --_Base;
        return *this;
    }

    ReverseIterator operator++(int) {
        ReverseIterator temp = *this;
        --_Base;
        return temp;
    }

    ReverseIterator& operator--() {
        ++_Base;
        return *this;
    }

    ReverseIterator operator--(int) {
        ReverseIterator temp = *this;
        ++_Base;
        return temp;
    }

    Pointer operator->() {
        Iterator temp = _Base;
        return (--temp).operator->();
    }

    Reference operator*() {
        Iterator temp = _Base;
        return *(--temp);
    }

    bool operator==(const ReverseIterator& other) const {
        return _Base == other._Base;
    }

    bool operator!=(const ReverseIterator& other) const {
	    return !(*this == other);
    }
}; // END ReverseIterator

CUSTOM_END