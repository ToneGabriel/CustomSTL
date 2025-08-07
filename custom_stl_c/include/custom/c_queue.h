#ifndef _C_QUEUE_H
#define _C_QUEUE_H


#include "custom/c_list.h"


#define _DEFINE_GENERIC_QUEUE_IMPL(                                                                         \
    QUEUE_NAME,                                                                                             \
    QUEUE_LIST_HELPER_NAME,                                                                                 \
    TYPE                                                                                                    \
)                                                                                                           \
                                                                                                            \
typedef struct                                                                                              \
{                                                                                                           \
    QUEUE_LIST_HELPER_NAME list;                                                                            \
} QUEUE_NAME;                                                                                               \
                                                                                                            \
static QUEUE_NAME   C_IDENTIFIER_BIND(QUEUE_NAME, create)();                                                \
static void         C_IDENTIFIER_BIND(QUEUE_NAME, destroy)(QUEUE_NAME* queue);                              \
static void         C_IDENTIFIER_BIND(QUEUE_NAME, clear)(QUEUE_NAME* queue);                                \
static void         C_IDENTIFIER_BIND(QUEUE_NAME, copy)(QUEUE_NAME* dest, const QUEUE_NAME* source);        \
static void         C_IDENTIFIER_BIND(QUEUE_NAME, move)(QUEUE_NAME* dest, QUEUE_NAME* source);              \
static size_t       C_IDENTIFIER_BIND(QUEUE_NAME, size)(QUEUE_NAME* queue);                                 \
static bool         C_IDENTIFIER_BIND(QUEUE_NAME, empty)(QUEUE_NAME* queue);                                \
static void         C_IDENTIFIER_BIND(QUEUE_NAME, insert)(QUEUE_NAME* queue, const TYPE* item);             \
static void         C_IDENTIFIER_BIND(QUEUE_NAME, pop)(QUEUE_NAME* queue);                                  \
static TYPE*        C_IDENTIFIER_BIND(QUEUE_NAME, peek)(QUEUE_NAME* queue);                                 \
static bool         C_IDENTIFIER_BIND(QUEUE_NAME, equals)(const QUEUE_NAME* left, const QUEUE_NAME* right); \
                                                                                                            \
static QUEUE_NAME C_IDENTIFIER_BIND(QUEUE_NAME, create)()                                                   \
{                                                                                                           \
    QUEUE_NAME queue = {                                                                                    \
        .list = C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, create)()                                         \
    };                                                                                                      \
    return queue;                                                                                           \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(QUEUE_NAME, destroy)(QUEUE_NAME* queue)                                       \
{                                                                                                           \
    _ASSERT(NULL != queue, "Queue is NULL");                                                                \
    C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, destroy)(&queue->list);                                       \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(QUEUE_NAME, clear)(QUEUE_NAME* queue)                                         \
{                                                                                                           \
    _ASSERT(NULL != queue, "Queue is NULL");                                                                \
    C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, clear)(&queue->list);                                         \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(QUEUE_NAME, copy)(QUEUE_NAME* dest, const QUEUE_NAME* source)                 \
{                                                                                                           \
    _ASSERT(NULL != dest, "Queue dest is NULL");                                                            \
    _ASSERT(NULL != source, "Queue source is NULL");                                                        \
    C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, copy)(&dest->list, &source->list);                            \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(QUEUE_NAME, move)(QUEUE_NAME* dest, QUEUE_NAME* source)                       \
{                                                                                                           \
    _ASSERT(NULL != dest, "Queue dest is NULL");                                                            \
    _ASSERT(NULL != source, "Queue source is NULL");                                                        \
    C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, move)(&dest->list, &source->list);                            \
}                                                                                                           \
                                                                                                            \
static size_t C_IDENTIFIER_BIND(QUEUE_NAME, size)(QUEUE_NAME* queue)                                        \
{                                                                                                           \
    _ASSERT(NULL != queue, "Queue is NULL");                                                                \
    return C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, size)(&queue->list);                                   \
}                                                                                                           \
                                                                                                            \
static bool C_IDENTIFIER_BIND(QUEUE_NAME, empty)(QUEUE_NAME* queue)                                         \
{                                                                                                           \
    _ASSERT(NULL != queue, "Queue is NULL");                                                                \
    return C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, empty)(&queue->list);                                  \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(QUEUE_NAME, insert)(QUEUE_NAME* queue, const TYPE* item)                      \
{                                                                                                           \
    _ASSERT(NULL != queue, "Queue is NULL");                                                                \
    C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, push_back)(&queue->list, item);                               \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(QUEUE_NAME, pop)(QUEUE_NAME* queue)                                           \
{                                                                                                           \
    _ASSERT(NULL != queue, "Queue is NULL");                                                                \
    C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, pop_front)(&queue->list);                                     \
}                                                                                                           \
                                                                                                            \
static TYPE* C_IDENTIFIER_BIND(QUEUE_NAME, peek)(QUEUE_NAME* queue)                                         \
{                                                                                                           \
    _ASSERT(NULL != queue, "Queue is NULL");                                                                \
    return C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, element_front)(&queue->list);                          \
}                                                                                                           \
                                                                                                            \
static bool C_IDENTIFIER_BIND(QUEUE_NAME, equals)(const QUEUE_NAME* left, const QUEUE_NAME* right)          \
{                                                                                                           \
    _ASSERT(NULL != left, "Queue left is NULL");                                                            \
    _ASSERT(NULL != right, "Queue right is NULL");                                                          \
    return C_IDENTIFIER_BIND(QUEUE_LIST_HELPER_NAME, equals)(&left->list, &right->list);                    \
}                                                                                                           \


#define DEFINE_GENERIC_QUEUE(                                                       \
    QUEUE_NAME_PUBLIC_PREFIX,                                                       \
    TYPE,                                                                           \
    TYPE_REF_EQUALS_FUNC,                                                           \
    TYPE_REF_COPY_FUNC,                                                             \
    TYPE_REF_DELETE_FUNC                                                            \
)                                                                                   \
                                                                                    \
DEFINE_GENERIC_LIST(                                                                \
    C_IDENTIFIER_BIND(QUEUE_NAME_PUBLIC_PREFIX, PRIVATE_LIST),                      \
    TYPE,                                                                           \
    TYPE_REF_EQUALS_FUNC,                                                           \
    TYPE_REF_COPY_FUNC,                                                             \
    TYPE_REF_DELETE_FUNC                                                            \
)                                                                                   \
                                                                                    \
_DEFINE_GENERIC_QUEUE_IMPL(                                                         \
    QUEUE_NAME_PUBLIC_PREFIX,                                                       \
    C_IDENTIFIER_BIND(QUEUE_NAME_PUBLIC_PREFIX, PRIVATE_LIST), /*same as above*/    \
    TYPE                                                                            \
)                                                                                   \


#endif // _C_QUEUE_H