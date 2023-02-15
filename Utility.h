#pragma once
#include "Common.h"


CUSTOM_BEGIN

// remove reference
template<class T>
struct RemoveReference { using Type = T; };

template<class T>
struct RemoveReference<T&> { using Type = T; };

template<class T>
struct RemoveReference<T&&> { using Type = T; };



// Class template IntegerSequence
template<class T, T... Idx>
struct IntegerSequence
{
    using ValueType = T;
    static constexpr size_t size() noexcept { return sizeof...(Idx); }
};

// Aliases
template<size_t... Idx>
using IndexSequence = IntegerSequence<size_t, Idx...>;

template<class T, T Num>
using MakeIntegerSequence = IntegerSequence<T, __integer_pack(Num)...>;

template<size_t Num>
using MakeIndexSequence = MakeIntegerSequence<size_t, Num>;

template<class... Types>
using IndexSequenceFor = MakeIndexSequence<sizeof...(Types)>;



// move
template<class T>
constexpr typename RemoveReference<T>::Type&& move(T&& obj) noexcept { 
    return static_cast<typename RemoveReference<T>::Type&&>(obj);
}

// forward
template<class T>
constexpr T&& forward(typename RemoveReference<T>::Type& obj) noexcept { 
    return static_cast<T&&>(obj);
}

template<class T>
constexpr T&& forward(typename RemoveReference<T>::Type&& obj) noexcept {
    // static_assert(!std::is_lvalue_reference<T>::value, "template argument"
    //     " substituting T must not be an lvalue reference Type");
    return static_cast<T&&>(obj);
}

CUSTOM_END