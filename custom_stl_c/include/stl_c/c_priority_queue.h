#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H


#include "y_core.h"


#define _DEFINE_PRIORITY_QUEUE_IMPL(                                                    \
    PQ_NAME,                                                                            \
    TYPE,                                                                               \
    TYPE_COMPARE_FUNC,                                                                  \
    _PQ_FUNC_NAME_TYPE_PTR_SWAP,                                                        \
    _PQ_FUNC_NAME_HEAP_INSERT_ADJUST,                                                   \
    _PQ_FUNC_NAME_HEAP_RETRIEVE_ADJUST,                                                 \
    PQ_FUNC_NAME_CREATE,                                                                \
    PQ_FUNC_NAME_DESTROY,                                                               \
    PQ_FUNC_NAME_SIZE,                                                                  \
    PQ_FUNC_NAME_CAPACITY,                                                              \
    PQ_FUNC_NAME_EMPTY,                                                                 \
    PQ_FUNC_NAME_INSERT,                                                                \
    PQ_FUNC_NAME_PEEK,                                                                  \
    PQ_FUNC_NAME_RETRIEVE                                                               \
)                                                                                       \
                                                                                        \
typedef struct                                                                          \
{                                                                                       \
    TYPE** arr;                                                                         \
    size_t size;                                                                        \
    size_t capacity;                                                                    \
} PQ_NAME;                                                                              \
                                                                                        \
static void _PQ_FUNC_NAME_TYPE_PTR_SWAP(TYPE** left, TYPE** right)                      \
{                                                                                       \
    TYPE* temp = *left;                                                                 \
    *left = *right;                                                                     \
    *right = temp;                                                                      \
}                                                                                       \
                                                                                        \
static void _PQ_FUNC_NAME_HEAP_INSERT_ADJUST(PQ_NAME* pq, size_t idx)                   \
{                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                      \
    while (idx > 0 && TYPE_COMPARE_FUNC(pq->arr[idx], pq->arr[(idx - 1) / 2]))          \
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
        if (left < pq->size && TYPE_COMPARE_FUNC(pq->arr[left], pq->arr[smallest]))     \
            smallest = left;                                                            \
        if (right < pq->size && TYPE_COMPARE_FUNC(pq->arr[right], pq->arr[smallest]))   \
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
    TYPE** temp_arr = (TYPE**)malloc(sizeof(TYPE*) * capacity);                         \
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
    for (size_t i = 0; i < pq->size; ++i) free(pq->arr[i]);                             \
    free(pq->arr);                                                                      \
    free(pq);                                                                           \
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
static bool PQ_FUNC_NAME_INSERT(PQ_NAME* pq, TYPE** item)                               \
{                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                      \
    if (NULL == item || NULL == *item) return false;                                    \
    if (pq->size >= pq->capacity)                                                       \
    {                                                                                   \
        size_t new_cap = pq->capacity + pq->capacity / 2 + 1;                           \
        TYPE** new_arr = (TYPE**)realloc(pq->arr, sizeof(TYPE*) * new_cap);             \
        if (NULL == new_arr) return false;                                              \
        pq->arr = new_arr;                                                              \
        pq->capacity = new_cap;                                                         \
    }                                                                                   \
    pq->arr[pq->size++] = *item;                                                        \
    *item = NULL;                                                                       \
    _PQ_FUNC_NAME_HEAP_INSERT_ADJUST(pq, pq->size - 1);                                 \
    return true;                                                                        \
}                                                                                       \
                                                                                        \
static const TYPE* PQ_FUNC_NAME_PEEK(PQ_NAME* pq)                                       \
{                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                      \
    if (pq->size == 0) return NULL;                                                     \
    return pq->arr[0];                                                                  \
}                                                                                       \
                                                                                        \
static TYPE* PQ_FUNC_NAME_RETRIEVE(PQ_NAME* pq)                                         \
{                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                      \
    if (pq->size == 0) return NULL;                                                     \
    TYPE* ret = pq->arr[0];                                                             \
    pq->arr[0] = pq->arr[--pq->size];                                                   \
    _PQ_FUNC_NAME_HEAP_RETRIEVE_ADJUST(pq, 0);                                          \
    return ret;                                                                         \
}                                                                                       \


#define _DEFINE_PRIORITY_QUEUE_BASE(PRIORITY_QUEUE_NAME, TYPE, TYPE_COMPARE_FUNC)   \
_DEFINE_PRIORITY_QUEUE_IMPL(                                                        \
    PRIORITY_QUEUE_NAME,                                                            \
    TYPE,                                                                           \
    TYPE_COMPARE_FUNC,                                                              \
    _##PRIORITY_QUEUE_NAME##_type_ptr_swap,                                         \
    _##PRIORITY_QUEUE_NAME##_heap_insert_adjust,                                    \
    _##PRIORITY_QUEUE_NAME##_heap_retrieve_adjust,                                  \
    PRIORITY_QUEUE_NAME##_create,                                                   \
    PRIORITY_QUEUE_NAME##_destroy,                                                  \
    PRIORITY_QUEUE_NAME##_size,                                                     \
    PRIORITY_QUEUE_NAME##_capacity,                                                 \
    PRIORITY_QUEUE_NAME##_empty,                                                    \
    PRIORITY_QUEUE_NAME##_insert,                                                   \
    PRIORITY_QUEUE_NAME##_peek,                                                     \
    PRIORITY_QUEUE_NAME##_retrieve                                                  \
)                                                                                   \


#define _DEFAULT_TYPE_PTR_LESS_COMPARE(LEFT, RIGHT) (*(LEFT) < *(RIGHT))


#define _DEFINE_PRIORITY_QUEUE_DEFAULT(PRIORITY_QUEUE_NAME, TYPE)   \
_DEFINE_PRIORITY_QUEUE_BASE(PRIORITY_QUEUE_NAME, TYPE, _DEFAULT_TYPE_PTR_LESS_COMPARE)


#define _DEFINE_PRIORITY_QUEUE_CUSTOM(PRIORITY_QUEUE_NAME, TYPE, TYPE_COMPARE_FUNC) \
_DEFINE_PRIORITY_QUEUE_BASE(PRIORITY_QUEUE_NAME, TYPE, TYPE_COMPARE_FUNC)


#define _GET_PRIORITY_QUEUE_DEFINITION(_1, _2, _3, DEFINITION, ...) DEFINITION

/**
 * @def DEFINE_PRIORITY_QUEUE
 * @brief Defines a priority queue structure and its corresponding functions.
 * @param _1 Desired NAME of the structure and functions prefix.
 * @param _2 Data TYPE.
 * @param _3 Optional: Function used to compare values for priority. Must accept two TYPE* arguments and return bool.
 * If this function is not provided, then the default `less` comparator is used.
 */
#define DEFINE_PRIORITY_QUEUE(...)  \
_GET_PRIORITY_QUEUE_DEFINITION(__VA_ARGS__, _DEFINE_PRIORITY_QUEUE_CUSTOM, _DEFINE_PRIORITY_QUEUE_DEFAULT)(__VA_ARGS__)


#endif // PRIORITY_QUEUE_H