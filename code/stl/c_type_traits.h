#pragma once
#include "y_core.h"


CUSTOM_BEGIN

#pragma region is_constant_evaluated
constexpr inline bool is_constant_evaluated() noexcept
{
    return __builtin_is_constant_evaluated();
}
#pragma endregion is_constant_evaluated

#pragma region integral_constant
template<class Ty, Ty Val>
struct integral_constant
{
    using value_type    = Ty;
    using type          = integral_constant;

    static constexpr value_type value = Val;

    constexpr operator value_type() const noexcept { return value; }

    constexpr value_type operator()() const noexcept { return value; }
};
#pragma endregion integral_constant

#pragma region bool_constant
template<bool Val>
using bool_constant = integral_constant<bool, Val>;

using true_type  = bool_constant<true>;
using false_type = bool_constant<false>;
#pragma endregion bool_constant

#pragma region type_identity
template<class Ty>
struct type_identity { using type = Ty; };

template<class Ty>
using type_identity_t = typename type_identity<Ty>::type;
#pragma endregion type_identity

#pragma region enable_if
template<bool Test, class Ty = void>
struct enable_if {};                             // no member "type" when !Test

template<class Ty>                              // type is Ty for Test
struct enable_if<true, Ty> { using type = Ty; };

template<bool Test, class Ty = void>
using enable_if_t = typename enable_if<Test, Ty>::type;
#pragma endregion enable_if

#pragma region conditional
template<bool Test, class Ty1, class Ty2>       // Choose Ty1 if Test is true, and Ty2 otherwise
struct conditional { using type = Ty1; };

template<class Ty1, class Ty2>
struct conditional<false, Ty1, Ty2> { using type = Ty2; };

template<bool Test, class Ty1, class Ty2>
using conditional_t = typename conditional<Test, Ty1, Ty2>::type;
#pragma endregion conditional

#pragma region negation
template<class Trait>
struct negation : bool_constant<!static_cast<bool>(Trait::value)> {}; // The negated result of Trait

template<class Trait>
constexpr bool negation_v = negation<Trait>::value;
#pragma endregion negation

#pragma region conjunction
template<class... Traits>
struct conjunction : true_type {};
 
template<class First>
struct conjunction<First> : First {};
 
template<class First, class... Rest>
struct conjunction<First, Rest...>
: conditional_t<static_cast<bool>(First::value), conjunction<Rest...>, First> {};

template<class... Traits>
constexpr bool conjunction_v = conjunction<Traits...>::value;
#pragma endregion conjunction

#pragma region disjunction
template<class... Traits>
struct disjunction : false_type {};
 
template<class First>
struct disjunction<First> : First {};
 
template<class First, class... Rest>
struct disjunction<First, Rest...>
: conditional_t<static_cast<bool>(First::value), First, disjunction<Rest...>> {};

template<class... Traits>
constexpr bool disjunction_v = disjunction<Traits...>::value;
#pragma endregion disjunction

#pragma region remove_const
template<class Ty>                                  // remove top-level const qualifier
struct remove_const { using type = Ty; };

template<class Ty>
struct remove_const<const Ty> { using type = Ty; };

template<class Ty>
using remove_const_t = typename remove_const<Ty>::type;
#pragma endregion remove_const

#pragma region remove_volatile
template<class Ty>                                  // remove top-level volatile qualifier
struct remove_volatile { using type = Ty; };

template<class Ty>
struct remove_volatile<volatile Ty> { using type = Ty; };

template<class Ty>
using remove_volatile_t = typename remove_volatile<Ty>::type;
#pragma endregion remove_volatile

#pragma region remove_cv
template<class Ty>
struct remove_cv                                     // remove top-level const and volatile qualifiers
{
    using type = Ty;

    template<template<class> class Fn>
    using _Apply = Fn<Ty>;                          // apply cv-qualifiers from the class template argument to Fn<Ty>
                                                    // used to reapply cv-qualifiers after removing them for another purpose
};

template<class Ty>
struct remove_cv<const Ty>
{
    using type = Ty;

    template<template<class> class Fn>
    using _Apply = const Fn<Ty>;
};

template<class Ty>
struct remove_cv<volatile Ty>
{
    using type = Ty;

    template<template<class> class Fn>
    using _Apply = volatile Fn<Ty>;
};

template<class Ty>
struct remove_cv<const volatile Ty>
{
    using type = Ty;

    template<template<class> class Fn>
    using _Apply = const volatile Fn<Ty>;
};

template<class Ty>
using remove_cv_t = typename remove_cv<Ty>::type;
#pragma endregion remove_cv

#pragma region remove_cv_ref
template<class Ty>
struct remove_cv_ref : remove_cv<Ty> {};

template<class Ty>
struct remove_cv_ref<Ty&> : remove_cv<Ty> {};

template<class Ty>
struct remove_cv_ref<Ty&&> : remove_cv<Ty> {};

template<class Ty>
using remove_cv_ref_t = typename remove_cv_ref<Ty>::type;
#pragma endregion remove_cv_ref

#pragma region remove_reference
template<class Ty>
struct remove_reference { using type = Ty; };

template<class Ty>
struct remove_reference<Ty&> { using type = Ty; };

template<class Ty>
struct remove_reference<Ty&&> { using type = Ty; };

template<class Ty>
using remove_reference_t = typename remove_reference<Ty>::type;
#pragma endregion remove_reference

#pragma region remove_pointer
template<class Ty>
struct remove_pointer { using type = Ty; };

template<class Ty>
struct remove_pointer<Ty*> { using type = Ty; };

template<class Ty>
struct remove_pointer<Ty* const> { using type = Ty; };

template<class Ty>
struct remove_pointer<Ty* volatile> { using type = Ty; };

template<class Ty>
struct remove_pointer<Ty* const volatile> { using type = Ty; };

