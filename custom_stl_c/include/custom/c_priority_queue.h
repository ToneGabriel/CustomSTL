#ifndef C_PRIORITY_QUEUE_H
#define C_PRIORITY_QUEUE_H


#include "custom/c_vector.h"


#define _DEFINE_PRIORITY_QUEUE_IMPL(                                                                                                    \
    PQ_NAME,                                                                                                                            \
    PQ_HEAPIFY_HELPER_NAME,                                                                                                             \
    PQ_VECTOR_HELPER_NAME,                                                                                                              \
    TYPE,                                                                                                                               \
    TYPE_REF_COMPARE_FUNC,                                                                                                              \
    TYPE_REF_EQUALS_FUNC,                                                                                                               \
    TYPE_REF_COPY_FUNC,                                                                                                                 \
    TYPE_REF_DELETE_FUNC                                                                                                                \
)                                                                                                                                       \
                                                                                                                                        \
DEFINE_GENERIC_HEAPIFY_FUNCTIONS(                                                                                                       \
    PQ_HEAPIFY_HELPER_NAME,                                                                                                             \
    TYPE,                                                                                                                               \
    TYPE_REF_COMPARE_FUNC,                                                                                                              \
    TYPE_REF_COPY_FUNC)                                                                                                                 \
                                                                                                                                        \
DEFINE_GENERIC_VECTOR(                                                                                                                  \
    PQ_VECTOR_HELPER_NAME,                                                                                                              \
    TYPE,                                                                                                                               \
    TYPE_REF_EQUALS_FUNC,                                                                                                               \
    TYPE_REF_COPY_FUNC,                                                                                                                 \
    TYPE_REF_DELETE_FUNC)                                                                                                               \
                                                                                                                                        \
typedef struct                                                                                                                          \
{                                                                                                                                       \
    PQ_VECTOR_HELPER_NAME vec;                                                                                                          \
} PQ_NAME;                                                                                                                              \
                                                                                                                                        \
static PQ_NAME _C_IDENTIFIER_BIND(PQ_NAME, create)()                                                                                    \
{                                                                                                                                       \
    PQ_NAME pq = {                                                                                                                      \
        .vec = _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, create)(8)                                                                     \
    };                                                                                                                                  \
    return pq;                                                                                                                          \
}                                                                                                                                       \
                                                                                                                                        \
static void _C_IDENTIFIER_BIND(PQ_NAME, destroy)(PQ_NAME* pq)                                                                           \
{                                                                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                                                                      \
    _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, destroy)(&pq->vec);                                                                       \
}                                                                                                                                       \
                                                                                                                                        \
static void _C_IDENTIFIER_BIND(PQ_NAME, clear)(PQ_NAME* pq)                                                                             \
{                                                                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                                                                      \
    _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, clear)(&pq->vec);                                                                         \
}                                                                                                                                       \
                                                                                                                                        \
static size_t _C_IDENTIFIER_BIND(PQ_NAME, size)(PQ_NAME* pq)                                                                            \
{                                                                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                                                                      \
    return _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, size)(&pq->vec);                                                                   \
}                                                                                                                                       \
                                                                                                                                        \
static bool _C_IDENTIFIER_BIND(PQ_NAME, empty)(PQ_NAME* pq)                                                                             \
{                                                                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                                                                      \
    return _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, empty)(&pq->vec);                                                                  \
}                                                                                                                                       \
                                                                                                                                        \
static void _C_IDENTIFIER_BIND(PQ_NAME, insert)(PQ_NAME* pq, const TYPE* item)                                                          \
{                                                                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                                                                      \
    _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, push_back)(&pq->vec, item);                                                               \
    _C_IDENTIFIER_BIND(PQ_HEAPIFY_HELPER_NAME, heapify_up)(                                                                             \
        _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, data)(&pq->vec),                                                                      \
        _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, size)(&pq->vec),                                                                      \
        _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, size)(&pq->vec) - 1);                                                                 \
}                                                                                                                                       \
                                                                                                                                        \
static void _C_IDENTIFIER_BIND(PQ_NAME, pop)(PQ_NAME* pq)                                                                               \
{                                                                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                                                                      \
    if (_C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, empty)(&pq->vec)) return;                                                             \
    TYPE_REF_COPY_FUNC(                                                                                                                 \
        _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, element_at)(&pq->vec, 0),                                                             \
        _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, element_at)(&pq->vec, _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, size)(&pq->vec) - 1)  \
    );                                                                                                                                  \
    _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, pop_back)(&pq->vec);                                                                      \
    _C_IDENTIFIER_BIND(PQ_HEAPIFY_HELPER_NAME, heapify_down)(                                                                           \
        _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, data)(&pq->vec),                                                                      \
        _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, size)(&pq->vec),                                                                      \
        0                                                                                                                               \
    );                                                                                                                                  \
}                                                                                                                                       \
                                                                                                                                        \
static TYPE* _C_IDENTIFIER_BIND(PQ_NAME, peek)(PQ_NAME* pq)                                                                             \
{                                                                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                                                                      \
    return _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, element_front)(&pq->vec);                                                          \
}                                                                                                                                       \


#define DEFINE_PRIORITY_QUEUE(                                                      \
    PRIORITY_QUEUE_NAME,                                                            \
    TYPE,                                                                           \
    TYPE_REF_COMPARE_FUNC,                                                          \
    TYPE_REF_EQUALS_FUNC,                                                           \
    TYPE_REF_COPY_FUNC,                                                             \
    TYPE_REF_DELETE_FUNC                                                            \
)                                                                                   \
                                                                                    \
_DEFINE_PRIORITY_QUEUE_IMPL(                                                        \
    PRIORITY_QUEUE_NAME,                                                            \
    _C_IDENTIFIER_HELPER_PREFIX(_C_IDENTIFIER_BIND(PRIORITY_QUEUE_NAME, HEAP)),     \
    _C_IDENTIFIER_HELPER_PREFIX(_C_IDENTIFIER_BIND(PRIORITY_QUEUE_NAME, VECTOR)),   \
    TYPE,                                                                           \
    TYPE_REF_COMPARE_FUNC,                                                          \
    TYPE_REF_EQUALS_FUNC,                                                           \
    TYPE_REF_COPY_FUNC,                                                             \
    TYPE_REF_DELETE_FUNC                                                            \
)                                                                                   \


#endif // C_PRIORITY_QUEUE_H
