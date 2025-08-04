#ifndef C_UTILITY_H
#define C_UTILITY_H


#include "custom/__c_core.h"


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
static void c_IDENTIFIER_BIND(SWAP_FUNC_NAME_PREFIX, swap)(TYPE* left, TYPE* right)     \
{                                                                                       \
    TYPE temp;                                                                          \
    TYPE_REF_COPY_FUNC(&temp, left);                                                    \
    TYPE_REF_COPY_FUNC(left, right);                                                    \
    TYPE_REF_COPY_FUNC(right, &temp);                                                   \
}                                                                                       \


// =====================================================================================================================

#define _DEFINE_GENERIC_HEAPIFY_FUNCTIONS_IMPL(                                             \
    HEAP_ADJUST_UP_FUNC_NAME,                                                               \
    HEAP_ADJUST_DOWN_FUNC_NAME,                                                             \
    TYPE,                                                                                   \
    TYPE_REF_COMPARE_FUNC,                                                                  \
    TYPE_REF_SWAP_FUNC                                                                      \
)                                                                                           \
                                                                                            \
static void HEAP_ADJUST_UP_FUNC_NAME(TYPE* arr, size_t arr_size, size_t idx)                \
{                                                                                           \
    _ASSERT(NULL != arr, "Heap array is NULL");                                             \
    _ASSERT(idx < arr_size, "Heap index is greater than array size");                       \
    while (idx > 0 && TYPE_REF_COMPARE_FUNC(&arr[idx], &arr[(idx - 1) / 2]))                \
    {                                                                                       \
        TYPE_REF_SWAP_FUNC(&arr[idx], &arr[(idx - 1) / 2]);                                 \
        idx = (idx - 1) / 2;                                                                \
    }                                                                                       \
}                                                                                           \
                                                                                            \
static void HEAP_ADJUST_DOWN_FUNC_NAME(TYPE* arr, size_t arr_size, size_t idx)              \
{                                                                                           \
    _ASSERT(NULL != arr, "Heap array is NULL");                                             \
    _ASSERT(idx < arr_size, "Heap index is greater than array size");                       \
    size_t smallest, left, right;                                                           \
    while (idx < arr_size)                                                                  \
    {                                                                                       \
        smallest = idx;                                                                     \
        left = 2 * idx + 1;                                                                 \
        right = 2 * idx + 2;                                                                \
        if (left < arr_size && TYPE_REF_COMPARE_FUNC(&arr[left], &arr[smallest]))           \
            smallest = left;                                                                \
        if (right < arr_size && TYPE_REF_COMPARE_FUNC(&arr[right], &arr[smallest]))         \
            smallest = right;                                                               \
        if (smallest == idx) return;                                                        \
        TYPE_REF_SWAP_FUNC(&arr[idx], &arr[smallest]);                                      \
        idx = smallest;                                                                     \
    }                                                                                       \
}                                                                                           \


#define DEFINE_GENERIC_HEAPIFY_FUNCTIONS(                           \
    HEAP_NAME_PREFIX,                                               \
    TYPE,                                                           \
    TYPE_REF_COMPARE_FUNC,                                          \
    TYPE_REF_COPY_FUNC                                              \
)                                                                   \
                                                                    \
DEFINE_GENERIC_SWAP_FUNCTION(                                       \
    _##HEAP_NAME_PREFIX##_type_ref,                                 \
    TYPE,                                                           \
    TYPE_REF_COPY_FUNC)                                             \
                                                                    \
_DEFINE_GENERIC_HEAPIFY_FUNCTIONS_IMPL(                             \
    HEAP_NAME_PREFIX##_heapify_up,                                  \
    HEAP_NAME_PREFIX##_heapify_down,                                \
    TYPE,                                                           \
    TYPE_REF_COMPARE_FUNC,                                          \
    _##HEAP_NAME_PREFIX##_type_ref_swap /*generated from above*/    \
)                                                                   \


#endif  // C_UTILITY_H