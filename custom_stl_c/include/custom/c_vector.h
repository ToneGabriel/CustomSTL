#ifndef VECTOR_H
#define VECTOR_H


#include "custom/c_utility.h"


#define _DEFINE_GENERIC_VECTOR_IMPL(                                                    \
    VECTOR_NAME,                                                                        \
    TYPE,                                                                               \
    TYPE_REF_EQUALS_FUNC,                                                               \
    TYPE_REF_COPY_FUNC,                                                                 \
    TYPE_REF_DELETE_FUNC,                                                               \
    VECTOR_FUNC_NAME_CREATE,                                                            \
    VECTOR_FUNC_NAME_DESTROY,                                                           \
    VECTOR_FUNC_NAME_CLEAR,                                                             \
    VECTOR_FUNC_NAME_COPY,                                                              \
    VECTOR_FUNC_NAME_MOVE,                                                              \
    VECTOR_FUNC_NAME_SIZE,                                                              \
    VECTOR_FUNC_NAME_CAPACITY,                                                          \
    VECTOR_FUNC_NAME_EMPTY,                                                             \
    VECTOR_FUNC_NAME_PUSH_BACK,                                                         \
    VECTOR_FUNC_NAME_POP_BACK,                                                          \
    VECTOR_FUNC_NAME_ELEMENT_FRONT,                                                     \
    VECTOR_FUNC_NAME_ELEMENT_BACK,                                                      \
    VECTOR_FUNC_NAME_ELEMENT_AT,                                                        \
    VECTOR_FUNC_NAME_EQUALS                                                             \
)                                                                                       \
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
        .arr = (TYPE*)malloc(sizeof(TYPE) * capacity),                                  \
        .size = 0,                                                                      \
        .capacity = capacity                                                            \
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
    *dest = VECTOR_FUNC_NAME_CREATE(source->capacity);                                  \
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
static size_t PQ_FUNC_NAME_SIZE(const VECTOR_NAME* vec)                                 \
{                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                             \
    return vec->size;                                                                   \
}                                                                                       \
                                                                                        \
static size_t PQ_FUNC_NAME_CAPACITY(const VECTOR_NAME* vec)                             \
{                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                             \
    return vec->capacity;                                                               \
}                                                                                       \
                                                                                        \
static bool PQ_FUNC_NAME_EMPTY(const VECTOR_NAME* vec)                                  \
{                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                             \
    return vec->size == 0;                                                              \
}                                                                                       \
                                                                                        \
static void VECTOR_FUNC_NAME_PUSH_BACK(VECTOR_NAME* vec, const TYPE* item)                  \
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
                                                                                        \
static TYPE* VECTOR_FUNC_NAME_ELEMENT_FRONT(VECTOR_NAME* vec)                           \
{                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                             \
    _ASSERT(vec->size != 0, "Vector is empty");                                         \
    return &vec->arr[0];                                                                \
}                                                                                       \
                                                                                        \
static TYPE* VECTOR_FUNC_NAME_ELEMENT_BACK(VECTOR_NAME* vec)                            \
{                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                             \
    _ASSERT(vec->size != 0, "Vector is empty");                                         \
    return &vec->arr[vec->size - 1];                                                    \
}                                                                                       \
                                                                                        \
static TYPE* VECTOR_FUNC_NAME_ELEMENT_AT(VECTOR_NAME* vec, size_t index)                \
{                                                                                       \
    _ASSERT(NULL != vec, "Vector is NULL");                                             \
    _ASSERT(vec->size != 0, "Vector is empty");                                         \
    _ASSERT(index < vec->size, "Index out of bounds");                                  \
    return &vec->arr[index];                                                            \
}                                                                                       \
                                                                                        \
static bool VECTOR_FUNC_NAME_EQUALS(const VECTOR_NAME* left, const VECTOR_NAME* right)  \
{                                                                                       \
    _ASSERT(NULL != left, "Vector left is NULL");                                       \
    _ASSERT(NULL != right, "Vector right is NULL");                                     \
    if (left->size != right->size) return false;                                        \
    for (size_t i = 0; i < left->size; ++i)                                             \
        if (!TYPE_REF_EQUALS_FUNC(&left->arr[i], &right->arr[i])) return false;         \
    return true;                                                                        \
}                                                                                       \


#define DEFINE_GENERIC_VECTOR(                                                      \
    VECTOR_NAME,                                                                    \
    TYPE,                                                                           \
    TYPE_REF_EQUALS_FUNC,                                                           \
    TYPE_REF_COPY_FUNC,                                                             \
    TYPE_REF_DELETE_FUNC                                                            \
)                                                                                   \
                                                                                    \
_DEFINE_GENERIC_VECTOR_IMPL(                                                        \
    VECTOR_NAME,                                                                    \
    TYPE,                                                                           \
    TYPE_REF_EQUALS_FUNC,                                                           \
    TYPE_REF_COPY_FUNC,                                                             \
    TYPE_REF_DELETE_FUNC,                                                           \
    VECTOR_NAME##_create,                                                           \
    VECTOR_NAME##_destroy,                                                          \
    VECTOR_NAME##_clear,                                                            \
    VECTOR_NAME##_copy,                                                             \
    VECTOR_NAME##_move,                                                             \
    VECTOR_NAME##_size,                                                             \
    VECTOR_NAME##_capacity,                                                         \
    VECTOR_NAME##_empty,                                                            \
    VECTOR_NAME##_push_back,                                                        \
    VECTOR_NAME##_pop_back,                                                         \
    VECTOR_NAME##_element_front,                                                    \
    VECTOR_NAME##_element_back,                                                     \
    VECTOR_NAME##_element_at,                                                       \
    VECTOR_NAME##_equals                                                            \
)                                                                                   \


#endif  // VECTOR_H