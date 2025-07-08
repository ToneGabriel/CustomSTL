#ifndef Y_CORE_H
#define Y_CORE_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <_custom_stl_config.hpp>   // project configuration


#ifndef NDEBUG
#   define _ASSERT(Expr, Msg) __Assert(Expr, Msg, #Expr, __FILE__, __LINE__)
#else
#   define _ASSERT(Expr, Msg) ((void)0)
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


#endif  // Y_CORE_H