template<class Ty>
using remove_pointer_t = typename remove_pointer<Ty>::type;
#pragma endregion remove_pointer

#pragma region is_same
template<class, class>                          // determine whether arguments are the same type
constexpr bool is_same_v = false;

template<class Ty>
constexpr bool is_same_v<Ty, Ty> = true;

template<class Ty1, class Ty2>
struct is_same : bool_constant<is_same_v<Ty1, Ty2>> {};
#pragma endregion is_same

#pragma region is_any_of
template<class Ty, class... Types>              // true if Ty is in Types
constexpr bool is_any_of_v = disjunction_v<is_same<Ty, Types>...>;

template<class Ty>
struct is_any_of : bool_constant<is_any_of_v<Ty>> {};
#pragma endregion is_any_of

#pragma region is_integral
template<class Ty>
constexpr bool is_integral_v = is_any_of_v<remove_cv_t<Ty>, bool, char, signed char, unsigned char,
wchar_t,
#ifdef __cpp_char8_t
char8_t,
#endif
char16_t, char32_t, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long>;

template<class Ty>
struct is_integral : bool_constant<is_integral_v<Ty>> {};
#pragma endregion is_integral

#pragma region is_unsigned_integer
template<class Ty>
constexpr bool is_unsigned_integer_v = is_any_of_v<remove_cv_t<Ty>, unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>;

template<class Ty>
struct is_unsigned_integer : bool_constant<is_unsigned_integer_v<Ty>> {};
#pragma endregion is_unsigned_integer

#pragma region is_char
template<class Ty>
constexpr bool is_char_v = is_any_of_v<Ty, char, wchar_t,
#ifdef __cpp_char8_t
char8_t,
#endif
char16_t, char32_t>;

template<class Ty>
struct is_char : bool_constant<is_char_v<Ty>> {};
#pragma endregion is_char

#pragma region is_floating_point
template<class Ty>
constexpr bool is_floating_point_v = is_any_of_v<remove_cv_t<Ty>, float, double, long double>;

template<class Ty>
struct is_floating_point : bool_constant<is_floating_point_v<Ty>> {};
#pragma endregion is_floating_point

#pragma region is_arithmetic
template<class Ty>
constexpr bool is_arithmetic_v = is_integral_v<Ty> || is_floating_point_v<Ty>;

template<class Ty>
struct is_arithmetic : bool_constant<is_arithmetic_v<Ty>> {};
#pragma endregion is_arithmetic

#pragma region always_false
// false value attached to a dependent name (for static_assert)
template<class>
constexpr bool always_false = false;
#pragma endregion always_false

#pragma region integer_sequence/index_sequence
template<class Ty, Ty... Vals>
struct integer_sequence
{
    using value_type = Ty;
    static constexpr size_t size() noexcept { return sizeof...(Vals); }
};

template<size_t... Vals>
using index_sequence = integer_sequence<size_t, Vals...>;

template<class Ty, Ty Size>
#if defined _MSC_VER
using make_integer_sequence = __make_integer_seq<integer_sequence, Ty, Size>;
#elif defined __GNUG__
using make_integer_sequence = integer_sequence<Ty, __integer_pack(Size)...>;
#else
#error No "make_integer_sequence" implementation
#endif  // make_integer_sequence

template<size_t Size>
using make_index_sequence = make_integer_sequence<size_t, Size>;

template<class... Types>
using index_sequence_for = make_index_sequence<sizeof...(Types)>;
#pragma endregion integer_sequence/index_sequence

#pragma region alignment
// alignment of
template<class Ty>
struct alignment_of : integral_constant<size_t, alignof(Ty)> {}; // determine alignment of Ty

template<class Ty>
constexpr size_t alignment_of_v = alignment_of<Ty>::value;

// align storage helpers
using max_align_t = double;

CUSTOM_DETAIL_BEGIN

// aligned_storage helpers
template<class Ty, size_t Len>
union _Align_Type   // union with size Len bytes and alignment of Ty
{
    Ty Val;
    char Pad[Len];
};

template<size_t Len, size_t Align, class Ty, bool Ok>
struct _Aligned_Storage_Impl;   // define type with size Len and alignment Ty

template<size_t Len, size_t Align, class Ty>
struct _Aligned_Storage_Impl<Len, Align, Ty, true>           // class
{
    using type = _Align_Type<Ty, Len>;
};

template<size_t Len, size_t Align>
struct _Aligned_Storage_Impl<Len, Align, double, false>      // double
{
    using type = _Align_Type<max_align_t, Len>;
};

template<size_t Len, size_t Align>
struct _Aligned_Storage_Impl<Len, Align, int, false>         // int
{
    using _Next = double;
    static constexpr bool Fits = Align <= alignof(_Next);
    using type = typename _Aligned_Storage_Impl<Len, Align, _Next, Fits>::type;
};

template<size_t Len, size_t Align>
struct _Aligned_Storage_Impl<Len, Align, short, false>       // short
{
    using _Next = int;
    static constexpr bool Fits = Align <= alignof(_Next);
    using type = typename _Aligned_Storage_Impl<Len, Align, _Next, Fits>::type;
};

template<size_t Len, size_t Align>
struct _Aligned_Storage_Impl<Len, Align, char, false>        // char
{
    using _Next = short;
    static constexpr bool Fits = Align <= alignof(_Next);
    using type = typename _Aligned_Storage_Impl<Len, Align, _Next, Fits>::type;
};

CUSTOM_DETAIL_END

// aligned_storage
template<size_t Len, size_t Align = alignof(max_align_t)>
struct aligned_storage  // define type with size Len and alignment Align
{
    using _Next = char;
    static constexpr bool Fits = Align <= alignof(_Next);
    using type = typename detail::_Aligned_Storage_Impl<Len, Align, _Next, Fits>::type;
};

