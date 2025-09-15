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
static PQ_NAME  _C_PUBLIC_MEMBER(PQ_NAME, create)();                                                                                    \
static void     _C_PUBLIC_MEMBER(PQ_NAME, destroy)(PQ_NAME* pq);                                                                        \
static void     _C_PUBLIC_MEMBER(PQ_NAME, clear)(PQ_NAME* pq);                                                                          \
static void     _C_PUBLIC_MEMBER(PQ_NAME, copy)(PQ_NAME* dest, const PQ_NAME* source);                                                  \
static void     _C_PUBLIC_MEMBER(PQ_NAME, move)(PQ_NAME* dest, PQ_NAME* source);                                                        \
static size_t   _C_PUBLIC_MEMBER(PQ_NAME, size)(PQ_NAME* pq);                                                                           \
static bool     _C_PUBLIC_MEMBER(PQ_NAME, empty)(PQ_NAME* pq);                                                                          \
static void     _C_PUBLIC_MEMBER(PQ_NAME, insert)(PQ_NAME* pq, const TYPE* item);                                                       \
static void     _C_PUBLIC_MEMBER(PQ_NAME, pop)(PQ_NAME* pq);                                                                            \
static TYPE*    _C_PUBLIC_MEMBER(PQ_NAME, peek)(PQ_NAME* pq);                                                                           \
static bool     _C_PUBLIC_MEMBER(PQ_NAME, equals)(const PQ_NAME* left, const PQ_NAME* right);                                           \
                                                                                                                                        \
/**                                                                                                                                     \
 * @brief Creates new priority queue. Initialize internal vector.                                                                       \
 * @return A new instance of PQ_NAME.                                                                                                   \
 */                                                                                                                                     \
static PQ_NAME _C_PUBLIC_MEMBER(PQ_NAME, create)()                                                                                      \
{                                                                                                                                       \
    PQ_NAME pq = {                                                                                                                      \
        .vec = _C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, create)(GENERIC_VECTOR_DEFAULT_CAPACITY)                                         \
    };                                                                                                                                  \
    return pq;                                                                                                                          \
}                                                                                                                                       \
                                                                                                                                        \
/**                                                                                                                                     \
 * @brief Destroys the priority queue and its internal data.                                                                            \
 * @param pq Pointer to the priority queue.                                                                                             \
 */                                                                                                                                     \
static void _C_PUBLIC_MEMBER(PQ_NAME, destroy)(PQ_NAME* pq)                                                                             \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != pq, "Priority Queue is NULL");                                                                             \
    _C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, destroy)(&pq->vec);                                                                         \
}                                                                                                                                       \
                                                                                                                                        \
/**                                                                                                                                     \
 * @brief Clears all elements from the priority queue without destroying it.                                                            \
 * @param pq Pointer to the priority queue.                                                                                             \
 */                                                                                                                                     \
static void _C_PUBLIC_MEMBER(PQ_NAME, clear)(PQ_NAME* pq)                                                                               \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != pq, "Priority Queue is NULL");                                                                             \
    _C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, clear)(&pq->vec);                                                                           \
}                                                                                                                                       \
                                                                                                                                        \
/**                                                                                                                                     \
 * @brief Copies contents of one pq to another.                                                                                         \
 * @param dest Destination pq pointer.                                                                                                  \
 * @param source Source pq pointer.                                                                                                     \
 */                                                                                                                                     \
static void _C_PUBLIC_MEMBER(PQ_NAME, copy)(PQ_NAME* dest, const PQ_NAME* source)                                                       \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != dest, "PQ dest is NULL");                                                                                  \
    _C_CUSTOM_ASSERT(NULL != source, "PQ source is NULL");                                                                              \
    _C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, copy)(&dest->vec, &source->vec);                                                            \
}                                                                                                                                       \
                                                                                                                                        \
/**                                                                                                                                     \
 * @brief Move contents of one pq to another.                                                                                           \
 * @param dest Destination pq pointer.                                                                                                  \
 * @param source Source pq pointer.                                                                                                     \
 */                                                                                                                                     \
