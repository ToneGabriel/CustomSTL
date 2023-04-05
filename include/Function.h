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

    template<class Func>
    static RetType _invoke(const Function& target, ArgTypes&&... args) {
        return (*reinterpret_cast<Func*>(target._functor))(custom::forward<ArgTypes>(args)...);
    }

    template<class Func>
    static void* _copy(const Function& target) {
        return reinterpret_cast<void*>(new Func(*reinterpret_cast<Func*>(target._functor)));
    }

    template<class Func>
    static void _destruct(const Function& target) {
        delete reinterpret_cast<Func*>(target._functor);
    }

public:
    // Constructors

    Function() = default;

    template<typename Func, typename std::enable_if<!std::is_same<typename std::decay<Func>::type, Function>::value, int>::type = 1>
    Function(Func&& f)
        :   _invoker(reinterpret_cast<Invoker>(_invoke<typename std::decay<Func>::type>)),
            _copiator(reinterpret_cast<Copiator>(_copy<typename std::decay<Func>::type>)),
            _destructor(reinterpret_cast<Destructor>(_destruct<typename std::decay<Func>::type>)),
            _functor(reinterpret_cast<void*>(new typename std::decay<Func>::type(custom::forward<Func>(f))))
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