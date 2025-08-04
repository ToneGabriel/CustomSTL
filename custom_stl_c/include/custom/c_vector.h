#ifndef VECTOR_H
#define VECTOR_H


#include "custom/c_utility.h"


#define _DEFINE_GENERIC_VECTOR_IMPL(                                                    \
    VECTOR_NAME,                                                                        \
    TYPE,                                                                               \
    TYPE_REF_COMPARE_FUNC,                                                              \
    TYPE_REF_COPY_FUNC,                                                                 \
    TYPE_REF_DELETE_FUNC,                                                               \
    _VECTOR_FUNC_NAME_TYPE_REF_SWAP,                                                    \
    VECTOR_FUNC_NAME_CREATE,                                                            \
    VECTOR_FUNC_NAME_DESTROY,                                                           \
    VECTOR_FUNC_NAME_CLEAR,                                                             \
    VECTOR_FUNC_NAME_COPY,                                                              \
    VECTOR_FUNC_NAME_MOVE,                                                              \
    VECTOR_FUNC_NAME_SIZE,                                                              \
    VECTOR_FUNC_NAME_CAPACITY,                                                          \
    VECTOR_FUNC_NAME_EMPTY,                                                             \
    VECTOR_FUNC_NAME_PUSH_BACK,                                                         \
    VECTOR_FUNC_NAME_POP_BACK                                                           \
)                                                                                       \
                                                                                        \
DEFINE_SWAP_GENERIC_FUNCTION(_VECTOR_FUNC_NAME_TYPE_REF_SWAP, TYPE, TYPE_REF_COPY_FUNC) \
                                                                                        \
typedef struct                                                                          \
{                                                                                       \
    TYPE* arr;                                                                          \
    size_t size;                                                                        \
    size_t capacity;                                                                    \
} VECTOR_NAME;                                                                          \
                                                                                        \
static VECTOR_NAME VECTOR_FUNC_NAME_CREATE(size_t capacity)                             \
{                                                                                       \
    _ASSERT(0 < capacity, "Vector capacity should be greater than 0");                  \
    VECTOR_NAME vec = {                                                                 \
        .capacity = capacity,                                                           \
        .size = 0,                                                                      \
        .arr = (TYPE*)malloc(sizeof(TYPE) * capacity)                                   \
    };                                                                                  \
    return vec;                                                                         \
}                                                                                       \
                                                                                        \
static void VECTOR_FUNC_NAME_DESTROY(VECTOR_NAME* vec)                                  \
{                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                             \
    if (NULL == vec->arr) return;                                                       \
    for (size_t i = 0; i < vec->size; ++i)                                              \
        TYPE_REF_DELETE_FUNC(&vec->arr[i]);                                             \
    free(vec->arr);                                                                     \
    vec->capacity = vec->size = 0;                                                      \
    vec->arr = NULL;                                                                    \
}                                                                                       \
                                                                                        \
static void VECTOR_FUNC_NAME_CLEAR(VECTOR_NAME* vec)                                    \
{                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                             \
    for (size_t i = 0; i < vec->size; ++i)                                              \
        TYPE_REF_DELETE_FUNC(&vec->arr[i]);                                             \
    vec->size = 0;                                                                      \
}                                                                                       \
                                                                                        \
static void VECTOR_FUNC_NAME_COPY(VECTOR_NAME* dest, const VECTOR_NAME* source)         \
{                                                                                       \
    _ASSERT(NULL != dest, "Vector dest is NULL");                                       \
    _ASSERT(NULL != source, "Vector source is NULL");                                   \
    _ASSERT(NULL != source->arr, "Vector source array is NULL");                        \
    if (dest == source) return;                                                         \
    VECTOR_FUNC_NAME_DESTROY(dest);                                                     \
    dest = VECTOR_FUNC_NAME_CREATE(source->capacity);                                   \
    for (size_t i = 0; i < source->size; ++i)                                           \
        TYPE_REF_COPY_FUNC(&dest->arr[i], &source->arr[i]);                             \
    dest->size = source->size;                                                          \
}                                                                                       \
                                                                                        \
static void VECTOR_FUNC_NAME_MOVE(VECTOR_NAME* dest, VECTOR_NAME* source)               \
{                                                                                       \
    _ASSERT(NULL != dest, "Vector dest is NULL");                                       \
    _ASSERT(NULL != source, "Vector source is NULL");                                   \
    _ASSERT(NULL != source->arr, "Vector source array is NULL");                        \
    if (dest == source) return;                                                         \
    VECTOR_FUNC_NAME_DESTROY(dest);                                                     \
    *dest = *source;                                                                    \
    source->capacity = source->size = 0;                                                \
    source->arr = NULL;                                                                 \
}                                                                                       \
                                                                                        \
static size_t PQ_FUNC_NAME_SIZE(PQ_NAME* vec)                                           \
{                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                             \
    return vec->size;                                                                   \
}                                                                                       \
                                                                                        \
static size_t PQ_FUNC_NAME_CAPACITY(PQ_NAME* vec)                                       \
{                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                             \
    return vec->capacity;                                                               \
}                                                                                       \
                                                                                        \
static bool PQ_FUNC_NAME_EMPTY(PQ_NAME* vec)                                            \
{                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                             \
    return vec->size == 0;                                                              \
}                                                                                       \
                                                                                        \
static void VECTOR_FUNC_NAME_PUSH_BACK(PQ_NAME* vec, const TYPE* item)                  \
{                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                             \
    if (vec->size >= vec->capacity)                                                     \
    {                                                                                   \
        size_t new_cap = vec->capacity + vec->capacity / 2 + 1;                         \
        vec->arr = (TYPE*)realloc(vec->arr, sizeof(TYPE) * new_cap);                    \
        vec->capacity = new_cap;                                                        \
    }                                                                                   \
    TYPE_REF_COPY_FUNC(&vec->arr[vec->size++], item);                                   \
}                                                                                       \
                                                                                        \
static void VECTOR_FUNC_NAME_POP_BACK(VECTOR_NAME* vec)                                 \
{                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                             \
    if (vec->size == 0) return;                                                         \
    TYPE_REF_DELETE_FUNC(&vec->arr[--vec->size]);                                       \
}                                                                                       \


#endif  // VECTOR_H