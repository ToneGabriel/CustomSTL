#ifndef C_LIST_H
#define C_LIST_H


#include "custom/_c_node.h"
#include "custom/c_utility.h"


#define _DEFINE_GENERIC_LIST_DATA(      \
    LIST_NAME,                          \
    NODE_NAME                           \
)                                       \
                                        \
typedef struct                          \
{                                       \
    size_t size;                        \
    NODE_NAME* head;                    \
} LIST_NAME;                            \


#define _DEFINE_GENERIC_LIST_ITERATOR(                                                                                          \
    LIST_ITERATOR_NAME,                                                                                                         \
    LIST_NAME,                                                                                                                  \
    NODE_NAME,                                                                                                                  \
    TYPE                                                                                                                        \
)                                                                                                                               \
                                                                                                                                \
typedef struct                                                                                                                  \
{                                                                                                                               \
    NODE_NAME* node;                                                                                                            \
    LIST_NAME* list;                                                                                                            \
} LIST_ITERATOR_NAME;                                                                                                           \
                                                                                                                                \
static LIST_ITERATOR_NAME   C_IDENTIFIER_BIND(LIST_ITERATOR_NAME, create)(NODE_NAME* node, LIST_NAME* list);                    \
static void                 C_IDENTIFIER_BIND(LIST_ITERATOR_NAME, pre_increment)(LIST_ITERATOR_NAME* iter);                     \
static LIST_ITERATOR_NAME   C_IDENTIFIER_BIND(LIST_ITERATOR_NAME, post_increment)(LIST_ITERATOR_NAME* iter);                    \
static void                 C_IDENTIFIER_BIND(LIST_ITERATOR_NAME, pre_decrement)(LIST_ITERATOR_NAME* iter);                     \
static LIST_ITERATOR_NAME   C_IDENTIFIER_BIND(LIST_ITERATOR_NAME, post_decrement)(LIST_ITERATOR_NAME* iter);                    \
static TYPE*                C_IDENTIFIER_BIND(LIST_ITERATOR_NAME, deref)(LIST_ITERATOR_NAME* iter);                             \
static bool                 C_IDENTIFIER_BIND(LIST_ITERATOR_NAME, equals)(LIST_ITERATOR_NAME* left, LIST_ITERATOR_NAME* right); \
                                                                                                                                \
static LIST_ITERATOR_NAME C_IDENTIFIER_BIND(LIST_ITERATOR_NAME, create)(NODE_NAME* node, LIST_NAME* list)                       \
{                                                                                                                               \
    LIST_ITERATOR_NAME iter = {                                                                                                 \
        .node = node,                                                                                                           \
        .list = list                                                                                                            \
    };                                                                                                                          \
    return iter;                                                                                                                \
}                                                                                                                               \
                                                                                                                                \
static void C_IDENTIFIER_BIND(LIST_ITERATOR_NAME, pre_increment)(LIST_ITERATOR_NAME* iter)                                      \
{                                                                                                                               \
    _C_CUSTOM_ASSERT(iter->node != iter->list->head, "Cannot increment end iterator.");                                         \
    iter->node = iter->node->next;                                                                                              \
}                                                                                                                               \
                                                                                                                                \
static LIST_ITERATOR_NAME C_IDENTIFIER_BIND(LIST_ITERATOR_NAME, post_increment)(LIST_ITERATOR_NAME* iter)                       \
{                                                                                                                               \
    LIST_ITERATOR_NAME temp = *iter;                                                                                            \
    C_IDENTIFIER_BIND(LIST_ITERATOR_NAME, pre_increment)(iter);                                                                 \
    return temp;                                                                                                                \
}                                                                                                                               \
                                                                                                                                \
static void C_IDENTIFIER_BIND(LIST_ITERATOR_NAME, pre_decrement)(LIST_ITERATOR_NAME* iter)                                      \
{                                                                                                                               \
    _C_CUSTOM_ASSERT(iter->node != iter->list->head->next, "Cannot decrement begin iterator.");                                 \
    iter->node = iter->node->prev;                                                                                              \
}                                                                                                                               \
                                                                                                                                \
