#pragma once
#include "Utility.h"

CUSTOM_BEGIN

template<class Signature>
class Function;

template<class RetType, class... ArgTypes>
class Function<RetType(ArgTypes...)>        // Wrapper template to function pointer
{
private:
    using Invoker = RetType(*)(ArgTypes&&...);  // TODO: check

    Invoker _invoker;

public:
    // Constructors

    Function()
        : _invoker(nullptr) {}

    Function(Invoker inv)
        : _invoker(inv) {}

    ~Function() = default;  // TODO: check if need delete
    
public:
    // Operators

    RetType operator()(ArgTypes... args) const {
        return _invoker(custom::forward<ArgTypes>(args)...);
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