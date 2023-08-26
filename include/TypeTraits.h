#pragma once
#include "xCommon.h"


CUSTOM_BEGIN

// false value attached to a dependent name (for static_assert)
template<class>
constexpr bool AlwaysFalse = false;

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

// alignment of
template<class Ty>
struct AlignmentOf : IntegralConstant<size_t, alignof(Ty)> {}; // determine alignment of Ty

template<class Ty>
constexpr size_t AlignmentOf_v = AlignmentOf<Ty>::Value;

// align storage helpers
using MaxAlign_t = double;

template<class Ty, size_t Len>
union _AlignType                // union with size Len bytes and alignment of Ty
{
    Ty Val;
    char Pad[Len];
};

template<size_t Len, size_t Align, class Ty, bool Ok>
struct _Aligned;                // define Type with size Len and alignment Ty

template<size_t Len, size_t Align, class Ty>
struct _Aligned<Len, Align, Ty, true>           // class
{
    using Type = _AlignType<Ty, Len>;
};

template<size_t Len, size_t Align>
struct _Aligned<Len, Align, double, false>      // double
{
    using Type = _AlignType<MaxAlign_t, Len>;
};

template<size_t Len, size_t Align>
struct _Aligned<Len, Align, int, false>         // int
{
    using Next = double;
    static constexpr bool Fits = Align <= alignof(Next);
    using Type = typename _Aligned<Len, Align, Next, Fits>::Type;
};

template<size_t Len, size_t Align>
struct _Aligned<Len, Align, short, false>       // short
{
    using Next = int;
    static constexpr bool Fits = Align <= alignof(Next);
    using Type = typename _Aligned<Len, Align, Next, Fits>::Type;
};

template<size_t Len, size_t Align>
struct _Aligned<Len, Align, char, false>        // char
{
    using Next = short;
    static constexpr bool Fits = Align <= alignof(Next);
    using Type = typename _Aligned<Len, Align, Next, Fits>::Type;
};

// aligned storage
template<size_t Len, size_t Align = alignof(MaxAlign_t)>
struct AlignedStorage                                     // define Type with size Len and alignment Align
{
    using Next = char;
    static constexpr bool Fits = Align <= alignof(Next);
    using Type = typename _Aligned<Len, Align, Next, Fits>::Type;
};

template<size_t Len, size_t Align = alignof(MaxAlign_t)>
using AlignedStorage_t = typename AlignedStorage<Len, Align>::Type;

// is void
template<class Ty>
constexpr bool IsVoid_v = IsSame_v<RemoveCV_t<Ty>, void>;

template<class Ty>
struct IsVoid : BoolConstant<IsVoid_v<Ty>> {};

template<class... Types>
using Void_t = void;

// add const
template<class Ty>
struct AddConst { using Type = const Ty; };

template<class Ty>
using AddConst_t = typename AddConst<Ty>::Type;

// add volatile
template<class Ty>
struct AddVolatile { using Type = volatile Ty; };

template<class Ty>
using AddVolatile_t = typename AddVolatile<Ty>::Type;

// add const volatile
template<class Ty>
struct AddCV { using Type = const volatile Ty; };

template<class Ty>
using AddCV_t = typename AddCV<Ty>::Type;

// add reference
template<class Ty, class = void>
struct _AddReference                    // add reference (non-referenceable Type)
{
    using Lvalue = Ty;
    using Rvalue = Ty;
};

template<class Ty>
struct _AddReference<Ty, Void_t<Ty&>>   // (referenceable Type)
{
    using Lvalue = Ty&;
    using Rvalue = Ty&&;
};

template<class Ty>
struct AddLvalueReference { using Type = typename _AddReference<Ty>::Lvalue; };

template<class Ty>
using AddLvalueReference_t = typename AddLvalueReference<Ty>::Type;

template<class Ty>
struct AddRvalueReference { using Type = typename _AddReference<Ty>::Rvalue; };

template<class Ty>
using AddRvalueReference_t = typename AddRvalueReference<Ty>::Type;

