#ifndef _C_LIST_H
#define _C_LIST_H


#include "custom/_c_stlcore.h"
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
static LIST_NAME    _C_IDENTIFIER_BIND(LIST_NAME, create)();                                                \
static void         _C_IDENTIFIER_BIND(LIST_NAME, destroy)(LIST_NAME* list);                                \
static void         _C_IDENTIFIER_BIND(LIST_NAME, clear)(LIST_NAME* list);                                  \
static void         _C_IDENTIFIER_BIND(LIST_NAME, copy)(LIST_NAME* dest, const LIST_NAME* source);          \
static void         _C_IDENTIFIER_BIND(LIST_NAME, move)(LIST_NAME* dest, LIST_NAME* source);                \
static size_t       _C_IDENTIFIER_BIND(LIST_NAME, size)(LIST_NAME* list);                                   \
static bool         _C_IDENTIFIER_BIND(LIST_NAME, empty)(const LIST_NAME* list);                            \
static void         _C_IDENTIFIER_BIND(LIST_NAME, push_back)(LIST_NAME* list, const TYPE* item);            \
static void         _C_IDENTIFIER_BIND(LIST_NAME, push_front)(LIST_NAME* list, const TYPE* item);           \
static void         _C_IDENTIFIER_BIND(LIST_NAME, pop_back)(LIST_NAME* list);                               \
static void         _C_IDENTIFIER_BIND(LIST_NAME, pop_front)(LIST_NAME* list);                              \
static TYPE*        _C_IDENTIFIER_BIND(LIST_NAME, element_front)(LIST_NAME* list);                          \
static TYPE*        _C_IDENTIFIER_BIND(LIST_NAME, element_back)(LIST_NAME* list);                           \
static bool         _C_IDENTIFIER_BIND(LIST_NAME, equals)(const LIST_NAME* left, const LIST_NAME* right);   \
                                                                                                            \
static LIST_NAME _C_IDENTIFIER_BIND(LIST_NAME, create)()                                                    \
{                                                                                                           \
    LIST_NAME list = {                                                                                      \
        .size = 0,                                                                                          \
        .head = (NODE_NAME*)malloc(sizeof(NODE_NAME))                                                       \
    };                                                                                                      \
    return list;                                                                                            \
}                                                                                                           \
                                                                                                            \
static void _C_IDENTIFIER_BIND(LIST_NAME, clear)(LIST_NAME* list)                                           \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    /*TODO: implement*/                                                                                     \
}                                                                                                           \
                                                                                                            \
static void _C_IDENTIFIER_BIND(LIST_NAME, destroy)(LIST_NAME* list)                                         \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    _C_IDENTIFIER_BIND(LIST_NAME, clear)(list);                                                             \
    /*TODO: delete head*/                                                                                   \
}                                                                                                           \
                                                                                                            \
static void _C_IDENTIFIER_BIND(LIST_NAME, copy)(LIST_NAME* dest, const LIST_NAME* source)                   \
{                                                                                                           \
    _ASSERT(NULL != dest, "List dest is NULL");                                                             \
    _ASSERT(NULL != source, "List source is NULL");                                                         \
    /*TODO: implement*/                                                                                     \
}                                                                                                           \
                                                                                                            \
static void _C_IDENTIFIER_BIND(LIST_NAME, move)(LIST_NAME* dest, LIST_NAME* source)                         \
{                                                                                                           \
    _ASSERT(NULL != dest, "List dest is NULL");                                                             \
    _ASSERT(NULL != source, "List source is NULL");                                                         \
    /*TODO: implement*/                                                                                     \
}                                                                                                           \
                                                                                                            \
static size_t _C_IDENTIFIER_BIND(LIST_NAME, size)(LIST_NAME* list)                                          \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    return list->size;                                                                                      \
}                                                                                                           \
                                                                                                            \
static bool _C_IDENTIFIER_BIND(LIST_NAME, empty)(const LIST_NAME* list)                                     \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    return list->size == 0;                                                                                 \
}                                                                                                           \
                                                                                                            \
static void _C_IDENTIFIER_BIND(LIST_NAME, push_back)(LIST_NAME* list, const TYPE* item)                     \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    /*TODO: implement*/                                                                                     \
}                                                                                                           \
                                                                                                            \
static void _C_IDENTIFIER_BIND(LIST_NAME, push_front)(LIST_NAME* list, const TYPE* item)                    \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    /*TODO: implement*/                                                                                     \
}                                                                                                           \
                                                                                                            \
static void _C_IDENTIFIER_BIND(LIST_NAME, pop_back)(LIST_NAME* list)                                        \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    /*TODO: implement*/                                                                                     \
}                                                                                                           \
                                                                                                            \
static void _C_IDENTIFIER_BIND(LIST_NAME, pop_front)(LIST_NAME* list)                                       \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    /*TODO: implement*/                                                                                     \
}                                                                                                           \
                                                                                                            \
static TYPE* _C_IDENTIFIER_BIND(LIST_NAME, element_front)(LIST_NAME* list)                                  \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    return NULL; /*TODO: implement*/                                                                        \
}                                                                                                           \
                                                                                                            \
static TYPE* _C_IDENTIFIER_BIND(LIST_NAME, element_back)(LIST_NAME* list)                                   \
{                                                                                                           \
    _ASSERT(NULL != list, "List is NULL");                                                                  \
    return NULL; /*TODO: implement*/                                                                        \
}                                                                                                           \
                                                                                                            \
static bool _C_IDENTIFIER_BIND(LIST_NAME, equals)(const LIST_NAME* left, const LIST_NAME* right)            \
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
    _C_IDENTIFIER_BIND(LIST_NAME_PUBLIC_PREFIX, PRIVATE_NODE),                      \
    TYPE                                                                            \
)                                                                                   \
                                                                                    \
_DEFINE_GENERIC_LIST_IMPL(                                                          \
    LIST_NAME_PUBLIC_PREFIX,                                                        \
    _C_IDENTIFIER_BIND(LIST_NAME_PUBLIC_PREFIX, PRIVATE_NODE), /*same as above*/    \
    TYPE,                                                                           \
    TYPE_REF_EQUALS_FUNC,                                                           \
    TYPE_REF_COPY_FUNC,                                                             \
    TYPE_REF_DELETE_FUNC                                                            \
)                                                                                   \


#endif  // _C_LIST_H