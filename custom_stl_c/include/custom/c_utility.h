#ifndef C_UTILITY_H
#define C_UTILITY_H


#include "custom/_c_stlcore.h"


/**
 * @def DEFAULT_TYPE_REF_LESS
 * @brief Default less-than comparator for type references.
 * @param LEFT_REF Left-hand side pointer to a value.
 * @param RIGHT_REF Right-hand side pointer to a value.
 */
#define DEFAULT_TYPE_REF_LESS(LEFT_REF, RIGHT_REF) (*(LEFT_REF) < *(RIGHT_REF))

/**
 * @def DEFAULT_TYPE_REF_GREATER
 * @brief Default greater-than comparator for type references.
 * @param LEFT_REF Left-hand side pointer to a value.
 * @param RIGHT_REF Right-hand side pointer to a value.
 */
#define DEFAULT_TYPE_REF_GREATER(LEFT_REF, RIGHT_REF) (*(LEFT_REF) > *(RIGHT_REF))

/**
 * @def DEFAULT_TYPE_REF_EQUALS
 * @brief Default equality comparator for type references.
 * @param LEFT_REF Left-hand side pointer to a value.
 * @param RIGHT_REF Right-hand side pointer to a value.
 */
#define DEFAULT_TYPE_REF_EQUALS(LEFT_REF, RIGHT_REF) (*(LEFT_REF) == *(RIGHT_REF))

/**
 * @def DEFAULT_TYPE_REF_COPY
 * @brief Default copy operation between two references.
 * @param DEST_REF Destination side pointer to a value.
 * @param SRC_REF Source side pointer to a value.
 */
#define DEFAULT_TYPE_REF_COPY(DEST_REF, SRC_REF) (*(DEST_REF) = *(SRC_REF))

/**
 * @def DEFAULT_TYPE_REF_MOVE
 * @brief Default move operation between two references.
 * @param DEST_REF Destination side pointer to a value.
 * @param SRC_REF Source side pointer to a value.
 */
#define DEFAULT_TYPE_REF_MOVE(DEST_REF, SRC_REF) (*(DEST_REF) = *(SRC_REF))

/**
 * @def DEFAULT_TYPE_REF_DELETE
 * @brief Default delete operation. Does nothing for trivial types (expands to given expression).
 * @param TARGET_REF Target object pointer to a value to be deleted.
 */
#define DEFAULT_TYPE_REF_DELETE(TARGET_REF) (TARGET_REF)


// =====================================================================================================================


/**
 * @def DEFINE_GENERIC_SWAP_FUNCTION
 * @brief Defines a generic swap function for a given type and copy function.
 *
 * This macro instantiates:
 * 
 * - The swap API (`_do_swap`)
 * 
 * @param SWAP_FUNC_NAME_PREFIX Prefix for the generated swap function name.
 * @param TYPE The data type of elements to be swapped.
 * @param TYPE_REF_COPY_FUNC Function used to copy/move elements by reference.
 */
#define DEFINE_GENERIC_SWAP_FUNCTION(                                                   \
    SWAP_FUNC_NAME_PREFIX,                                                              \
    TYPE,                                                                               \
    TYPE_REF_COPY_FUNC                                                                  \
)                                                                                       \
                                                                                        \
static void _C_IDENTIFIER_BIND(SWAP_FUNC_NAME_PREFIX, do_swap)(TYPE* left, TYPE* right) \
{                                                                                       \
    TYPE temp;                                                                          \
    TYPE_REF_COPY_FUNC(&temp, left);                                                    \
    TYPE_REF_COPY_FUNC(left, right);                                                    \
    TYPE_REF_COPY_FUNC(right, &temp);                                                   \
}                                                                                       \


// =====================================================================================================================


/**
 * @def _DEFINE_GENERIC_HEAPIFY_FUNCTIONS_IMPL
 * @brief Internal macro to define heapify up and down operations for generic types.
 *
 * @param HEAP_ADJUST_NAME Name prefix for heapify functions.
 * @param HEAP_ADJUST_SWAP_HELPER_NAME Swap helper prefix used inside heapify.
 * @param TYPE The data type stored in the heap.
 * @param TYPE_REF_COMPARE_FUNC A comparison function taking two pointers to TYPE.
 */
#define _DEFINE_GENERIC_HEAPIFY_FUNCTIONS_IMPL(                                                                 \
    HEAP_ADJUST_NAME,                                                                                           \
    HEAP_ADJUST_SWAP_HELPER_NAME,                                                                               \
    TYPE,                                                                                                       \
    TYPE_REF_COMPARE_FUNC                                                                                       \
)                                                                                                               \
                                                                                                                \
