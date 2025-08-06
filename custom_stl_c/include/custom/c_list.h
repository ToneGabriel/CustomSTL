#ifndef _C_LIST_H
#define _C_LIST_H


#include "custom/_c_node.h"
#include "custom/c_utility.h"


#define _DEFINE_GENERIC_LIST_IMPL(                                                                          \
    LIST_NAME,                                                                                              \
    NODE_NAME,                                                                                              \
    TYPE,                                                                                                   \
    TYPE_REF_EQUALS_FUNC,                                                                                   \
    TYPE_REF_COPY_FUNC,                                                                                     \
    TYPE_REF_DELETE_FUNC                                                                                    \
)                                                                                                           \
                                                                                                            \
typedef struct                                                                                              \
{                                                                                                           \
    size_t size;                                                                                            \
    NODE_NAME* head;                                                                                        \
} LIST_NAME;                                                                                                \
                                                                                                            \
static LIST_NAME    C_IDENTIFIER_BIND(LIST_NAME, create)();                                                 \
static void         C_IDENTIFIER_BIND(LIST_NAME, destroy)(LIST_NAME* list);                                 \
static void         C_IDENTIFIER_BIND(LIST_NAME, clear)(LIST_NAME* list);                                   \
static void         C_IDENTIFIER_BIND(LIST_NAME, copy)(LIST_NAME* dest, const LIST_NAME* source);           \
static void         C_IDENTIFIER_BIND(LIST_NAME, move)(LIST_NAME* dest, LIST_NAME* source);                 \
static size_t       C_IDENTIFIER_BIND(LIST_NAME, size)(LIST_NAME* list);                                    \
static bool         C_IDENTIFIER_BIND(LIST_NAME, empty)(const LIST_NAME* list);                             \
static void         C_IDENTIFIER_BIND(LIST_NAME, push_back)(LIST_NAME* list, const TYPE* item);             \
static void         C_IDENTIFIER_BIND(LIST_NAME, push_front)(LIST_NAME* list, const TYPE* item);            \
static void         C_IDENTIFIER_BIND(LIST_NAME, pop_back)(LIST_NAME* list);                                \
static void         C_IDENTIFIER_BIND(LIST_NAME, pop_front)(LIST_NAME* list);                               \
static TYPE*        C_IDENTIFIER_BIND(LIST_NAME, element_front)(LIST_NAME* list);                           \
static TYPE*        C_IDENTIFIER_BIND(LIST_NAME, element_back)(LIST_NAME* list);                            \
static bool         C_IDENTIFIER_BIND(LIST_NAME, equals)(const LIST_NAME* left, const LIST_NAME* right);    \
                                                                                                            \
static void         C_IDENTIFIER_BIND(LIST_NAME, PRIVATE_unlink_node)(LIST_NAME* list, NODE_NAME* node);    \
static void         C_IDENTIFIER_BIND(LIST_NAME, PRIVATE_destroy_node)(LIST_NAME* list, NODE_NAME* node);   \
                                                                                                            \
static void C_IDENTIFIER_BIND(LIST_NAME, PRIVATE_unlink_node)(LIST_NAME* list, NODE_NAME* node)             \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    _ASSERT(node->next != NULL && node->prev != NULL, "Unlink only if linked");                             \
    node->prev->next = node->next;                                                                          \
    node->next->prev = node->prev;                                                                          \
    node->next = node->prev = NULL;                                                                         \
    --list->size;                                                                                           \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(LIST_NAME, PRIVATE_destroy_node)(LIST_NAME* list, NODE_NAME* node)            \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    C_IDENTIFIER_BIND(LIST_NAME, PRIVATE_unlink_node)(list, node);                                          \
    TYPE_REF_DELETE_FUNC(&node->value);                                                                     \
    free(node);                                                                                             \
}                                                                                                           \
                                                                                                            \
