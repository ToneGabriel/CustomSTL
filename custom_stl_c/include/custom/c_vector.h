#ifndef C_VECTOR_H
#define C_VECTOR_H


#include "custom/c_utility.h"


#define _DEFINE_GENERIC_VECTOR_IMPL(                                                                    \
    VECTOR_NAME,                                                                                        \
    TYPE,                                                                                               \
    TYPE_REF_EQUALS_FUNC,                                                                               \
    TYPE_REF_COPY_FUNC,                                                                                 \
    TYPE_REF_DELETE_FUNC                                                                                \
)                                                                                                       \
                                                                                                        \
typedef struct                                                                                          \
{                                                                                                       \
    TYPE* arr;                                                                                          \
    size_t size;                                                                                        \
    size_t capacity;                                                                                    \
} VECTOR_NAME;                                                                                          \
                                                                                                        \
static VECTOR_NAME _C_IDENTIFIER_BIND(VECTOR_NAME, create)(size_t capacity)                             \
{                                                                                                       \
    _ASSERT(0 < capacity, "Vector capacity should be greater than 0");                                  \
    VECTOR_NAME vec = {                                                                                 \
        .arr = (TYPE*)malloc(sizeof(TYPE) * capacity),                                                  \
        .size = 0,                                                                                      \
        .capacity = capacity                                                                            \
    };                                                                                                  \
    return vec;                                                                                         \
}                                                                                                       \
                                                                                                        \
static void _C_IDENTIFIER_BIND(VECTOR_NAME, destroy)(VECTOR_NAME* vec)                                  \
{                                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                                             \
    if (NULL == vec->arr) return;                                                                       \
    for (size_t i = 0; i < vec->size; ++i)                                                              \
        TYPE_REF_DELETE_FUNC(&vec->arr[i]);                                                             \
    free(vec->arr);                                                                                     \
    vec->capacity = vec->size = 0;                                                                      \
    vec->arr = NULL;                                                                                    \
}                                                                                                       \
                                                                                                        \
static void _C_IDENTIFIER_BIND(VECTOR_NAME, clear)(VECTOR_NAME* vec)                                    \
{                                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                                             \
    for (size_t i = 0; i < vec->size; ++i)                                                              \
        TYPE_REF_DELETE_FUNC(&vec->arr[i]);                                                             \
    vec->size = 0;                                                                                      \
}                                                                                                       \
                                                                                                        \
static void _C_IDENTIFIER_BIND(VECTOR_NAME, copy)(VECTOR_NAME* dest, const VECTOR_NAME* source)         \
{                                                                                                       \
    _ASSERT(NULL != dest, "Vector dest is NULL");                                                       \
    _ASSERT(NULL != source, "Vector source is NULL");                                                   \
    _ASSERT(NULL != source->arr, "Vector source array is NULL");                                        \
    if (dest == source) return;                                                                         \
    _C_IDENTIFIER_BIND(VECTOR_NAME, destroy)(dest);                                                     \
    *dest = _C_IDENTIFIER_BIND(VECTOR_NAME, create)(source->capacity);                                  \
    for (size_t i = 0; i < source->size; ++i)                                                           \
        TYPE_REF_COPY_FUNC(&dest->arr[i], &source->arr[i]);                                             \
    dest->size = source->size;                                                                          \
}                                                                                                       \
                                                                                                        \
static void _C_IDENTIFIER_BIND(VECTOR_NAME, move)(VECTOR_NAME* dest, VECTOR_NAME* source)               \
{                                                                                                       \
    _ASSERT(NULL != dest, "Vector dest is NULL");                                                       \
    _ASSERT(NULL != source, "Vector source is NULL");                                                   \
    _ASSERT(NULL != source->arr, "Vector source array is NULL");                                        \
    if (dest == source) return;                                                                         \
    _C_IDENTIFIER_BIND(VECTOR_NAME, destroy)(dest);                                                     \
    *dest = *source;                                                                                    \
    source->capacity = source->size = 0;                                                                \
    source->arr = NULL;                                                                                 \
}                                                                                                       \
                                                                                                        \
static TYPE* _C_IDENTIFIER_BIND(VECTOR_NAME, data)(const VECTOR_NAME* vec)                              \
{                                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                                             \
    return vec->arr;                                                                                    \
}                                                                                                       \
                                                                                                        \
