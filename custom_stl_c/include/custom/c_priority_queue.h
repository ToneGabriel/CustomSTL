#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H


#include "custom/__c_core.h"


#define _DEFINE_PRIORITY_QUEUE_IMPL(                                                    \
    PQ_NAME,                                                                            \
    TYPE,                                                                               \
    TYPE_REF_COMPARE_FUNC,                                                              \
    TYPE_REF_COPY_FUNC,                                                                 \
    TYPE_REF_DELETE_FUNC,                                                               \
    _PQ_FUNC_NAME_TYPE_PTR_SWAP,                                                        \
    _PQ_FUNC_NAME_HEAP_INSERT_ADJUST,                                                   \
    _PQ_FUNC_NAME_HEAP_RETRIEVE_ADJUST,                                                 \
    PQ_FUNC_NAME_CREATE,                                                                \
    PQ_FUNC_NAME_DESTROY,                                                               \
    PQ_FUNC_NAME_CLEAR,                                                                 \
    PQ_FUNC_NAME_SIZE,                                                                  \
    PQ_FUNC_NAME_CAPACITY,                                                              \
    PQ_FUNC_NAME_EMPTY,                                                                 \
    PQ_FUNC_NAME_INSERT,                                                                \
    PQ_FUNC_NAME_RETRIEVE,                                                              \
    PQ_FUNC_NAME_PEEK                                                                   \
)                                                                                       \
                                                                                        \
typedef struct                                                                          \
{                                                                                       \
    TYPE* arr;                                                                          \
    size_t size;                                                                        \
    size_t capacity;                                                                    \
} PQ_NAME;                                                                              \
                                                                                        \
static void _PQ_FUNC_NAME_TYPE_PTR_SWAP(TYPE* left, TYPE* right)                        \
{                                                                                       \
    TYPE temp;                                                                          \
    TYPE_REF_COPY_FUNC(&temp, left);                                                    \
    TYPE_REF_COPY_FUNC(left, right);                                                    \
    TYPE_REF_COPY_FUNC(right, &temp);                                                   \
}                                                                                       \
                                                                                        \
static void _PQ_FUNC_NAME_HEAP_INSERT_ADJUST(PQ_NAME* pq, size_t idx)                   \
{                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                      \
    while (idx > 0 && TYPE_REF_COMPARE_FUNC(&pq->arr[idx], &pq->arr[(idx - 1) / 2]))    \
    {                                                                                   \
        _PQ_FUNC_NAME_TYPE_PTR_SWAP(&pq->arr[idx], &pq->arr[(idx - 1) / 2]);            \
        idx = (idx - 1) / 2;                                                            \
    }                                                                                   \
}                                                                                       \
                                                                                        \
static void _PQ_FUNC_NAME_HEAP_RETRIEVE_ADJUST(PQ_NAME* pq, size_t idx)                 \
{                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                      \
    size_t smallest, left, right;                                                       \
    while (idx < pq->size)                                                              \
    {                                                                                   \
        smallest = idx;                                                                 \
        left = 2 * idx + 1;                                                             \
        right = 2 * idx + 2;                                                            \
        if (left < pq->size && TYPE_REF_COMPARE_FUNC(&pq->arr[left], &pq->arr[smallest]))   \
            smallest = left;                                                            \
        if (right < pq->size && TYPE_REF_COMPARE_FUNC(&pq->arr[right], &pq->arr[smallest])) \
            smallest = right;                                                           \
        if (smallest == idx) return;                                                    \
        _PQ_FUNC_NAME_TYPE_PTR_SWAP(&pq->arr[idx], &pq->arr[smallest]);                 \
        idx = smallest;                                                                 \
    }                                                                                   \
}                                                                                       \
                                                                                        \
static PQ_NAME* PQ_FUNC_NAME_CREATE(size_t capacity)                                    \
{                                                                                       \
    _ASSERT(capacity != 0, "Capacity should be greater than 0");                        \
    PQ_NAME* pq = (PQ_NAME*)malloc(sizeof(PQ_NAME));                                    \
    if (NULL == pq) return NULL;                                                        \
    TYPE* temp_arr = (TYPE*)malloc(sizeof(TYPE) * capacity);                            \
    if (NULL == temp_arr)                                                               \
    {                                                                                   \
        free(pq);                                                                       \
        return NULL;                                                                    \
    }                                                                                   \
    pq->arr = temp_arr;                                                                 \
    pq->size = 0;                                                                       \
    pq->capacity = capacity;                                                            \
    return pq;                                                                          \
}                                                                                       \
                                                                                        \
