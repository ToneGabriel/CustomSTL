#pragma once
#include "Utility.h"
#include <iostream>

CUSTOM_BEGIN

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

    template<class FuncType>
    static RetType _invoke(const Function& target, ArgTypes&&... args) {
        return (*reinterpret_cast<FuncType*>(target._functor))(custom::forward<ArgTypes>(args)...);
    }

    template<class FuncType>
    static void* _copy(const Function& target) {
        return reinterpret_cast<void*>(new FuncType(*reinterpret_cast<FuncType*>(target._functor)));
    }

    template<class FuncType>
    static void _destruct(const Function& target) {
        delete reinterpret_cast<FuncType*>(target._functor);
    }

public:
    // Constructors

    Function() = default;

    template<class FuncType, custom::EnableIf_t<!custom::IsSame<custom::Decay_t<FuncType>, Function>::Value, bool> = true>
    Function(FuncType&& f)
        :   _invoker(reinterpret_cast<Invoker>(_invoke<custom::Decay_t<FuncType>>)),
            _copiator(reinterpret_cast<Copiator>(_copy<custom::Decay_t<FuncType>>)),
            _destructor(reinterpret_cast<Destructor>(_destruct<custom::Decay_t<FuncType>>)),
            _functor(reinterpret_cast<void*>(new custom::Decay_t<FuncType>(custom::forward<FuncType>(f))))
            { /*Empty*/ }

    Function(const Function& other)
        :   _invoker(other._invoker),
            _copiator(other._copiator),
            _destructor(other._destructor),
            _functor(other._copiator(other))
            { /*Empty*/ }

    Function(Function&& other) noexcept {
        _invoker            = other._invoker;
        _copiator           = other._copiator;
        _destructor         = other._destructor;
        _functor            = other._functor;

        other._invoker      = nullptr;
        other._copiator     = nullptr;
        other._destructor   = nullptr;
        other._functor      = nullptr;
    }

    ~Function() {
        if (_functor != nullptr)
            _destructor(*this);
    }
    
public:
    // Operators

    RetType operator()(ArgTypes&&... args) const {
        if (_functor == nullptr) 
            throw std::runtime_error("call an empty function object");
        
        return _invoker(*this, custom::forward<ArgTypes>(args)...);
    }

    Function& operator=(const Function& other) {
        if (this != &other)
        {
            if (_functor != nullptr)
                _destructor(*this);

            _invoker    = other._invoker;
            _copiator   = other._copiator;
            _destructor = other._destructor;
            _functor    = other._copiator(other);
        }

        return *this;
    }

    Function& operator=(Function&& other) {
        if (this != &other)
        {
            if (_functor != nullptr)
                _destructor(*this);

            _invoker            = other._invoker;
            _copiator           = other._copiator;
            _destructor         = other._destructor;
            _functor            = other._functor;

            other._invoker      = nullptr;
            other._copiator     = nullptr;
            other._destructor   = nullptr;
            other._functor      = nullptr;
        }

        return *this;
    }
}; // END Function template

CUSTOM_END