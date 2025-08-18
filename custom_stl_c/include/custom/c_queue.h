#ifndef C_QUEUE_H
#define C_QUEUE_H


#include "custom/c_list.h"


/**
 * @def _DEFINE_GENERIC_QUEUE_IMPL
 * @brief Internal macro that defines the complete implementation of a FIFO queue.
 *
 * The generated queue operates on elements of type `TYPE`.
 * 
 * @param QUEUE_NAME                Public-facing name prefix for struct and functions (e.g., `MyStack` -> `MyStack_create`, etc.)
 * @param QUEUE_LIST_HELPER_NAME    Name prefix for the list implementation used internally (defined beforehand via another macro)
 * @param TYPE                      Type of elements to be stored in the queue.
 */
#define _DEFINE_GENERIC_QUEUE_IMPL(                                                                             \
    QUEUE_NAME,                                                                                                 \
    QUEUE_LIST_HELPER_NAME,                                                                                     \
    TYPE                                                                                                        \
)                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @struct QUEUE_NAME                                                                                           \
 * @brief Struct representing the queue. Contains the internal list.                                            \
 */                                                                                                             \
typedef struct                                                                                                  \
{                                                                                                               \
    QUEUE_LIST_HELPER_NAME list;                                                                                \
} QUEUE_NAME;                                                                                                   \
                                                                                                                \
static QUEUE_NAME   _C_IDENTIFIER_BIND(QUEUE_NAME, create)();                                                   \
static void         _C_IDENTIFIER_BIND(QUEUE_NAME, destroy)(QUEUE_NAME* queue);                                 \
static void         _C_IDENTIFIER_BIND(QUEUE_NAME, clear)(QUEUE_NAME* queue);                                   \
static void         _C_IDENTIFIER_BIND(QUEUE_NAME, copy)(QUEUE_NAME* dest, const QUEUE_NAME* source);           \
static void         _C_IDENTIFIER_BIND(QUEUE_NAME, move)(QUEUE_NAME* dest, QUEUE_NAME* source);                 \
static size_t       _C_IDENTIFIER_BIND(QUEUE_NAME, size)(QUEUE_NAME* queue);                                    \
static bool         _C_IDENTIFIER_BIND(QUEUE_NAME, empty)(QUEUE_NAME* queue);                                   \
static void         _C_IDENTIFIER_BIND(QUEUE_NAME, insert)(QUEUE_NAME* queue, const TYPE* item);                \
static void         _C_IDENTIFIER_BIND(QUEUE_NAME, pop)(QUEUE_NAME* queue);                                     \
static TYPE*        _C_IDENTIFIER_BIND(QUEUE_NAME, peek)(QUEUE_NAME* queue);                                    \
static bool         _C_IDENTIFIER_BIND(QUEUE_NAME, equals)(const QUEUE_NAME* left, const QUEUE_NAME* right);    \
                                                                                                                \
/**                                                                                                             \
 * @brief Creates a new queue. Initialize internal list.                                                        \
 * @return A new instance of QUEUE_NAME.                                                                        \
 */                                                                                                             \
static QUEUE_NAME _C_IDENTIFIER_BIND(QUEUE_NAME, create)()                                                      \
{                                                                                                               \
    QUEUE_NAME queue = {                                                                                        \
        .list = _C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, create)()                                            \
    };                                                                                                          \
    return queue;                                                                                               \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Destroys the queue and its internal data.                                                             \
 * @param queue Pointer to the queue.                                                                           \
 */                                                                                                             \
static void _C_IDENTIFIER_BIND(QUEUE_NAME, destroy)(QUEUE_NAME* queue)                                          \
{                                                                                                               \
    _C_CUSTOM_ASSERT(NULL != queue, "Queue is NULL");                                                           \
    _C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, destroy)(&queue->list);                                          \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Clears all elements from the queue without destroying it.                                             \
 * @param queue Pointer to the queue.                                                                           \
 */                                                                                                             \
static void _C_IDENTIFIER_BIND(QUEUE_NAME, clear)(QUEUE_NAME* queue)                                            \
{                                                                                                               \
    _C_CUSTOM_ASSERT(NULL != queue, "Queue is NULL");                                                           \
    _C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, clear)(&queue->list);                                            \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Copies contents of one queue to another.                                                              \
 * @param dest Destination queue pointer.                                                                       \
 * @param source Source queue pointer.                                                                          \
 */                                                                                                             \
static void _C_IDENTIFIER_BIND(QUEUE_NAME, copy)(QUEUE_NAME* dest, const QUEUE_NAME* source)                    \
{                                                                                                               \
    _C_CUSTOM_ASSERT(NULL != dest, "Queue dest is NULL");                                                       \
    _C_CUSTOM_ASSERT(NULL != source, "Queue source is NULL");                                                   \
    _C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, copy)(&dest->list, &source->list);                               \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Move contents of one queue to another.                                                                \
 * @param dest Destination queue pointer.                                                                       \
 * @param source Source queue pointer.                                                                          \
 */                                                                                                             \
static void _C_IDENTIFIER_BIND(QUEUE_NAME, move)(QUEUE_NAME* dest, QUEUE_NAME* source)                          \
{                                                                                                               \
    _C_CUSTOM_ASSERT(NULL != dest, "Queue dest is NULL");                                                       \
    _C_CUSTOM_ASSERT(NULL != source, "Queue source is NULL");                                                   \
    _C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, move)(&dest->list, &source->list);                               \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Returns the number of elements in the queue.                                                          \
 * @param queue Pointer to the queue.                                                                           \
 * @return The number of elements.                                                                              \
 */                                                                                                             \
