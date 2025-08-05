#ifndef C_UTILITY_H
#define C_UTILITY_H


#include "custom/_c_core.h"


#define DEFAULT_TYPE_REF_LESS(LEFT_REF, RIGHT_REF) (*(LEFT_REF) < *(RIGHT_REF))
#define DEFAULT_TYPE_REF_GREATER(LEFT_REF, RIGHT_REF) (*(LEFT_REF) > *(RIGHT_REF))
#define DEFAULT_TYPE_REF_EQUALS(LEFT_REF, RIGHT_REF) (*(LEFT_REF) == *(RIGHT_REF))

#define DEFAULT_TYPE_REF_COPY(DEST_REF, SRC_REF) (*(DEST_REF) = *(SRC_REF))
#define DEFAULT_TYPE_REF_MOVE(DEST_REF, SRC_REF) (*(DEST_REF) = *(SRC_REF))
#define DEFAULT_TYPE_REF_DELETE(TARGET_REF)


// =====================================================================================================================


#define DEFINE_GENERIC_SWAP_FUNCTION(                                                   \
    SWAP_FUNC_NAME_PREFIX,                                                              \
    TYPE,                                                                               \
    TYPE_REF_COPY_FUNC                                                                  \
)                                                                                       \
                                                                                        \
static void _C_IDENTIFIER_BIND(SWAP_FUNC_NAME_PREFIX, swap)(TYPE* left, TYPE* right)    \
{                                                                                       \
    TYPE temp;                                                                          \
    TYPE_REF_COPY_FUNC(&temp, left);                                                    \
    TYPE_REF_COPY_FUNC(left, right);                                                    \
    TYPE_REF_COPY_FUNC(right, &temp);                                                   \
}                                                                                       \


// =====================================================================================================================


#define _DEFINE_GENERIC_HEAPIFY_FUNCTIONS_IMPL(                                                                 \
    HEAP_ADJUST_NAME,                                                                                           \
    HEAP_ADJUST_SWAP_HELPER_NAME,                                                                               \
    TYPE,                                                                                                       \
    TYPE_REF_COMPARE_FUNC                                                                                       \
)                                                                                                               \
                                                                                                                \
static void _C_IDENTIFIER_BIND(HEAP_ADJUST_NAME, heapify_up)(TYPE* arr, size_t arr_size, size_t idx)            \
{                                                                                                               \
    _ASSERT(NULL != arr, "Heap array is NULL");                                                                 \
    _ASSERT(idx < arr_size, "Heap index is greater than array size");                                           \
    while (idx > 0 && TYPE_REF_COMPARE_FUNC(&arr[idx], &arr[(idx - 1) / 2]))                                    \
    {                                                                                                           \
        _C_IDENTIFIER_BIND(HEAP_ADJUST_SWAP_HELPER_NAME, swap)(&arr[idx], &arr[(idx - 1) / 2]);                 \
        idx = (idx - 1) / 2;                                                                                    \
    }                                                                                                           \
}                                                                                                               \
                                                                                                                \
static void _C_IDENTIFIER_BIND(HEAP_ADJUST_NAME, heapify_down)(TYPE* arr, size_t arr_size, size_t idx)          \
{                                                                                                               \
    _ASSERT(NULL != arr, "Heap array is NULL");                                                                 \
    _ASSERT(idx < arr_size, "Heap index is greater than array size");                                           \
    size_t smallest, left, right;                                                                               \
    while (idx < arr_size)                                                                                      \
    {                                                                                                           \
        smallest = idx;                                                                                         \
        left = 2 * idx + 1;                                                                                     \
        right = 2 * idx + 2;                                                                                    \
        if (left < arr_size && TYPE_REF_COMPARE_FUNC(&arr[left], &arr[smallest]))                               \
            smallest = left;                                                                                    \
        if (right < arr_size && TYPE_REF_COMPARE_FUNC(&arr[right], &arr[smallest]))                             \
            smallest = right;                                                                                   \
        if (smallest == idx) return;                                                                            \
        _C_IDENTIFIER_BIND(HEAP_ADJUST_SWAP_HELPER_NAME, swap)(&arr[idx], &arr[smallest]);                      \
        idx = smallest;                                                                                         \
    }                                                                                                           \
}                                                                                                               \


#define DEFINE_GENERIC_HEAPIFY_FUNCTIONS(                                                                       \
    HEAP_ADJUST_NAME_PUBLIC_PREFIX,                                                                             \
    HEAP_ADJUST_NAME_PRIVATE_PREFIX,                                                                            \
    TYPE,                                                                                                       \
    TYPE_REF_COMPARE_FUNC,                                                                                      \
    TYPE_REF_COPY_FUNC                                                                                          \
)                                                                                                               \
                                                                                                                \
DEFINE_GENERIC_SWAP_FUNCTION(                                                                                   \
    _C_IDENTIFIER_BIND(HEAP_ADJUST_NAME_PRIVATE_PREFIX, SWAP_FUNC),                                             \
    TYPE,                                                                                                       \
    TYPE_REF_COPY_FUNC                                                                                          \
)                                                                                                               \
                                                                                                                \
_DEFINE_GENERIC_HEAPIFY_FUNCTIONS_IMPL(                                                                         \
    HEAP_ADJUST_NAME_PUBLIC_PREFIX,                                                                             \
    _C_IDENTIFIER_BIND(HEAP_ADJUST_NAME_PRIVATE_PREFIX, SWAP_FUNC),                                             \
    TYPE,                                                                                                       \
    TYPE_REF_COMPARE_FUNC                                                                                       \
)                                                                                                               \


#endif  // C_UTILITY_H