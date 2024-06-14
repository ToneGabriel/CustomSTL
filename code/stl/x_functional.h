#pragma once
#include "c_type_traits.h"


CUSTOM_BEGIN

template<class Ty>
class reference_wrapper;


#pragma region invoke

CUSTOM_DETAIL_BEGIN

// All invoke implmentations
struct _InvokerFunctor
{
    template<class Callable, class... Args>
    static constexpr auto invoke_impl(Callable&& func, Args&&... args) noexcept
    -> decltype(static_cast<Callable&&>(func)(static_cast<Args&&>(args)...)) {
        return static_cast<Callable&&>(func)(static_cast<Args&&>(args)...);
    }
};

struct _InvokerPMFObject
{
    template<class Callable, class Type, class... Args>
    static constexpr auto invoke_impl(Callable&& pmf, Type&& typeObj, Args&&... args) noexcept
    -> decltype((static_cast<Type&&>(typeObj).*pmf)(static_cast<Args&&>(args)...)) {
        return (static_cast<Type&&>(typeObj).*pmf)(static_cast<Args&&>(args)...);
    }
};

struct _InvokerPMFRefwrap
{
    template<class Callable, class Refwrap, class... Args>
    static constexpr auto invoke_impl(Callable&& pmf, Refwrap&& rw, Args&&... args) noexcept
    -> decltype((rw.get().*pmf)(static_cast<Args&&>(args)...)) {
        return (rw.get().*pmf)(static_cast<Args&&>(args)...);
    }
};

struct _InvokerPMFPointer
{
    template<class Callable, class Type, class... Args>
    static constexpr auto invoke_impl(Callable&& pmf, Type&& typeObj, Args&&... args) noexcept
    -> decltype(((*static_cast<Type&&>(typeObj)).*pmf)(static_cast<Args&&>(args)...)) {
        return ((*static_cast<Type&&>(typeObj)).*pmf)(static_cast<Args&&>(args)...);
    }
};

struct _InvokerPMDObject
{
    template<class Callable, class Type>
    static constexpr auto invoke_impl(Callable&& pmd, Type&& typeObj) noexcept
    -> decltype(static_cast<Type&&>(typeObj).*pmd) {
        return static_cast<Type&&>(typeObj).*pmd;
    }
};

struct _InvokerPMDRefwrap
{
    template<class Callable, class Refwrap>
    static constexpr auto invoke_impl(Callable&& pmd, Refwrap&& rw) noexcept 
    -> decltype(rw.get().*pmd) {
        return rw.get().*pmd;
    }
};

struct _InvokerPMDPointer
{
    template<class Callable, class Type>
    static constexpr auto invoke_impl(Callable&& pmd, Type&& typeObj) noexcept
    -> decltype((*static_cast<Type&&>(typeObj)).*pmd) {
        return (*static_cast<Type&&>(typeObj)).*pmd;
    }
};

// Invoker implementation choice
template<class Callable, class Type = void, class NoCVRef_t = remove_cv_ref_t<Callable>,
bool IsPMF = is_member_function_pointer_v<NoCVRef_t>,
bool IsPMD = is_member_object_pointer_v<NoCVRef_t>>
struct _Invoker;

template<class Callable, class Type, class NoCVRef_t>
struct _Invoker<Callable, Type, NoCVRef_t, false, false>    // non-member function
: _InvokerFunctor {};

template<class Callable, class Type, class NoCVRef_t>
struct _Invoker<Callable, Type, NoCVRef_t, true, false>     // pointer to member function
: conditional_t<is_base_of_v<typename _IsMemberFunctionPointer<NoCVRef_t>::_Class_Type, remove_reference_t<Type>>,
_InvokerPMFObject,
conditional_t<is_specialization_v<remove_cv_ref_t<Type>, reference_wrapper>, _InvokerPMFRefwrap, _InvokerPMFPointer>> {};

