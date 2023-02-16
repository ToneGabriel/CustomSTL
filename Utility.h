#pragma once
#include "Common.h"


CUSTOM_BEGIN

// remove reference
template<class Ty>
struct RemoveReference { using Type = Ty; };

template<class Ty>
struct RemoveReference<Ty&> { using Type = Ty; };

template<class Ty>
struct RemoveReference<Ty&&> { using Type = Ty; };

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

// Aliases
template<size_t... Vals>
using IndexSequence = IntegerSequence<size_t, Vals...>;


template<class Ty, Ty Size>
#if __has_builtin(__make_integer_seq)
using MakeIntegerSequence = __make_integer_seq<IntegerSequence, Ty, Size>;
#else
using MakeIntegerSequence = IntegerSequence<Ty, __integer_pack(Size)...>;
#endif // __make_integer_seq

template<size_t Size>
using MakeIndexSequence = MakeIntegerSequence<size_t, Size>;

template<class... Types>
using IndexSequenceFor = MakeIndexSequence<sizeof...(Types)>;



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

CUSTOM_END