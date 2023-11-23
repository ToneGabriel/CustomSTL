#pragma once
#include "xUtility.h"


CUSTOM_BEGIN

template<class Ty>
class ReferenceWrapper;


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
template<class Callable, class Type = void, class NoCVRef_t = RemoveCVRef_t<Callable>,
bool IsPMF = IsMemberFunctionPointer_v<NoCVRef_t>,
bool IsPMD = IsMemberObjectPointer_v<NoCVRef_t>>
struct _Invoker;

template<class Callable, class Type, class NoCVRef_t>
struct _Invoker<Callable, Type, NoCVRef_t, false, false>    // non-member function
: _InvokerFunctor {};

template<class Callable, class Type, class NoCVRef_t>
struct _Invoker<Callable, Type, NoCVRef_t, true, false>     // pointer to member function
: Conditional_t<IsBaseOf_v<typename _IsMemberFunctionPointer<NoCVRef_t>::ClassType, RemoveReference_t<Type>>,
_InvokerPMFObject,
Conditional_t<IsSpecialization_v<RemoveCVRef_t<Type>, ReferenceWrapper>, _InvokerPMFRefwrap, _InvokerPMFPointer>> {};

template<class Callable, class Type, class NoCVRef_t>
struct _Invoker<Callable, Type, NoCVRef_t, false, true>     // pointer to member data
: Conditional_t<IsBaseOf_v<typename _IsMemberObjectPointer<NoCVRef_t>::ClassType, RemoveReference_t<Type>>,
_InvokerPMDObject,
Conditional_t<IsSpecialization_v<RemoveCVRef_t<Type>, ReferenceWrapper>, _InvokerPMDRefwrap, _InvokerPMDPointer>> {};

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

#pragma endregion invoke


#pragma region ReferenceWrapper

// ReferenceWrapper
template<class Ty>
void _RefwrapConstructorFunc(TypeIdentity_t<Ty&>) noexcept; // not defined

template<class Ty>
void _RefwrapConstructorFunc(TypeIdentity_t<Ty&&>) = delete;

template<class Ty, class Base, class = void>
struct _RefwrapHasConstructorFrom : FalseType {};

template<class Ty, class Base>
struct _RefwrapHasConstructorFrom<Ty, Base,
Void_t<decltype(_RefwrapConstructorFunc<Ty>(custom::declval<Base>()))>> : TrueType {};

template<class Ty>
class ReferenceWrapper      // ReferenceWrapper Template
{
public:
    static_assert(IsObject_v<Ty> || IsFunction_v<Ty>,
                    "ReferenceWrapper<Ty> requires Ty to be an object type or a function type.");
    
    using Type = Ty;

private:
    Type* _ptr = nullptr;

public:
    // Constructors & Operators

    template<class Base,
    EnableIf_t<Conjunction_v<
                    Negation<IsSame<RemoveCVRef_t<Base>, ReferenceWrapper>>, 
                    _RefwrapHasConstructorFrom<Type, Base>>, bool> = true>
    constexpr ReferenceWrapper(Base&& val)
    noexcept(noexcept(_RefwrapConstructorFunc<Type>(custom::declval<Base>()))) {
        _ptr = &static_cast<Base&&>(val);
    }

    template<class... Args>
    constexpr auto operator()(Args&&... args) const
    noexcept(noexcept(custom::invoke(*_ptr, static_cast<Args&&>(args)...)))
    -> decltype(custom::invoke(*_ptr, static_cast<Args&&>(args)...)) {
        return custom::invoke(*_ptr, static_cast<Args&&>(args)...);
    }

    constexpr operator Type& () const noexcept {
        return *_ptr;
    }

public:
    // Main functions

    constexpr Type& get() const noexcept {
        return *_ptr;
    }
}; // END ReferenceWrapper

// ReferenceWrapper builders
template<class Ty>
ReferenceWrapper(Ty&) -> ReferenceWrapper<Ty>;  // deduce type

