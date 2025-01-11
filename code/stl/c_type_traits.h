#pragma once
#include "y_core.h"


CUSTOM_BEGIN

#pragma region declarations/types/values

/**
 * @note Defined here due to dependecies in detail::
 */
template<class Ty, Ty Val>
struct integral_constant
{
    using value_type    = Ty;
    using type          = integral_constant;

    static constexpr value_type value = Val;

    constexpr operator value_type() const noexcept { return value; }

    constexpr value_type operator()() const noexcept { return value; }
};

template<bool Val>
using bool_constant = integral_constant<bool, Val>;

using true_type  = bool_constant<true>;
using false_type = bool_constant<false>;

// false value attached to a dependent name (for static_assert)
template<class>
constexpr bool always_false_v = false;

template<class Ty>
struct type_identity;

template<class Ty>
using type_identity_t = typename type_identity<Ty>::type;

template<bool Test, class Ty = void>
struct enable_if;

template<bool Test, class Ty = void>
using enable_if_t = typename enable_if<Test, Ty>::type;

template<bool Test, class Ty1, class Ty2>   // Choose Ty1 if Test is true, and Ty2 otherwise
struct conditional;

template<bool Test, class Ty1, class Ty2>
using conditional_t = typename conditional<Test, Ty1, Ty2>::type;

template<class Trait>
struct negation;

template<class Trait>
constexpr bool negation_v = negation<Trait>::value;

template<class... Traits>
struct conjunction;

template<class... Traits>
constexpr bool conjunction_v = conjunction<Traits...>::value;

template<class... Traits>
struct disjunction;

template<class... Traits>
constexpr bool disjunction_v = disjunction<Traits...>::value;

template<class Ty>  // remove top-level const qualifier
struct remove_const;

template<class Ty>
using remove_const_t = typename remove_const<Ty>::type;

template<class Ty>  // remove top-level volatile qualifier
struct remove_volatile;

template<class Ty>
using remove_volatile_t = typename remove_volatile<Ty>::type;

template<class Ty>  // remove top-level const and volatile qualifiers
struct remove_cv;

template<class Ty>
using remove_cv_t = typename remove_cv<Ty>::type;

template<class Ty>
struct remove_cv_ref;

template<class Ty>
using remove_cv_ref_t = typename remove_cv_ref<Ty>::type;

template<class Ty>
struct remove_reference;

template<class Ty>
using remove_reference_t = typename remove_reference<Ty>::type;

template<class Ty>
struct remove_pointer;

template<class Ty>
using remove_pointer_t = typename remove_pointer<Ty>::type;

template<class Ty1, class Ty2>
struct is_same;

template<class Ty1, class Ty2>
constexpr bool is_same_v = is_same<Ty1, Ty2>::value;

template<class Ty, class... Types>  // true if Ty is in Types
struct is_any_of;

template<class Ty, class... Types>
constexpr bool is_any_of_v = is_any_of<Ty, Types...>::value;

template<class Ty>
struct is_integral;

template<class Ty>
constexpr bool is_integral_v = is_integral<Ty>::value;

template<class Ty>
struct is_unsigned_integer;

template<class Ty>
constexpr bool is_unsigned_integer_v = is_unsigned_integer<Ty>::value;

template<class Ty>
struct is_char;

template<class Ty>
constexpr bool is_char_v = is_char<Ty>::value;

template<class Ty>
struct is_floating_point;

template<class Ty>
constexpr bool is_floating_point_v = is_floating_point<Ty>::value;

template<class Ty>
struct is_arithmetic;

template<class Ty>
constexpr bool is_arithmetic_v = is_arithmetic<Ty>::value;

template<class Ty, Ty... Vals>
struct integer_sequence;

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

template<class Ty>
struct alignment_of;

template<class Ty>
constexpr size_t alignment_of_v = alignment_of<Ty>::value;

using max_align_t = double;

template<size_t Len, size_t Align = alignof(max_align_t)>
struct aligned_storage; // define type with size Len and alignment Align