template<size_t Len, size_t Align = alignof(max_align_t)>
using aligned_storage_t = typename aligned_storage<Len, Align>::type;
#pragma endregion alignment

#pragma region is_void
template<class Ty>
constexpr bool is_void_v = is_same_v<remove_cv_t<Ty>, void>;

template<class Ty>
struct is_void : bool_constant<is_void_v<Ty>> {};

template<class... Types>
using void_t = void;
#pragma endregion is_void

#pragma region add_const
template<class Ty>
struct add_const { using type = const Ty; };

template<class Ty>
using add_const_t = typename add_const<Ty>::type;
#pragma endregion add_const

#pragma region add_volatile
template<class Ty>
struct add_volatile { using type = volatile Ty; };

template<class Ty>
using add_volatile_t = typename add_volatile<Ty>::type;
#pragma endregion add_volatile

#pragma region add_cv
template<class Ty>
struct add_cv { using type = const volatile Ty; };

template<class Ty>
using add_cv_t = typename add_cv<Ty>::type;
#pragma endregion add_cv

#pragma region add_lvalue_reference/add_rvalue_reference
CUSTOM_DETAIL_BEGIN

// add reference impl
template<class Ty, class = void>
struct _Add_Reference_Impl  // add reference (non-referenceable type)
{
    using _Lvalue = Ty;
    using _Rvalue = Ty;
};

template<class Ty>
struct _Add_Reference_Impl<Ty, void_t<Ty&>> // (referenceable type)
{
    using _Lvalue = Ty&;
    using _Rvalue = Ty&&;
};

CUSTOM_DETAIL_END

template<class Ty>
struct add_lvalue_reference { using type = typename detail::_Add_Reference_Impl<Ty>::_Lvalue; };

template<class Ty>
using add_lvalue_reference_t = typename add_lvalue_reference<Ty>::type;

template<class Ty>
struct add_rvalue_reference { using type = typename detail::_Add_Reference_Impl<Ty>::_Rvalue; };

template<class Ty>
using add_rvalue_reference_t = typename add_rvalue_reference<Ty>::type;

// declval (ex: used in decltype)
template<class Ty>
add_rvalue_reference_t<Ty> declval() noexcept
{
    static_assert(always_false<Ty>, "declval not allowed in an evaluated context");
}
#pragma endregion add_lvalue_reference/add_rvalue_reference

#pragma region add_pointer
template<class Ty, class = void>                // add pointer (pointer type cannot be formed)
struct add_pointer { using type = Ty; };

template<class Ty>                              // (pointer type can be formed)
struct add_pointer<Ty, void_t<remove_reference_t<Ty>*>> { using type = remove_reference_t<Ty>*; };

template<class Ty>
using add_pointer_t = typename add_pointer<Ty>::type;
#pragma endregion add_pointer

#pragma region extent
template<class Ty, unsigned int Ix = 0>
constexpr size_t extent_v = 0;                  // determine extent of dimension Ix of array Ty

template<class Ty, size_t Nx>
constexpr size_t extent_v<Ty[Nx], 0> = Nx;

template<class Ty, unsigned int Ix, size_t Nx>
constexpr size_t extent_v<Ty[Nx], Ix> = extent_v<Ty, Ix - 1>;

template<class Ty, unsigned int Ix>
constexpr size_t extent_v<Ty[], Ix> = extent_v<Ty, Ix - 1>;

template<class Ty, unsigned int Ix = 0>
struct extent : integral_constant<size_t, extent_v<Ty, Ix>> {};
#pragma endregion extent

#pragma region remove_extent/remove_all_extents
template<class Ty>                              // remove array extent
struct remove_extent { using type = Ty; };

template<class Ty, size_t Size>
struct remove_extent<Ty[Size]> { using type = Ty; };

template<class Ty>
struct remove_extent<Ty[]> { using type = Ty; };

template<class Ty>
using remove_extent_t = typename remove_extent<Ty>::type;

template<class Ty>
struct remove_all_extents { using type = Ty; };

template<class Ty, size_t Ix>
struct remove_all_extents<Ty[Ix]> { using type = typename remove_all_extents<Ty>::type; };

template<class Ty>
struct remove_all_extents<Ty[]> { using type = typename remove_all_extents<Ty>::type; };

template<class Ty>
using remove_all_extents_t = typename remove_all_extents<Ty>::type;
#pragma endregion remove_extent/remove_all_extents

#pragma region is_array
template<class>                                 // determine whether type argument is an array
constexpr bool is_array_v = false;

template<class Ty, size_t Size>
constexpr bool is_array_v<Ty[Size]> = true;

template<class Ty>
constexpr bool is_array_v<Ty[]> = true;

template<class Ty>
struct is_array : bool_constant<is_array_v<Ty>> {};
#pragma endregion is_array

#pragma region is_bounded_array
template<class>
constexpr bool is_bounded_array_v = false;

template<class Ty, size_t Nx>
constexpr bool is_bounded_array_v<Ty[Nx]> = true;

template<class Ty>
struct is_bounded_array : bool_constant<is_bounded_array_v<Ty>> {};
#pragma endregion is_bounded_array

#pragma region is_unbounded_array
template<class>
constexpr bool is_unbounded_array_v = false;

template<class Ty>
constexpr bool is_unbounded_array_v<Ty[]> = true;

template<class Ty>
struct is_unbounded_array : bool_constant<is_unbounded_array_v<Ty>> {};
#pragma endregion is_unbounded_array

#pragma region is_lvalue_reference
template<class>                                 // determine whether type argument is an lvalue reference
constexpr bool is_lvalue_reference_v = false;

template<class Ty>
constexpr bool is_lvalue_reference_v<Ty&> = true;

template<class Ty>
struct is_lvalue_reference : bool_constant<is_lvalue_reference_v<Ty>> {};
#pragma endregion is_lvalue_reference

