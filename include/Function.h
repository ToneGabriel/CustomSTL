#pragma once
#include "Utility.h"

CUSTOM_BEGIN

template<class Signature>
class Function;

template<class RetType, class... ArgTypes>
class Function<RetType(ArgTypes...)>        // Wrapper template to function pointer
{
private:
    using Invoker       = RetType(*)(char*, ArgTypes&&...);
    using Constructor   = void(*)(char*, char*);
    using Destructor    = void(*)(char*);

private:
    Invoker _invoker;
    Constructor _constructor;
    Destructor _destructor;

    char* _data;
    size_t _size;

private:

    template<class Func>
    static RetType invoke(Func* f, ArgTypes&&... args) {
        return (*f)(custom::forward<ArgTypes>(args)...);
    }

    template<class Func>
    static void construct(Func* constr1, Func* constr2) {
        new (constr1) Func(*constr2);
    }

    template<class Func>
    static void destruct(Func* f) {
        f->~Func();
    }

public:
    // Constructors

    Function()
        :   _invoker(nullptr),
            _constructor(nullptr),
            _destructor(nullptr),
            _data(nullptr),
            _size(0) { /*Empty*/ }

    template<class Func>
    Function(Func f)
        :   _invoker(reinterpret_cast<Invoker>(invoke<Func>)),
            _constructor(reinterpret_cast<Constructor>(construct<Func>)),
            _destructor(reinterpret_cast<Destructor>(destruct<Func>)),
            _data(new char[sizeof(Func)]),
            _size(sizeof(Func))
    {
        _constructor(_data, reinterpret_cast<char*>(&f));
    }

    Function(const Function& other)
        :   _invoker(other._invoker),
            _constructor(other._constructor),
            _destructor(other._destructor),
            _size(other._size)
    {
        if (_invoker != nullptr)
        {
            delete[] _data;
            _data = new char[_size];
            _constructor(_data, other._data);
        }
    }

    Function(Function&& other) {
        // TODO: implement
    }

    ~Function() {
        if (_data != nullptr)
        {
            _destructor(_data);
            delete[] _data;
        }
    }
    
public:
    // Operators

    RetType operator()(ArgTypes... args) const {
        return _invoker(_data, custom::forward<ArgTypes>(args)...);
    }

    Function& operator=(const Function& other) {
        // TODO: implement
        return *this;
    }

    Function& operator=(Function&& other) {
        // TODO: implement
        return *this;
    }

    bool operator==(const Function& other) const {
        // TODO: implement
        return false;
    }

    bool operator!=(const Function& other) const {
		return !(*this == other);
    }

}; // END Function template

CUSTOM_END