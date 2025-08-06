#ifndef C_VECTOR_H
#define C_VECTOR_H


#include "custom/c_utility.h"


/**
 * @def _DEFINE_GENERIC_VECTOR_IMPL
 * @brief Internal macro that defines the complete implementation of a vector.
 *
 * This macro generates a full implementation of a dynamic array.
 * The generated vector operates on elements of type `TYPE`.
 *
 * @param VECTOR_NAME               Public-facing name prefix for struct and functions (e.g., `MyVec` -> `MyVec_create`, etc.)
 * @param TYPE                      Type of elements to be stored in the vector.
 * @param TYPE_REF_EQUALS_FUNC      Function used to compare two `TYPE*` (for equality).
 * @param TYPE_REF_COPY_FUNC        Function used to copy one `TYPE*` to another.
 * @param TYPE_REF_DELETE_FUNC      Function used to delete a `TYPE*`
 */
#define _DEFINE_GENERIC_VECTOR_IMPL(                                                                            \
    VECTOR_NAME,                                                                                                \
    TYPE,                                                                                                       \
    TYPE_REF_EQUALS_FUNC,                                                                                       \
    TYPE_REF_COPY_FUNC,                                                                                         \
    TYPE_REF_DELETE_FUNC                                                                                        \
)                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @struct VECTOR_NAME                                                                                          \
 * @brief Represents a dynamic array of elements of type TYPE.                                                  \
 */                                                                                                             \
typedef struct                                                                                                  \
{                                                                                                               \
    TYPE* arr;                                                                                                  \
    size_t size;                                                                                                \
    size_t capacity;                                                                                            \
} VECTOR_NAME;                                                                                                  \
                                                                                                                \
static VECTOR_NAME  C_IDENTIFIER_BIND(VECTOR_NAME, create)(size_t capacity);                                    \
static void         C_IDENTIFIER_BIND(VECTOR_NAME, destroy)(VECTOR_NAME* vec);                                  \
static void         C_IDENTIFIER_BIND(VECTOR_NAME, clear)(VECTOR_NAME* vec);                                    \
static void         C_IDENTIFIER_BIND(VECTOR_NAME, copy)(VECTOR_NAME* dest, const VECTOR_NAME* source);         \
static void         C_IDENTIFIER_BIND(VECTOR_NAME, move)(VECTOR_NAME* dest, VECTOR_NAME* source);               \
static TYPE*        C_IDENTIFIER_BIND(VECTOR_NAME, data)(const VECTOR_NAME* vec);                               \
static size_t       C_IDENTIFIER_BIND(VECTOR_NAME, size)(const VECTOR_NAME* vec);                               \
static size_t       C_IDENTIFIER_BIND(VECTOR_NAME, capacity)(const VECTOR_NAME* vec);                           \
static bool         C_IDENTIFIER_BIND(VECTOR_NAME, empty)(const VECTOR_NAME* vec);                              \
static void         C_IDENTIFIER_BIND(VECTOR_NAME, push_back)(VECTOR_NAME* vec, const TYPE* item);              \
static void         C_IDENTIFIER_BIND(VECTOR_NAME, pop_back)(VECTOR_NAME* vec);                                 \
static TYPE*        C_IDENTIFIER_BIND(VECTOR_NAME, element_front)(VECTOR_NAME* vec);                            \
static TYPE*        C_IDENTIFIER_BIND(VECTOR_NAME, element_back)(VECTOR_NAME* vec);                             \
static TYPE*        C_IDENTIFIER_BIND(VECTOR_NAME, element_at)(VECTOR_NAME* vec, size_t index);                 \
static bool         C_IDENTIFIER_BIND(VECTOR_NAME, equals)(const VECTOR_NAME* left, const VECTOR_NAME* right);  \
                                                                                                                \
/**                                                                                                             \
 * @brief Creates a vector with an initial capacity.                                                            \
 * @param capacity Initial capacity of the vector.                                                              \
 * @return A new instance of VECTOR_NAME.                                                                       \
 */                                                                                                             \