static LIST_NAME C_IDENTIFIER_BIND(LIST_NAME, create)()                                                     \
{                                                                                                           \
    LIST_NAME list = {                                                                                      \
        .size = 0,                                                                                          \
        .head = (NODE_NAME*)malloc(sizeof(NODE_NAME))                                                       \
    };                                                                                                      \
    list.head->next = list.head->prev = list.head; /*head value remains default*/                           \
    return list;                                                                                            \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(LIST_NAME, destroy)(LIST_NAME* list)                                          \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    if (NULL == list->head) return;                                                                         \
    C_IDENTIFIER_BIND(LIST_NAME, clear)(list);                                                              \
    free(list->head);                                                                                       \
    list->size = 0;                                                                                         \
    list->head = NULL;                                                                                      \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(LIST_NAME, clear)(LIST_NAME* list)                                            \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    while (list->size)                                                                                      \
        C_IDENTIFIER_BIND(LIST_NAME, pop_back)(list);                                                       \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(LIST_NAME, copy)(LIST_NAME* dest, const LIST_NAME* source)                    \
{                                                                                                           \
    _ASSERT(NULL != dest, "List dest is NULL");                                                             \
    _ASSERT(NULL != source, "List source is NULL");                                                         \
    /*TODO: implement*/                                                                                     \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(LIST_NAME, move)(LIST_NAME* dest, LIST_NAME* source)                          \
{                                                                                                           \
    _ASSERT(NULL != dest, "List dest is NULL");                                                             \
    _ASSERT(NULL != source, "List source is NULL");                                                         \
    /*TODO: implement*/                                                                                     \
}                                                                                                           \
                                                                                                            \
static size_t C_IDENTIFIER_BIND(LIST_NAME, size)(LIST_NAME* list)                                           \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    return list->size;                                                                                      \
}                                                                                                           \
                                                                                                            \
static bool C_IDENTIFIER_BIND(LIST_NAME, empty)(const LIST_NAME* list)                                      \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    return 0 == list->size;                                                                                 \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(LIST_NAME, push_back)(LIST_NAME* list, const TYPE* item)                      \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    /*TODO: implement*/                                                                                     \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(LIST_NAME, push_front)(LIST_NAME* list, const TYPE* item)                     \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    /*TODO: implement*/                                                                                     \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(LIST_NAME, pop_back)(LIST_NAME* list)                                         \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    if (0 == list->size) return;                                                                            \
    C_IDENTIFIER_BIND(LIST_NAME, PRIVATE_destroy_node)(list, list->head->prev);                             \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(LIST_NAME, pop_front)(LIST_NAME* list)                                        \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    if (0 == list->size) return;                                                                            \
    C_IDENTIFIER_BIND(LIST_NAME, PRIVATE_destroy_node)(list, list->head->next);                             \
}                                                                                                           \
                                                                                                            \
static TYPE* C_IDENTIFIER_BIND(LIST_NAME, element_front)(LIST_NAME* list)                                   \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    _ASSERT(list->size != 0, "List is empty");                                                              \
    return &list->head->next->value;                                                                        \
}                                                                                                           \
                                                                                                            \
static TYPE* C_IDENTIFIER_BIND(LIST_NAME, element_back)(LIST_NAME* list)                                    \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    _ASSERT(list->size != 0, "List is empty");                                                              \
    return &list->head->prev->value;                                                                        \
}                                                                                                           \
                                                                                                            \
static bool C_IDENTIFIER_BIND(LIST_NAME, equals)(const LIST_NAME* left, const LIST_NAME* right)             \
{                                                                                                           \
    _ASSERT(NULL != left, "List left is NULL");                                                             \
    _ASSERT(NULL != right, "List right is NULL");                                                           \
    if (left->size != right->size) return false;                                                            \
    return true; /*TODO: implement*/                                                                        \
}                                                                                                           \


#define DEFINE_GENERIC_LIST(                                                        \
    LIST_NAME_PUBLIC_PREFIX,                                                        \
    TYPE,                                                                           \
    TYPE_REF_EQUALS_FUNC,                                                           \
    TYPE_REF_COPY_FUNC,                                                             \
    TYPE_REF_DELETE_FUNC                                                            \
)                                                                                   \
                                                                                    \
DEFINE_GENERIC_DOUBLE_NODE(                                                         \
    C_IDENTIFIER_BIND(LIST_NAME_PUBLIC_PREFIX, PRIVATE_NODE),                       \
    TYPE                                                                            \
)                                                                                   \
                                                                                    \
_DEFINE_GENERIC_LIST_IMPL(                                                          \
    LIST_NAME_PUBLIC_PREFIX,                                                        \
    C_IDENTIFIER_BIND(LIST_NAME_PUBLIC_PREFIX, PRIVATE_NODE), /*same as above*/     \
    TYPE,                                                                           \
    TYPE_REF_EQUALS_FUNC,                                                           \
    TYPE_REF_COPY_FUNC,                                                             \
    TYPE_REF_DELETE_FUNC                                                            \
)                                                                                   \


// DEFINE_GENERIC_LIST(MyListINT, int, DEFAULT_TYPE_REF_EQUALS, DEFAULT_TYPE_REF_COPY, DEFAULT_TYPE_REF_DELETE)


#endif  // _C_LIST_H