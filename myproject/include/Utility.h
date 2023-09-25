#pragma once
#include "xUtility.h"


CUSTOM_BEGIN

template<class Ty>
class ReferenceWrapper;

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

template<class Ty,
EnableIf_t<IsNothrowMoveConstructible_v<Ty> && IsNothrowMoveAssignable_v<Ty> , bool> = true>
constexpr void swap(Ty& first, Ty& second) noexcept {
    Ty temp = custom::move(first);
    first   = custom::move(second);
    second  = custom::move(temp);
}

template<class Ty, class Other = Ty,
EnableIf_t<IsNothrowMoveConstructible_v<Ty> && IsNothrowAssignable_v<Ty&, Other>, bool> = true>
constexpr Ty exchange(Ty& val, Other&& newVal) noexcept {
    // assign newVal to val, return previous val
    Ty old  = custom::move(val);
    val     = custom::move(newVal);
    return old;
}

template<class Iterator>
class ReverseIterator                           // Adaptor for backwards iteration
{
public:
    // TODO: use iterator_traits
    using IteratorType  = Iterator;
    using ValueType     = typename Iterator::ValueType;
    using Reference		= typename Iterator::Reference;
	using Pointer		= typename Iterator::Pointer;

    IteratorType Current;

private:
    // helpers for ReverseIterator operator->()
    template<bool = IsPointer_v<RemoveCVRef_t<Iterator>>>
    constexpr static bool _HasNothrowOperatorArrow = IsNothrowConvertible_v<Iterator, Pointer>;

    template<>
    constexpr static bool _HasNothrowOperatorArrow<false> =
    noexcept(_fake_copy_init<Pointer>(custom::declval<Iterator>().operator->()));

public:

    constexpr ReverseIterator()                             = default;
    constexpr ReverseIterator(const ReverseIterator& other) = default;

    constexpr explicit ReverseIterator(IteratorType base)
    noexcept(IsNothrowCopyConstructible_v<IteratorType>)
        : Current(base) { /*Empty*/ }

    constexpr ReverseIterator& operator=(const ReverseIterator& other) = default;

    constexpr ReverseIterator& operator++() noexcept(noexcept(--Current)) {
        --Current;
        return *this;
    }

    constexpr ReverseIterator operator++(int)
    noexcept(IsNothrowCopyConstructible_v<IteratorType> && noexcept(--Current)) {
        ReverseIterator temp = *this;
        --Current;
        return temp;
    }

    constexpr ReverseIterator& operator--() noexcept(noexcept(++Current)) {
        ++Current;
        return *this;
    }

    constexpr ReverseIterator operator--(int)
    noexcept(IsNothrowCopyConstructible_v<IteratorType> && noexcept(++Current)) {
        ReverseIterator temp = *this;
        ++Current;
        return temp;
    }

    constexpr Pointer operator->() const
    noexcept(   IsNothrowCopyConstructible_v<IteratorType> &&
                noexcept(--(custom::declval<IteratorType&>())) &&
                _HasNothrowOperatorArrow<>) {

        Iterator temp = Current;
        --temp;

        if constexpr (IsPointer_v<IteratorType>)
            return temp;

        return temp.operator->();
    }

    constexpr Reference operator*() const
    noexcept(IsNothrowCopyConstructible_v<IteratorType> && noexcept(*--(custom::declval<IteratorType&>()))) {
        Iterator temp = Current;
        return *(--temp);
    }

    constexpr IteratorType base() const
    noexcept(IsNothrowCopyConstructible_v<IteratorType>) {
        return Current;
    }

    constexpr const IteratorType& const_ref_base() const noexcept {
        return Current;
    }
}; // END ReverseIterator


// ReverseIterator binary operators
template<class Iterator1, class Iterator2>
constexpr bool operator==(const ReverseIterator<Iterator1>& left, const ReverseIterator<Iterator2>& right)
noexcept(noexcept(_fake_copy_init<bool>(left.const_ref_base() == right.const_ref_base()))) {
    return left.const_ref_base() == right.const_ref_base();
}

template<class Iterator1, class Iterator2>
constexpr bool operator!=(const ReverseIterator<Iterator1>& left, const ReverseIterator<Iterator2>& right)
noexcept(noexcept(_fake_copy_init<bool>(left.const_ref_base() != right.const_ref_base()))) {
    return left.const_ref_base() != right.const_ref_base();
}


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

// invoke
template<class Callable>
constexpr auto invoke(Callable&& func) noexcept
-> decltype(_Invoker<Callable>::invoke_impl(static_cast<Callable&&>(func))) {
    return _Invoker<Callable>::invoke_impl(static_cast<Callable&&>(func));
}

template<class Callable, class Type, class... Args>
constexpr auto invoke(Callable&& func, Type&& arg1, Args&&... args) noexcept
-> decltype(_Invoker<Callable, Type>::invoke_impl(static_cast<Callable&&>(func), static_cast<Type&&>(arg1), static_cast<Args&&>(args)...)) {
    return _Invoker<Callable, Type>::invoke_impl(static_cast<Callable&&>(func), static_cast<Type&&>(arg1), static_cast<Args&&>(args)...);
}

// ReferenceWrapper
template<class Ty>
void _RefwrapConstructorFunc(TypeIdentity_t<Ty&>) noexcept; // not defined

template<class Ty>
void _RefwrapConstructorFunc(TypeIdentity_t<Ty&&>) = delete;

template<class Ty, class Base, class = void>
struct _RefwrapHasConstructorFrom : FalseType {};

template<class Ty, class Base>
struct _RefwrapHasConstructorFrom<Ty, Base, Void_t<decltype(_RefwrapConstructorFunc<Ty>(custom::declval<Base>()))>>
: TrueType {}; // _RefwrapConstructorFunc is qualified: avoid ADL, handle incomplete types

template<class Ty>
class ReferenceWrapper      // ReferenceWrapper Template
{
public:
    static_assert(IsObject_v<Ty> || IsFunction_v<Ty>, "ReferenceWrapper<Ty> requires Ty to be an object type or a function type.");
    using Type = Ty;

private:
    Type* _ptr;

public:
    // Constructors & Operators

    template<class Base,
    EnableIf_t<Conjunction_v<
                    Negation<IsSame<RemoveCVRef_t<Base>, ReferenceWrapper>>, 
                    _RefwrapHasConstructorFrom<Ty, Base>>, bool> = true>
    ReferenceWrapper(Base&& val) noexcept {
        _ptr = &static_cast<Base&&>(val);       // TODO: check
    }

    template<class... Args>
    auto operator()(Args&&... args) const
    -> decltype(custom::invoke(*_ptr, static_cast<Args&&>(args)...)) {
        return custom::invoke(*_ptr, static_cast<Args&&>(args)...);
    }

    operator Type& () const noexcept {
        return *_ptr;
    }

public:
    // Main functions

    Type& get() const noexcept {
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

CUSTOM_END