static VECTOR_NAME C_IDENTIFIER_BIND(VECTOR_NAME, create)(size_t capacity)                                      \
{                                                                                                               \
    _ASSERT(0 < capacity, "Vector capacity should be greater than 0");                                          \
    VECTOR_NAME vec = {                                                                                         \
        .arr = (TYPE*)malloc(sizeof(TYPE) * capacity),                                                          \
        .size = 0,                                                                                              \
        .capacity = capacity                                                                                    \
    };                                                                                                          \
    return vec;                                                                                                 \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Destroys a vector and releases allocated memory.                                                      \
 * @param vec Pointer to the vector.                                                                            \
 */                                                                                                             \
static void C_IDENTIFIER_BIND(VECTOR_NAME, destroy)(VECTOR_NAME* vec)                                           \
{                                                                                                               \
    _ASSERT(NULL != vec, "Vector is NULL");                                                                     \
    if (NULL == vec->arr) return;                                                                               \
    for (size_t i = 0; i < vec->size; ++i)                                                                      \
        TYPE_REF_DELETE_FUNC(&vec->arr[i]);                                                                     \
    free(vec->arr);                                                                                             \
    vec->capacity = vec->size = 0;                                                                              \
    vec->arr = NULL;                                                                                            \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Clears the vector contents but retains capacity.                                                      \
 * @param vec Pointer to the vector.                                                                            \
 */                                                                                                             \
static void C_IDENTIFIER_BIND(VECTOR_NAME, clear)(VECTOR_NAME* vec)                                             \
{                                                                                                               \
    _ASSERT(NULL != vec, "Vector is NULL");                                                                     \
    for (size_t i = 0; i < vec->size; ++i)                                                                      \
        TYPE_REF_DELETE_FUNC(&vec->arr[i]);                                                                     \
    vec->size = 0;                                                                                              \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Copies contents of one vector to another.                                                             \
 * @param dest Destination vector pointer.                                                                      \
 * @param source Source vector pointer.                                                                         \
 */                                                                                                             \
static void C_IDENTIFIER_BIND(VECTOR_NAME, copy)(VECTOR_NAME* dest, const VECTOR_NAME* source)                  \
{                                                                                                               \
    _ASSERT(NULL != dest, "Vector dest is NULL");                                                               \
    _ASSERT(NULL != source, "Vector source is NULL");                                                           \
    _ASSERT(NULL != source->arr, "Vector source array is NULL");                                                \
    if (dest == source) return;                                                                                 \
    C_IDENTIFIER_BIND(VECTOR_NAME, destroy)(dest);                                                              \
    *dest = C_IDENTIFIER_BIND(VECTOR_NAME, create)(source->capacity);                                           \
    for (size_t i = 0; i < source->size; ++i)                                                                   \
        TYPE_REF_COPY_FUNC(&dest->arr[i], &source->arr[i]);                                                     \
    dest->size = source->size;                                                                                  \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Moves contents from one vector to another.                                                            \
 * @param dest Destination vector pointer.                                                                      \
 * @param source Source vector pointer.                                                                         \
 */                                                                                                             \
static void C_IDENTIFIER_BIND(VECTOR_NAME, move)(VECTOR_NAME* dest, VECTOR_NAME* source)                        \
{                                                                                                               \
    _ASSERT(NULL != dest, "Vector dest is NULL");                                                               \
    _ASSERT(NULL != source, "Vector source is NULL");                                                           \
    _ASSERT(NULL != source->arr, "Vector source array is NULL");                                                \
    if (dest == source) return;                                                                                 \
    C_IDENTIFIER_BIND(VECTOR_NAME, destroy)(dest);                                                              \
    *dest = *source;                                                                                            \
    source->capacity = source->size = 0;                                                                        \
    source->arr = NULL;                                                                                         \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Returns the internal data array.                                                                      \
 * @param vec Pointer to vector.                                                                                \
 * @return Pointer to the data array.                                                                           \
 */                                                                                                             \
static TYPE* C_IDENTIFIER_BIND(VECTOR_NAME, data)(const VECTOR_NAME* vec)                                       \
{                                                                                                               \
    _ASSERT(NULL != vec, "Vector is NULL");                                                                     \
    return vec->arr;                                                                                            \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Returns the number of elements in the vector.                                                         \
 * @param vec Pointer to vector.                                                                                \
 * @return Number of elements.                                                                                  \
 */                                                                                                             \
static size_t C_IDENTIFIER_BIND(VECTOR_NAME, size)(const VECTOR_NAME* vec)                                      \
{                                                                                                               \
    _ASSERT(NULL != vec, "Vector is NULL");                                                                     \
    return vec->size;                                                                                           \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Returns the current capacity of the vector.                                                           \
 * @param vec Pointer to vector.                                                                                \
 * @return Current capacity.                                                                                    \
 */                                                                                                             \
static size_t C_IDENTIFIER_BIND(VECTOR_NAME, capacity)(const VECTOR_NAME* vec)                                  \
{                                                                                                               \
    _ASSERT(NULL != vec, "Vector is NULL");                                                                     \
    return vec->capacity;                                                                                       \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Checks if the vector is empty.                                                                        \
 * @param vec Pointer to vector.                                                                                \
 * @return `true` if empty, `false` otherwise.                                                                  \
 */                                                                                                             \
static bool C_IDENTIFIER_BIND(VECTOR_NAME, empty)(const VECTOR_NAME* vec)                                       \
{                                                                                                               \
    _ASSERT(NULL != vec, "Vector is NULL");                                                                     \
    return vec->size == 0;                                                                                      \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Adds an element to the end of the vector.                                                             \
 * @param vec Pointer to vector.                                                                                \
 * @param item Pointer to item to push.                                                                         \
 */                                                                                                             \
static void C_IDENTIFIER_BIND(VECTOR_NAME, push_back)(VECTOR_NAME* vec, const TYPE* item)                       \
{                                                                                                               \
    _ASSERT(NULL != vec, "Vector is NULL");                                                                     \
    if (vec->size >= vec->capacity)                                                                             \
    {                                                                                                           \
        size_t new_cap = vec->capacity + vec->capacity / 2 + 1;                                                 \
        vec->arr = (TYPE*)realloc(vec->arr, sizeof(TYPE) * new_cap);                                            \
        vec->capacity = new_cap;                                                                                \
    }                                                                                                           \
    TYPE_REF_COPY_FUNC(&vec->arr[vec->size++], item);                                                           \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Removes the last element from the vector.                                                             \
 * @param vec Pointer to vector.                                                                                \
 */                                                                                                             \
static void C_IDENTIFIER_BIND(VECTOR_NAME, pop_back)(VECTOR_NAME* vec)                                          \
{                                                                                                               \
    _ASSERT(NULL != vec, "Vector is NULL");                                                                     \
    if (vec->size == 0) return;                                                                                 \
    TYPE_REF_DELETE_FUNC(&vec->arr[--vec->size]);                                                               \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Returns a pointer to the first element.                                                               \
 * @param vec Pointer to vector.                                                                                \
 * @return Pointer to the first element.                                                                        \
 */                                                                                                             \
static TYPE* C_IDENTIFIER_BIND(VECTOR_NAME, element_front)(VECTOR_NAME* vec)                                    \
{                                                                                                               \
    _ASSERT(NULL != vec, "Vector is NULL");                                                                     \
    _ASSERT(vec->size != 0, "Vector is empty");                                                                 \
    return &vec->arr[0];                                                                                        \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Returns a pointer to the last element.                                                                \
 * @param vec Pointer to vector.                                                                                \
 * @return Pointer to the last element.                                                                         \
 */                                                                                                             \
static TYPE* C_IDENTIFIER_BIND(VECTOR_NAME, element_back)(VECTOR_NAME* vec)                                     \
{                                                                                                               \
    _ASSERT(NULL != vec, "Vector is NULL");                                                                     \
    _ASSERT(vec->size != 0, "Vector is empty");                                                                 \
    return &vec->arr[vec->size - 1];                                                                            \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Returns a pointer to the element at index.                                                            \
 * @param vec Pointer to vector.                                                                                \
 * @param index Position of the element to get.                                                                 \
 * @return Pointer to the element at index.                                                                     \
 */                                                                                                             \
static TYPE* C_IDENTIFIER_BIND(VECTOR_NAME, element_at)(VECTOR_NAME* vec, size_t index)                         \
{                                                                                                               \
    _ASSERT(NULL != vec, "Vector is NULL");                                                                     \
    _ASSERT(vec->size != 0, "Vector is empty");                                                                 \
    _ASSERT(index < vec->size, "Index out of bounds");                                                          \
    return &vec->arr[index];                                                                                    \
}                                                                                                               \
                                                                                                                \
/**                                                                                                             \
 * @brief Checks whether two vectors are equal by comparing each element.                                       \
 * @param left Left-hand side pointer to a vector.                                                              \
 * @param right Right-hand side pointer to a vector.                                                            \
 * @return `true` if equal, `false` otherwise.                                                                  \
 */                                                                                                             \
static bool C_IDENTIFIER_BIND(VECTOR_NAME, equals)(const VECTOR_NAME* left, const VECTOR_NAME* right)           \
{                                                                                                               \
    _ASSERT(NULL != left, "Vector left is NULL");                                                               \
    _ASSERT(NULL != right, "Vector right is NULL");                                                             \
    if (left->size != right->size) return false;                                                                \
    for (size_t i = 0; i < left->size; ++i)                                                                     \
        if (!TYPE_REF_EQUALS_FUNC(&left->arr[i], &right->arr[i])) return false;                                 \
    return true;                                                                                                \
}                                                                                                               \


/**
 * @brief Public macro to define a generic vector for a given type with all required dependencies.
 *
 * This macro instantiates:
 * 
 * - The vector API (   `_create`, `_destroy`, `_clear`, `_copy`, `_move`, `_data`, `_size`, `_capacity`, `_empty`,
 *                      `_push_back`, `_pop_back`, `_element_front`, `_element_back`, `_element_at`, `_equals`
 *                  )
 * 
 * @param VECTOR_NAME_PUBLIC_PREFIX     The public name prefix for generated vector.
 * @param TYPE                          Type stored in the vector.
 * @param TYPE_REF_EQUALS_FUNC          Equal comparison function for type.
 * @param TYPE_REF_COPY_FUNC            Copy function for type.
 * @param TYPE_REF_DELETE_FUNC          Deletion/cleanup function for type.
 */
#define DEFINE_GENERIC_VECTOR(                  \
    VECTOR_NAME_PUBLIC_PREFIX,                  \
    TYPE,                                       \
    TYPE_REF_EQUALS_FUNC,                       \
    TYPE_REF_COPY_FUNC,                         \
    TYPE_REF_DELETE_FUNC                        \
)                                               \
                                                \
_DEFINE_GENERIC_VECTOR_IMPL(                    \
    VECTOR_NAME_PUBLIC_PREFIX,                  \
    TYPE,                                       \
    TYPE_REF_EQUALS_FUNC,                       \
    TYPE_REF_COPY_FUNC,                         \
    TYPE_REF_DELETE_FUNC                        \
)                                               \


#endif  // C_VECTOR_H