static void _C_PUBLIC_MEMBER(PQ_NAME, move)(PQ_NAME* dest, PQ_NAME* source)                                                             \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != dest, "PQ dest is NULL");                                                                                  \
    _C_CUSTOM_ASSERT(NULL != source, "PQ source is NULL");                                                                              \
    _C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, move)(&dest->vec, &source->vec);                                                            \
}                                                                                                                                       \
                                                                                                                                        \
/**                                                                                                                                     \
 * @brief Returns the number of elements in the priority queue.                                                                         \
 * @param pq Pointer to the priority queue.                                                                                             \
 * @return The number of elements.                                                                                                      \
 */                                                                                                                                     \
static size_t _C_PUBLIC_MEMBER(PQ_NAME, size)(PQ_NAME* pq)                                                                              \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != pq, "Priority Queue is NULL");                                                                             \
    return _C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, size)(&pq->vec);                                                                     \
}                                                                                                                                       \
                                                                                                                                        \
/**                                                                                                                                     \
 * @brief Checks if the priority queue is empty.                                                                                        \
 * @param pq Pointer to the priority queue.                                                                                             \
 * @return `true` if empty, `false` otherwise.                                                                                          \
 */                                                                                                                                     \
static bool _C_PUBLIC_MEMBER(PQ_NAME, empty)(PQ_NAME* pq)                                                                               \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != pq, "Priority Queue is NULL");                                                                             \
    return _C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, empty)(&pq->vec);                                                                    \
}                                                                                                                                       \
                                                                                                                                        \
/**                                                                                                                                     \
 * @brief Inserts a new element into the priority queue.                                                                                \
 * Performs a heapify-up after insertion to maintain heap order.                                                                        \
 * @param pq Pointer to the priority queue.                                                                                             \
 * @param item Pointer to the element to insert.                                                                                        \
 */                                                                                                                                     \
static void _C_PUBLIC_MEMBER(PQ_NAME, insert)(PQ_NAME* pq, const TYPE* item)                                                            \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != pq, "Priority Queue is NULL");                                                                             \
    _C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, push_back)(&pq->vec, item);                                                                 \
    _C_PUBLIC_MEMBER(PQ_HEAPIFY_HELPER_NAME, heapify_up)(                                                                               \
        _C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, data)(&pq->vec),                                                                        \
        _C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, size)(&pq->vec),                                                                        \
        _C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, size)(&pq->vec) - 1);                                                                   \
}                                                                                                                                       \
                                                                                                                                        \
/**                                                                                                                                     \
 * @brief Removes the top element (highest priority) from the priority queue.                                                           \
 * Performs a heapify-down after removal to maintain heap order.                                                                        \
 * @param pq Pointer to the priority queue.                                                                                             \
 */                                                                                                                                     \
static void _C_PUBLIC_MEMBER(PQ_NAME, pop)(PQ_NAME* pq)                                                                                 \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != pq, "Priority Queue is NULL");                                                                             \
    if (_C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, empty)(&pq->vec)) return;                                                               \
    TYPE_REF_COPY_FUNC(                                                                                                                 \
        _C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, element_at)(&pq->vec, 0),                                                               \
        _C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, element_at)(&pq->vec, _C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, size)(&pq->vec) - 1)      \
    );                                                                                                                                  \
    _C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, pop_back)(&pq->vec);                                                                        \
    _C_PUBLIC_MEMBER(PQ_HEAPIFY_HELPER_NAME, heapify_down)(                                                                             \
        _C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, data)(&pq->vec),                                                                        \
        _C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, size)(&pq->vec),                                                                        \
        0                                                                                                                               \
    );                                                                                                                                  \
}                                                                                                                                       \
                                                                                                                                        \
/**                                                                                                                                     \
 * @brief Returns a pointer to the top element (highest priority) of the queue.                                                         \
 * @param pq Pointer to the priority queue.                                                                                             \
 * @return Pointer to the top element.                                                                                                  \
 */                                                                                                                                     \
static TYPE* _C_PUBLIC_MEMBER(PQ_NAME, peek)(PQ_NAME* pq)                                                                               \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != pq, "Priority Queue is NULL");                                                                             \
    return _C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, element_front)(&pq->vec);                                                            \
}                                                                                                                                       \
                                                                                                                                        \