template<class Callable, class Type, class NoCVRef_t>
struct _Invoker<Callable, Type, NoCVRef_t, false, true>     // pointer to member data
: conditional_t<is_base_of_v<typename _IsMemberObjectPointer<NoCVRef_t>::_Class_Type, remove_reference_t<Type>>,
_InvokerPMDObject,
conditional_t<is_specialization_v<remove_cv_ref_t<Type>, reference_wrapper>, _InvokerPMDRefwrap, _InvokerPMDPointer>> {};

CUSTOM_DETAIL_END


// invoke
template<class Callable>
constexpr auto invoke(Callable&& func) noexcept
-> decltype(detail::_Invoker<Callable>::invoke_impl(static_cast<Callable&&>(func))) {
    return detail::_Invoker<Callable>::invoke_impl(static_cast<Callable&&>(func));
}

template<class Callable, class Type, class... Args>
constexpr auto invoke(Callable&& func, Type&& arg1, Args&&... args) noexcept
-> decltype(detail::_Invoker<Callable, Type>::invoke_impl(static_cast<Callable&&>(func), static_cast<Type&&>(arg1), static_cast<Args&&>(args)...)) {
    return detail::_Invoker<Callable, Type>::invoke_impl(static_cast<Callable&&>(func), static_cast<Type&&>(arg1), static_cast<Args&&>(args)...);
}

// is_invocable
template<class Callable, class AlwaysVoid, class... Args>
struct _IsInvocableImpl : custom::false_type {};

template<class Callable, class... Args>
struct _IsInvocableImpl<Callable,
custom::void_t<decltype(custom::declval<Callable>()(custom::declval<Args>()...))>, Args...> : custom::true_type {};

template<class Callable, class... Args>
struct is_invocable : _IsInvocableImpl<Callable, /*AlwaysVoid*/ void, Args...> {};

template<class Callable, class... Args>
constexpr bool is_invocable_v = is_invocable<Callable, Args...>::Value;

// is_invocable_ret
template<class Ret, class Callable, class AlwaysVoid, class... Args>
struct _IsInvocableRetImpl : custom::false_type {};

template<class Ret, class Callable, class... Args>
struct _IsInvocableRetImpl<Ret, Callable,
custom::void_t<decltype(custom::declval<Callable>()(custom::declval<Args>()...))>, Args...> :
custom::is_convertible<decltype(custom::declval<Callable>()(custom::declval<Args>()...)), Ret> {};

template<class Ret, class Callable, class... Args>
struct is_invocable_ret : _IsInvocableRetImpl<Ret, Callable, /*AlwaysVoid*/ void, Args...> {};

template<class Ret, class Callable, class... Args>
constexpr bool is_invocable_ret_v = is_invocable_ret<Ret, Callable, Args...>::Value;

// is_nothrow_invocable
template<class Callable, class AlwaysVoid, class... Args>
struct _IsNothrowInvocableImpl : custom::false_type {};

template<class Callable, class... Args>
struct _IsNothrowInvocableImpl<Callable,
custom::void_t<decltype(custom::declval<Callable>()(custom::declval<Args>()...))>, Args...> : 
custom::bool_constant<noexcept(custom::declval<Callable>()(custom::declval<Args>()...))> {};

template<class Callable, class... Args>
struct is_nothrow_invocable : _IsNothrowInvocableImpl<Callable, /*AlwaysVoid*/ void, Args...> {};

template<class Callable, class... Args>
constexpr bool is_nothrow_invocable_v = is_nothrow_invocable<Callable, Args...>::Value;

// is_nothrow_invocable_ret
template<class Ret, class Callable, class AlwaysVoid, class... Args>
struct _IsNothrowInvocableRetImpl : custom::false_type {};

template<class Ret, class Callable, class... Args>
struct _IsNothrowInvocableRetImpl<Ret, Callable,
custom::void_t<decltype(custom::declval<Callable>()(custom::declval<Args>()...))>, Args...> :
custom::conjunction<custom::is_convertible<decltype(custom::declval<Callable>()(custom::declval<Args>()...)), Ret>,
                    custom::bool_constant<noexcept(custom::declval<Callable>()(custom::declval<Args>()...))>> {};

