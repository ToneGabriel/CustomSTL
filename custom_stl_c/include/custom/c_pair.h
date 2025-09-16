#ifndef C_PAIR_H
#define C_PAIR_H


#include "custom/c_utility.h"


#define DEFINE_GENERIC_PAIR(                                                                                \
    PAIR_NAME,                                                                                              \
    TYPE_1,                                                                                                 \
    TYPE_2                                                                                                  \
)                                                                                                           \
                                                                                                            \
typedef struct                                                                                              \
{                                                                                                           \
    TYPE_1 first;                                                                                           \
    TYPE_2 second;                                                                                          \
} PAIR_NAME;                                                                                                \
                                                                                                            \
static PAIR_NAME    _C_PUBLIC_MEMBER(PAIR_NAME, create)(const TYPE_1* first, const TYPE_2* second);         \
static void         _C_PUBLIC_MEMBER(PAIR_NAME, destroy)(PAIR_NAME* pair);                                  \
static void         _C_PUBLIC_MEMBER(PAIR_NAME, copy)(PAIR_NAME* dest, const PAIR_NAME* source);            \
static void         _C_PUBLIC_MEMBER(PAIR_NAME, move)(PAIR_NAME* dest, PAIR_NAME* source);                  \
static bool         _C_PUBLIC_MEMBER(PAIR_NAME, equals)(const PAIR_NAME* left, const PAIR_NAME* right);     \
                                                                                                            \
static PAIR_NAME _C_PUBLIC_MEMBER(PAIR_NAME, create)(const TYPE_1* first, const TYPE_2* second)             \
{                                                                                                           \
    PAIR_NAME pair = {};                                                                                    \
    _C_PUBLIC_MEMBER(TYPE_1, copy)(&pair.first, first);                                                     \
    _C_PUBLIC_MEMBER(TYPE_2, copy)(&pair.second, second);                                                   \
    return pair;                                                                                            \
}                                                                                                           \
                                                                                                            \
static void _C_PUBLIC_MEMBER(PAIR_NAME, destroy)(PAIR_NAME* pair)                                           \
{                                                                                                           \
    _C_CUSTOM_ASSERT(NULL != pair, "Pair is NULL");                                                         \
    _C_PUBLIC_MEMBER(TYPE_1, destroy)(&pair->first);                                                        \
    _C_PUBLIC_MEMBER(TYPE_2, destroy)(&pair->second);                                                       \
}                                                                                                           \
                                                                                                            \
static void _C_PUBLIC_MEMBER(PAIR_NAME, copy)(PAIR_NAME* dest, const PAIR_NAME* source)                     \
{                                                                                                           \
    _C_CUSTOM_ASSERT(NULL != dest, "Pair dest is NULL");                                                    \
    _C_CUSTOM_ASSERT(NULL != source, "Pair source is NULL");                                                \
    if (dest == source) return;                                                                             \
    _C_PUBLIC_MEMBER(TYPE_1, copy)(&dest->first, &source->first);                                           \
    _C_PUBLIC_MEMBER(TYPE_2, copy)(&dest->second, &source->second);                                         \
}                                                                                                           \
                                                                                                            \
static void _C_PUBLIC_MEMBER(PAIR_NAME, move)(PAIR_NAME* dest, PAIR_NAME* source)                           \
{                                                                                                           \
    _C_CUSTOM_ASSERT(NULL != dest, "Pair dest is NULL");                                                    \
    _C_CUSTOM_ASSERT(NULL != source, "Pair source is NULL");                                                \
    if (dest == source) return;                                                                             \
    _C_PUBLIC_MEMBER(TYPE_1, move)(&dest->first, &source->first);                                           \
    _C_PUBLIC_MEMBER(TYPE_2, move)(&dest->second, &source->second);                                         \
}                                                                                                           \
                                                                                                            \
static bool _C_PUBLIC_MEMBER(PAIR_NAME, equals)(const PAIR_NAME* left, const PAIR_NAME* right)              \
{                                                                                                           \
    return  _C_PUBLIC_MEMBER(TYPE_1, equals)(&left->first, &right->first) &&                                \
            _C_PUBLIC_MEMBER(TYPE_2, equals)(&left->second, &right->second);                                \
}                                                                                                           \


#endif // C_PAIR_H