#pragma once
#include <cstddef>      // size_t, nullptr_t
#include <stdexcept>    // exceptions
#include <climits>
#include <iostream>     // debugging


#define CUSTOM_BEGIN namespace custom { // gloabal namespace for this project
#define CUSTOM_END }

#define CUSTOM_DETAIL_BEGIN namespace detail {  // used to hide detailed implementations
#define CUSTOM_DETAIL_END }

#define CUSTOM_ASSERT(Expr, Msg) custom::__Assert(Expr, Msg, #Expr, __FILE__, __LINE__)

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

// math vals
#define PI_LONG 3.1415926535897932384626433832795029L
#define PI_BY_2_LONG 1.5707963267948966192313216916397514L
#define E_LONG 2.7182818284590452353602874713526624L
#define LOG10_E_LONG 0.4342944819032518276511289189166051L
#define SQRT_2_LONG 1.4142135623730950488016887242096981L



CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN // ensure the "detail" namespace is placed only in "custom" namespace
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