static LIST_ITERATOR_NAME C_IDENTIFIER_BIND(LIST_ITERATOR_NAME, post_decrement)(LIST_ITERATOR_NAME* iter)                       \
{                                                                                                                               \
    LIST_ITERATOR_NAME temp = *iter;                                                                                            \
    C_IDENTIFIER_BIND(LIST_ITERATOR_NAME, pre_decrement)(iter);                                                                 \
    return temp;                                                                                                                \
}                                                                                                                               \
                                                                                                                                \
static TYPE* C_IDENTIFIER_BIND(LIST_ITERATOR_NAME, deref)(LIST_ITERATOR_NAME* iter)                                             \
{                                                                                                                               \
    _C_CUSTOM_ASSERT(iter->node != iter->list->head, "Cannot dereference end iterator.");                                       \
    return &iter->node->value;                                                                                                  \
}                                                                                                                               \
                                                                                                                                \
static bool C_IDENTIFIER_BIND(LIST_ITERATOR_NAME, equals)(LIST_ITERATOR_NAME* left, LIST_ITERATOR_NAME* right)                  \
{                                                                                                                               \
    return left->node == right->node;                                                                                           \
}                                                                                                                               \


#define _DEFINE_GENERIC_LIST_IMPL(                                                                                                      \
    LIST_NAME,                                                                                                                          \
    LIST_ITERATOR_NAME,                                                                                                                 \
    NODE_NAME,                                                                                                                          \
    TYPE,                                                                                                                               \
    TYPE_REF_EQUALS_FUNC,                                                                                                               \
    TYPE_REF_COPY_FUNC,                                                                                                                 \
    TYPE_REF_DELETE_FUNC                                                                                                                \
)                                                                                                                                       \
                                                                                                                                        \
static LIST_NAME            C_IDENTIFIER_BIND(LIST_NAME, create)();                                                                     \
static void                 C_IDENTIFIER_BIND(LIST_NAME, initialize)(LIST_NAME* list);                                                  \
static void                 C_IDENTIFIER_BIND(LIST_NAME, destroy)(LIST_NAME* list);                                                     \
static void                 C_IDENTIFIER_BIND(LIST_NAME, clear)(LIST_NAME* list);                                                       \
static void                 C_IDENTIFIER_BIND(LIST_NAME, copy)(LIST_NAME* dest, const LIST_NAME* source);                               \
static void                 C_IDENTIFIER_BIND(LIST_NAME, move)(LIST_NAME* dest, LIST_NAME* source);                                     \
static size_t               C_IDENTIFIER_BIND(LIST_NAME, size)(LIST_NAME* list);                                                        \
static bool                 C_IDENTIFIER_BIND(LIST_NAME, empty)(const LIST_NAME* list);                                                 \
static void                 C_IDENTIFIER_BIND(LIST_NAME, push_back)(LIST_NAME* list, const TYPE* item);                                 \
static void                 C_IDENTIFIER_BIND(LIST_NAME, push_front)(LIST_NAME* list, const TYPE* item);                                \
static void                 C_IDENTIFIER_BIND(LIST_NAME, pop_back)(LIST_NAME* list);                                                    \
static void                 C_IDENTIFIER_BIND(LIST_NAME, pop_front)(LIST_NAME* list);                                                   \
static TYPE*                C_IDENTIFIER_BIND(LIST_NAME, element_front)(LIST_NAME* list);                                               \
static TYPE*                C_IDENTIFIER_BIND(LIST_NAME, element_back)(LIST_NAME* list);                                                \
static bool                 C_IDENTIFIER_BIND(LIST_NAME, equals)(const LIST_NAME* left, const LIST_NAME* right);                        \
static LIST_ITERATOR_NAME   C_IDENTIFIER_BIND(LIST_NAME, begin)(LIST_NAME* list);                                                       \
static LIST_ITERATOR_NAME   C_IDENTIFIER_BIND(LIST_NAME, end)(LIST_NAME* list);                                                         \
                                                                                                                                        \