template<class Ret, class Callable, class... Args>
struct is_nothrow_invocable_ret : _IsNothrowInvocableRetImpl<Ret, Callable, /*AlwaysVoid*/ void, Args...> {};

template<class Ret, class Callable, class... Args>
constexpr bool is_nothrow_invocable_ret_v = is_nothrow_invocable_ret<Ret, Callable, Args...>::Value;

#pragma endregion invoke


#pragma region ReferenceWrapper

// reference_wrapper
template<class Ty>
void _RefwrapConstructorFunc(type_identity_t<Ty&>) noexcept; // not defined

template<class Ty>
void _RefwrapConstructorFunc(type_identity_t<Ty&&>) = delete;

template<class Ty, class Base, class = void>
struct _RefwrapHasConstructorFrom : false_type {};

template<class Ty, class Base>
struct _RefwrapHasConstructorFrom<Ty, Base,
void_t<decltype(_RefwrapConstructorFunc<Ty>(custom::declval<Base>()))>> : true_type {};

template<class Ty>
class reference_wrapper      // reference_wrapper Template
{
public:
    static_assert(is_object_v<Ty> || is_function_v<Ty>,
                    "reference_wrapper<Ty> requires Ty to be an object type or a function type.");
    
    using type = Ty;

private:
    type* _ptr = nullptr;

public:
    // Constructors & Operators

    template<class Base,
    enable_if_t<conjunction_v<
                    negation<is_same<remove_cv_ref_t<Base>, reference_wrapper>>, 
                    _RefwrapHasConstructorFrom<type, Base>>, bool> = true>
    constexpr reference_wrapper(Base&& val)
    noexcept(noexcept(_RefwrapConstructorFunc<type>(custom::declval<Base>()))) {
        _ptr = &static_cast<Base&&>(val);
    }

    template<class... Args>
    constexpr auto operator()(Args&&... args) const
    noexcept(noexcept(custom::invoke(*_ptr, static_cast<Args&&>(args)...)))
    -> decltype(custom::invoke(*_ptr, static_cast<Args&&>(args)...)) {
        return custom::invoke(*_ptr, static_cast<Args&&>(args)...);
    }

    constexpr operator type& () const noexcept {
        return *_ptr;
    }

public:
    // Main functions

    constexpr type& get() const noexcept {
        return *_ptr;
    }
}; // END reference_wrapper

// reference_wrapper builders
template<class Ty>
reference_wrapper(Ty&) -> reference_wrapper<Ty>;  // deduce type

template<class Ty>
reference_wrapper<Ty> ref(Ty& val) noexcept {
    return reference_wrapper<Ty>(val);
}

template<class Ty>
void ref(const Ty&&) = delete;

template<class Ty>
reference_wrapper<Ty> ref(reference_wrapper<Ty> val) noexcept {
    return val;
}

template<class Ty>
reference_wrapper<const Ty> cref(const Ty& val) noexcept {
    return reference_wrapper<const Ty>(val);
}

template<class Ty>
void cref(const Ty&&) = delete;

template<class Ty>
reference_wrapper<const Ty> cref(reference_wrapper<Ty> val) noexcept {
    return val;
}

// decay, then unwrap a reference_wrapper
template<class Ty>
struct unrefwrap { using type = Ty; };

template<class Ty>
struct unrefwrap<reference_wrapper<Ty>> { using type = Ty&; };

template<class Ty>
using unrefwrap_t = typename unrefwrap<decay_t<Ty>>::type;

#pragma endregion ReferenceWrapper


#pragma region Hash

CUSTOM_DETAIL_BEGIN     // hash representation

#ifdef _WIN64
constexpr size_t _FNVOffsetBasis    = 14695981039346656037ULL;
constexpr size_t _FNVPrime          = 1099511628211ULL;
#else
constexpr size_t _FNVOffsetBasis    = 2166136261U;
constexpr size_t _FNVPrime          = 16777619U;
#endif // _WIN64