template<size_t Len, size_t Align = alignof(max_align_t)>
using aligned_storage_t = typename aligned_storage<Len, Align>::type;

template<class Ty>
struct is_void;

template<class Ty>
constexpr bool is_void_v = is_void<Ty>::value;

template<class... Types>
using void_t = void;

template<class Ty>
struct add_const;

template<class Ty>
using add_const_t = typename add_const<Ty>::type;

template<class Ty>
struct add_volatile;

template<class Ty>
using add_volatile_t = typename add_volatile<Ty>::type;

template<class Ty>
struct add_cv;

template<class Ty>
using add_cv_t = typename add_cv<Ty>::type;

template<class Ty>
struct add_lvalue_reference;

template<class Ty>
using add_lvalue_reference_t = typename add_lvalue_reference<Ty>::type;

template<class Ty>
struct add_rvalue_reference;

template<class Ty>
using add_rvalue_reference_t = typename add_rvalue_reference<Ty>::type;

template<class Ty, class AlwaysVoid = void>
struct add_pointer;

template<class Ty>
using add_pointer_t = typename add_pointer<Ty>::type;

template<class Ty, unsigned int Ix = 0>
struct extent;  // determine extent of dimension Ix of array Ty

template<class Ty, unsigned int Ix = 0>
constexpr size_t extent_v = extent<Ty, Ix>::value;

template<class Ty>  // remove array extent
struct remove_extent;

template<class Ty>
using remove_extent_t = typename remove_extent<Ty>::type;

template<class Ty>
struct remove_all_extents;

template<class Ty>
using remove_all_extents_t = typename remove_all_extents<Ty>::type;

template<class Ty>
struct is_array;

template<class Ty>
constexpr bool is_array_v = is_array<Ty>::value;

template<class Ty>
struct is_bounded_array;

template<class Ty>
constexpr bool is_bounded_array_v = is_bounded_array<Ty>::value;

template<class Ty>
struct is_unbounded_array;

template<class Ty>
constexpr bool is_unbounded_array_v = is_unbounded_array<Ty>::value;

template<class Ty>
struct is_lvalue_reference;

template<class Ty>
constexpr bool is_lvalue_reference_v = is_lvalue_reference<Ty>::value;

template<class Ty>
struct is_rvalue_reference;

template<class Ty>
constexpr bool is_rvalue_reference_v = is_rvalue_reference<Ty>::value;

template<class Ty>
struct is_reference;

template<class Ty>
constexpr bool is_reference_v = is_reference<Ty>::value;

template<class Ty>
struct is_pointer;

template<class Ty>
constexpr bool is_pointer_v = is_pointer<Ty>::value;

template<class Ty>
struct is_enum;

template<class Ty>
constexpr bool is_enum_v = is_enum<Ty>::value;

template<class Ty>
struct is_union;

template<class Ty>
constexpr bool is_union_v = is_union<Ty>::value;

template<class Ty>
struct is_trivial;

template<class Ty>
constexpr bool is_trivial_v = is_trivial<Ty>::value;

template<class Ty>
struct is_trivially_copyable;

template<class Ty>
constexpr bool is_trivially_copyable_v = is_trivially_copyable<Ty>::value;

// TODO: check trivial type traits

template<class Ty>
struct is_standard_layout;

template<class Ty>
constexpr bool is_standard_layout_v = is_standard_layout<Ty>::value;

template<class Ty>
struct is_pod;  // plain old data

template<class Ty>
constexpr bool is_pod_v = is_pod<Ty>::value;

template<class Ty>
struct is_class;

template<class Ty>
constexpr bool is_class_v = is_class<Ty>::value;

template<class From, class To>
struct is_convertible;

template<class From, class To>
constexpr bool is_convertible_v = is_convertible<From, To>::value;

template<class Ty>
struct is_const;

template<class Ty>
constexpr bool is_const_v = is_const<Ty>::value;

template<class Ty>
struct is_volatile;

template<class Ty>
constexpr bool is_volatile_v = is_volatile<Ty>::value;

