#pragma once
#include <cstddef>      // size_t, nullptr_t
#include <stdexcept>    // exceptions
#include <climits>
#include <iostream>     // debugging


#define CUSTOM_BEGIN namespace custom { // gloabal namespace for this project
#define CUSTOM_END }


CUSTOM_BEGIN

#define CUSTOM_DETAIL_BEGIN namespace detail {  // used to hide detailed implementations
#define CUSTOM_DETAIL_END }

#define CUSTOM_ASSERT(Expr, Msg) __Assert(Expr, Msg, #Expr, __FILE__, __LINE__)

#define CUSTOM_OPTIMAL_IMPLEMENTATION    // some implementations are easier to understand, but have lower performance

#ifdef _MSC_VER
// This is a Microsoft Specific. This is a __declspec extended attribute.
// This form of __declspec can be applied to any class declaration,
// but should only be applied to pure interface classes, that is,
// classes that will never be instantiated on their own.
// The __declspec stops the compiler from generating code to initialize the vfptr in the constructor(s)
// and destructor of the class. In many cases, this removes the only references to the vtable
// that are associated with the class and, thus, the linker will remove it.
// Using this form of __declspec can result in a significant reduction in code size.
#define CUSTOM_NOVTABLE_ATTR __declspec(novtable)
#else
#define CUSTOM_NOVTABLE_ATTR    // Defined only for _MSC_VER
#endif


// custom assert function
inline void __Assert(bool expr, const char* msg, const char* exprStr, const char* file, int line) {
    if (!expr)
    {
        std::cerr   << "Assert failed:\t"   << msg << "\n"
                    << "Expected:\t"        << exprStr << "\n"
                    << "Source:\t\t"        << file << ", line " << line << "\n";
        ::abort();
    }
}

// is constant evaluated
constexpr inline bool is_constant_evaluated() noexcept {
    return __builtin_is_constant_evaluated();
}

// select
template<bool>
struct Select                                   // Select between aliases that extract either their first or second parameter
{
    template<class Ty1, class>
    using Apply = Ty1;
};

template<>
struct Select<false>
{
    template<class, class Ty2>
    using Apply = Ty2;
};

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
template<bool Val>
using BoolConstant = IntegralConstant<bool, Val>;

// true / false type
using TrueType  = BoolConstant<true>;
using FalseType = BoolConstant<false>;

// identity
template<class Ty>
struct TypeIdentity { using Type = Ty; };

template<class Ty>
using TypeIdentity_t = typename TypeIdentity<Ty>::Type;

// enable if
template<bool Test, class Ty = void>
struct EnableIf {};                             // no member "Type" when !Test

template<class Ty>                              // Type is Ty for Test
struct EnableIf<true, Ty> { using Type = Ty; };

template<bool Test, class Ty = void>
using EnableIf_t = typename EnableIf<Test, Ty>::Type;

// conditional
template<bool Test, class Ty1, class Ty2>       // Choose Ty1 if Test is true, and Ty2 otherwise
struct Conditional { using Type = Ty1; };

template<class Ty1, class Ty2>
struct Conditional<false, Ty1, Ty2> { using Type = Ty2; };

template<bool Test, class Ty1, class Ty2>
using Conditional_t = typename Conditional<Test, Ty1, Ty2>::Type;

// negation
template<class Trait>
struct Negation : BoolConstant<!static_cast<bool>(Trait::Value)> {}; // The negated result of Trait

template<class Trait>
constexpr bool Negation_v = Negation<Trait>::Value;

// conjunction
template<bool FirstValue, class First, class... Rest>
struct _Conjunction { using Type = First; };        // handle false trait or last trait

template<class True, class Next, class... Rest>     // the first trait is true, try the next one
struct _Conjunction<true, True, Next, Rest...> { using Type = typename _Conjunction<Next::Value, Next, Rest...>::Type; };

template<class... Traits>
struct Conjunction : TrueType {};                   // If Traits is empty, true type

template<class First, class... Rest>                // the first false trait in Traits, or the last trait if none are false
struct Conjunction<First, Rest...> : _Conjunction<First::Value, First, Rest...>::Type {};

template<class... Traits>
constexpr bool Conjunction_v = Conjunction<Traits...>::Value;

// disjunction
template<bool FirstValue, class First, class... Rest>
struct _Disjunction { using Type = First; };        // handle true trait or last trait

template<class False, class Next, class... Rest>    // first trait is false, try the next trait
struct _Disjunction<false, False, Next, Rest...> { using Type = typename _Disjunction<Next::Value, Next, Rest...>::Type; };

template<class... Traits>
struct Disjunction : FalseType {};                  // If Traits is empty, false_type

