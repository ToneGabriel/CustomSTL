#ifndef _C_STLCORE_H
#define _C_STLCORE_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#ifndef NDEBUG
#   define _C_CUSTOM_ASSERT(Expr, Msg) __Assert(Expr, Msg, #Expr, __FILE__, __LINE__)
#else
#   define _C_CUSTOM_ASSERT(Expr, Msg) ((void)0)
#endif


static inline void __Assert(bool expr, const char* msg, const char* expected, const char* file, int line)
{
    if (!expr)
    {
        fprintf(stderr, "Assert failed:\t%s\n", msg);
        fprintf(stderr, "Expected:\t%s\n", expected);
        fprintf(stderr, "File:\t\t%s\n", file);
        fprintf(stderr, "Line:\t\t%d\n", line);
        abort();
    }
    else
    {
        // assert OK - do nothing
    }
}


// Helper macros to bind from 2 to 5 args
#define _C_IDENTIFIER_BIND_IMPL_2(name1, name2) name1##_##name2
#define _C_IDENTIFIER_BIND_IMPL_3(name1, name2, name3) name1##_##name2##_##name3
#define _C_IDENTIFIER_BIND_IMPL_4(name1, name2, name3, name4) name1##_##name2##_##name3##_##name4
#define _C_IDENTIFIER_BIND_IMPL_5(name1, name2, name3, name4, name5) name1##_##name2##_##name3##_##name4##_##name5

// Helper macro to count number of arguments
#define _C_IDENTIFIER_BIND_IMPL_SELECTOR_HELPER(_1, _2, _3, _4, _5, IMPL, ...) IMPL

// Helper macro to choose the correct implementation
#define _C_IDENTIFIER_BIND_IMPL_SELECTOR(...)   \
_C_IDENTIFIER_BIND_IMPL_SELECTOR_HELPER(        \
    __VA_ARGS__,                                \
    _C_IDENTIFIER_BIND_IMPL_5,                  \
    _C_IDENTIFIER_BIND_IMPL_4,                  \
    _C_IDENTIFIER_BIND_IMPL_3,                  \
    _C_IDENTIFIER_BIND_IMPL_2                   \
)

// Macro to bind 2 to 5 args
#define C_IDENTIFIER_BIND(...) _C_IDENTIFIER_BIND_IMPL_SELECTOR(__VA_ARGS__)(__VA_ARGS__)


#endif  // _C_STLCORE_H