template<class Ty>
struct is_function;

template<class Ty>
constexpr bool is_function_v = is_function<Ty>::value;

template<class Ty>
struct is_object;

template<class Ty>
constexpr bool is_object_v = is_object<Ty>::value;

template<class Ty>
struct is_member_object_pointer;

template<class Ty>
constexpr bool is_member_object_pointer_v = is_member_object_pointer<Ty>::value;

template<class Ty>
struct is_member_function_pointer;

template<class Ty>
constexpr bool is_member_function_pointer_v = is_member_function_pointer<Ty>::value;

template<class Ty>
struct is_member_pointer;

template<class Ty>
constexpr bool is_member_pointer_v = is_member_pointer<Ty>::value;

template<class Ty>
struct is_null_pointer;

template<class Ty>
constexpr bool is_null_pointer_v = is_null_pointer<Ty>::value;

template<class Ty>
struct is_scalar;

template<class Ty>
constexpr bool is_scalar_v = is_scalar<Ty>::value;

template<class Ty>
struct is_empty;

template<class Ty>
constexpr bool is_empty_v = is_empty<Ty>::value;

template<class Ty>
struct is_polymorphic;

template<class Ty>
constexpr bool is_polymorphic_v = is_polymorphic<Ty>::value;

template<class Ty>
struct is_abstract;

template<class Ty>
constexpr bool is_abstract_v = is_abstract<Ty>::value;

template<class Ty>
struct is_final;

template<class Ty>
constexpr bool is_final_v = is_final<Ty>::value;

template<class Ty, class... Args>
struct is_constructible;

template<class Ty, class... Args>
constexpr bool is_constructible_v = is_constructible<Ty, Args...>::value;

template<class Ty>
struct is_default_constructible;

template<class Ty>
constexpr bool is_default_constructible_v = is_default_constructible<Ty>::value;

template<class Ty, class AlwaysVoid = void>
struct is_implicitly_default_constructible;

template<class Ty>
constexpr bool is_implicitly_default_constructible_v = is_implicitly_default_constructible<Ty>::value;

template<class Ty>
struct is_copy_constructible;

template<class Ty>
constexpr bool is_copy_constructible_v = is_copy_constructible<Ty>::value;

template<class Ty>
struct is_move_constructible;

template<class Ty>
constexpr bool is_move_constructible_v = is_move_constructible<Ty>::value;

template<class Ty, class... Args>
struct is_nothrow_constructible;

template<class Ty, class... Args>
constexpr bool is_nothrow_constructible_v = is_nothrow_constructible<Ty, Args...>::value;

template<class Ty>
struct is_nothrow_default_constructible;

template<class Ty>
constexpr bool is_nothrow_default_constructible_v = is_nothrow_default_constructible<Ty>::value;

template<class Ty>
struct is_nothrow_copy_constructible;

template<class Ty>
constexpr bool is_nothrow_copy_constructible_v = is_nothrow_copy_constructible<Ty>::value;

template<class Ty>
struct is_nothrow_move_constructible;

template<class Ty>
constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<Ty>::value;

template<class To, class From>
struct is_assignable;

template<class To, class From>
constexpr bool is_assignable_v = is_assignable<To, From>::value;

template<class Ty>
struct is_copy_assignable;

template<class Ty>
constexpr bool is_copy_assignable_v = is_copy_assignable<Ty>::value;

template<class Ty>
struct is_move_assignable;

template<class Ty>
constexpr bool is_move_assignable_v = is_move_assignable<Ty>::value;

template<class To, class From>
struct is_nothrow_assignable;

template<class To, class From>
constexpr bool is_nothrow_assignable_v = is_nothrow_assignable<To, From>::value;

template<class Ty>
struct is_nothrow_copy_assignable;

template<class Ty>
constexpr bool is_nothrow_copy_assignable_v = is_nothrow_copy_assignable<Ty>::value;

template<class Ty>
struct is_nothrow_move_assignable;