static void _C_IDENTIFIER_BIND(HEAP_ADJUST_NAME, heapify_up)(TYPE* arr, size_t arr_size, size_t idx);           \
static void _C_IDENTIFIER_BIND(HEAP_ADJUST_NAME, heapify_down)(TYPE* arr, size_t arr_size, size_t idx);         \
                                                                                                                \
/**                                                                                                             \
 * @brief Adjusts the heap upwards to restore the heap property.                                                \
 * @param arr Pointer to the heap array.                                                                        \
 * @param arr_size Total number of elements in the array.                                                       \
 * @param idx Index of the newly inserted element to heapify.                                                   \
 */                                                                                                             \
static void _C_IDENTIFIER_BIND(HEAP_ADJUST_NAME, heapify_up)(TYPE* arr, size_t arr_size, size_t idx)            \
{                                                                                                               \
    _C_CUSTOM_ASSERT(NULL != arr, "Heap array is NULL");                                                        \
    _C_CUSTOM_ASSERT(idx < arr_size, "Heap index is greater than array size");                                  \
    while (idx > 0 && TYPE_REF_COMPARE_FUNC(&arr[idx], &arr[(idx - 1) / 2]))                                    \
    {                                                                                                           \
        _C_IDENTIFIER_BIND(HEAP_ADJUST_SWAP_HELPER_NAME, do_swap)(&arr[idx], &arr[(idx - 1) / 2]);              \
        idx = (idx - 1) / 2;                                                                                    \
    }                                                                                                           \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Adjusts the heap downwards to restore the heap property.                                              \
 * @param arr Pointer to the heap array.                                                                        \
 * @param arr_size Total number of elements in the array.                                                       \
 * @param idx Index of the newly inserted element to heapify.                                                   \
 */                                                                                                             \
static void _C_IDENTIFIER_BIND(HEAP_ADJUST_NAME, heapify_down)(TYPE* arr, size_t arr_size, size_t idx)          \
{                                                                                                               \
    _C_CUSTOM_ASSERT(NULL != arr, "Heap array is NULL");                                                        \
    _C_CUSTOM_ASSERT(idx < arr_size, "Heap index is greater than array size");                                  \
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
        _C_IDENTIFIER_BIND(HEAP_ADJUST_SWAP_HELPER_NAME, do_swap)(&arr[idx], &arr[smallest]);                   \
        idx = smallest;                                                                                         \
    }                                                                                                           \
}                                                                                                               \


/**
 * @def DEFINE_GENERIC_HEAPIFY_FUNCTIONS
 * @brief Public macro to define heapify operations for a type with all required dependencies.
 *
 * This macro instantiates:
 * 
 * - A `PRIVATE_Swap` helper for heap operations
 * 
 * - The heapify API (`_heapify_up`, `_heapify_down`)
 * 
 * @param HEAP_ADJUST_NAME_PUBLIC_PREFIX Public name prefix for heapify functions.
 * @param TYPE The data type in the heap.
 * @param TYPE_REF_COMPARE_FUNC A comparison function used for heap ordering.
 * @param TYPE_REF_COPY_FUNC A copy function used for swapping values.
 */
#define DEFINE_GENERIC_HEAPIFY_FUNCTIONS(                                                                               \
    HEAP_ADJUST_NAME_PUBLIC_PREFIX,                                                                                     \
    TYPE,                                                                                                               \
    TYPE_REF_COMPARE_FUNC,                                                                                              \
    TYPE_REF_COPY_FUNC                                                                                                  \
)                                                                                                                       \
                                                                                                                        \
DEFINE_GENERIC_SWAP_FUNCTION(                                                                                           \
    _C_IDENTIFIER_BIND(HEAP_ADJUST_NAME_PUBLIC_PREFIX, _C_IDENTIFIER_PRIVATE, _C_IDENTIFIER_SWAP),                      \
    TYPE,                                                                                                               \
    TYPE_REF_COPY_FUNC                                                                                                  \
)                                                                                                                       \
                                                                                                                        \
_DEFINE_GENERIC_HEAPIFY_FUNCTIONS_IMPL(                                                                                 \
    HEAP_ADJUST_NAME_PUBLIC_PREFIX,                                                                                     \
    _C_IDENTIFIER_BIND(HEAP_ADJUST_NAME_PUBLIC_PREFIX, _C_IDENTIFIER_PRIVATE, _C_IDENTIFIER_SWAP), /*same as above*/    \
    TYPE,                                                                                                               \
    TYPE_REF_COMPARE_FUNC                                                                                               \
)                                                                                                                       \


#endif  // C_UTILITY_H