template<class Ty>
ReferenceWrapper<Ty> ref(Ty& val) noexcept {
    return ReferenceWrapper<Ty>(val);
}

template<class Ty>
void ref(const Ty&&) = delete;

template<class Ty>
ReferenceWrapper<Ty> ref(ReferenceWrapper<Ty> val) noexcept {
    return val;
}

template<class Ty>
ReferenceWrapper<const Ty> cref(const Ty& val) noexcept {
    return ReferenceWrapper<const Ty>(val);
}

template<class Ty>
void cref(const Ty&&) = delete;

template<class Ty>
ReferenceWrapper<const Ty> cref(ReferenceWrapper<Ty> val) noexcept {
    return val;
}

// decay, then unwrap a ReferenceWrapper
template<class Ty>
struct UnRefWrap { using Type = Ty; };

template<class Ty>
struct UnRefWrap<ReferenceWrapper<Ty>> { using Type = Ty&; };

template<class Ty>
using UnRefWrap_t = typename UnRefWrap<Decay_t<Ty>>::Type;

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

    static_assert(IsTrivial_v<Ty>, "Only trivial types can be directly hashed.");

    const auto firstAsUChar = reinterpret_cast<const unsigned char*>(first);
    const auto lastAsUChar  = reinterpret_cast<const unsigned char*>(last);

    return _fnv1a_append_bytes(val, firstAsUChar, static_cast<size_t>(lastAsUChar - firstAsUChar));
}

// accumulate key into partial FNV-1a hash val
template<class Key>
size_t _fnv1a_append_value( const size_t val,
                            const Key& key) noexcept {

    static_assert(IsTrivial_v<Key>, "Only trivial types can be directly hashed.");
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

    static_assert(IsTrivial_v<Key>, "Only trivial types can be directly hashed.");
    return _fnv1a_append_bytes(_FNVOffsetBasis, reinterpret_cast<const unsigned char*>(first), count * sizeof(Key));
}

CUSTOM_DETAIL_END   // hash representation


template<class Key>
struct Hash;

template<class Key, bool Enabled>
struct _BaseHashEnabler  // conditionally enabled Hash base
{
    size_t operator()(const Key& key) const
    noexcept(noexcept(Hash<Key>::compute_hash(key))) {
        return Hash<Key>::compute_hash(key);
    }
};

template<class Key>
struct _BaseHashEnabler<Key, false>  // conditionally disabled Hash base
{
    _BaseHashEnabler()                                      = delete;
    _BaseHashEnabler(const _BaseHashEnabler&)               = delete;
    _BaseHashEnabler(_BaseHashEnabler&&)                    = delete;
    _BaseHashEnabler& operator=(const _BaseHashEnabler&)    = delete;
    _BaseHashEnabler& operator=(_BaseHashEnabler&&)         = delete;
};

template<class Key>
struct Hash : _BaseHashEnabler<Key, (!IsConst_v<Key> &&
                                     !IsVolatile_v<Key> &&
                                     (IsEnum_v<Key> || IsIntegral_v<Key> || IsPointer_v<Key>))>
{
    static size_t compute_hash(const Key& key) noexcept {
        return detail::_hash_representation(key);
    }
};  // END Hash


// Hash specializations
template<>
struct Hash<float>
{
    size_t operator()(const float key) const noexcept {
        return detail::_hash_representation(key == 0.0F ? 0.0F : key);  // map -0 to 0
    }
};

template<>
struct Hash<double>
{
    size_t operator()(const double key) const noexcept {
        return detail::_hash_representation(key == 0.0 ? 0.0 : key);    // map -0 to 0
    }
};

template<>
struct Hash<long double>
{
    size_t operator()(const long double key) const noexcept {
        return detail::_hash_representation(key == 0.0L ? 0.0L : key); // map -0 to 0
    }
};

template<>
struct Hash<std::nullptr_t>
{
    size_t operator()(std::nullptr_t) const noexcept {
        void* voidPtr{};
        return detail::_hash_representation(voidPtr);
    }
};

#pragma endregion Hash

CUSTOM_END