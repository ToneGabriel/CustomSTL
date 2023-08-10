#pragma once
#include "Common.h"

CUSTOM_BEGIN

class TypeInfo    // TODO: implement
{
protected:
    const char* _name;

public:
    // Constructors & Operators

    virtual ~TypeInfo();

public:
    // Main functions

    size_t hash_code() const noexcept {
        return 0;
    }

    const char* name() const noexcept {
        return _name;
    }
};

CUSTOM_END