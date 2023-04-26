#pragma once
#include "Utility.h"

CUSTOM_BEGIN


template<class Ty>
class ReferenceWrapper;

// template <class _Ty>
// void _Refwrap_ctor_fun(_Identity_t<_Ty&>) noexcept; // not defined
// template <class _Ty>
// void _Refwrap_ctor_fun(_Identity_t<_Ty&&>) = delete;

// template <class _Ty, class _Uty, class = void>
// struct _Refwrap_has_ctor_from : false_type {};

// template <class _Ty, class _Uty>
// struct _Refwrap_has_ctor_from<_Ty, _Uty, void_t<decltype(_STD _Refwrap_ctor_fun<_Ty>(_STD declval<_Uty>()))>>
//     : true_type {}; // _STD _Refwrap_ctor_fun is qualified: avoid ADL, handle incomplete types

// All invoke implmentations
struct _InvokerFunctor
{
    template<class Callable, class... Args>
    static constexpr auto invoke_impl(Callable&& func, Args&&... _Args) noexcept
    -> decltype(static_cast<Callable&&>(func)(static_cast<Args&&>(_Args)...)) {
        return static_cast<Callable&&>(func)(static_cast<Args&&>(_Args)...);
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

struct _InvokerPMFRefwrap   // not implemented
{
    // template <class Callable, class _Refwrap, class... Args>
    // static constexpr auto invoke_impl(Callable pmf, _Refwrap _Rw, Args&&... args) noexcept(
    //     noexcept((_Rw.get().*pmf)(static_cast<Args&&>(args)...)))
    //     -> decltype((_Rw.get().*pmf)(static_cast<Args&&>(args)...)) {
    //     return (_Rw.get().*pmf)(static_cast<Args&&>(args)...);
    // }
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

struct _InvokerPMDRefwrap   // not implemented
{
    // template <class Callable, class _Refwrap>
    // static constexpr auto invoke_impl(Callable pmd, _Refwrap _Rw) noexcept -> decltype(_Rw.get().*pmd) {
    //     return _Rw.get().*pmd;
    // }
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

// TODO: implement
template<class Ty>
class ReferenceWrapper
{
public:
    static_assert(IsObject_v<Ty> || IsFunction_v<Ty>, "ReferenceWrapper<Ty> requires Ty to be an object type or a function type.");

    using Type = Ty;

    template <class Base, EnableIf_t<Conjunction_v<
    Negation<IsSame<RemoveCVRef_t<Base>, ReferenceWrapper>>, 
    _Refwrap_has_ctor_from<Ty, Base>>, bool> = true>
    ReferenceWrapper(Base&& val) noexcept(
        noexcept(_STD _Refwrap_ctor_fun<Ty>(_STD declval<Base>()))) { // qualified: avoid ADL, handle incomplete types
        _Ptr = &static_cast<Base&&>(val);
    }

    operator Type& () const noexcept {
        return *_Ptr;
    }

    Type& get() const noexcept {
        return *_Ptr;
    }

private:
    Type* _Ptr;

public:
    template<class... Types>
    auto operator()(Types&&... _Args) const
        noexcept(noexcept(_STD invoke(*_Ptr, static_cast<Types&&>(_Args)...))) // strengthened
        -> decltype(_STD invoke(*_Ptr, static_cast<Types&&>(_Args)...)) {
        return _STD invoke(*_Ptr, static_cast<Types&&>(_Args)...);
    }
}; // END ReferenceWrapper


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

CUSTOM_END