// accumulate range [first, first + count) into partial FNV-1a hash val
inline size_t _fnv1a_append_bytes(  size_t val,
                                    const unsigned char* const first,
                                    const size_t count) noexcept {

    for (size_t i = 0; i < count; ++i)
    {
        val ^= static_cast<size_t>(first[i]);
        val *= _FNVPrime;
    }

    return val;
}

// accumulate range [first, last) into partial FNV-1a hash val
template<class Ty>
size_t _fnv1a_append_range( const size_t val,
                            const Ty* const first,
                            const Ty* const last) noexcept {

    static_assert(is_trivial_v<Ty>, "Only trivial types can be directly hashed.");

    const auto firstAsUChar = reinterpret_cast<const unsigned char*>(first);
    const auto lastAsUChar  = reinterpret_cast<const unsigned char*>(last);

    return _fnv1a_append_bytes(val, firstAsUChar, static_cast<size_t>(lastAsUChar - firstAsUChar));
}

// accumulate key into partial FNV-1a hash val
template<class Key>
size_t _fnv1a_append_value( const size_t val,
                            const Key& key) noexcept {

    static_assert(is_trivial_v<Key>, "Only trivial types can be directly hashed.");
    return _fnv1a_append_bytes(val, &reinterpret_cast<const unsigned char&>(key), sizeof(Key));
}

// bitwise hashes the representation of a key
template<class Key>
size_t _hash_representation(const Key& key) noexcept {
    return _fnv1a_append_value(_FNVOffsetBasis, key);
}

// bitwise hashes the representation of an array
template<class Key>
size_t _hash_array_representation(  const Key* const first,
                                    const size_t count) noexcept {

    static_assert(is_trivial_v<Key>, "Only trivial types can be directly hashed.");
    return _fnv1a_append_bytes(_FNVOffsetBasis, reinterpret_cast<const unsigned char*>(first), count * sizeof(Key));
}

CUSTOM_DETAIL_END   // hash representation


template<class Key>
struct hash;

template<class Key, bool Enabled>
struct _BaseHashEnabler  // conditionally enabled hash base
{
    size_t operator()(const Key& key) const
    noexcept(noexcept(hash<Key>::compute_hash(key))) {
        return hash<Key>::compute_hash(key);
    }
};

template<class Key>
struct _BaseHashEnabler<Key, false>  // conditionally disabled hash base
{
    _BaseHashEnabler()                                      = delete;
    _BaseHashEnabler(const _BaseHashEnabler&)               = delete;
    _BaseHashEnabler(_BaseHashEnabler&&)                    = delete;
    _BaseHashEnabler& operator=(const _BaseHashEnabler&)    = delete;
    _BaseHashEnabler& operator=(_BaseHashEnabler&&)         = delete;
};

template<class Key>
struct hash : _BaseHashEnabler<Key, (!is_const_v<Key> &&
                                     !is_volatile_v<Key> &&
                                     (is_enum_v<Key> || is_integral_v<Key> || is_pointer_v<Key>))>
{
    static size_t compute_hash(const Key& key) noexcept {
        return detail::_hash_representation(key);
    }
};  // END hash


// hash specializations
template<>
struct hash<float>
{
    size_t operator()(const float key) const noexcept {
        return detail::_hash_representation(key == 0.0F ? 0.0F : key);  // map -0 to 0
    }
};

template<>
struct hash<double>
{
    size_t operator()(const double key) const noexcept {
        return detail::_hash_representation(key == 0.0 ? 0.0 : key);    // map -0 to 0
    }
};

template<>
struct hash<long double>
{
    size_t operator()(const long double key) const noexcept {
        return detail::_hash_representation(key == 0.0L ? 0.0L : key); // map -0 to 0
    }
};

template<>
struct hash<std::nullptr_t>
{
    size_t operator()(std::nullptr_t) const noexcept {
        void* voidPtr{};
        return detail::_hash_representation(voidPtr);
    }
};

#pragma endregion Hash

CUSTOM_END