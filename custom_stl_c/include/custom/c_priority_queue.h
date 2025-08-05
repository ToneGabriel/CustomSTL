#ifndef C_PRIORITY_QUEUE_H
#define C_PRIORITY_QUEUE_H


#include "custom/c_vector.h"


/**
 * @def _DEFINE_GENERIC_PRIORITY_QUEUE_IMPL
 * @brief Internal macro that defines the complete implementation of a priority queue.
 *
 * This macro generates a full implementation of a min/max heap-based priority queue using a vector as the backing store.
 * The generated queue operates on elements of type `TYPE`, and uses helper macros for heap and vector operations.
 *
 * @param PQ_NAME                   Public-facing name prefix for struct and functions (e.g., `MyPQ` -> `MyPQ_create`, etc.)
 * @param PQ_HEAPIFY_HELPER_NAME    Name prefix of the helper heapify functions (defined beforehand via another macro)
 * @param PQ_VECTOR_HELPER_NAME     Name prefix for the vector implementation used internally (defined beforehand via another macro)
 * @param TYPE                      Type of elements to be stored in the priority queue
 * @param TYPE_REF_COPY_FUNC        Function used to copy one `TYPE*` to another `TYPE*`
 */
#define _DEFINE_GENERIC_PRIORITY_QUEUE_IMPL(                                                                                            \
    PQ_NAME,                                                                                                                            \
    PQ_HEAPIFY_HELPER_NAME,                                                                                                             \
    PQ_VECTOR_HELPER_NAME,                                                                                                              \
    TYPE,                                                                                                                               \
    TYPE_REF_COPY_FUNC                                                                                                                  \
)                                                                                                                                       \
                                                                                                                                        \
/**                                                                                                                                     \
 * @struct PQ_NAME                                                                                                                      \
 * @brief Struct representing the priority queue. Contains the internal vector.                                                         \
 */                                                                                                                                     \
typedef struct                                                                                                                          \
{                                                                                                                                       \
    PQ_VECTOR_HELPER_NAME vec;                                                                                                          \
} PQ_NAME;                                                                                                                              \
                                                                                                                                        \
/**                                                                                                                                     \
 * @brief Creates and initializes a new priority queue.                                                                                 \
 * @return A new instance of PQ_NAME.                                                                                                   \
 */                                                                                                                                     \
static PQ_NAME _C_IDENTIFIER_BIND(PQ_NAME, create)()                                                                                    \
{                                                                                                                                       \
    PQ_NAME pq = {                                                                                                                      \
        .vec = _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, create)(8)                                                                     \
    };                                                                                                                                  \
    return pq;                                                                                                                          \
}                                                                                                                                       \
                                                                                                                                        \
/**                                                                                                                                     \
 * @brief Destroys the priority queue and its internal data.                                                                            \
 * @param pq Pointer to the priority queue.                                                                                             \
 */                                                                                                                                     \
static void _C_IDENTIFIER_BIND(PQ_NAME, destroy)(PQ_NAME* pq)                                                                           \
{                                                                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                                                                      \
    _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, destroy)(&pq->vec);                                                                       \
}                                                                                                                                       \
                                                                                                                                        \
/**                                                                                                                                     \
 * @brief Clears all elements from the priority queue without destroying it.                                                            \
 * @param pq Pointer to the priority queue.                                                                                             \
 */                                                                                                                                     \
static void _C_IDENTIFIER_BIND(PQ_NAME, clear)(PQ_NAME* pq)                                                                             \
{                                                                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                                                                      \
    _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, clear)(&pq->vec);                                                                         \
}                                                                                                                                       \
                                                                                                                                        \
/**                                                                                                                                     \
 * @brief Returns the number of elements in the priority queue.                                                                         \
 * @param pq Pointer to the priority queue.                                                                                             \
 * @return The number of elements.                                                                                                      \
 */                                                                                                                                     \
static size_t _C_IDENTIFIER_BIND(PQ_NAME, size)(PQ_NAME* pq)                                                                            \
{                                                                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                                                                      \
    return _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, size)(&pq->vec);                                                                   \
}                                                                                                                                       \
                                                                                                                                        \
/**                                                                                                                                     \
 * @brief Checks if the priority queue is empty.                                                                                        \
 * @param pq Pointer to the priority queue.                                                                                             \
 * @return `true` if empty, `false` otherwise.                                                                                          \
 */                                                                                                                                     \
static bool _C_IDENTIFIER_BIND(PQ_NAME, empty)(PQ_NAME* pq)                                                                             \
{                                                                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                                                                      \
    return _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, empty)(&pq->vec);                                                                  \
}                                                                                                                                       \
                                                                                                                                        \