// declval (ex: used in decltype)
template<class Ty>
AddRvalueReference_t<Ty> declval() noexcept {
    static_assert(AlwaysFalse<Ty>, "declval not allowed in an evaluated context");
}

// add pointer
template<class Ty, class = void>                // add pointer (pointer Type cannot be formed)
struct AddPointer { using Type = Ty; };

template<class Ty>                              // (pointer Type can be formed)
struct AddPointer<Ty, Void_t<RemoveReference_t<Ty>*>> { using Type = RemoveReference_t<Ty>*; };

template<class Ty>
using AddPointer_t = typename AddPointer<Ty>::Type;

// extent
template<class Ty, unsigned int Ix = 0>
constexpr size_t Extent_v = 0;                  // determine extent of dimension Ix of array Ty

template<class Ty, size_t Nx>
constexpr size_t Extent_v<Ty[Nx], 0> = Nx;

template<class Ty, unsigned int Ix, size_t Nx>
constexpr size_t Extent_v<Ty[Nx], Ix> = Extent_v<Ty, Ix - 1>;

template<class Ty, unsigned int Ix>
constexpr size_t Extent_v<Ty[], Ix> = Extent_v<Ty, Ix - 1>;

template<class Ty, unsigned int Ix = 0>
struct Extent : IntegralConstant<size_t, Extent_v<Ty, Ix>> {};

// remove extent
template<class Ty>                              // remove array extent
struct RemoveExtent { using Type = Ty; };

template<class Ty, size_t Size>
struct RemoveExtent<Ty[Size]> { using Type = Ty; };

template<class Ty>
struct RemoveExtent<Ty[]> { using Type = Ty; };

template<class Ty>
using RemoveExtent_t = typename RemoveExtent<Ty>::Type;

template<class Ty>
struct RemoveAllExtents { using Type = Ty; };

template<class Ty, size_t Ix>
struct RemoveAllExtents<Ty[Ix]> { using Type = typename RemoveAllExtents<Ty>::Type; };

template<class Ty>
struct RemoveAllExtents<Ty[]> { using Type = typename RemoveAllExtents<Ty>::Type; };

template<class Ty>
using RemoveAllExtents_t = typename RemoveAllExtents<Ty>::Type;

// is array
template<class>                                 // determine whether Type argument is an array
constexpr bool IsArray_v = false;

template<class Ty, size_t Size>
constexpr bool IsArray_v<Ty[Size]> = true;

template<class Ty>
constexpr bool IsArray_v<Ty[]> = true;

template<class Ty>
struct IsArray : BoolConstant<IsArray_v<Ty>> {};

template<class>
constexpr bool IsBoundedArray_v = false;

template<class Ty, size_t Nx>
constexpr bool IsBoundedArray_v<Ty[Nx]> = true;

template<class Ty>
struct IsBoundedArray : BoolConstant<IsBoundedArray_v<Ty>> {};

template<class>
constexpr bool IsUnboundedArray_v = false;

template<class Ty>
constexpr bool IsUnboundedArray_v<Ty[]> = true;

template<class Ty>
struct IsUnboundedArray : BoolConstant<IsUnboundedArray_v<Ty>> {};

// is lvalue reference
template<class>                                 // determine whether Type argument is an lvalue reference
constexpr bool IsLvalueReference_v = false;

template<class Ty>
constexpr bool IsLvalueReference_v<Ty&> = true;

template<class Ty>
struct IsLvalueReference : BoolConstant<IsLvalueReference_v<Ty>> {};

// is rvalue reference
template<class>                                 // determine whether Type argument is an rvalue reference
constexpr bool IsRvalueReference_v = false;

template<class Ty>
constexpr bool IsRvalueReference_v<Ty&&> = true;

template<class Ty>
struct IsRvalueReference : BoolConstant<IsRvalueReference_v<Ty>> {};

// is reference
template<class>                                 // determine whether Type argument is a reference
constexpr bool IsReference_v = false;

template<class Ty>
constexpr bool IsReference_v<Ty&> = true;

