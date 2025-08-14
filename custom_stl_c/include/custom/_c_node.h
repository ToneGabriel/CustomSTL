#ifndef _C_NODE_H
#define _C_NODE_H


#include "custom/_c_stlcore.h"


#define DEFINE_GENERIC_SINGLE_NODE(                                                                 \
    SINGLE_NODE_NAME,                                                                               \
    TYPE,                                                                                           \
    TYPE_REF_COPY_FUNC,                                                                             \
    TYPE_REF_DELETE_FUNC                                                                            \
)                                                                                                   \
                                                                                                    \
typedef struct SINGLE_NODE_NAME                                                                     \
{                                                                                                   \
    TYPE value;                                                                                     \
    SINGLE_NODE_NAME* next;                                                                         \
} SINGLE_NODE_NAME;                                                                                 \
                                                                                                    \
static SINGLE_NODE_NAME*    C_IDENTIFIER_BIND(SINGLE_NODE_NAME, create)(const TYPE* val);           \
static void                 C_IDENTIFIER_BIND(SINGLE_NODE_NAME, destroy)(SINGLE_NODE_NAME* node);   \
                                                                                                    \
static SINGLE_NODE_NAME* C_IDENTIFIER_BIND(SINGLE_NODE_NAME, create)(const TYPE* val)               \
{                                                                                                   \
    SINGLE_NODE_NAME* new_node = (SINGLE_NODE_NAME*)malloc(sizeof(SINGLE_NODE_NAME));               \
    new_node->next = NULL;                                                                          \
    if (NULL != val)                                                                                \
        TYPE_REF_COPY_FUNC(&new_node->value, val);                                                  \
    return new_node;                                                                                \
}                                                                                                   \
                                                                                                    \
static void C_IDENTIFIER_BIND(SINGLE_NODE_NAME, destroy)(SINGLE_NODE_NAME* node)                    \
{                                                                                                   \
    TYPE_REF_DELETE_FUNC(&node->value);                                                             \
    free(node);                                                                                     \
}                                                                                                   \


// ==================================================================================================================


#define DEFINE_GENERIC_DOUBLE_NODE(                                                                 \
    DOUBLE_NODE_NAME,                                                                               \
    TYPE,                                                                                           \
    TYPE_REF_COPY_FUNC,                                                                             \
    TYPE_REF_DELETE_FUNC                                                                            \
)                                                                                                   \
                                                                                                    \
typedef struct DOUBLE_NODE_NAME                                                                     \
{                                                                                                   \
    TYPE value;                                                                                     \
    struct DOUBLE_NODE_NAME* next;                                                                  \
    struct DOUBLE_NODE_NAME* prev;                                                                  \
} DOUBLE_NODE_NAME;                                                                                 \
                                                                                                    \
static DOUBLE_NODE_NAME*    C_IDENTIFIER_BIND(DOUBLE_NODE_NAME, create)(const TYPE* val);           \
static void                 C_IDENTIFIER_BIND(DOUBLE_NODE_NAME, destroy)(DOUBLE_NODE_NAME* node);   \
                                                                                                    \
static DOUBLE_NODE_NAME* C_IDENTIFIER_BIND(DOUBLE_NODE_NAME, create)(const TYPE* val)               \
{                                                                                                   \
    DOUBLE_NODE_NAME* new_node = (DOUBLE_NODE_NAME*)malloc(sizeof(DOUBLE_NODE_NAME));               \
    new_node->next = new_node->prev = NULL;                                                         \
    if (NULL != val)                                                                                \
        TYPE_REF_COPY_FUNC(&new_node->value, val);                                                  \
    return new_node;                                                                                \
}                                                                                                   \
                                                                                                    \
static void C_IDENTIFIER_BIND(DOUBLE_NODE_NAME, destroy)(DOUBLE_NODE_NAME* node)                    \
{                                                                                                   \
    TYPE_REF_DELETE_FUNC(&node->value);                                                             \
    free(node);                                                                                     \
}                                                                                                   \

#endif  // _C_NODE_H