/**                                                                                                                                     \
 * @brief Inserts a new element into the priority queue.                                                                                \
 * Performs a heapify-up after insertion to maintain heap order.                                                                        \
 * @param pq Pointer to the priority queue.                                                                                             \
 * @param item Pointer to the element to insert.                                                                                        \
 */                                                                                                                                     \
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
/**                                                                                                                                     \
 * @brief Removes the top element (highest priority) from the priority queue.                                                           \
 * Performs a heapify-down after removal to maintain heap order.                                                                        \
 * @param pq Pointer to the priority queue.                                                                                             \
 */                                                                                                                                     \
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
/**                                                                                                                                     \
 * @brief Returns a pointer to the top element (highest priority) of the queue.                                                         \
 * @param pq Pointer to the priority queue.                                                                                             \
 * @return Pointer to the top element.                                                                                                  \
 */                                                                                                                                     \
static TYPE* _C_IDENTIFIER_BIND(PQ_NAME, peek)(PQ_NAME* pq)                                                                             \
{                                                                                                                                       \
    _ASSERT(NULL != pq, "Priority Queue is NULL");                                                                                      \
    return _C_IDENTIFIER_BIND(PQ_VECTOR_HELPER_NAME, element_front)(&pq->vec);                                                          \
}                                                                                                                                       \


/**
 * @def DEFINE_GENERIC_PRIORITY_QUEUE
 * @brief Fully defines a priority queue for a given type with all required dependencies.
 *
 * This macro instantiates:
 * - A heapify helper for maintaining priority ordering
 * - A dynamic vector container for internal storage
 * - The priority queue API (create, insert, pop, etc.)
 *
 * @param PRIORITY_QUEUE_NAME_PUBLIC_PREFIX   Public prefix (e.g. `MyPQ` → `MyPQ_create`)
 * @param PRIORITY_QUEUE_NAME_PRIVATE_PREFIX  Private helper prefix (for heap/vector implementation)
 * @param TYPE                                The element type stored in the priority queue
 * @param TYPE_REF_COMPARE_FUNC               Function comparing two `TYPE*` (for priority)
 * @param TYPE_REF_EQUALS_FUNC                Function comparing two `TYPE*` for equality
 * @param TYPE_REF_COPY_FUNC                  Function that copies from `TYPE*` to `TYPE*`
 * @param TYPE_REF_DELETE_FUNC                Function that deletes/frees a `TYPE*`
 */
#define DEFINE_GENERIC_PRIORITY_QUEUE(                                              \
    PRIORITY_QUEUE_NAME_PUBLIC_PREFIX,                                              \
    PRIORITY_QUEUE_NAME_PRIVATE_PREFIX,                                             \
    TYPE,                                                                           \
    TYPE_REF_COMPARE_FUNC,                                                          \
    TYPE_REF_EQUALS_FUNC,                                                           \
    TYPE_REF_COPY_FUNC,                                                             \
    TYPE_REF_DELETE_FUNC                                                            \
)                                                                                   \
                                                                                    \
DEFINE_GENERIC_HEAPIFY_FUNCTIONS(                                                   \
    _C_IDENTIFIER_BIND(PRIORITY_QUEUE_NAME_PRIVATE_PREFIX, HEAP),                   \
    _C_IDENTIFIER_BIND(PRIORITY_QUEUE_NAME_PRIVATE_PREFIX, HEAP_PRIVATE),           \
    TYPE,                                                                           \
    TYPE_REF_COMPARE_FUNC,                                                          \
    TYPE_REF_COPY_FUNC                                                              \
)                                                                                   \
                                                                                    \
DEFINE_GENERIC_VECTOR(                                                              \
    _C_IDENTIFIER_BIND(PRIORITY_QUEUE_NAME_PRIVATE_PREFIX, VECTOR),                 \
    _C_IDENTIFIER_BIND(PRIORITY_QUEUE_NAME_PRIVATE_PREFIX, VECTOR_PRIVATE),         \
    TYPE,                                                                           \
    TYPE_REF_EQUALS_FUNC,                                                           \
    TYPE_REF_COPY_FUNC,                                                             \
    TYPE_REF_DELETE_FUNC                                                            \
)                                                                                   \
                                                                                    \
_DEFINE_GENERIC_PRIORITY_QUEUE_IMPL(                                                \
    PRIORITY_QUEUE_NAME_PUBLIC_PREFIX,                                              \
    _C_IDENTIFIER_BIND(PRIORITY_QUEUE_NAME_PRIVATE_PREFIX, HEAP),                   \
    _C_IDENTIFIER_BIND(PRIORITY_QUEUE_NAME_PRIVATE_PREFIX, VECTOR),                 \
    TYPE,                                                                           \
    TYPE_REF_COPY_FUNC                                                              \
)                                                                                   \


#endif // C_PRIORITY_QUEUE_H