template<class Ty>
constexpr bool IsReference_v<Ty&&> = true;

template<class Ty>
struct IsReference : BoolConstant<IsReference_v<Ty>> {};

// is pointer
template<class>                                 // determine whether Ty is a pointer
constexpr bool IsPointer_v = false;

template<class Ty>
constexpr bool IsPointer_v<Ty*> = true;

template<class Ty>
constexpr bool IsPointer_v<Ty* const> = true;

template<class Ty>
constexpr bool IsPointer_v<Ty* volatile> = true;

template<class Ty>
constexpr bool IsPointer_v<Ty* const volatile> = true;

template<class Ty>
struct IsPointer : BoolConstant<IsPointer_v<Ty>> {};

// is enum
template<class Ty>
struct IsEnum : BoolConstant<__is_enum(Ty)> {};

template<class Ty>
constexpr bool IsEnum_v = IsEnum<Ty>::Value;

// is union
template<class Ty>
struct IsUnion : BoolConstant<__is_union(Ty)> {};

template<class Ty>
constexpr bool IsUnion_v = IsUnion<Ty>::Value;

// is class
template<class Ty>
struct IsClass: BoolConstant<__is_class(Ty)> {};

template<class Ty>
constexpr bool IsClass_v = IsClass<Ty>::Value;

// is convertible
#if defined _MSC_VER
template<class From, class To>
constexpr bool IsConvertible_v = __is_convertible_to(From, To);

template<class From, class To>
struct IsConvertible : BoolConstant<IsConvertible_v<From, To>> {};
#elif defined __GNUG__
template<class From, class To>
struct _IsConvertible           // TODO: check
{
    template<class Ty>
    static auto test_returnable(int) -> decltype(void(static_cast<Ty(*)()>(nullptr)), TrueType{});

    template<class>
    static auto test_returnable(...) -> FalseType;

    template<class _From, class _To>
    static auto test_implicitly_convertible(int) -> decltype(void(custom::declval<void(&)(_To)>()(custom::declval<_From>())), TrueType{});

    template<class, class>
    static auto test_implicitly_convertible(...) -> FalseType;

    static constexpr bool Value = 
    ((decltype(test_returnable<To>(0))::Value && decltype(test_implicitly_convertible<From, To>(0))::Value) ||
    (IsVoid_v<From> && IsVoid_v<To>));
};

template<class From, class To>
struct IsConvertible : public BoolConstant<_IsConvertible<From, To>::Value> {};

template<class From, class To>
constexpr bool IsConvertible_v = IsConvertible<From, To>::Value;
#endif  // is convertible

// is const
template<class>                                 // determine whether Type argument is const qualified
constexpr bool IsConst_v = false;

template<class Ty>
constexpr bool IsConst_v<const Ty> = true;

template<class Ty>
struct IsConst : BoolConstant<IsConst_v<Ty>> {};

// is volatile
template<class>
constexpr bool IsVolatile_v = false;            // determine whether Type argument is volatile qualified

template<class Ty>
constexpr bool IsVolatile_v<volatile Ty> = true;

template<class Ty>
struct IsVolatile : BoolConstant<IsVolatile_v<Ty>> {};

// is function
template<class Ty>                              // only function types and reference types can't be const qualified
constexpr bool IsFunction_v = !IsConst_v<const Ty> && !IsReference_v<Ty>;

template<class Ty>
struct IsFunction : BoolConstant<IsFunction_v<Ty>> {};

// is object
template<class Ty>                            // only function types and reference types can't be const qualified
constexpr bool IsObject_v = IsConst_v<const Ty> && !IsVoid_v<Ty>;

template<class Ty>
struct IsObject : BoolConstant<IsObject_v<Ty>> {};

// is member object pointer
template<class>
struct _IsMemberObjectPointer
{
    static constexpr bool Value = false;
};

template<class Ty1, class Ty2>
struct _IsMemberObjectPointer<Ty1 Ty2::*>
{
    static constexpr bool Value = !IsFunction_v<Ty1>;
    using ClassType = Ty2;
};