template<class Ty>
constexpr bool is_nothrow_move_assignable_v = is_nothrow_move_assignable<Ty>::value;

template<class From, class To>
struct is_nothrow_convertible;

template<class From, class To>
constexpr bool is_nothrow_convertible_v = is_nothrow_convertible<From, To>::value;

template<class Ty>
struct is_destructible;

template<class Ty>
constexpr bool is_destructible_v = is_destructible<Ty>::value;

template<class Ty>
struct is_nothrow_destructible;

template<class Ty>
constexpr bool is_nothrow_destructible_v = is_nothrow_destructible<Ty>::value;

template<class Ty1, class Ty2>
struct is_swappable_with;

template<class Ty1, class Ty2>
constexpr bool is_swappable_with_v = is_swappable_with<Ty1, Ty2>::value;

template<class Ty1, class Ty2>
struct is_nothrow_swappable_with;

template<class Ty1, class Ty2>
constexpr bool is_nothrow_swappable_with_v = is_nothrow_swappable_with<Ty1, Ty2>::value;

template<class Ty>
struct is_swappable;

template<class Ty>
constexpr bool is_swappable_v = is_swappable<Ty>::value;

template<class Ty>
struct is_nothrow_swappable;

template<class Ty>
constexpr bool is_nothrow_swappable_v = is_nothrow_swappable<Ty>::value;

template<class Ty,
enable_if_t<is_nothrow_move_constructible_v<Ty> && is_nothrow_move_assignable_v<Ty>, bool> = true>
constexpr void swap(Ty&, Ty&) noexcept;

template<class Ty, size_t Size,
enable_if_t<is_swappable_v<Ty>, bool> = true>
constexpr void swap(Ty(&)[Size], Ty(&)[Size]) noexcept(is_nothrow_swappable_v<Ty>);

template<class Base, class Derived>
struct is_base_of;

template<class Base, class Derived>
constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;

template<class Type, template<class...> class Template>
struct is_specialization;   // true if and only if Type is a specialization of Template

template<class Type, template<class...> class Template>
constexpr bool is_specialization_v = is_specialization<Type, Template>::value;

template<class Ty>
struct is_signed;

template<class Ty>
constexpr bool is_signed_v = is_signed<Ty>::value;

template<class Ty>
struct is_unsigned;

template<class Ty>
constexpr bool is_unsigned_v = is_unsigned<Ty>::value;

template<class Ty>
struct is_nonbool_integral;

template<class Ty>
constexpr bool is_nonbool_integral_v = is_nonbool_integral<Ty>::value;

template<class Ty>
struct make_signed;

template<class Ty>
using make_signed_t = typename make_signed<Ty>::type;

template<class Ty>
struct make_unsigned;

template<class Ty>
using make_unsigned_t = typename make_unsigned<Ty>::type;

/// @brief Performs the type conversions equivalent to the ones performed when passing function arguments by value
template<class Ty>
struct decay;

template<class Ty>
using decay_t = typename decay<Ty>::type;

template<class... Ty>
struct common_type;

template<class... Ty>
using common_type_t = typename common_type<Ty...>::type;

constexpr inline bool is_constant_evaluated() noexcept
{
    return __builtin_is_constant_evaluated();
}

template<class Ty>
add_rvalue_reference_t<Ty> declval() noexcept
{
    static_assert(always_false_v<Ty>, "declval not allowed in an evaluated context");
}

#pragma endregion declarations


#pragma region helpers

CUSTOM_DETAIL_BEGIN

template<class Ty>
Ty _Fake_Copy_Init(Ty) noexcept;
// _Fake_Copy_Init<T>(E):
// (1) has type T [decay_t<decltype((E))> if T is deduced],
// (2) is well-formed if and only if E is implicitly convertible to T and T is destructible, and
// (3) is non-throwing if and only if both conversion from decltype((E)) to T and destruction of T are non-throwing.

template<class Ty>
void _Implicitly_Default_Construct(const Ty&);

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

