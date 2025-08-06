#ifndef _C_NODE_H
#define _C_NODE_H


#include "custom/_c_stlcore.h"


#define DEFINE_GENERIC_SINGLE_NODE(     \
    SINGLE_NODE_NAME,                   \
    TYPE                                \
)                                       \
                                        \
typedef struct                          \
{                                       \
    TYPE value;                         \
    SINGLE_NODE_NAME* next;             \
} SINGLE_NODE_NAME;                     \



#define DEFINE_GENERIC_DOUBLE_NODE(     \
    DOUBLE_NODE_NAME,                   \
    TYPE                                \
)                                       \
                                        \
typedef struct                          \
{                                       \
    TYPE value;                         \
    DOUBLE_NODE_NAME* next;             \
    DOUBLE_NODE_NAME* prev;             \
} DOUBLE_NODE_NAME;                     \


#endif  // _C_NODE_H