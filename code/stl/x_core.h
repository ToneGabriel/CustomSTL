#pragma once
#include <cstddef>      // size_t, nullptr_t
#include <stdexcept>    // exceptions
#include <climits>
#include <iostream>     // debugging

#include "Config.hpp"   // project configuration


#define CUSTOM_BEGIN namespace custom { // gloabal namespace for this project
#define CUSTOM_END }

#define CUSTOM_DETAIL_BEGIN namespace detail {  // used to hide detailed implementations
#define CUSTOM_DETAIL_END }

#define CUSTOM_ASSERT(Expr, Msg) custom::__Assert(Expr, Msg, #Expr, __FILE__, __LINE__)

#define CUSTOM_RERAISE throw    // used to terminate in a catch block

#define CUSTOM_OPTIMAL_IMPLEMENTATION 0    // some implementations are easier to understand, but have lower performance

#ifdef _MSC_VER
// This is a Microsoft Specific. This is a __declspec extended attribute.
// This form of __declspec can be applied to any class declaration,
// but should only be applied to pure interface classes, that is,
// classes that will never be instantiated on their own.
// The __declspec stops the compiler from generating code to initialize the vfptr in the constructor(s)
// and destructor of the class. In many cases, this removes the only references to the vtable
// that are associated with the class and, thus, the linker will remove it.
// Using this form of __declspec can result in a significant reduction in code size.
#define CUSTOM_NOVTABLE_ATTR __declspec(novtable)
#else
#define CUSTOM_NOVTABLE_ATTR    // Defined only for _MSC_VER
#endif


CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN // create the "detail" namespace inside "custom" namespace
CUSTOM_DETAIL_END

inline void __Assert(bool expr, const char* msg, const char* exprStr, const char* file, int line) {
    if (!expr)
    {
        std::cerr   << "Assert failed:\t"   << msg << "\n"
                    << "Expected:\t"        << exprStr << "\n"
                    << "Source:\t\t"        << file << ", line " << line << "\n";
        ::abort();
    }
}

CUSTOM_END