template<class From, class To, bool = is_convertible_v<From, To>, bool = is_void_v<To>>
struct _Is_Nothrow_Convertible_Impl : bool_constant<noexcept(_Fake_Copy_Init<To>(custom::declval<From>()))> {};

template<class From, class To, bool IsVoidResult>
struct _Is_Nothrow_Convertible_Impl<From, To, false, IsVoidResult> : false_type {};

template<class From, class To>
struct _Is_Nothrow_Convertible_Impl<From, To, true, true> : true_type {};

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
#endif  // _Is_Nothrow_Destructible_Impl

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
using _Make_Signed = typename _Make_Signed_Choice<sizeof(Ty)>::template _Apply<Ty>;

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
using _Make_Unsigned = typename _Make_Unsigned_Choice<sizeof(Ty)>::template _Apply<Ty>;

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

#pragma endregion helpers


#pragma region definitions

template<class Ty>
struct type_identity { using type = Ty; };

template<bool Test, class Ty>
struct enable_if {};

template<class Ty>
struct enable_if<true, Ty> { using type = Ty; };

template<bool Test, class Ty1, class Ty2>       // Choose Ty1 if Test is true, and Ty2 otherwise
struct conditional { using type = Ty1; };

template<class Ty1, class Ty2>
struct conditional<false, Ty1, Ty2> { using type = Ty2; };

template<class Trait>
struct negation : bool_constant<!static_cast<bool>(Trait::value)> {}; // The negated result of Trait

template<class... Traits>
struct conjunction : true_type {};
 
template<class First>
struct conjunction<First> : First {};
 
template<class First, class... Rest>
struct conjunction<First, Rest...>
: conditional_t<static_cast<bool>(First::value), conjunction<Rest...>, First> {};

template<class... Traits>
struct disjunction : false_type {};
 
template<class First>
struct disjunction<First> : First {};
 
template<class First, class... Rest>
struct disjunction<First, Rest...>
: conditional_t<static_cast<bool>(First::value), First, disjunction<Rest...>> {};

template<class Ty>
struct remove_const { using type = Ty; };

template<class Ty>
struct remove_const<const Ty> { using type = Ty; };

template<class Ty>                                  // remove top-level volatile qualifier
struct remove_volatile { using type = Ty; };

template<class Ty>
struct remove_volatile<volatile Ty> { using type = Ty; };

template<class Ty>
struct remove_cv
{
    using type = Ty;