static void                 C_IDENTIFIER_BIND(LIST_NAME, PRIVATE_link_node_before)(LIST_NAME* list, NODE_NAME* before, NODE_NAME* node);\
static void                 C_IDENTIFIER_BIND(LIST_NAME, PRIVATE_unlink_node)(LIST_NAME* list, NODE_NAME* node);                        \
                                                                                                                                        \
static void C_IDENTIFIER_BIND(LIST_NAME, PRIVATE_link_node_before)(LIST_NAME* list, NODE_NAME* before, NODE_NAME* node)                 \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != list, "List is NULL");                                                                                     \
    node->prev = before->prev;                                                                                                          \
    node->next = before;                                                                                                                \
    before->prev->next = node;                                                                                                          \
    before->prev = node;                                                                                                                \
    ++list->size;                                                                                                                       \
}                                                                                                                                       \
                                                                                                                                        \
static void C_IDENTIFIER_BIND(LIST_NAME, PRIVATE_unlink_node)(LIST_NAME* list, NODE_NAME* node)                                         \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != list, "List is NULL");                                                                                     \
    node->prev->next = node->next;                                                                                                      \
    node->next->prev = node->prev;                                                                                                      \
    node->next = node->prev = NULL;                                                                                                     \
    --list->size;                                                                                                                       \
}                                                                                                                                       \
                                                                                                                                        \
static LIST_NAME C_IDENTIFIER_BIND(LIST_NAME, create)()                                                                                 \
{                                                                                                                                       \
    LIST_NAME list = {                                                                                                                  \
        .size = 0,                                                                                                                      \
        .head = NULL                                                                                                                    \
    };                                                                                                                                  \
    return list;                                                                                                                        \
}                                                                                                                                       \
                                                                                                                                        \
static void C_IDENTIFIER_BIND(LIST_NAME, initialize)(LIST_NAME* list)                                                                   \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != list, "List is NULL");                                                                                     \
    if (NULL != list->head)                                                                                                             \
        C_IDENTIFIER_BIND(LIST_NAME, destroy)(list);                                                                                    \
    list->size = 0;                                                                                                                     \
    list->head = C_IDENTIFIER_BIND(NODE_NAME, create)();    /*head value remains default*/                                              \
    list->head->next = list->head->prev = list->head;                                                                                   \
}                                                                                                                                       \
                                                                                                                                        \
static void C_IDENTIFIER_BIND(LIST_NAME, destroy)(LIST_NAME* list)                                                                      \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != list, "List is NULL");                                                                                     \
    if (NULL == list->head) return;                                                                                                     \
    C_IDENTIFIER_BIND(LIST_NAME, clear)(list);                                                                                          \
    C_IDENTIFIER_BIND(NODE_NAME, destroy)(list->head);                                                                                  \
    list->size = 0;                                                                                                                     \
    list->head = NULL;                                                                                                                  \
}                                                                                                                                       \
                                                                                                                                        \
static void C_IDENTIFIER_BIND(LIST_NAME, clear)(LIST_NAME* list)                                                                        \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != list, "List is NULL");                                                                                     \
    while (list->size)                                                                                                                  \
        C_IDENTIFIER_BIND(LIST_NAME, pop_back)(list);                                                                                   \
}                                                                                                                                       \
                                                                                                                                        \
static void C_IDENTIFIER_BIND(LIST_NAME, copy)(LIST_NAME* dest, const LIST_NAME* source)                                                \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != dest, "List dest is NULL");                                                                                \
    _C_CUSTOM_ASSERT(NULL != source, "List source is NULL");                                                                            \
    if (dest == source) return;                                                                                                         \
    C_IDENTIFIER_BIND(LIST_NAME, destroy)(dest);                                                                                        \
    if (NULL == source->head) return;                                                                                                   \
    C_IDENTIFIER_BIND(LIST_NAME, initialize)(dest);                                                                                     \
    for (NODE_NAME* temp = source->head->next; dest->size < source->size; temp = temp->next)                                            \
        C_IDENTIFIER_BIND(LIST_NAME, push_back)(dest, &temp->value);                                                                    \
}                                                                                                                                       \
                                                                                                                                        \
