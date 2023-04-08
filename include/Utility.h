#pragma once
#include <cstddef>      // std::size_t
#include <stdexcept>    // exceptions
#include <cassert>      // assert

#define CUSTOM_BEGIN namespace custom {
#define CUSTOM_END }

#define STD_BEGIN namespace std {
#define STD_END }




CUSTOM_BEGIN

// integral constant
template<class Ty, Ty Val>
struct IntegralConstant
{
    using ValueType = Ty;
    using Type      = IntegralConstant;

    static constexpr ValueType Value = Val;

    constexpr operator ValueType() const noexcept {
        return Value;
    }

    constexpr ValueType operator()() const noexcept {
        return Value;
    }
};

// bool constant
template <bool Val>
using BoolConstant = IntegralConstant<bool, Val>;

template <class... Types>
using Void_t = void;

// is same
template<class, class>                          // determine whether arguments are the same type
constexpr bool IsSame_v = false;

template<class Ty>
constexpr bool IsSame_v<Ty, Ty> = true;

template<class Ty1, class Ty2>
struct IsSame : BoolConstant<IsSame_v<Ty1, Ty2>> {};

// is lvalue reference
template<class>                                 // determine whether type argument is an lvalue reference
constexpr bool IsLvalueReference_v = false;

template<class Ty>
constexpr bool IsLvalueReference_v<Ty&> = true;

template<class Ty>
struct IsLvalueReference : BoolConstant<IsLvalueReference_v<Ty>> {};

// is reference
template<class>                                 // determine whether type argument is a reference
constexpr bool IsReference_v = false;

template<class Ty>
constexpr bool IsReference_v<Ty&> = true;

template<class Ty>
constexpr bool IsReference_v<Ty&&> = true;

template<class Ty>
struct IsReference : BoolConstant<IsReference_v<Ty>> {};

// is const
template<class>                                 // determine whether type argument is const qualified
constexpr bool IsConst_v = false;

template<class Ty>
constexpr bool IsConst_v<const Ty> = true;

template<class Ty>
struct IsConst : BoolConstant<IsConst_v<Ty>> {};

// is function
template<class Ty>                              // only function types and reference types can't be const qualified
constexpr bool IsFunction_v = !IsConst_v<const Ty> && !IsReference_v<Ty>;

template<class Ty>
struct IsFunction : BoolConstant<IsFunction_v<Ty>> {};

// is array
template<class>                                 // determine whether type argument is an array
constexpr bool IsArray_v = false;

template<class Ty, size_t Size>
constexpr bool IsArray_v<Ty[Size]> = true;

template<class Ty>
constexpr bool IsArray_v<Ty[]> = true;

template<class Ty>
struct IsArray : BoolConstant<IsArray_v<Ty>> {};

// remove reference
template<class Ty>
struct RemoveReference { using Type = Ty; };

template<class Ty>
struct RemoveReference<Ty&> { using Type = Ty; };

template<class Ty>
struct RemoveReference<Ty&&> { using Type = Ty; };

template <class Ty>
using RemoveReference_t = typename RemoveReference<Ty>::Type;

// add pointer
template<class Ty, class = void>                // add pointer (pointer type cannot be formed)
struct _AddPointer { using Type = Ty; };

template<class Ty>                              // (pointer type can be formed)
struct _AddPointer<Ty, Void_t<RemoveReference<Ty>*>> { using Type = RemoveReference<Ty>*; };

template<class Ty>
struct AddPointer { using Type = typename _AddPointer<Ty>::Type; };

template<class Ty>
using AddPointer_t = typename _AddPointer<Ty>::Type;

// remove extent
template<class Ty>                              // remove array extent
struct RemoveExtent { using Type = Ty; };

template<class Ty, size_t Size>
struct RemoveExtent<Ty[Size]> { using Type = Ty; };

template<class Ty>
struct RemoveExtent<Ty[]> { using Type = Ty; };

template <class Ty>
using RemoveExtent_t = typename RemoveExtent<Ty>::Type;

// remove const volatile
template<class Ty>
struct RemoveCV                                 // remove top-level const and volatile qualifiers
{
    using Type = Ty;

    template<template<class> class Fn>
    using Apply = Fn<Ty>;                       // apply cv-qualifiers from the class template argument to Fn<Ty>
};

template<class Ty>
struct RemoveCV<const Ty>
{
    using Type = Ty;

    template<template<class> class Fn>
    using Apply = const Fn<Ty>;
};

template<class Ty>
struct RemoveCV<volatile Ty>
{
    using Type = Ty;

    template<template<class> class Fn>
    using Apply = volatile Fn<Ty>;
};

template<class Ty>
struct RemoveCV<const volatile Ty>
{
    using Type = Ty;

    template<template<class> class Fn>
    using Apply = const volatile Fn<Ty>;
};

template <class Ty>
using RemoveCV_t = typename RemoveCV<Ty>::Type;

// Select
template<bool>
struct Select                                   // Select between aliases that extract either their first or second parameter
{
    template <class Ty1, class>
    using Apply = Ty1;
};

template<>
struct Select<false> 
{
    template <class, class Ty2>
    using Apply = Ty2;
};

// Decay
template<class Ty>
struct Decay                                    // determines decayed version of Ty
{
    using Ty1   = RemoveReference_t<Ty>;
    using Ty2   = typename Select<IsFunction_v<Ty1>>::template Apply<AddPointer<Ty1>, RemoveCV<Ty1>>;
    using Type  = typename Select<IsArray_v<Ty1>>::template Apply<AddPointer<RemoveExtent_t<Ty1>>, Ty2>::Type;  // TODO: not ok
};

template <class Ty>
using Decay_t = typename Decay<Ty>::Type;

// Enable If
template<bool Test, class Ty = void>
struct EnableIf {};                             // no member "Type" when !Test

template<class Ty>                              // Type is Ty for Test
struct EnableIf<true, Ty> { using Type = Ty; };

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
    using BaseIterator  = Iterator;
    using ValueType     = typename BaseIterator::ValueType;
    using Reference		= typename BaseIterator::Reference;
	using Pointer		= typename BaseIterator::Pointer;

    BaseIterator _Base;

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
        BaseIterator temp = _Base;
        return (--temp).operator->();
    }

    Reference operator*() {
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