#pragma region is_rvalue_reference
template<class>                                 // determine whether type argument is an rvalue reference
constexpr bool is_rvalue_reference_v = false;

template<class Ty>
constexpr bool is_rvalue_reference_v<Ty&&> = true;

template<class Ty>
struct is_rvalue_reference : bool_constant<is_rvalue_reference_v<Ty>> {};
#pragma endregion is_rvalue_reference

#pragma region is_reference
template<class>                                 // determine whether type argument is a reference
constexpr bool is_reference_v = false;

template<class Ty>
constexpr bool is_reference_v<Ty&> = true;

template<class Ty>
constexpr bool is_reference_v<Ty&&> = true;

template<class Ty>
struct is_reference : bool_constant<is_reference_v<Ty>> {};
#pragma endregion is_reference

#pragma region is_pointer
template<class>                                 // determine whether Ty is a pointer
constexpr bool is_pointer_v = false;

template<class Ty>
constexpr bool is_pointer_v<Ty*> = true;

template<class Ty>
constexpr bool is_pointer_v<Ty* const> = true;

template<class Ty>
constexpr bool is_pointer_v<Ty* volatile> = true;

template<class Ty>
constexpr bool is_pointer_v<Ty* const volatile> = true;

template<class Ty>
struct is_pointer : bool_constant<is_pointer_v<Ty>> {};
#pragma endregion is_pointer

#pragma region is_enum
template<class Ty>
struct is_enum : bool_constant<__is_enum(Ty)> {};

template<class Ty>
constexpr bool is_enum_v = is_enum<Ty>::value;
#pragma endregion is_enum

#pragma region is_union
template<class Ty>
struct is_union : bool_constant<__is_union(Ty)> {};

template<class Ty>
constexpr bool is_union_v = is_union<Ty>::value;
#pragma endregion is_union

#pragma region is_trivial/is_trivially_copyable
template<class Ty>
struct is_trivial : bool_constant<__is_trivially_constructible(Ty) && __is_trivially_copyable(Ty)> {};

template<class Ty>
constexpr bool is_trivial_v = is_trivial<Ty>::value;

template<class Ty>
struct is_trivially_copyable : bool_constant<__is_trivially_copyable(Ty)> {};

template<class Ty>
constexpr bool is_trivially_copyable_v = is_trivially_copyable<Ty>::value;
#pragma endregion is_trivial/is_trivially_copyable

#pragma region is_standard_layout
template<class Ty>
struct is_standard_layout : bool_constant<__is_standard_layout(Ty)> {};

template<class Ty>
constexpr bool is_standard_layout_v = is_standard_layout<Ty>::value;
#pragma endregion is_standard_layout

#pragma region is_pod (Plain Old Data)
template<class Ty>
struct is_pod : bool_constant<is_standard_layout_v<Ty> && is_trivial_v<Ty>> {};
//bool_constant<__is_pod(Ty)> {};

template<class Ty>
constexpr bool is_pod_v = is_pod<Ty>::value;
#pragma endregion is_pod

#pragma region is_class
template<class Ty>
struct is_class : bool_constant<__is_class(Ty)> {};

template<class Ty>
constexpr bool is_class_v = is_class<Ty>::value;
#pragma endregion is_class

#pragma region is_convertible
CUSTOM_DETAIL_BEGIN

#if defined _MSC_VER
template<class From, class To>
struct _Is_Convertible_Impl : bool_constant<__is_convertible_to(From, To)> {};
#elif defined __GNUG__
template<class From, class To>
struct _Is_Convertible_Impl
{
    template<class Ty, class = decltype(static_cast<Ty(*)()>(nullptr))>
    static auto test_returnable(int) -> true_type;

    template<class>
    static auto test_returnable(...) -> false_type;

    template<class _From, class _To, class = decltype(custom::declval<void(&)(_To)>()(custom::declval<_From>()))>
    static auto test_implicitly_convertible(int) -> true_type;

    template<class, class>
    static auto test_implicitly_convertible(...) -> false_type;

    static constexpr bool value = 
    ((decltype(test_returnable<To>(0))::value && decltype(test_implicitly_convertible<From, To>(0))::value) ||
    (is_void_v<From> && is_void_v<To>));
};  // END _Is_Convertible_Impl
#endif  // is_convertible helpers

CUSTOM_DETAIL_END

template<class From, class To>
struct is_convertible : public bool_constant<detail::_Is_Convertible_Impl<From, To>::value> {};

template<class From, class To>
constexpr bool is_convertible_v = is_convertible<From, To>::value;
#pragma endregion is_convertible

#pragma region is_const
template<class>                                 // determine whether type argument is const qualified
constexpr bool is_const_v = false;

template<class Ty>
constexpr bool is_const_v<const Ty> = true;

template<class Ty>
struct is_const : bool_constant<is_const_v<Ty>> {};
#pragma endregion is_const

#pragma region is_volatile
template<class>
constexpr bool is_volatile_v = false;            // determine whether type argument is volatile qualified

template<class Ty>
constexpr bool is_volatile_v<volatile Ty> = true;

template<class Ty>
struct is_volatile : bool_constant<is_volatile_v<Ty>> {};
#pragma endregion is_volatile

#pragma region is_function
template<class Ty>                              // only function types and reference types can't be const qualified
constexpr bool is_function_v = !is_const_v<const Ty> && !is_reference_v<Ty>;

template<class Ty>
struct is_function : bool_constant<is_function_v<Ty>> {};
#pragma endregion is_function

#pragma region is_object
template<class Ty>                            // only function types and reference types can't be const qualified
constexpr bool is_object_v = is_const_v<const Ty> && !is_void_v<Ty>;

template<class Ty>
struct is_object : bool_constant<is_object_v<Ty>> {};
#pragma endregion is_object

#pragma region is_member_object_pointer
CUSTOM_DETAIL_BEGIN