static size_t _C_IDENTIFIER_BIND(VECTOR_NAME, size)(const VECTOR_NAME* vec)                             \
{                                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                                             \
    return vec->size;                                                                                   \
}                                                                                                       \
                                                                                                        \
static size_t _C_IDENTIFIER_BIND(VECTOR_NAME, capacity)(const VECTOR_NAME* vec)                         \
{                                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                                             \
    return vec->capacity;                                                                               \
}                                                                                                       \
                                                                                                        \
static bool _C_IDENTIFIER_BIND(VECTOR_NAME, empty)(const VECTOR_NAME* vec)                              \
{                                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                                             \
    return vec->size == 0;                                                                              \
}                                                                                                       \
                                                                                                        \
static void _C_IDENTIFIER_BIND(VECTOR_NAME, push_back)(VECTOR_NAME* vec, const TYPE* item)              \
{                                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                                             \
    if (vec->size >= vec->capacity)                                                                     \
    {                                                                                                   \
        size_t new_cap = vec->capacity + vec->capacity / 2 + 1;                                         \
        vec->arr = (TYPE*)realloc(vec->arr, sizeof(TYPE) * new_cap);                                    \
        vec->capacity = new_cap;                                                                        \
    }                                                                                                   \
    TYPE_REF_COPY_FUNC(&vec->arr[vec->size++], item);                                                   \
}                                                                                                       \
                                                                                                        \
static void _C_IDENTIFIER_BIND(VECTOR_NAME, pop_back)(VECTOR_NAME* vec)                                 \
{                                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                                             \
    if (vec->size == 0) return;                                                                         \
    TYPE_REF_DELETE_FUNC(&vec->arr[--vec->size]);                                                       \
}                                                                                                       \
                                                                                                        \
static TYPE* _C_IDENTIFIER_BIND(VECTOR_NAME, element_front)(VECTOR_NAME* vec)                           \
{                                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                                             \
    _ASSERT(vec->size != 0, "Vector is empty");                                                         \
    return &vec->arr[0];                                                                                \
}                                                                                                       \
                                                                                                        \
static TYPE* _C_IDENTIFIER_BIND(VECTOR_NAME, element_back)(VECTOR_NAME* vec)                            \
{                                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                                             \
    _ASSERT(vec->size != 0, "Vector is empty");                                                         \
    return &vec->arr[vec->size - 1];                                                                    \
}                                                                                                       \
                                                                                                        \
static TYPE* _C_IDENTIFIER_BIND(VECTOR_NAME, element_at)(VECTOR_NAME* vec, size_t index)                \
{                                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                                             \
    _ASSERT(vec->size != 0, "Vector is empty");                                                         \
    _ASSERT(index < vec->size, "Index out of bounds");                                                  \
    return &vec->arr[index];                                                                            \
}                                                                                                       \
                                                                                                        \
static bool _C_IDENTIFIER_BIND(VECTOR_NAME, equals)(const VECTOR_NAME* left, const VECTOR_NAME* right)  \
{                                                                                                       \
    _ASSERT(NULL != left, "Vector left is NULL");                                                       \
    _ASSERT(NULL != right, "Vector right is NULL");                                                     \
    if (left->size != right->size) return false;                                                        \
    for (size_t i = 0; i < left->size; ++i)                                                             \
        if (!TYPE_REF_EQUALS_FUNC(&left->arr[i], &right->arr[i])) return false;                         \
    return true;                                                                                        \
}                                                                                                       \


#define DEFINE_GENERIC_VECTOR(                  \
    VECTOR_NAME_PUBLIC_PREFIX,                  \
    VECTOR_NAME_PRIVATE_PREFIX, /*unused*/      \
    TYPE,                                       \
    TYPE_REF_EQUALS_FUNC,                       \
    TYPE_REF_COPY_FUNC,                         \
    TYPE_REF_DELETE_FUNC                        \
)                                               \
                                                \
_DEFINE_GENERIC_VECTOR_IMPL(                    \
    VECTOR_NAME_PUBLIC_PREFIX,                  \
    TYPE,                                       \
    TYPE_REF_EQUALS_FUNC,                       \
    TYPE_REF_COPY_FUNC,                         \
    TYPE_REF_DELETE_FUNC                        \
)                                               \


#endif  // C_VECTOR_H