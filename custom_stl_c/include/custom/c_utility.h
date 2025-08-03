#ifndef UTILITY_H
#define UTILITY_H


#include "custom/__c_core.h"


#define DEFAULT_TYPE_REF_LESS(LEFT_REF, RIGHT_REF) (*(LEFT_REF) < *(RIGHT_REF))
#define DEFAULT_TYPE_REF_COPY(DEST_REF, SRC_REF) (*(DEST_REF) = *(SRC_REF))
#define DEFAULT_TYPE_REF_DELETE(TARGET_REF) // nothing special


#define DEFINE_SWAP_GENERIC_FUNCTION(                   \
    SWAP_FUNC_NAME,                                     \
    TYPE,                                               \
    TYPE_REF_COPY_FUNC                                  \
)                                                       \
                                                        \
static void SWAP_FUNC_NAME(TYPE* left, TYPE* right)     \
{                                                       \
    TYPE temp;                                          \
    TYPE_REF_COPY_FUNC(&temp, left);                    \
    TYPE_REF_COPY_FUNC(left, right);                    \
    TYPE_REF_COPY_FUNC(right, &temp);                   \
}                                                       \


#endif  // UTILITY_H