// is member object pointer impl
template<class>
struct _Is_Member_Object_Pointer_Impl
{
    static constexpr bool value = false;
};

template<class Ty1, class Ty2>
struct _Is_Member_Object_Pointer_Impl<Ty1 Ty2::*>
{
    static constexpr bool value = !is_function_v<Ty1>;
    using _Class_Type = Ty2;
};

CUSTOM_DETAIL_END

template<class Ty>
constexpr bool is_member_object_pointer_v = detail::_Is_Member_Object_Pointer_Impl<remove_cv_t<Ty>>::value;

template<class Ty>
struct is_member_object_pointer : bool_constant<is_member_object_pointer_v<Ty>> {};
#pragma endregion is_member_object_pointer

#pragma region is_member_function_pointer
CUSTOM_DETAIL_BEGIN

// is member function pointer impl
template<class>
struct _Is_Member_Function_Pointer_Impl
{
    static constexpr bool value = false;
};

template<class Ty1, class Ty2>
struct _Is_Member_Function_Pointer_Impl<Ty1 Ty2::*>
{
    static constexpr bool value = is_function_v<Ty1>;
    using _Class_Type = Ty2;
};

CUSTOM_DETAIL_END

template<class Ty>
constexpr bool is_member_function_pointer_v = detail::_Is_Member_Function_Pointer_Impl<remove_cv_t<Ty>>::value;

template<class Ty>
struct is_member_function_pointer : bool_constant<is_member_function_pointer_v<Ty>> {};
#pragma endregion is_member_function_pointer

#pragma region is_member_pointer
template<class Ty>
constexpr bool is_member_pointer_v = is_member_object_pointer_v<Ty> || is_member_function_pointer_v<Ty>;

template<class Ty>
struct is_member_pointer : bool_constant<is_member_pointer_v<Ty>> {};
#pragma endregion is_member_pointer

#pragma region is_null_pointer
template<class Ty>
constexpr bool is_null_pointer_v = is_same_v<remove_cv_t<Ty>, std::nullptr_t>;

template<class Ty>
struct is_null_pointer : bool_constant<is_null_pointer_v<Ty>> {};
#pragma endregion is_null_pointer

#pragma region is_scalar
template<class Ty>
constexpr bool is_scalar_v = is_arithmetic_v<Ty> || is_enum_v<Ty> || is_pointer_v<Ty> || is_member_pointer_v<Ty> || is_null_pointer_v<Ty>;

template<class Ty>
struct is_scalar : bool_constant<is_scalar_v<Ty>> {};
#pragma endregion is_scalar

#pragma region is_empty
template<class Ty>
struct is_empty : bool_constant<__is_empty(Ty)> {};

template<class Ty>
constexpr bool is_empty_v = is_empty<Ty>::value;
#pragma endregion is_empty

#pragma region is_polymorphic
template<class Ty>
struct is_polymorphic : bool_constant<__is_polymorphic(Ty)> {};

template<class Ty>
constexpr bool is_polymorphic_v = is_polymorphic<Ty>::value;
#pragma endregion is_polymorphic

#pragma region is_abstract
template<class Ty>
struct is_abstract : bool_constant<__is_abstract(Ty)> {};

template<class Ty>
constexpr bool is_abstract_v = is_abstract<Ty>::value;
#pragma endregion is_abstract

#pragma region is_final
template<class Ty>
struct is_final : bool_constant<__is_final(Ty)> {};

template<class Ty>
constexpr bool is_final_v = is_final<Ty>::value;
#pragma endregion is_final

#pragma region is_constructible
template<class Ty, class... Args>
struct is_constructible : bool_constant<__is_constructible(Ty, Args...)> {};

template<class Ty, class... Args>
constexpr bool is_constructible_v = is_constructible<Ty, Args...>::value;
#pragma endregion is_constructible

#pragma region is_default_constructible
template<class Ty>
struct is_default_constructible : bool_constant<__is_constructible(Ty)> {};

template<class Ty>
constexpr bool is_default_constructible_v = is_default_constructible<Ty>::value;
#pragma endregion is_default_constructible

#pragma region is_implicitly_default_constructible
CUSTOM_DETAIL_BEGIN

template<class Ty>
void _Implicitly_Default_Construct(const Ty&);

CUSTOM_DETAIL_END

template<class Ty, class = void>
struct is_implicitly_default_constructible : false_type {}; // determine whether Ty can be copy-initialized with {}

template<class Ty>
struct is_implicitly_default_constructible<Ty, void_t<decltype(detail::_Implicitly_Default_Construct<Ty>({}))>> : true_type {};

template<class Ty>
constexpr bool is_implicitly_default_constructible_v = is_implicitly_default_constructible<Ty>::value;
#pragma endregion is_implicitly_default_constructible

#pragma region is_copy_constructible
template<class Ty>
struct is_copy_constructible : bool_constant<__is_constructible(Ty, add_lvalue_reference_t<const Ty>)> {};

template<class Ty>
constexpr bool is_copy_constructible_v = is_copy_constructible<Ty>::value;
#pragma endregion is_copy_constructible

#pragma region is_move_constructible
template<class Ty>
struct is_move_constructible : bool_constant<__is_constructible(Ty, Ty)> {};

template<class Ty>
constexpr bool is_move_constructible_v = is_move_constructible<Ty>::value;
#pragma endregion is_move_constructible

#pragma region is_nothrow_constructible
template<class Ty, class... Args>
struct is_nothrow_constructible : bool_constant<__is_nothrow_constructible(Ty, Args...)> {};

template<class Ty, class... Args>
constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<Ty, Args...>::value;
#pragma endregion is_nothrow_constructible

#pragma region is_nothrow_default_constructible
template<class Ty>
struct is_nothrow_default_constructible : bool_constant<__is_nothrow_constructible(Ty)> {};

