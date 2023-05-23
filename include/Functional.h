#pragma once
#include "Utility.h"
#include "Tuple.h"


CUSTOM_BEGIN

template<int Num> struct Placeholder {};

namespace placeholders
{
    extern const Placeholder<1> _1;
    extern const Placeholder<2> _2;
    extern const Placeholder<3> _3;
    extern const Placeholder<4> _4;
    extern const Placeholder<5> _5;
    extern const Placeholder<6> _6;
    extern const Placeholder<7> _7;
    extern const Placeholder<8> _8;
    extern const Placeholder<9> _9;
    extern const Placeholder<10> _10;
    extern const Placeholder<11> _11;
    extern const Placeholder<12> _12;
    extern const Placeholder<13> _13;
    extern const Placeholder<14> _14;
    extern const Placeholder<15> _15;
    extern const Placeholder<16> _16;
    extern const Placeholder<17> _17;
    extern const Placeholder<18> _18;
    extern const Placeholder<19> _19;
    extern const Placeholder<20> _20;
}

// TODO: refactor
// This only works with normal function pointers
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

// bind
auto _call_binder() -> decltype(/*something*/) {
    // TODO: implement
}

template<class Functor, class... Args>
class Binder
{
private:
    Decay_t<Functor> _functor;
    Tuple<Decay_t<Args>...> _arguments;

public:
    explicit Binder(Functor&& func, Args&&... args)
        : _functor(custom::forward<Functor>(func)), _arguments(custom::forward<Args>(args)...) { /*Empty*/ }

    template<class... UnboundArgs>
    auto operator()(UnboundArgs&&... unbArgs) noexcept
    -> decltype(_call_binder(_functor, _arguments, custom::forward_as_tuple(custom::forward<UnboundArgs>(unbArgs)...))) {
        return _call_binder(_functor, _arguments, custom::forward_as_tuple(custom::forward<UnboundArgs>(unbArgs)...));
        // TODO: check impl
    }
};

template<class Functor, class... Args>
constexpr Binder<Functor, Args...> bind(Functor&& func, Args&&... args) {
    return Binder<Functor, Args...>(custom::forward<Functor>(func), custom::forward<Args>(args)...);
}

CUSTOM_END