template<class Ty>
constexpr bool IsMemberObjectPointer_v = _IsMemberObjectPointer<RemoveCV_t<Ty>>::Value;

template<class Ty>
struct IsMemberObjectPointer : BoolConstant<IsMemberObjectPointer_v<Ty>> {};

// is member function pointer
template<class>
struct _IsMemberFunctionPointer
{
    static constexpr bool Value = false;
};

template<class Ty1, class Ty2>
struct _IsMemberFunctionPointer<Ty1 Ty2::*>
{
    static constexpr bool Value = IsFunction_v<Ty1>;
    using ClassType = Ty2;
};

template<class Ty>
constexpr bool IsMemberFunctionPointer_v = _IsMemberFunctionPointer<RemoveCV_t<Ty>>::Value;

template<class Ty>
struct IsMemberFunctionPointer : BoolConstant<IsMemberFunctionPointer_v<Ty>> {};

// is member pointer
template<class Ty>
constexpr bool IsMemberPointer_v = IsMemberObjectPointer_v<Ty> || IsMemberFunctionPointer_v<Ty>;

template<class Ty>
struct IsMemberPointer : BoolConstant<IsMemberPointer_v<Ty>> {};

// is null pointer
template<class Ty>
constexpr bool IsNullPointer_v = IsSame_v<RemoveCV_t<Ty>, std::nullptr_t>;

template<class Ty>
struct IsNullPointer : BoolConstant<IsNullPointer_v<Ty>> {};

// is scalar
template<class Ty>
constexpr bool IsScalar_v = IsArithmetic_v<Ty> || IsEnum_v<Ty> || IsPointer_v<Ty> || IsMemberPointer_v<Ty> || IsNullPointer_v<Ty>;

template<class Ty>
struct IsScalar : BoolConstant<IsScalar_v<Ty>> {};

// is empty
template<class Ty>
struct IsEmpty : BoolConstant<__is_empty(Ty)> {};

template<class Ty>
constexpr bool IsEmpty_v = IsEmpty<Ty>::Value;

// is polymorphic
template<class Ty>
struct IsPolymorphic : BoolConstant<__is_polymorphic(Ty)> {};

template<class Ty>
constexpr bool IsPolymorphic_v = IsPolymorphic<Ty>::Value;

// is abstract
template<class Ty>
struct IsAbstract : BoolConstant<__is_abstract(Ty)> {};

template<class Ty>
constexpr bool IsAbstract_v = IsAbstract<Ty>::Value;

// is final
template<class Ty>
struct IsFinal : BoolConstant<__is_final(Ty)> {};

template<class Ty>
constexpr bool IsFinal_v = IsFinal<Ty>::Value;

// is constructible
template<class Ty, class... Args>
struct IsConstructible : BoolConstant<__is_constructible(Ty, Args...)> {};

template<class Ty, class... Args>
constexpr bool IsConstructible_v = IsConstructible<Ty, Args...>::Value;

template<class Ty>
struct IsDefaultConstructible : BoolConstant<__is_constructible(Ty)> {};

template<class Ty>
constexpr bool IsDefaultConstructible_v = IsDefaultConstructible<Ty>::Value;

template<class Ty>
struct IsCopyConstructible : BoolConstant<__is_constructible(Ty, AddLvalueReference_t<const Ty>)> {};

template<class Ty>
constexpr bool IsCopyConstructible_v = IsCopyConstructible<Ty>::Value;

template<class Ty>
struct IsMoveConstructible : BoolConstant<__is_constructible(Ty, Ty)> {};

template<class Ty>
constexpr bool IsMoveConstructible_v = IsMoveConstructible<Ty>::Value;

// is nothrow constructible
template<class Ty, class... Args>
struct IsNothrowConstructible : BoolConstant<__is_nothrow_constructible(Ty, Args...)> {};

template<class Ty, class... Args>
constexpr bool IsNothrowConstructible_v = IsNothrowConstructible<Ty, Args...>::Value;