static void C_IDENTIFIER_BIND(LIST_NAME, move)(LIST_NAME* dest, LIST_NAME* source)                                                      \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != dest, "List dest is NULL");                                                                                \
    _C_CUSTOM_ASSERT(NULL != source, "List source is NULL");                                                                            \
    if (dest == source) return;                                                                                                         \
    C_IDENTIFIER_BIND(LIST_NAME, destroy)(dest);                                                                                        \
    if (NULL == source->head) return;                                                                                                   \
    *dest = *source;                                                                                                                    \
    source->size = 0;                                                                                                                   \
    source->head = NULL;                                                                                                                \
}                                                                                                                                       \
                                                                                                                                        \
static size_t C_IDENTIFIER_BIND(LIST_NAME, size)(LIST_NAME* list)                                                                       \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != list, "List is NULL");                                                                                     \
    return list->size;                                                                                                                  \
}                                                                                                                                       \
                                                                                                                                        \
static bool C_IDENTIFIER_BIND(LIST_NAME, empty)(const LIST_NAME* list)                                                                  \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != list, "List is NULL");                                                                                     \
    return 0 == list->size;                                                                                                             \
}                                                                                                                                       \
                                                                                                                                        \
static void C_IDENTIFIER_BIND(LIST_NAME, push_back)(LIST_NAME* list, const TYPE* item)                                                  \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != list, "List is NULL");                                                                                     \
    NODE_NAME* new_node = C_IDENTIFIER_BIND(NODE_NAME, create)();                                                                       \
    if (NULL != item) TYPE_REF_COPY_FUNC(&new_node->value, item);                                                                       \
    C_IDENTIFIER_BIND(LIST_NAME, PRIVATE_link_node_before)(list, list->head, new_node);                                                 \
}                                                                                                                                       \
                                                                                                                                        \
static void C_IDENTIFIER_BIND(LIST_NAME, push_front)(LIST_NAME* list, const TYPE* item)                                                 \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != list, "List is NULL");                                                                                     \
    NODE_NAME* new_node = C_IDENTIFIER_BIND(NODE_NAME, create)();                                                                       \
    if (NULL != item) TYPE_REF_COPY_FUNC(&new_node->value, item);                                                                       \
    C_IDENTIFIER_BIND(LIST_NAME, PRIVATE_link_node_before)(list, list->head->next, new_node);                                           \
}                                                                                                                                       \
                                                                                                                                        \
static void C_IDENTIFIER_BIND(LIST_NAME, pop_back)(LIST_NAME* list)                                                                     \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != list, "List is NULL");                                                                                     \
    if (0 == list->size) return;                                                                                                        \
    NODE_NAME* junk_node = list->head->prev;                                                                                            \
    C_IDENTIFIER_BIND(LIST_NAME, PRIVATE_unlink_node)(list, junk_node);                                                                 \
    TYPE_REF_DELETE_FUNC(&junk_node->value);                                                                                            \
    C_IDENTIFIER_BIND(NODE_NAME, destroy)(junk_node);                                                                                   \
}                                                                                                                                       \
                                                                                                                                        \
static void C_IDENTIFIER_BIND(LIST_NAME, pop_front)(LIST_NAME* list)                                                                    \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != list, "List is NULL");                                                                                     \
    if (0 == list->size) return;                                                                                                        \
    NODE_NAME* junk_node = list->head->next;                                                                                            \
    C_IDENTIFIER_BIND(LIST_NAME, PRIVATE_unlink_node)(list, junk_node);                                                                 \
    TYPE_REF_DELETE_FUNC(&junk_node->value);                                                                                            \
    C_IDENTIFIER_BIND(NODE_NAME, destroy)(junk_node);                                                                                   \
}                                                                                                                                       \
                                                                                                                                        \