template<class Ty>
constexpr bool is_nothrow_default_constructible_v = is_nothrow_default_constructible<Ty>::value;
#pragma endregion is_nothrow_default_constructible

#pragma region is_nothrow_copy_constructible
template<class Ty>
struct is_nothrow_copy_constructible : bool_constant<__is_nothrow_constructible(Ty, add_lvalue_reference_t<const Ty>)> {};

template<class Ty>
constexpr bool is_nothrow_copy_constructible_v = is_nothrow_copy_constructible<Ty>::value;
#pragma endregion is_nothrow_copy_constructible

#pragma region is_nothrow_move_constructible
template<class Ty>
struct is_nothrow_move_constructible : bool_constant<__is_nothrow_constructible(Ty, Ty)> {};

template<class Ty>
constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<Ty>::value;
#pragma endregion is_nothrow_move_constructible

#pragma region is_assignable
template<class To, class From>
struct is_assignable : bool_constant<__is_assignable(To, From)> {};

template<class To, class From>
constexpr bool is_assignable_v = is_assignable<To, From>::value;
#pragma endregion is_assignable

#pragma region is_copy_assignable
template<class Ty>
struct is_copy_assignable : bool_constant<__is_assignable(add_lvalue_reference_t<Ty>, add_lvalue_reference_t<const Ty>)> {};

template<class Ty>
constexpr bool is_copy_assignable_v = is_copy_assignable<Ty>::value;
#pragma endregion is_copy_assignable

#pragma region is_move_assignable
template<class Ty>
struct is_move_assignable : bool_constant<__is_assignable(add_lvalue_reference_t<Ty>, Ty)> {};

template<class Ty>
constexpr bool is_move_assignable_v = is_move_assignable<Ty>::value;
#pragma endregion is_move_assignable

#pragma region is_nothrow_assignable
template<class To, class From>
struct is_nothrow_assignable : bool_constant<__is_nothrow_assignable(To, From)> {};

template<class To, class From>
constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<To, From>::value;
#pragma endregion is_nothrow_assignable

#pragma region is_nothrow_copy_assignable
template<class Ty>
struct is_nothrow_copy_assignable : bool_constant<__is_nothrow_assignable(add_lvalue_reference_t<Ty>, add_lvalue_reference_t<const Ty>)> {};

template<class Ty>
constexpr bool is_nothrow_copy_assignable_v = is_nothrow_copy_assignable<Ty>::value;
#pragma endregion is_nothrow_copy_assignable

#pragma region is_nothrow_move_assignable
template<class Ty>
struct is_nothrow_move_assignable : bool_constant<__is_nothrow_assignable(add_lvalue_reference_t<Ty>, Ty)> {};

template<class Ty>
constexpr bool is_nothrow_move_assignable_v = is_nothrow_move_assignable<Ty>::value;
#pragma endregion is_nothrow_move_assignable

#pragma region is_nothrow_convertible
template<class From, class To, bool = is_convertible_v<From, To>, bool = is_void_v<To>>
constexpr bool is_nothrow_convertible_v = noexcept(_fake_copy_init<To>(custom::declval<From>()));

template<class From, class To, bool IsVoidResult>
constexpr bool is_nothrow_convertible_v<From, To, false, IsVoidResult> = false;

template<class From, class To>
constexpr bool is_nothrow_convertible_v<From, To, true, true> = true;

template<class From, class To>
struct is_nothrow_convertible : bool_constant<is_nothrow_convertible_v<From, To>> {};
#pragma endregion is_nothrow_convertible

#pragma region is_destructible
CUSTOM_DETAIL_BEGIN

#if defined _MSC_VER
template<class Ty>
struct _Is_Destructible_Impl : bool_constant<__is_destructible(Ty)> {};
#elif defined __GNUG__
template<class Ty>
struct _Is_Destructible_Test
{
    template<class _Ty, class = decltype(custom::declval<_Ty&>().~_Ty())>
    static auto test_destructible(int) -> true_type;

    template<class>
    static auto test_destructible(...) -> false_type;

    using type = decltype(test_destructible<Ty>(0));
};

template<class Ty,
bool = (is_void_v<Ty> || is_unbounded_array_v<Ty> || is_function_v<Ty>),
bool = (is_reference_v<Ty> || is_scalar_v<Ty>)>
struct _Is_Destructible_Impl;

template<class Ty>
struct _Is_Destructible_Impl<Ty, false, false> : _Is_Destructible_Test<remove_all_extents_t<Ty>>::type {};

template<class Ty>
struct _Is_Destructible_Impl<Ty, true, false> : false_type {};

template<class Ty>
struct _Is_Destructible_Impl<Ty, false, true> : true_type {};
#endif  // _Is_Destructible_Impl

CUSTOM_DETAIL_END

template<class Ty>
struct is_destructible : detail::_Is_Destructible_Impl<Ty>::type {};

template<class Ty>
constexpr bool is_destructible_v = is_destructible<Ty>::value;
#pragma endregion is_destructible

#pragma region is_nothrow_destructible
CUSTOM_DETAIL_BEGIN

#if defined _MSC_VER
template<class Ty>
struct _Is_Nothrow_Destructible_Impl : bool_constant<__is_nothrow_destructible(Ty)> {};
#elif defined __GNUG__
template<class Ty>
struct _Is_Nothrow_Destructible_Test
{
    template<class _Ty, class Return = bool_constant<noexcept(custom::declval<_Ty&>().~_Ty())>>
    static auto test_nothrow_destructible(int) -> Return;

    template<class>
    static auto test_nothrow_destructible(...) -> false_type;

    using type = decltype(test_nothrow_destructible<Ty>(0));
};

template<class Ty,
bool = (is_void_v<Ty> || is_unbounded_array_v<Ty> || is_function_v<Ty>),
bool = (is_reference_v<Ty> || is_scalar_v<Ty>)>
struct _Is_Nothrow_Destructible_Impl;