static void PQ_FUNC_NAME_DESTROY(PQ_NAME* pq)                                           \
{                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                      \
    for (size_t i = 0; i < pq->size; ++i) TYPE_REF_DELETE_FUNC(&pq->arr[i]);            \
    free(pq->arr);                                                                      \
    free(pq);                                                                           \
}                                                                                       \
                                                                                        \
static void PQ_FUNC_NAME_CLEAR(PQ_NAME* pq)                                             \
{                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                      \
    for (size_t i = 0; i < pq->size; ++i) TYPE_REF_DELETE_FUNC(&pq->arr[i]);            \
    pq->size = 0;                                                                       \
}                                                                                       \
                                                                                        \
static size_t PQ_FUNC_NAME_SIZE(PQ_NAME* pq)                                            \
{                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                      \
    return pq->size;                                                                    \
}                                                                                       \
                                                                                        \
static size_t PQ_FUNC_NAME_CAPACITY(PQ_NAME* pq)                                        \
{                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                      \
    return pq->capacity;                                                                \
}                                                                                       \
                                                                                        \
static bool PQ_FUNC_NAME_EMPTY(PQ_NAME* pq)                                             \
{                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                      \
    return pq->size == 0;                                                               \
}                                                                                       \
                                                                                        \
static bool PQ_FUNC_NAME_INSERT(PQ_NAME* pq, TYPE item)                                 \
{                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                      \
    if (pq->size >= pq->capacity)                                                       \
    {                                                                                   \
        size_t new_cap = pq->capacity + pq->capacity / 2 + 1;                           \
        TYPE* new_arr = (TYPE*)realloc(pq->arr, sizeof(TYPE) * new_cap);                \
        if (NULL == new_arr) return false;                                              \
        pq->arr = new_arr;                                                              \
        pq->capacity = new_cap;                                                         \
    }                                                                                   \
    TYPE_REF_COPY_FUNC(&pq->arr[pq->size++], &item);                                    \
    _PQ_FUNC_NAME_HEAP_INSERT_ADJUST(pq, pq->size - 1);                                 \
    return true;                                                                        \
}                                                                                       \
                                                                                        \
static bool PQ_FUNC_NAME_RETRIEVE(PQ_NAME* pq, TYPE* out)                               \
{                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                      \
    if (pq->size == 0) return false;                                                    \
    TYPE_REF_COPY_FUNC(out, &pq->arr[0]);                                               \
    TYPE_REF_COPY_FUNC(&pq->arr[0], &pq->arr[--pq->size]);                              \
    TYPE_REF_DELETE_FUNC(&pq->arr[pq->size]);                                           \
    _PQ_FUNC_NAME_HEAP_RETRIEVE_ADJUST(pq, 0);                                          \
    return true;                                                                        \
}                                                                                       \
                                                                                        \
static const TYPE* PQ_FUNC_NAME_PEEK(PQ_NAME* pq)                                       \
{                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                      \
    if (pq->size == 0) return NULL;                                                     \
    return &pq->arr[0];                                                                 \
}                                                                                       \


#define DEFINE_PRIORITY_QUEUE(                                                      \
    PRIORITY_QUEUE_NAME,                                                            \
    TYPE,                                                                           \
    TYPE_REF_COMPARE_FUNC,                                                          \
    TYPE_REF_COPY_FUNC,                                                             \
    TYPE_REF_DELETE_FUNC                                                            \
)                                                                                   \
                                                                                    \
_DEFINE_PRIORITY_QUEUE_IMPL(                                                        \
    PRIORITY_QUEUE_NAME,                                                            \
    TYPE,                                                                           \
    TYPE_REF_COMPARE_FUNC,                                                          \
    TYPE_REF_COPY_FUNC,                                                             \
    TYPE_REF_DELETE_FUNC,                                                           \
    _##PRIORITY_QUEUE_NAME##_type_ptr_swap,                                         \
    _##PRIORITY_QUEUE_NAME##_heap_insert_adjust,                                    \
    _##PRIORITY_QUEUE_NAME##_heap_retrieve_adjust,                                  \
    PRIORITY_QUEUE_NAME##_create,                                                   \
    PRIORITY_QUEUE_NAME##_destroy,                                                  \
    PRIORITY_QUEUE_NAME##_clear,                                                    \
    PRIORITY_QUEUE_NAME##_size,                                                     \
    PRIORITY_QUEUE_NAME##_capacity,                                                 \
    PRIORITY_QUEUE_NAME##_empty,                                                    \
    PRIORITY_QUEUE_NAME##_insert,                                                   \
    PRIORITY_QUEUE_NAME##_retrieve,                                                 \
    PRIORITY_QUEUE_NAME##_peek                                                      \
)                                                                                   \

#endif // PRIORITY_QUEUE_H
