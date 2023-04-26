#pragma once
#include "Utility.h"

CUSTOM_BEGIN


template<class Ty>
class ReferenceWrapper;

// invoke ==========================================================================

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
    template <class Callable, class Refwrap, class... Args>
    static constexpr auto invoke_impl(Callable pmf, Refwrap rw, Args&&... args) noexcept
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
    template <class Callable, class Refwrap>
    static constexpr auto invoke_impl(Callable pmd, Refwrap rw) noexcept 
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

// Choose and call Invoke implementation
template<class Callable>
auto invoke(Callable&& func) noexcept
-> decltype(_Invoker<Callable>::invoke_impl(static_cast<Callable&&>(func))) {
    return _Invoker<Callable>::invoke_impl(static_cast<Callable&&>(func));
}

template<class Callable, class Type, class... Args>
auto invoke(Callable&& func, Type&& arg1, Args&&... args) noexcept
-> decltype(_Invoker<Callable, Type>::invoke_impl(static_cast<Callable&&>(func), static_cast<Type&&>(arg1), static_cast<Args&&>(args)...)) {
    return _Invoker<Callable, Type>::invoke_impl(static_cast<Callable&&>(func), static_cast<Type&&>(arg1), static_cast<Args&&>(args)...);
}

// END invoke ==========================================================================


// ReferenceWrapper ====================================================================

template<class Ty>
void _RefwrapConstructorFunc(TypeIdentity_t<Ty&>) noexcept; // not defined

template<class Ty>
void _RefwrapConstructorFunc(TypeIdentity_t<Ty&&>) = delete;

template<class Ty, class Base, class = void>
struct _RefwrapHasConstructorFrom : FalseType {};

template<class Ty, class Base>
struct _RefwrapHasConstructorFrom<Ty, Base, Void_t<decltype(_RefwrapConstructorFunc<Ty>(std::declval<Base>()))>>
: TrueType {}; // _RefwrapConstructorFunc is qualified: avoid ADL, handle incomplete types

template<class Ty>
class ReferenceWrapper      // ReferenceWrapper Template
{
public:
    static_assert(IsObject_v<Ty> || IsFunction_v<Ty>, "ReferenceWrapper<Ty> requires Ty to be an object type or a function type.");
    using Type = Ty;

private:
    Type* _Ptr;

public:
    // Constructors & Operators

    template <class Base, EnableIf_t<Conjunction_v<
    Negation<IsSame<RemoveCVRef_t<Base>, ReferenceWrapper>>, 
    _RefwrapHasConstructorFrom<Ty, Base>>, bool> = true>
    ReferenceWrapper(Base&& val) noexcept {
        _Ptr = &static_cast<Base&&>(val);
    }

    template<class... Types>
    auto operator()(Types&&... args) const
    -> decltype(custom::invoke(*_Ptr, static_cast<Types&&>(args)...)) {
        return custom::invoke(*_Ptr, static_cast<Types&&>(args)...);
    }

    operator Type& () const noexcept {
        return *_Ptr;
    }

public:
    // Main functions

    Type& get() const noexcept {
        return *_Ptr;
    }
}; // END ReferenceWrapper

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

// END ReferenceWrapper ================================================================

// Function ============================================================================

// TODO: refactor
template<class Signature>
class Function;

template<class RetType, class... ArgTypes>
class Function<RetType(ArgTypes...)>        // Wrapper template to function pointer
{
private:
    using Invoker           = RetType(*)(const Function&, ArgTypes&&...);
    using Copiator          = void*(*)(const Function&);
    using Destructor        = void(*)(const Function&);

private:
    void* _functor          = nullptr;
    Invoker _invoker        = nullptr;
    Copiator _copiator      = nullptr;
    Destructor _destructor  = nullptr;

private:
    // Core functions

    template<class Functor>
    static RetType _invoke_impl(const Function& target, ArgTypes&&... args) {
        return (*reinterpret_cast<Functor*>(target._functor))(custom::forward<ArgTypes>(args)...);
    }

    template<class Functor>
    static void* _copy_impl(const Function& target) {
        return reinterpret_cast<void*>(new Functor(*reinterpret_cast<Functor*>(target._functor)));
    }

    template<class Functor>
    static void _destruct_impl(const Function& target) {
        delete reinterpret_cast<Functor*>(target._functor);
    }

public:
    // Constructors

    Function() = default;

    template<class Functor, EnableIf_t<!IsSame<Decay_t<Functor>, Function>::Value, bool> = true>
    Function(Functor&& f) {
        using FunctorType = Decay_t<Functor>;

        _invoker    = reinterpret_cast<Invoker>(_invoke_impl<FunctorType>);
        _copiator   = reinterpret_cast<Copiator>(_copy_impl<FunctorType>);
        _destructor = reinterpret_cast<Destructor>(_destruct_impl<FunctorType>);
        _functor    = reinterpret_cast<void*>(new FunctorType(custom::forward<Functor>(f)));
    }

    Function(const Function& other) {
        _copy(other);
    }

    Function(Function&& other) noexcept {
        _move(custom::move(other));
    }

    ~Function() {
        _clear();
    }
    
public:
    // Operators

    RetType operator()(ArgTypes&&... args) const {
        if (_functor == nullptr) 
            throw std::runtime_error("call an empty function object");
        
        return _invoker(*this, custom::forward<ArgTypes>(args)...);
    }

    template<class Functor, EnableIf_t<!IsSame<Decay_t<Functor>, Function>::Value, bool> = true>
    Function& operator=(Functor&& func) {
        using FunctorType = Decay_t<Functor>;

        _clear();
        _invoker    = reinterpret_cast<Invoker>(_invoke_impl<FunctorType>);
        _copiator   = reinterpret_cast<Copiator>(_copy_impl<FunctorType>);
        _destructor = reinterpret_cast<Destructor>(_destruct_impl<FunctorType>);
        _functor    = reinterpret_cast<void*>(new FunctorType(custom::forward<Functor>(func)));

        return *this;
    }

    Function& operator=(const Function& other) {
        if (_functor != other._functor)
        {
            _clear();
            _copy(other);
        }

        return *this;
    }

    Function& operator=(Function&& other) {
        if (_functor != other._functor)
        {
            _clear();
            _move(custom::move(other));
        }

        return *this;
    }

    explicit operator bool() const noexcept {
        if (_functor != nullptr)
            return true;

        return false;
    }

private:
    // Helpers

    void _clear() {
        if (_functor != nullptr)
        {
            _destructor(*this);
            _functor = nullptr;
        }
    }

    void _copy(const Function& other) {
        _invoker        = other._invoker;
        _copiator       = other._copiator;
        _destructor     = other._destructor;
        _functor        = other._copiator(other);
    }

    void _move(Function&& other) {
        _invoker        = other._invoker;
        _copiator       = other._copiator;
        _destructor     = other._destructor;
        _functor        = other._functor;

        other._functor  = nullptr;
    }
}; // END Function template

// END Function ========================================================================


CUSTOM_END