template<class Ty>
struct _Is_Nothrow_Destructible_Impl<Ty, false, false> : _Is_Nothrow_Destructible_Test<remove_all_extents_t<Ty>>::type {};

template<class Ty>
struct _Is_Nothrow_Destructible_Impl<Ty, true, false> : false_type {};

template<class Ty>
struct _Is_Nothrow_Destructible_Impl<Ty, false, true> : true_type {};
#endif  // is nothrow destructible

CUSTOM_DETAIL_END

template<class Ty>
struct is_nothrow_destructible : detail::_Is_Nothrow_Destructible_Impl<Ty>::type {};

template<class Ty>
constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<Ty>::value;
#pragma endregion is_nothrow_destructible

#pragma region is_swappable_with/is_nothrow_swappable_with
template<class Ty>
struct is_swappable;

template<class Ty>
struct is_nothrow_swappable;

template<class Ty,
enable_if_t<is_nothrow_move_constructible_v<Ty> && is_nothrow_move_assignable_v<Ty>, bool> = true>
constexpr void swap(Ty&, Ty&) noexcept;

template<class Ty, size_t Size,
enable_if_t<is_swappable<Ty>::value, bool> = true>
constexpr void swap(Ty(&)[Size], Ty(&)[Size]) noexcept(is_nothrow_swappable<Ty>::value);

CUSTOM_DETAIL_BEGIN

template<class Ty1, class Ty2, class = void>
struct _Is_Swappable_With_Impl : false_type {};

template<class Ty1, class Ty2>
struct _Is_Swappable_With_Impl<Ty1, Ty2,
void_t< decltype(custom::swap(custom::declval<Ty1>(), custom::declval<Ty2>())),
        decltype(custom::swap(custom::declval<Ty2>(), custom::declval<Ty1>()))>> : true_type {};

template<class Ty1, class Ty2>
struct _Is_Nothrow_Swappable_With_Impl
: bool_constant<noexcept(custom::swap(custom::declval<Ty1>(), custom::declval<Ty2>())) &&
                noexcept(custom::swap(custom::declval<Ty2>(), custom::declval<Ty1>()))> {};

CUSTOM_DETAIL_END

template<class Ty1, class Ty2>
struct is_swappable_with : detail::_Is_Swappable_With_Impl<Ty1, Ty2>::type {};

template<class Ty1, class Ty2>
constexpr bool is_swappable_with_v = is_swappable_with<Ty1, Ty2>::value;

template<class Ty1, class Ty2>
struct is_nothrow_swappable_with : bool_constant<conjunction_v<
                                                    detail::_Is_Swappable_With_Impl<Ty1, Ty2>,
                                                    detail::_Is_Nothrow_Swappable_With_Impl<Ty1, Ty2>>> {};

template<class Ty1, class Ty2>
constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<Ty1, Ty2>::value;
#pragma endregion is_swappable_with/is_nothrow_swappable_with

#pragma region is_swappable
template<class Ty>
struct is_swappable : detail::_Is_Swappable_With_Impl<add_lvalue_reference_t<Ty>, add_lvalue_reference_t<Ty>>::type {};

template<class Ty>
constexpr bool is_swappable_v = is_swappable<Ty>::value;
#pragma endregion is_swappable

#pragma region is_nothrow_swappable
template<class Ty>
struct is_nothrow_swappable : detail::_Is_Nothrow_Swappable_With_Impl<add_lvalue_reference_t<Ty>, add_lvalue_reference_t<Ty>>::type {};

template<class Ty>
constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<Ty>::value;
#pragma endregion is_nothrow_swappable

#pragma region is_base_of
template<class Base, class Derived>
struct is_base_of : bool_constant<__is_base_of(Base, Derived)> {};

template<class Base, class Derived>
constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;
#pragma endregion is_base_of

#pragma region is_specialization
template<class type, template<class...> class Template>
constexpr bool is_specialization_v = false; // true if and only if type is a specialization of Template

template<template<class...> class Template, class... Types>
constexpr bool is_specialization_v<Template<Types...>, Template> = true;

template<class type, template<class...> class Template>
struct is_specialization : bool_constant<is_specialization_v<type, Template>> {};
#pragma endregion is_specialization

#pragma region is_signed/is_unsigned
CUSTOM_DETAIL_BEGIN

template<class Ty, bool = is_integral_v<Ty>>
struct _Sign_Choice
{
    using _No_cv_t = remove_cv_t<Ty>;

    static constexpr bool _Signed   = static_cast<_No_cv_t>(-1) < static_cast<_No_cv_t>(0);
    static constexpr bool _Unsigned = !_Signed;
};

template<class Ty>
struct _Sign_Choice<Ty, false>
{
    // floating-point Ty is signed
    // non-arithmetic Ty is neither signed nor unsigned
    static constexpr bool _Signed   = is_floating_point_v<Ty>;
    static constexpr bool _Unsigned = false;
};

CUSTOM_DETAIL_END

template<class Ty>
struct is_signed : bool_constant<detail::_Sign_Choice<Ty>::_Signed> {};

template<class Ty>
constexpr bool is_signed_v = is_signed<Ty>::value;

template<class Ty>
struct is_unsigned : bool_constant<detail::_Sign_Choice<Ty>::_Unsigned> {};

template<class Ty>
constexpr bool is_unsigned_v = is_unsigned<Ty>::value;
#pragma endregion is_signed/is_unsigned

#pragma region is_nonbool_integral_v
template<class Ty>
constexpr bool is_nonbool_integral_v = is_integral_v<Ty> && !is_same_v<remove_cv_t<Ty>, bool>;
#pragma endregion is_nonbool_integral_v

#pragma region make_signed
CUSTOM_DETAIL_BEGIN

template<size_t>
struct _Make_Signed_Choice; // Choose make_signed strategy by type size

