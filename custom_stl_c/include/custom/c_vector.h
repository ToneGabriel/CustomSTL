#ifndef VECTOR_H
#define VECTOR_H


#include "custom/c_utility.h"


#define _DEFINE_GENERIC_VECTOR_IMPL(                                                    \
    VECTOR_NAME,                                                                            \
    TYPE,                                                                               \
    TYPE_REF_COMPARE_FUNC,                                                              \
    TYPE_REF_COPY_FUNC,                                                                 \
    TYPE_REF_DELETE_FUNC,                                                               \
    _VECTOR_FUNC_NAME_TYPE_REF_SWAP,                                                        \
    VECTOR_FUNC_NAME_CREATE,                                                                \
    VECTOR_FUNC_NAME_CLEAR,                                                                 \
    VECTOR_FUNC_NAME_DESTROY,                                                               \
    VECTOR_FUNC_NAME_COPY,                                                               \
    VECTOR_FUNC_NAME_MOVE,                                                               \
    VECTOR_FUNC_NAME_SIZE,                                                                  \
    VECTOR_FUNC_NAME_CAPACITY,                                                              \
    VECTOR_FUNC_NAME_EMPTY,                                                                 \
    VECTOR_FUNC_NAME_PUSH_BACK,                                                                \
    VECTOR_FUNC_NAME_POP_BACK                                                                \
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
static VECTOR_NAME* VECTOR_FUNC_NAME_CREATE(size_t capacity)                                    \
{                                                                                       \
    _ASSERT(capacity != 0, "Capacity should be greater than 0");                        \
    VECTOR_NAME* vec = (VECTOR_NAME*)malloc(sizeof(VECTOR_NAME));                                    \
    if (NULL == vec) return NULL;                                                        \
    TYPE* temp_arr = (TYPE*)malloc(sizeof(TYPE) * capacity);                            \
    if (NULL == temp_arr)                                                               \
    {                                                                                   \
        free(vec);                                                                       \
        return NULL;                                                                    \
    }                                                                                   \
    vec->arr = temp_arr;                                                                 \
    vec->size = 0;                                                                       \
    vec->capacity = capacity;                                                            \
    return vec;                                                                          \
}                                                                                       \
                                                                                        \
static void VECTOR_FUNC_NAME_CLEAR(VECTOR_NAME* vec)                                             \
{                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                      \
    for (size_t i = 0; i < vec->size; ++i) TYPE_REF_DELETE_FUNC(&vec->arr[i]);            \
    vec->size = 0;                                                                       \
}                                                                                       \
                                                                                        \
static void VECTOR_FUNC_NAME_DESTROY(VECTOR_NAME* vec)                                           \
{                                                                                       \
    VECTOR_FUNC_NAME_CLEAR(vec);                                                        \
    free(vec->arr);                                                                     \
    free(vec);                                                                          \
}                                                                                       \
                                                                                        \
static void VECTOR_FUNC_NAME_COPY(const VECTOR_NAME* dest, VECTOR_NAME* source)                         \
{                                                                                                       \
    VECTOR_FUNC_NAME_DESTROY(dest);                                                                     \
    dest = VECTOR_FUNC_NAME_CREATE(source->capacity);                                                   \
    for (size_t i = 0; i < source->size; ++i) VECTOR_FUNC_NAME_PUSH_BACK(dest, &source->arr[i]);        \
}                                                                                                       \
                                                                                                        \
static void VECTOR_FUNC_NAME_MOVE(VECTOR_NAME** dest, VECTOR_NAME** source)             \
{                                                                                       \
    VECTOR_FUNC_NAME_DESTROY(*dest);                                                    \
    *dest = *source;                                                                    \
    *source = NULL;                                                                     \
}                                                                                       \
                                                                                        \
static size_t PQ_FUNC_NAME_SIZE(PQ_NAME* vec)                                            \
{                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                      \
    return vec->size;                                                                    \
}                                                                                       \
                                                                                        \
static size_t PQ_FUNC_NAME_CAPACITY(PQ_NAME* vec)                                        \
{                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                      \
    return vec->capacity;                                                                \
}                                                                                       \
                                                                                        \
static bool PQ_FUNC_NAME_EMPTY(PQ_NAME* vec)                                             \
{                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                      \
    return vec->size == 0;                                                               \
}                                                                                       \
                                                                                        \
static bool VECTOR_FUNC_NAME_PUSH_BACK(PQ_NAME* vec, TYPE* item)                                \
{                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                      \
    if (vec->size >= vec->capacity)                                                       \
    {                                                                                   \
        size_t new_cap = vec->capacity + vec->capacity / 2 + 1;                           \
        TYPE* new_arr = (TYPE*)realloc(vec->arr, sizeof(TYPE) * new_cap);                \
        if (NULL == new_arr) return false;                                              \
        vec->arr = new_arr;                                                              \
        vec->capacity = new_cap;                                                         \
    }                                                                                   \
    TYPE_REF_COPY_FUNC(&vec->arr[vec->size++], item);                                    \
    return true;                                                                        \
}                                                                                       \
                                                                                        \
static bool VECTOR_FUNC_NAME_POP_BACK(VECTOR_NAME* vec)                      \
{                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                      \
    if (vec->size == 0) return false;                                                    \
    return true;                                                                        \
}                                                                                       \


#endif  // VECTOR_H