#ifndef C_PAIR_H
#define C_PAIR_H


#include "custom/c_utility.h"


#define DEFINE_GENERIC_PAIR(                                                    \
    PAIR_NAME,                                                                  \
    TYPE_1,                                                                     \
    TYPE_2                                                                      \
)                                                                               \
                                                                                \
typedef struct                                                                  \
{                                                                               \
    TYPE_1 first;                                                               \
    TYPE_2 second;                                                              \
} PAIR_NAME;                                                                    \
                                                                                \
static PAIR_NAME    _C_IDENTIFIER_BIND(PAIR_NAME, create)();                    \
static void         _C_IDENTIFIER_BIND(PAIR_NAME, destroy)(PAIR_NAME* pair);    \
                                                                                \
static PAIR_NAME _C_IDENTIFIER_BIND(PAIR_NAME, create)()                        \
{                                                                               \
    PAIR_NAME pair = {                                                          \
        .first,                                                                 \
        .second                                                                 \
    };                                                                          \
    return pair;                                                                \
}                                                                               \
                                                                                \
static void _C_IDENTIFIER_BIND(PAIR_NAME, destroy)(PAIR_NAME* pair)             \
{                                                                               \
    _C_CUSTOM_ASSERT(NULL != pair, "Pair is NULL");                             \
}                                                                               \


#endif // C_PAIR_H