static TYPE* C_IDENTIFIER_BIND(LIST_NAME, element_front)(LIST_NAME* list)                                                               \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != list, "List is NULL");                                                                                     \
    _C_CUSTOM_ASSERT(list->size != 0, "List is empty");                                                                                 \
    return &list->head->next->value;                                                                                                    \
}                                                                                                                                       \
                                                                                                                                        \
static TYPE* C_IDENTIFIER_BIND(LIST_NAME, element_back)(LIST_NAME* list)                                                                \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != list, "List is NULL");                                                                                     \
    _C_CUSTOM_ASSERT(list->size != 0, "List is empty");                                                                                 \
    return &list->head->prev->value;                                                                                                    \
}                                                                                                                                       \
                                                                                                                                        \
static bool C_IDENTIFIER_BIND(LIST_NAME, equals)(const LIST_NAME* left, const LIST_NAME* right)                                         \
{                                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != left, "List left is NULL");                                                                                \
    _C_CUSTOM_ASSERT(NULL != right, "List right is NULL");                                                                              \
    if (left->size != right->size) return false;                                                                                        \
    return true; /*TODO: implement*/                                                                                                    \
}                                                                                                                                       \
                                                                                                                                        \
static LIST_ITERATOR_NAME C_IDENTIFIER_BIND(LIST_NAME, begin)(LIST_NAME* list)                                                          \
{                                                                                                                                       \
    LIST_ITERATOR_NAME iter = C_IDENTIFIER_BIND(LIST_ITERATOR_NAME, create)(list->head->next, list);                                    \
    return iter;                                                                                                                        \
}                                                                                                                                       \
                                                                                                                                        \
static LIST_ITERATOR_NAME C_IDENTIFIER_BIND(LIST_NAME, end)(LIST_NAME* list)                                                            \
{                                                                                                                                       \
    LIST_ITERATOR_NAME iter = C_IDENTIFIER_BIND(LIST_ITERATOR_NAME, create)(list->head, list);                                          \
    return iter;                                                                                                                        \
}                                                                                                                                       \


#define DEFINE_GENERIC_LIST(                                                        \
    LIST_NAME_PUBLIC_PREFIX,                                                        \
    TYPE,                                                                           \
    TYPE_REF_EQUALS_FUNC,                                                           \
    TYPE_REF_COPY_FUNC,                                                             \
    TYPE_REF_DELETE_FUNC                                                            \
)                                                                                   \
                                                                                    \
DEFINE_GENERIC_DOUBLE_NODE(                                                         \
    C_IDENTIFIER_BIND(LIST_NAME_PUBLIC_PREFIX, PRIVATE_Node),                       \
    TYPE                                                                            \
)                                                                                   \
                                                                                    \
_DEFINE_GENERIC_LIST_DATA(                                                          \
    LIST_NAME_PUBLIC_PREFIX,                                                        \
    C_IDENTIFIER_BIND(LIST_NAME_PUBLIC_PREFIX, PRIVATE_Node)                        \
)                                                                                   \
                                                                                    \
_DEFINE_GENERIC_LIST_ITERATOR(                                                      \
    C_IDENTIFIER_BIND(LIST_NAME_PUBLIC_PREFIX, Iterator),                           \
    LIST_NAME_PUBLIC_PREFIX,                                                        \
    C_IDENTIFIER_BIND(LIST_NAME_PUBLIC_PREFIX, PRIVATE_Node), /*same as above*/     \
    TYPE                                                                            \
)                                                                                   \
                                                                                    \
_DEFINE_GENERIC_LIST_IMPL(                                                          \
    LIST_NAME_PUBLIC_PREFIX,                                                        \
    C_IDENTIFIER_BIND(LIST_NAME_PUBLIC_PREFIX, Iterator), /*same as above*/         \
    C_IDENTIFIER_BIND(LIST_NAME_PUBLIC_PREFIX, PRIVATE_Node), /*same as above*/     \
    TYPE,                                                                           \
    TYPE_REF_EQUALS_FUNC,                                                           \
    TYPE_REF_COPY_FUNC,                                                             \
    TYPE_REF_DELETE_FUNC                                                            \
)                                                                                   \


#endif  // C_LIST_H