template<class Ty>
struct IsNothrowDefaultConstructible : BoolConstant<__is_nothrow_constructible(Ty)> {};

template<class Ty>
constexpr bool IsNothrowDefaultConstructible_v = IsNothrowDefaultConstructible<Ty>::Value;

template<class Ty>
struct IsNothrowCopyConstructible : BoolConstant<__is_nothrow_constructible(Ty, AddLvalueReference_t<const Ty>)> {};

template<class Ty>
constexpr bool IsNothrowCopyConstructible_v = IsNothrowCopyConstructible<Ty>::Value;

template<class Ty>
struct IsNothrowMoveConstructible : BoolConstant<__is_nothrow_constructible(Ty, Ty)> {};

template<class Ty>
constexpr bool IsNothrowMoveConstructible_v = IsNothrowMoveConstructible<Ty>::Value;

// is assignable
template<class To, class From>
struct IsAssignable : BoolConstant<__is_assignable(To, From)> {};

template<class To, class From>
constexpr bool IsAssignable_v = IsAssignable<To, From>::Value;

template<class Ty>
struct IsCopyAssignable : BoolConstant<__is_assignable(AddLvalueReference_t<Ty>, AddLvalueReference_t<const Ty>)> {};

template<class Ty>
constexpr bool IsCopyAssignable_v = IsCopyAssignable<Ty>::Value;

template<class Ty>
struct IsMoveAssignable : BoolConstant<__is_assignable(AddLvalueReference_t<Ty>, Ty)> {};

template<class Ty>
constexpr bool IsMoveAssignable_v = IsMoveAssignable<Ty>::Value;

// is nothrow assignable
template<class To, class From>
struct IsNothrowAssignable : BoolConstant<__is_nothrow_assignable(To, From)> {};

template<class To, class From>
constexpr bool IsNothrowAssignable_v = IsNothrowAssignable<To, From>::Value;

template<class Ty>
struct IsNothrowCopyAssignable : BoolConstant<__is_nothrow_assignable(AddLvalueReference_t<Ty>, AddLvalueReference_t<const Ty>)> {};

template<class Ty>
constexpr bool IsNothrowCopyAssignable_v = IsNothrowCopyAssignable<Ty>::Value;

template<class Ty>
struct IsNothrowMoveAssignable : BoolConstant<__is_nothrow_assignable(AddLvalueReference_t<Ty>, Ty)> {};

template<class Ty>
constexpr bool IsNothrowMoveAssignable_v = IsNothrowMoveAssignable<Ty>::Value;

// is destructible
#if defined _MSC_VER
template<class Ty>
struct IsDestructible : BoolConstant<__is_destructible(Ty)> {};
#elif defined __GNUG__
template<class Ty>
struct _IsDestructibleTest
{
    template<class _Ty, class = decltype(custom::declval<_Ty&>().~_Ty())>
    static TrueType test_destructible(int);

    template<class>
    static FalseType test_destructible(...);

    using Type = decltype(test_destructible<Ty>(0));
};

template<class Ty,
bool = (IsVoid_v<Ty> || IsUnboundedArray_v<Ty> || IsFunction_v<Ty>),
bool = (IsReference_v<Ty> || IsScalar_v<Ty>)>
struct _IsDestructible;

template<class Ty>
struct _IsDestructible<Ty, false, false> : _IsDestructibleTest<RemoveAllExtents_t<Ty>>::Type {};

template<class Ty>
struct _IsDestructible<Ty, true, false> : FalseType {};

template<class Ty>
struct _IsDestructible<Ty, false, true> : TrueType {};

template<class Ty>
struct IsDestructible : _IsDestructible<Ty>::Type {};
#endif  // is destructible

template<class Ty>
constexpr bool IsDestructible_v = IsDestructible<Ty>::Value;

