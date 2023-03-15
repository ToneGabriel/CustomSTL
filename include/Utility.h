#pragma once
#include <cstddef>      // std::size_t
#include <stdexcept>    // exceptions
#include <cassert>      // assert

#define CUSTOM_BEGIN namespace custom {
#define CUSTOM_END }

#define STD_BEGIN namespace std {
#define STD_END }




CUSTOM_BEGIN

// is_lvalue_reference
template <class>
constexpr bool is_lvalue_reference = false;   // determine whether type argument is an lvalue reference

template <class Ty>
constexpr bool is_lvalue_reference<Ty&> = true;

// Class template IntegerSequence
template<class Ty, Ty... Vals>
struct IntegerSequence
{
    using ValueType = Ty;
    static constexpr size_t size() noexcept { return sizeof...(Vals); }
};

// Alias helpers for IntegerSequence
template<size_t... Vals>
using IndexSequence = IntegerSequence<size_t, Vals...>;

template<class Ty, Ty Size>
#if defined _MSC_VER
using MakeIntegerSequence = __make_integer_seq<IntegerSequence, Ty, Size>;
#elif defined __GNUG__
using MakeIntegerSequence = IntegerSequence<Ty, __integer_pack(Size)...>;
#else
#error No "MakeIntegerSequence" implementation
#endif  // MakeIntegerSequence

template<size_t Size>
using MakeIndexSequence = MakeIntegerSequence<size_t, Size>;

template<class... Types>
using IndexSequenceFor = MakeIndexSequence<sizeof...(Types)>;
// END Alias helpers for IntegerSequence

// remove reference
template<class Ty>
struct RemoveReference { using Type = Ty; };

template<class Ty>
struct RemoveReference<Ty&> { using Type = Ty; };

template<class Ty>
struct RemoveReference<Ty&&> { using Type = Ty; };

// move
template<class Ty>
constexpr typename RemoveReference<Ty>::Type&& move(Ty&& val) noexcept { 
    return static_cast<typename RemoveReference<Ty>::Type&&>(val);
}

// forward
template<class Ty>
constexpr Ty&& forward(typename RemoveReference<Ty>::Type& val) noexcept { 
    return static_cast<Ty&&>(val);
}

template<class Ty>
constexpr Ty&& forward(typename RemoveReference<Ty>::Type&& val) noexcept {
    static_assert(!is_lvalue_reference<Ty>, "bad forward call");
    return static_cast<Ty&&>(val);
}

template<class Iterator>
class ReverseIterator       // Adaptor for backwards iteration
{
public:
    using BaseIterator  = Iterator;
    using ValueType     = typename BaseIterator::ValueType;
    using IterType      = typename BaseIterator::IterType;

    BaseIterator _Base;

public:

    explicit ReverseIterator(const Iterator& base)
        : _Base(base) { /*Empty*/ }

    ~ReverseIterator() = default;

public:

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

    IterType* operator->() {
        BaseIterator temp = _Base;
        return (--temp).operator->();
    }

    ValueType& operator*() {
        BaseIterator temp = _Base;
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