template<>
struct _Make_Signed_Choice<1>
{
    template<class>
    using _Apply = signed char;
};

template<>
struct _Make_Signed_Choice<2>
{
    template<class>
    using _Apply = short;
};

template<>
struct _Make_Signed_Choice<4>
{
    // assumes LLP64

    template<class Ty>
    using _Apply = conditional_t<   is_same_v<Ty, long> || is_same_v<Ty, unsigned long>,
                                    long,
                                    int>;
};

template<>
struct _Make_Signed_Choice<8>
{
    template<class>
    using _Apply = long long;
};

template<class Ty>
using _Make_Signed = typename _Make_Signed_Choice<sizeof(Ty)>::template _Apply<Ty>;   // signed partner to cv-unqualified Ty

CUSTOM_DETAIL_END

template<class Ty>
struct make_signed      // signed partner to Ty
{
    static_assert(is_nonbool_integral_v<Ty> || is_enum_v<Ty>,
                    "make_signed<T> requires that T shall be a (possibly cv-qualified) "
                    "integral type or enumeration but not a bool type.");

    // removes CV from Ty, makes choice of signment, reapplies CV
    using type = typename remove_cv<Ty>::template _Apply<detail::_Make_Signed>;
};

template<class Ty>
using make_signed_t = typename make_signed<Ty>::type;
#pragma endregion make_signed

#pragma region make_unsigned
CUSTOM_DETAIL_BEGIN

template<size_t>
struct _Make_Unsigned_Choice; // Choose make_unsigned strategy by type size

template<>
struct _Make_Unsigned_Choice<1>
{
    template<class>
    using _Apply = unsigned char;
};

template<>
struct _Make_Unsigned_Choice<2>
{
    template<class>
    using _Apply = unsigned short;
};

template<>
struct _Make_Unsigned_Choice<4>
{
    // assumes LLP64

    template<class Ty>
    using _Apply = conditional_t<   is_same_v<Ty, long> || is_same_v<Ty, unsigned long>,
                                    unsigned long,
                                    unsigned int>;
};

template<>
struct _Make_Unsigned_Choice<8>
{
    template<class>
    using _Apply = unsigned long long;
};

template<class Ty>
using _Make_Unsigned = typename _Make_Unsigned_Choice<sizeof(Ty)>::template _Apply<Ty>;    // unsigned partner to cv-unqualified Ty

CUSTOM_DETAIL_END

template<class Ty>
struct make_unsigned    // unsigned partner to Ty
{
    static_assert(is_nonbool_integral_v<Ty> || is_enum_v<Ty>,
                    "make_unsigned<Ty> requires that Ty shall be a (possibly cv-qualified) "
                    "integral type or enumeration but not a bool type.");

    // removes CV from Ty, makes choice of unsignment, reapplies CV
    using type = typename remove_cv<Ty>::template _Apply<detail::_Make_Unsigned>;
};

template<class Ty>
using make_unsigned_t = typename make_unsigned<Ty>::type;
#pragma endregion make_unsigned

#pragma region decay
/// @brief Performs the type conversions equivalent to the ones performed when passing function arguments by value
template<class Ty>
struct decay
{
private:
    using _No_ref_t = remove_reference_t<Ty>;

public:
    using type = conditional_t<is_array_v<_No_ref_t>,
                                    add_pointer_t<remove_extent_t<_No_ref_t>>,
                                    conditional_t<is_function_v<_No_ref_t>,
                                                        add_pointer_t<_No_ref_t>,
                                                        remove_cv_t<_No_ref_t>>>;
};

template<class Ty>
using decay_t = typename decay<Ty>::type;
#pragma endregion decay

#pragma region common_type
CUSTOM_DETAIL_BEGIN

// this decides if Ty1 and Ty2 are compatible and the result is the implicit conversion type
template<class Ty1, class Ty2>
using _Conditional_Result_t = decltype(true /*or false*/ ? custom::declval<Ty1>() : custom::declval<Ty2>());

template<class, class, class = void>
struct _Decay_Conditional_Result {};

template<class Ty1, class Ty2>
struct _Decay_Conditional_Result<Ty1, Ty2, void_t<_Conditional_Result_t<Ty1, Ty2>>> : decay<_Conditional_Result_t<Ty1, Ty2>> {};

// apply decay for Ty1 and Ty2 recursively until the decayed type is the same as original
template<class Ty1, class Ty2, class Decayed1 = decay_t<Ty1>, class Decayed2 = decay_t<Ty2>>
struct _Common_Type2_Impl : _Common_Type2_Impl<Decayed1, Decayed2> {};

template<class Ty1, class Ty2>
struct _Common_Type2_Impl<Ty1, Ty2, Ty1, Ty2> : _Decay_Conditional_Result<Ty1, Ty2> {};

CUSTOM_DETAIL_END

template<class... Ty>
struct common_type;

template<class... Ty>
using common_type_t = typename common_type<Ty...>::type;

template<>
struct common_type<> {};    // 0 types

template<class Ty>
struct common_type<Ty> : common_type<Ty, Ty> {};    // 1 type

template<class Ty1, class Ty2>
struct common_type<Ty1, Ty2> : detail::_Common_Type2_Impl<Ty1, Ty2> {};  // 2 types

template<class Ty1, class Ty2, class... Rest>
struct common_type<Ty1, Ty2, Rest...> : common_type<common_type_t<Ty1, Ty2>, Rest...> {};   // 3+ types
#pragma endregion common_type

template<class Ty>
Ty _fake_copy_init(Ty) noexcept;
// _fake_copy_init<T>(E):
// (1) has type T [decay_t<decltype((E))> if T is deduced],
// (2) is well-formed if and only if E is implicitly convertible to T and T is destructible, and
// (3) is non-throwing if and only if both conversion from decltype((E)) to T and destruction of T are non-throwing.

CUSTOM_END