static size_t _C_IDENTIFIER_BIND(QUEUE_NAME, size)(QUEUE_NAME* queue)                                           \
{                                                                                                               \
    _C_CUSTOM_ASSERT(NULL != queue, "Queue is NULL");                                                           \
    return _C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, size)(&queue->list);                                      \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Checks if the queue is empty.                                                                         \
 * @param queue Pointer to the queue.                                                                           \
 * @return `true` if empty, `false` otherwise.                                                                  \
 */                                                                                                             \
static bool _C_IDENTIFIER_BIND(QUEUE_NAME, empty)(QUEUE_NAME* queue)                                            \
{                                                                                                               \
    _C_CUSTOM_ASSERT(NULL != queue, "Queue is NULL");                                                           \
    return _C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, empty)(&queue->list);                                     \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Inserts a new element into the queue.                                                                 \
 * @param queue Pointer to the queue.                                                                           \
 * @param item Pointer to the element to insert.                                                                \
 */                                                                                                             \
static void _C_IDENTIFIER_BIND(QUEUE_NAME, insert)(QUEUE_NAME* queue, const TYPE* item)                         \
{                                                                                                               \
    _C_CUSTOM_ASSERT(NULL != queue, "Queue is NULL");                                                           \
    _C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, push_back)(&queue->list, item);                                  \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Removes the first element from the queue.                                                             \
 * @param queue Pointer to the queue.                                                                           \
 */                                                                                                             \
static void _C_IDENTIFIER_BIND(QUEUE_NAME, pop)(QUEUE_NAME* queue)                                              \
{                                                                                                               \
    _C_CUSTOM_ASSERT(NULL != queue, "Queue is NULL");                                                           \
    _C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, pop_front)(&queue->list);                                        \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Returns a pointer to the first element of the queue.                                                  \
 * @param queue Pointer to the queue.                                                                           \
 * @return Pointer to the top element.                                                                          \
 */                                                                                                             \
static TYPE* _C_IDENTIFIER_BIND(QUEUE_NAME, peek)(QUEUE_NAME* queue)                                            \
{                                                                                                               \
    _C_CUSTOM_ASSERT(NULL != queue, "Queue is NULL");                                                           \
    return _C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, element_front)(&queue->list);                             \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Checks whether two queues are equal by comparing each element.                                        \
 * @param left Left-hand side pointer to a queue.                                                               \
 * @param right Right-hand side pointer to a queue.                                                             \
 * @return `true` if equal, `false` otherwise.                                                                  \
 */                                                                                                             \
static bool _C_IDENTIFIER_BIND(QUEUE_NAME, equals)(const QUEUE_NAME* left, const QUEUE_NAME* right)             \
{                                                                                                               \
    _C_CUSTOM_ASSERT(NULL != left, "Queue left is NULL");                                                       \
    _C_CUSTOM_ASSERT(NULL != right, "Queue right is NULL");                                                     \
    return _C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, equals)(&left->list, &right->list);                       \
}                                                                                                               \


/**
 * @def DEFINE_GENERIC_QUEUE
 * @brief Fully defines a queue for a given type with all required dependencies.
 *
 * This macro instantiates:
 * 
 * - A `PRIVATE_List` container for internal storage
 * 
 * - The queue API (`_create`, `_destroy`, `_clear`, `_copy`, `_move`, `_size`, `_empty`,
 *                  `_insert`, `_pop`, `_peek`, `_equals`
 *                  )
 *
 * @param STACK_NAME_PUBLIC_PREFIX      Public prefix (e.g. `MyQueue` → `MyQueue_create`)
 * @param TYPE                          The element type stored in the queue
 * @param TYPE_REF_EQUALS_FUNC          Function comparing two `TYPE*` for equality
 * @param TYPE_REF_COPY_FUNC            Function that copies from `TYPE*` to `TYPE*`
 * @param TYPE_REF_DELETE_FUNC          Function that deletes/frees the internal data of a `TYPE*`
 */
#define DEFINE_GENERIC_QUEUE(                                                                                   \
    QUEUE_NAME_PUBLIC_PREFIX,                                                                                   \
    TYPE,                                                                                                       \
    TYPE_REF_EQUALS_FUNC,                                                                                       \
    TYPE_REF_COPY_FUNC,                                                                                         \
    TYPE_REF_DELETE_FUNC                                                                                        \
)                                                                                                               \
                                                                                                                \
DEFINE_GENERIC_LIST(                                                                                            \
    _C_IDENTIFIER_BIND(QUEUE_NAME_PUBLIC_PREFIX, _C_IDENTIFIER_PRIVATE, _C_IDENTIFIER_LIST),                    \
    TYPE,                                                                                                       \
    TYPE_REF_EQUALS_FUNC,                                                                                       \
    TYPE_REF_COPY_FUNC,                                                                                         \
    TYPE_REF_DELETE_FUNC                                                                                        \
)                                                                                                               \
                                                                                                                \
_DEFINE_GENERIC_QUEUE_IMPL(                                                                                     \
    QUEUE_NAME_PUBLIC_PREFIX,                                                                                   \
    _C_IDENTIFIER_BIND(QUEUE_NAME_PUBLIC_PREFIX, _C_IDENTIFIER_PRIVATE, _C_IDENTIFIER_LIST), /*same as above*/  \
    TYPE                                                                                                        \
)                                                                                                               \


#endif // C_QUEUE_H