/**                                                                                                                                     \
 * @brief Checks whether two pq are equal by comparing each element.                                                                    \
 * @param left Left-hand side pointer to a pq.                                                                                          \
 * @param right Right-hand side pointer to a pq.                                                                                        \
 * @return `true` if equal, `false` otherwise.                                                                                          \
 */                                                                                                                                     \
static bool _C_PUBLIC_MEMBER(PQ_NAME, equals)(const PQ_NAME* left, const PQ_NAME* right)                                                \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != left, "PQ left is NULL");                                                                                  \
    _C_CUSTOM_ASSERT(NULL != right, "PQ right is NULL");                                                                                \
    return _C_PUBLIC_MEMBER(PQ_VECTOR_HELPER_NAME, equals)(&left->vec, &right->vec);                                                    \
}                                                                                                                                       \


/**
 * @def DEFINE_GENERIC_PRIORITY_QUEUE
 * @brief Fully defines a priority queue for a given type with all required dependencies.
 *
 * This macro instantiates:
 * 
 * - A `PRIVATE_Heap` helper for maintaining priority ordering
 * 
 * - A `PRIVATE_Vector` container for internal storage
 * 
 * - The priority queue API (   `_create`, `_destroy`, `_clear`, `_copy`, `_move`, `_size`, `_empty`,
 *                              `_insert`, `_pop`, `_peek`, `_equals`
 *                          )
 *
 * @param PRIORITY_QUEUE_NAME_PUBLIC_PREFIX   Public prefix (e.g. `MyPQ` → `MyPQ_create`)
 * @param TYPE                                The element type stored in the priority queue
 * @param TYPE_REF_COMPARE_FUNC               Function comparing two `TYPE*` (for priority)
 * @param TYPE_REF_EQUALS_FUNC                Function comparing two `TYPE*` for equality
 * @param TYPE_REF_COPY_FUNC                  Function that copies from `TYPE*` to `TYPE*`
 * @param TYPE_REF_DELETE_FUNC                Function that deletes/frees the internal data of a `TYPE*`
 */
#define DEFINE_GENERIC_PRIORITY_QUEUE(                                                                                      \
    PRIORITY_QUEUE_NAME_PUBLIC_PREFIX,                                                                                      \
    TYPE,                                                                                                                   \
    TYPE_REF_COMPARE_FUNC,                                                                                                  \
    TYPE_REF_EQUALS_FUNC,                                                                                                   \
    TYPE_REF_COPY_FUNC,                                                                                                     \
    TYPE_REF_DELETE_FUNC                                                                                                    \
)                                                                                                                           \
                                                                                                                            \
DEFINE_GENERIC_HEAPIFY_FUNCTIONS(                                                                                           \
    _C_PRIVATE_MEMBER(PRIORITY_QUEUE_NAME_PUBLIC_PREFIX, Heap),                                                             \
    TYPE,                                                                                                                   \
    TYPE_REF_COMPARE_FUNC,                                                                                                  \
    TYPE_REF_COPY_FUNC                                                                                                      \
)                                                                                                                           \
                                                                                                                            \
DEFINE_GENERIC_VECTOR(                                                                                                      \
    _C_PRIVATE_MEMBER(PRIORITY_QUEUE_NAME_PUBLIC_PREFIX, Vector),                                                           \
    TYPE,                                                                                                                   \
    TYPE_REF_EQUALS_FUNC,                                                                                                   \
    TYPE_REF_COPY_FUNC,                                                                                                     \
    TYPE_REF_DELETE_FUNC                                                                                                    \
)                                                                                                                           \
                                                                                                                            \
_DEFINE_GENERIC_PRIORITY_QUEUE_IMPL(                                                                                        \
    PRIORITY_QUEUE_NAME_PUBLIC_PREFIX,                                                                                      \
    _C_PRIVATE_MEMBER(PRIORITY_QUEUE_NAME_PUBLIC_PREFIX, Heap), /*same as above*/                                           \
    _C_PRIVATE_MEMBER(PRIORITY_QUEUE_NAME_PUBLIC_PREFIX, Vector), /*same as above*/                                         \
    TYPE,                                                                                                                   \
    TYPE_REF_COPY_FUNC                                                                                                      \
)                                                                                                                           \


#endif // C_PRIORITY_QUEUE_H