// is nothrow destructible
#if defined _MSC_VER
template<class Ty>
struct IsNothrowDestructible : BoolConstant<__is_nothrow_destructible(Ty)> {};
#elif defined __GNUG__
template<class Ty>
struct _IsNothrowDestructibleTest
{
    template<typename _Ty>
    static BoolConstant<noexcept(custom::declval<_Ty&>().~_Ty())> test_nothrow_destructible(int);

    template<class>
    static FalseType test_nothrow_destructible(...);

    using Type = decltype(test_nothrow_destructible<Ty>(0));
};

template<class Ty,
bool = (IsVoid_v<Ty> || IsUnboundedArray_v<Ty> || IsFunction_v<Ty>),
bool = (IsReference_v<Ty> || IsScalar_v<Ty>)>
struct _IsNothrowDestructible;

template<class Ty>
struct _IsNothrowDestructible<Ty, false, false> : _IsNothrowDestructibleTest<RemoveAllExtents_t<Ty>>::Type {};

template<class Ty>
struct _IsNothrowDestructible<Ty, true, false> : FalseType {};

template<class Ty>
struct _IsNothrowDestructible<Ty, false, true> : TrueType {};

template<class Ty>
struct IsNothrowDestructible : _IsNothrowDestructible<Ty>::Type {};
#endif  // is nothrow destructible

template<class Ty>
constexpr bool IsNothrowDestructible_v = IsNothrowDestructible<Ty>::Value;

// is base of
template<class Base, class Derived>
struct IsBaseOf : BoolConstant<__is_base_of(Base, Derived)> {};

template<class Base, class Derived>
constexpr bool IsBaseOf_v = IsBaseOf<Base, Derived>::Value;

// is specialization
template<class Type, template<class...> class Template>
constexpr bool IsSpecialization_v = false; // true if and only if Type is a specialization of Template

template<template<class...> class Template, class... Types>
constexpr bool IsSpecialization_v<Template<Types...>, Template> = true;

template<class Type, template<class...> class Template>
struct IsSpecialization : BoolConstant<IsSpecialization_v<Type, Template>> {};

// is signed/unsigned
template<class Ty, bool = IsIntegral_v<Ty>>
struct _SignChoice
{
    using NoCV_t = RemoveCV_t<Ty>;

    static constexpr bool Signed    = static_cast<NoCV_t>(-1) < static_cast<NoCV_t>(0);
    static constexpr bool Unsigned  = !Signed;
};

template<class Ty>
struct _SignChoice<Ty, false>
{
    // floating-point Ty is signed
    // non-arithmetic Ty is neither signed nor unsigned
    static constexpr bool Signed    = IsFloatingPoint_v<Ty>;
    static constexpr bool Unsigned  = false;
};

template<class Ty>
struct IsSigned : BoolConstant<_SignChoice<Ty>::Signed> {};

template<class Ty>
constexpr bool IsSigned_v = IsSigned<Ty>::Value;

template<class Ty>
struct IsUnsigned : BoolConstant<_SignChoice<Ty>::Unsigned> {};

template<class Ty>
constexpr bool IsUnsigned_v = IsUnsigned<Ty>::Value;

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

// decay - Performs the type conversions equivalent to the ones performed when passing function arguments by value
template<class Ty>
struct Decay
{
    // old
    // using Ty1   = RemoveReference_t<Ty>;
    // using Ty2   = typename Select<IsFunction_v<Ty1>>::template Apply<AddPointer<Ty1>, RemoveCV<Ty1>>;
    // using Type  = typename Select<IsArray_v<Ty1>>::template Apply<AddPointer<RemoveExtent_t<Ty1>>, Ty2>::Type;

private:
    using NoRefTy = RemoveReference_t<Ty>;

public:
    using Type = Conditional_t<IsArray_v<NoRefTy>,
                                    AddPointer_t<RemoveExtent_t<NoRefTy>>,
                                    Conditional_t<IsFunction_v<NoRefTy>,
                                                        AddPointer_t<NoRefTy>,
                                                        RemoveCV_t<NoRefTy>>>;
};

template<class Ty>
using Decay_t = typename Decay<Ty>::Type;

CUSTOM_END