    template<template<class> class Fn>
    using _Apply = Fn<Ty>;  // apply cv-qualifiers from the class template argument to Fn<Ty>
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
struct remove_cv_ref : remove_cv<Ty> {};

template<class Ty>
struct remove_cv_ref<Ty&> : remove_cv<Ty> {};

template<class Ty>
struct remove_cv_ref<Ty&&> : remove_cv<Ty> {};

template<class Ty>
struct remove_reference { using type = Ty; };

template<class Ty>
struct remove_reference<Ty&> { using type = Ty; };

template<class Ty>
struct remove_reference<Ty&&> { using type = Ty; };

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

template<class Ty1, class Ty2>
struct is_same : false_type {};

template<class Ty1>
struct is_same<Ty1, Ty1> : true_type {};

template<class Ty, class... Types>  // true if Ty is in Types
struct is_any_of : disjunction<is_same<Ty, Types>...> {};

template<class Ty>
struct is_integral : is_any_of<remove_cv_t<Ty>, bool, char, signed char, unsigned char,
wchar_t,
#ifdef __cpp_char8_t
char8_t,
#endif
char16_t, char32_t, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long> {};

template<class Ty>
struct is_unsigned_integer : is_any_of<remove_cv_t<Ty>, unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long> {};

template<class Ty>
struct is_char : is_any_of<remove_cv_t<Ty>, char, wchar_t,
#ifdef __cpp_char8_t
char8_t,
#endif
char16_t, char32_t> {};

template<class Ty>
struct is_floating_point : is_any_of<remove_cv_t<Ty>, float, double, long double> {};

template<class Ty>
struct is_arithmetic : disjunction<is_integral<Ty>, is_floating_point<Ty>> {};

template<class Ty, Ty... Vals>
struct integer_sequence
{
    using value_type = Ty;
    static constexpr size_t size() noexcept { return sizeof...(Vals); }
};

template<class Ty>
struct alignment_of : integral_constant<size_t, alignof(Ty)> {}; // determine alignment of Ty

template<size_t Len, size_t Align>
struct aligned_storage
{
    using _Next = char;
    static constexpr bool Fits = Align <= alignof(_Next);
    using type = typename detail::_Aligned_Storage_Impl<Len, Align, _Next, Fits>::type;
};

template<class Ty>
struct is_void : is_same<remove_cv_t<Ty>, void> {};

template<class Ty>
struct add_const { using type = const Ty; };

template<class Ty>
struct add_volatile { using type = volatile Ty; };

template<class Ty>
struct add_cv { using type = const volatile Ty; };

template<class Ty>
struct add_lvalue_reference { using type = typename detail::_Add_Reference_Impl<Ty>::_Lvalue; };

template<class Ty>
struct add_rvalue_reference { using type = typename detail::_Add_Reference_Impl<Ty>::_Rvalue; };

template<class Ty, class AlwaysVoid>    // add pointer (pointer type cannot be formed)
struct add_pointer { using type = Ty; };

template<class Ty>                      // (pointer type can be formed)
struct add_pointer<Ty, void_t<remove_reference_t<Ty>*>> { using type = remove_reference_t<Ty>*; };

template<class Ty, unsigned int Ix>
struct extent : std::integral_constant<size_t, 0> {}; // Default case: extent is 0

template<class Ty, size_t Nx>
struct extent<Ty[Nx], 0> : std::integral_constant<size_t, Nx> {}; // First dimension extent

template<class Ty, unsigned int Ix, size_t Nx>
struct extent<Ty[Nx], Ix> : extent<Ty, Ix - 1> {}; // Recurse to the next dimension

template<class Ty, unsigned int Ix>
struct extent<Ty[], Ix> : extent<Ty, Ix - 1> {}; // Recurse for unknown-size arrays

template<class Ty>                              // remove array extent
struct remove_extent { using type = Ty; };

template<class Ty, size_t Size>
struct remove_extent<Ty[Size]> { using type = Ty; };

template<class Ty>
struct remove_extent<Ty[]> { using type = Ty; };

template<class Ty>
struct remove_all_extents { using type = Ty; };

template<class Ty, size_t Ix>
struct remove_all_extents<Ty[Ix]> { using type = typename remove_all_extents<Ty>::type; };

template<class Ty>
struct remove_all_extents<Ty[]> { using type = typename remove_all_extents<Ty>::type; };

template<class Ty>
struct is_array : false_type {};

template<class Ty, size_t Size>
struct is_array<Ty[Size]> : true_type {};

template<class Ty>
struct is_array<Ty[]> : true_type {};

template<class Ty>
struct is_bounded_array : false_type {};

template<class Ty, size_t Nx>
struct is_bounded_array<Ty[Nx]> : true_type {};

template<class Ty>
struct is_unbounded_array : false_type {};

template<class Ty>
struct is_unbounded_array<Ty[]> : true_type {};

template<class Ty>
struct is_lvalue_reference : false_type {};

template<class Ty>
struct is_lvalue_reference<Ty&> : true_type {};

template<class Ty>
struct is_rvalue_reference : false_type {};

template<class Ty>
struct is_rvalue_reference<Ty&&> : true_type {};

template<class Ty>
struct is_reference : false_type {};

template<class Ty>
struct is_reference<Ty&> : true_type {};

template<class Ty>
struct is_reference<Ty&&> : true_type {};

template<class Ty>
struct is_pointer : false_type {};

template<class Ty>
struct is_pointer<Ty*> : true_type {};

template<class Ty>
struct is_pointer<Ty* const> : true_type {};

template<class Ty>
struct is_pointer<Ty* volatile> : true_type {};

template<class Ty>
struct is_pointer<Ty* const volatile> : true_type {};

template<class Ty>
struct is_enum : bool_constant<__is_enum(Ty)> {};

template<class Ty>
struct is_union : bool_constant<__is_union(Ty)> {};

template<class Ty>
struct is_trivial : bool_constant<__is_trivially_constructible(Ty) && __is_trivially_copyable(Ty)> {};

template<class Ty>
struct is_trivially_copyable : bool_constant<__is_trivially_copyable(Ty)> {};

template<class Ty>
struct is_standard_layout : bool_constant<__is_standard_layout(Ty)> {};

template<class Ty>
struct is_pod : bool_constant<is_standard_layout_v<Ty> && is_trivial_v<Ty>> {};

template<class Ty>
struct is_class : bool_constant<__is_class(Ty)> {};

template<class From, class To>
struct is_convertible : public bool_constant<detail::_Is_Convertible_Impl<From, To>::value> {};

template<class Ty>
struct is_const : false_type {};

template<class Ty>
struct is_const<const Ty> : true_type {};

template<class Ty>
struct is_volatile : false_type {};

template<class Ty>
struct is_volatile<volatile Ty> : true_type {};

template<class Ty>  // only function types and reference types can't be const qualified
struct is_function : bool_constant<!is_const_v<const Ty> && !is_reference_v<Ty>> {};

template<class Ty>  // only function types and reference types can't be const qualified
struct is_object : bool_constant<is_const_v<const Ty> && !is_void_v<Ty>> {};

template<class Ty>
struct is_member_object_pointer : detail::_Is_Member_Object_Pointer_Impl<remove_cv_t<Ty>> {};

template<class Ty>
struct is_member_function_pointer : detail::_Is_Member_Function_Pointer_Impl<remove_cv_t<Ty>> {};

template<class Ty>
struct is_member_pointer : bool_constant<is_member_object_pointer_v<Ty> || is_member_function_pointer_v<Ty>> {};

template<class Ty>
struct is_null_pointer : is_same<remove_cv_t<Ty>, std::nullptr_t> {};

template<class Ty>
struct is_scalar : bool_constant<is_arithmetic_v<Ty> || is_enum_v<Ty> || is_pointer_v<Ty> || is_member_pointer_v<Ty> || is_null_pointer_v<Ty>> {};

template<class Ty>
struct is_empty : bool_constant<__is_empty(Ty)> {};

template<class Ty>
struct is_polymorphic : bool_constant<__is_polymorphic(Ty)> {};

template<class Ty>
struct is_abstract : bool_constant<__is_abstract(Ty)> {};

template<class Ty>
struct is_final : bool_constant<__is_final(Ty)> {};

template<class Ty, class... Args>
struct is_constructible : bool_constant<__is_constructible(Ty, Args...)> {};

template<class Ty>
struct is_default_constructible : bool_constant<__is_constructible(Ty)> {};

template<class Ty, class AlwaysVoid>
struct is_implicitly_default_constructible : false_type {}; // determine whether Ty can be copy-initialized with {}

template<class Ty>
struct is_implicitly_default_constructible<Ty, void_t<decltype(detail::_Implicitly_Default_Construct<Ty>({}))>> : true_type {};

template<class Ty>
struct is_copy_constructible : bool_constant<__is_constructible(Ty, add_lvalue_reference_t<const Ty>)> {};

template<class Ty>
struct is_move_constructible : bool_constant<__is_constructible(Ty, Ty)> {};

template<class Ty, class... Args>
struct is_nothrow_constructible : bool_constant<__is_nothrow_constructible(Ty, Args...)> {};

template<class Ty>
struct is_nothrow_default_constructible : bool_constant<__is_nothrow_constructible(Ty)> {};

template<class Ty>
struct is_nothrow_copy_constructible : bool_constant<__is_nothrow_constructible(Ty, add_lvalue_reference_t<const Ty>)> {};

template<class Ty>
struct is_nothrow_move_constructible : bool_constant<__is_nothrow_constructible(Ty, Ty)> {};

template<class To, class From>
struct is_assignable : bool_constant<__is_assignable(To, From)> {};

template<class Ty>
struct is_copy_assignable : bool_constant<__is_assignable(add_lvalue_reference_t<Ty>, add_lvalue_reference_t<const Ty>)> {};

template<class Ty>
struct is_move_assignable : bool_constant<__is_assignable(add_lvalue_reference_t<Ty>, Ty)> {};

template<class To, class From>
struct is_nothrow_assignable : bool_constant<__is_nothrow_assignable(To, From)> {};

template<class Ty>
struct is_nothrow_copy_assignable : bool_constant<__is_nothrow_assignable(add_lvalue_reference_t<Ty>, add_lvalue_reference_t<const Ty>)> {};

template<class Ty>
struct is_nothrow_move_assignable : bool_constant<__is_nothrow_assignable(add_lvalue_reference_t<Ty>, Ty)> {};

template<class From, class To>
struct is_nothrow_convertible : detail::_Is_Nothrow_Convertible_Impl<From, To> {};

template<class Ty>
struct is_destructible : detail::_Is_Destructible_Impl<Ty>::type {};

template<class Ty>
struct is_nothrow_destructible : detail::_Is_Nothrow_Destructible_Impl<Ty>::type {};

template<class Ty1, class Ty2>
struct is_swappable_with : detail::_Is_Swappable_With_Impl<Ty1, Ty2>::type {};

template<class Ty1, class Ty2>
struct is_nothrow_swappable_with : bool_constant<conjunction_v<
                                                    detail::_Is_Swappable_With_Impl<Ty1, Ty2>,
                                                    detail::_Is_Nothrow_Swappable_With_Impl<Ty1, Ty2>>> {};

template<class Ty>
struct is_swappable : detail::_Is_Swappable_With_Impl<add_lvalue_reference_t<Ty>, add_lvalue_reference_t<Ty>>::type {};

template<class Ty>
struct is_nothrow_swappable : detail::_Is_Nothrow_Swappable_With_Impl<add_lvalue_reference_t<Ty>, add_lvalue_reference_t<Ty>>::type {};

template<class Base, class Derived>
struct is_base_of : bool_constant<__is_base_of(Base, Derived)> {};

template<class Type, template<class...> class Template>
struct is_specialization : false_type {};   // true if and only if Type is a specialization of Template

template<template<class...> class Template, class... Types>
struct is_specialization<Template<Types...>, Template> : true_type {};

template<class Ty>
struct is_signed : bool_constant<detail::_Sign_Choice<Ty>::_Signed> {};

template<class Ty>
struct is_unsigned : bool_constant<detail::_Sign_Choice<Ty>::_Unsigned> {};

template<class Ty>
struct is_nonbool_integral : bool_constant<is_integral_v<Ty> && !is_same_v<remove_cv_t<Ty>, bool>> {};

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
struct make_unsigned    // unsigned partner to Ty
{
    static_assert(is_nonbool_integral_v<Ty> || is_enum_v<Ty>,
                    "make_unsigned<Ty> requires that Ty shall be a (possibly cv-qualified) "
                    "integral type or enumeration but not a bool type.");

    // removes CV from Ty, makes choice of unsignment, reapplies CV
    using type = typename remove_cv<Ty>::template _Apply<detail::_Make_Unsigned>;
};

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

template<>
struct common_type<> {};    // 0 types

template<class Ty>
struct common_type<Ty> : common_type<Ty, Ty> {};    // 1 type

template<class Ty1, class Ty2>
struct common_type<Ty1, Ty2> : detail::_Common_Type2_Impl<Ty1, Ty2> {};  // 2 types

template<class Ty1, class Ty2, class... Rest>
struct common_type<Ty1, Ty2, Rest...> : common_type<common_type_t<Ty1, Ty2>, Rest...> {};   // 3+ types

#pragma endregion definitions

CUSTOM_END
