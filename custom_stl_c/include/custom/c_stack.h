#ifndef C_STACK_H
#define C_STACK_H


#include "custom/c_vector.h"


#define _DEFINE_GENERIC_STACK_IMPL(                                                                         \
    STACK_NAME,                                                                                             \
    STACK_VECTOR_HELPER_NAME,                                                                               \
    TYPE                                                                                                    \
)                                                                                                           \
                                                                                                            \
typedef struct                                                                                              \
{                                                                                                           \
    STACK_VECTOR_HELPER_NAME vec;                                                                           \
} STACK_NAME;                                                                                               \
                                                                                                            \
static STACK_NAME   C_IDENTIFIER_BIND(STACK_NAME, create)();                                                \
static void         C_IDENTIFIER_BIND(STACK_NAME, destroy)(STACK_NAME* stack);                              \
static void         C_IDENTIFIER_BIND(STACK_NAME, clear)(STACK_NAME* stack);                                \
static void         C_IDENTIFIER_BIND(STACK_NAME, copy)(STACK_NAME* dest, const STACK_NAME* source);        \
static void         C_IDENTIFIER_BIND(STACK_NAME, move)(STACK_NAME* dest, STACK_NAME* source);              \
static size_t       C_IDENTIFIER_BIND(STACK_NAME, size)(STACK_NAME* stack);                                 \
static bool         C_IDENTIFIER_BIND(STACK_NAME, empty)(STACK_NAME* stack);                                \
static void         C_IDENTIFIER_BIND(STACK_NAME, insert)(STACK_NAME* stack, const TYPE* item);             \
static void         C_IDENTIFIER_BIND(STACK_NAME, pop)(STACK_NAME* stack);                                  \
static TYPE*        C_IDENTIFIER_BIND(STACK_NAME, peek)(STACK_NAME* stack);                                 \
static bool         C_IDENTIFIER_BIND(STACK_NAME, equals)(const STACK_NAME* left, const STACK_NAME* right); \
                                                                                                            \
static STACK_NAME C_IDENTIFIER_BIND(STACK_NAME, create)()                                                   \
{                                                                                                           \
    STACK_NAME stack = {                                                                                    \
        .vec = C_IDENTIFIER_BIND(STACK_VECTOR_HELPER_NAME, create)(8)                                       \
    };                                                                                                      \
    return stack;                                                                                           \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(STACK_NAME, destroy)(STACK_NAME* stack)                                       \
{                                                                                                           \
    _ASSERT(NULL != stack, "Stack is NULL");                                                                \
    C_IDENTIFIER_BIND(STACK_VECTOR_HELPER_NAME, destroy)(&stack->vec);                                      \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(STACK_NAME, clear)(STACK_NAME* stack)                                         \
{                                                                                                           \
    _ASSERT(NULL != stack, "Stack is NULL");                                                                \
    C_IDENTIFIER_BIND(STACK_VECTOR_HELPER_NAME, clear)(&stack->vec);                                        \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(STACK_NAME, copy)(STACK_NAME* dest, const STACK_NAME* source)                 \
{                                                                                                           \
    _ASSERT(NULL != dest, "Stack dest is NULL");                                                            \
    _ASSERT(NULL != source, "Stack source is NULL");                                                        \
    C_IDENTIFIER_BIND(STACK_VECTOR_HELPER_NAME, copy)(&dest->vec, &source->vec);                            \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(STACK_NAME, move)(STACK_NAME* dest, STACK_NAME* source)                       \
{                                                                                                           \
    _ASSERT(NULL != dest, "Stack dest is NULL");                                                            \
    _ASSERT(NULL != source, "Stack source is NULL");                                                        \
    C_IDENTIFIER_BIND(STACK_VECTOR_HELPER_NAME, move)(&dest->vec, &source->vec);                            \
}                                                                                                           \
                                                                                                            \
static size_t C_IDENTIFIER_BIND(STACK_NAME, size)(STACK_NAME* stack)                                        \
{                                                                                                           \
    _ASSERT(NULL != stack, "Stack is NULL");                                                                \
    return C_IDENTIFIER_BIND(STACK_VECTOR_HELPER_NAME, size)(&stack->vec);                                  \
}                                                                                                           \
                                                                                                            \
static bool C_IDENTIFIER_BIND(STACK_NAME, empty)(STACK_NAME* stack)                                         \
{                                                                                                           \
    _ASSERT(NULL != stack, "Stack is NULL");                                                                \
    return C_IDENTIFIER_BIND(STACK_VECTOR_HELPER_NAME, empty)(&stack->vec);                                 \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(STACK_NAME, insert)(STACK_NAME* stack, const TYPE* item)                      \
{                                                                                                           \
    _ASSERT(NULL != stack, "Stack is NULL");                                                                \
    C_IDENTIFIER_BIND(STACK_VECTOR_HELPER_NAME, push_back)(&stack->vec, item);                              \
}                                                                                                           \
                                                                                                            \
static void C_IDENTIFIER_BIND(STACK_NAME, pop)(STACK_NAME* stack)                                           \
{                                                                                                           \
    _ASSERT(NULL != stack, "Stack is NULL");                                                                \
    C_IDENTIFIER_BIND(STACK_VECTOR_HELPER_NAME, pop_back)(&stack->vec);                                     \
}                                                                                                           \
                                                                                                            \
static TYPE* C_IDENTIFIER_BIND(STACK_NAME, peek)(STACK_NAME* stack)                                         \
{                                                                                                           \
    _ASSERT(NULL != stack, "Stack is NULL");                                                                \
    return C_IDENTIFIER_BIND(STACK_VECTOR_HELPER_NAME, element_back)(&stack->vec);                          \
}                                                                                                           \
                                                                                                            \
static bool C_IDENTIFIER_BIND(STACK_NAME, equals)(const STACK_NAME* left, const STACK_NAME* right)          \
{                                                                                                           \
    _ASSERT(NULL != left, "Stack left is NULL");                                                            \
    _ASSERT(NULL != right, "Stack right is NULL");                                                          \
    return C_IDENTIFIER_BIND(STACK_VECTOR_HELPER_NAME, equals)(&left->vec, &right->vec);                    \
}                                                                                                           \


#define DEFINE_GENERIC_STACK(                                                       \
    STACK_NAME_PUBLIC_PREFIX,                                                       \
    TYPE,                                                                           \
    TYPE_REF_EQUALS_FUNC,                                                           \
    TYPE_REF_COPY_FUNC,                                                             \
    TYPE_REF_DELETE_FUNC                                                            \
)                                                                                   \
                                                                                    \
DEFINE_GENERIC_VECTOR(                                                              \
    C_IDENTIFIER_BIND(STACK_NAME_PUBLIC_PREFIX, PRIVATE_VECTOR),                    \
    TYPE,                                                                           \
    TYPE_REF_EQUALS_FUNC,                                                           \
    TYPE_REF_COPY_FUNC,                                                             \
    TYPE_REF_DELETE_FUNC                                                            \
)                                                                                   \
                                                                                    \
_DEFINE_GENERIC_STACK_IMPL(                                                         \
    STACK_NAME_PUBLIC_PREFIX,                                                       \
    C_IDENTIFIER_BIND(STACK_NAME_PUBLIC_PREFIX, PRIVATE_VECTOR), /*same as above*/  \
    TYPE                                                                            \
)                                                                                   \


#endif // C_STACK_H