template<class First, class... Rest>
struct Disjunction<First, Rest...> : _Disjunction<First::Value, First, Rest...>::Type {};

template<class... Traits>
constexpr bool Disjunction_v = Disjunction<Traits...>::Value;

// remove const
template<class Ty>                                  // remove top-level const qualifier
struct RemoveConst { using Type = Ty; };

template<class Ty>
struct RemoveConst<const Ty> { using Type = Ty; };

template<class Ty>
using RemoveConst_t = typename RemoveConst<Ty>::Type;

// remove volatile
template<class Ty>                                  // remove top-level volatile qualifier
struct RemoveVolatile { using Type = Ty; };

template<class Ty>
struct RemoveVolatile<volatile Ty> { using Type = Ty; };

template<class Ty>
using RemoveVolatile_t = typename RemoveVolatile<Ty>::Type;

// remove const volatile
template<class Ty>
struct RemoveCV                                     // remove top-level const and volatile qualifiers
{
    using Type = Ty;

    template<template<class> class Fn>
    using Apply = Fn<Ty>;                           // apply cv-qualifiers from the class template argument to Fn<Ty>
                                                    // used to reapply cv-qualifiers after removing them for another purpose
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

template<class Ty>
using RemoveCV_t = typename RemoveCV<Ty>::Type;

// remove const volatile and ref
template<class Ty>
struct RemoveCVRef : RemoveCV<Ty> {};

template<class Ty>
struct RemoveCVRef<Ty&> : RemoveCV<Ty> {};

template<class Ty>
struct RemoveCVRef<Ty&&> : RemoveCV<Ty> {};

template<class Ty>
using RemoveCVRef_t = typename RemoveCVRef<Ty>::Type;

// remove reference
template<class Ty>
struct RemoveReference { using Type = Ty; };

template<class Ty>
struct RemoveReference<Ty&> { using Type = Ty; };

template<class Ty>
struct RemoveReference<Ty&&> { using Type = Ty; };

template<class Ty>
using RemoveReference_t = typename RemoveReference<Ty>::Type;

// remove pointer
template<class Ty>
struct RemovePointer { using Type = Ty; };

template<class Ty>
struct RemovePointer<Ty*> { using Type = Ty; };

template<class Ty>
struct RemovePointer<Ty* const> { using Type = Ty; };

template<class Ty>
struct RemovePointer<Ty* volatile> { using Type = Ty; };

template<class Ty>
struct RemovePointer<Ty* const volatile> { using Type = Ty; };

template<class Ty>
using RemovePointer_t = typename RemovePointer<Ty>::Type;

// is same
template<class, class>                          // determine whether arguments are the same type
constexpr bool IsSame_v = false;

template<class Ty>
constexpr bool IsSame_v<Ty, Ty> = true;

template<class Ty1, class Ty2>
struct IsSame : BoolConstant<IsSame_v<Ty1, Ty2>> {};

// is any of
template<class Ty, class... Types>              // true if Ty is in Types
constexpr bool IsAnyOf_v = Disjunction_v<IsSame<Ty, Types>...>;

template<class Ty>
struct IsAnyOf : BoolConstant<IsAnyOf_v<Ty>> {};

// is integral
template<class Ty>
constexpr bool IsIntegral_v = IsAnyOf_v<RemoveCV_t<Ty>, bool, char, signed char, unsigned char,
wchar_t,
#ifdef __cpp_char8_t
char8_t,
#endif
char16_t, char32_t, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long>;

template<class Ty>
struct IsIntegral : BoolConstant<IsIntegral_v<Ty>> {};

// is unsigned integer
template<class Ty>
constexpr bool IsUnsignedInteger_v = IsAnyOf_v<RemoveCV_t<Ty>, unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>;

template<class Ty>
struct IsUnsignedInteger : BoolConstant<IsUnsignedInteger_v<Ty>> {};

// is char
template<class Ty>
constexpr bool IsChar_v = IsAnyOf_v<Ty, char, wchar_t,
#ifdef __cpp_char8_t
char8_t,
#endif
char16_t, char32_t>;

template<class Ty>
struct IsChar : BoolConstant<IsChar_v<Ty>> {};

// is floating point
template<class Ty>
constexpr bool IsFloatingPoint_v = IsAnyOf_v<RemoveCV_t<Ty>, float, double, long double>;

template<class Ty>
struct IsFloatingPoint : BoolConstant<IsFloatingPoint_v<Ty>> {};

// is arithmetic
template<class Ty>
constexpr bool IsArithmetic_v = IsIntegral_v<Ty> || IsFloatingPoint_v<Ty>;

template<class Ty>
struct IsArithmetic : BoolConstant<IsArithmetic_v<Ty>> {};

CUSTOM_END