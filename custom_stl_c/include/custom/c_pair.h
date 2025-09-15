#ifndef C_PAIR_H
#define C_PAIR_H


#include "custom/c_utility.h"


#define DEFINE_GENERIC_PAIR(                                                                                \
    PAIR_NAME,                                                                                              \
    TYPE_1,                                                                                                 \
    TYPE_1_REF_COPY_FUNC,                                                                                   \
    TYPE_1_REF_MOVE_FUNC,                                                                                   \
    TYPE_1_REF_DELETE_FUNC,                                                                                 \
    TYPE_2,                                                                                                 \
    TYPE_2_REF_COPY_FUNC,                                                                                   \
    TYPE_2_REF_MOVE_FUNC,                                                                                   \
    TYPE_2_REF_DELETE_FUNC                                                                                  \
)                                                                                                           \
                                                                                                            \
typedef struct                                                                                              \
{                                                                                                           \
    TYPE_1 first;                                                                                           \
    TYPE_2 second;                                                                                          \
} PAIR_NAME;                                                                                                \
                                                                                                            \
static PAIR_NAME    _C_IDENTIFIER_BIND(PAIR_NAME, create)(const TYPE_1* first, const TYPE_2* second);       \
static void         _C_IDENTIFIER_BIND(PAIR_NAME, destroy)(PAIR_NAME* pair);                                \
static void         _C_IDENTIFIER_BIND(PAIR_NAME, copy)(PAIR_NAME* dest, const PAIR_NAME* source);          \
static void         _C_IDENTIFIER_BIND(PAIR_NAME, move)(PAIR_NAME* dest, PAIR_NAME* source);                \
static bool         _C_IDENTIFIER_BIND(PAIR_NAME, equals)(const PAIR_NAME* left, const PAIR_NAME* right);   \
                                                                                                            \
static PAIR_NAME _C_IDENTIFIER_BIND(PAIR_NAME, create)(const TYPE_1* first, const TYPE_2* second)           \
{                                                                                                           \
    PAIR_NAME pair = {};                                                                                    \
    TYPE_1_REF_COPY_FUNC(&pair.first, first);                                                               \
    TYPE_2_REF_COPY_FUNC(&pair.second, second);                                                             \
    return pair;                                                                                            \
}                                                                                                           \
                                                                                                            \
static void _C_IDENTIFIER_BIND(PAIR_NAME, destroy)(PAIR_NAME* pair)                                         \
{                                                                                                           \
    _C_CUSTOM_ASSERT(NULL != pair, "Pair is NULL");                                                         \
    TYPE_1_REF_DELETE_FUNC(&pair->first);                                                                   \
    TYPE_2_REF_DELETE_FUNC(&pair->second);                                                                  \
}                                                                                                           \
                                                                                                            \
static void _C_IDENTIFIER_BIND(PAIR_NAME, copy)(PAIR_NAME* dest, const PAIR_NAME* source)                   \
{                                                                                                           \
    _C_CUSTOM_ASSERT(NULL != dest, "Pair dest is NULL");                                                    \
    _C_CUSTOM_ASSERT(NULL != source, "Pair source is NULL");                                                \
    if (dest == source) return;                                                                             \
    TYPE_1_REF_COPY_FUNC(&dest->first, &source->first);                                                     \
    TYPE_2_REF_COPY_FUNC(&dest->second, &source->second);                                                   \
}                                                                                                           \
                                                                                                            \
static void _C_IDENTIFIER_BIND(PAIR_NAME, move)(PAIR_NAME* dest, PAIR_NAME* source)                         \
{                                                                                                           \
    _C_CUSTOM_ASSERT(NULL != dest, "Pair dest is NULL");                                                    \
    _C_CUSTOM_ASSERT(NULL != source, "Pair source is NULL");                                                \
    if (dest == source) return;                                                                             \
    TYPE_1_REF_MOVE_FUNC(&dest->first, &source->first);                                                     \
    TYPE_2_REF_MOVE_FUNC(&dest->second, &source->second);                                                   \
}                                                                                                           \
                                                                                                            \
static bool _C_IDENTIFIER_BIND(PAIR_NAME, equals)(const PAIR_NAME* left, const PAIR_NAME* right)            \
{                                                                                                           \
    return false;                                                                                           \
}                                                                                                           \

// return left->first == right->first && left->second == right->second;

#endif // C_PAIR_H