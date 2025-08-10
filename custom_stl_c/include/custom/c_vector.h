#ifndef C_VECTOR_H
#define C_VECTOR_H


#include "custom/c_utility.h"


#define _DEFINE_GENERIC_VECTOR_DATA(                                \
    VECTOR_NAME,                                                    \
    TYPE                                                            \
)                                                                   \
                                                                    \
/**                                                                 \
 * @struct VECTOR_NAME                                              \
 * @brief Represents a dynamic array of elements of type TYPE.      \
 */                                                                 \
typedef struct                                                      \
{                                                                   \
    TYPE* first;                                                    \
    TYPE* last;                                                     \
    TYPE* final;                                                    \
} VECTOR_NAME;                                                      \


#define _DEFINE_GENERIC_VECTOR_ITERATOR(                                                                                    \
    VECTOR_ITERATOR_NAME,                                                                                                   \
    VECTOR_NAME,                                                                                                            \
    TYPE                                                                                                                    \
)                                                                                                                           \
                                                                                                                            \
typedef struct                                                                                                              \
{                                                                                                                           \
    TYPE* ptr;                                                                                                              \
    VECTOR_NAME* vec;                                                                                                       \
} VECTOR_ITERATOR_NAME;                                                                                                     \
                                                                                                                            \
static VECTOR_ITERATOR_NAME C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, create)(TYPE* ptr, VECTOR_NAME* vec);                   \
static void C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, pre_increment)(VECTOR_ITERATOR_NAME* iter);                             \
static VECTOR_ITERATOR_NAME C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, post_increment)(VECTOR_ITERATOR_NAME* iter);            \
static void C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, increment_by)(VECTOR_ITERATOR_NAME* iter, ptrdiff_t diff);              \
static VECTOR_ITERATOR_NAME C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, increment)(VECTOR_ITERATOR_NAME* iter, ptrdiff_t diff); \
static void C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, pre_decrement)(VECTOR_ITERATOR_NAME* iter);                             \
static VECTOR_ITERATOR_NAME C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, post_decrement)(VECTOR_ITERATOR_NAME* iter);            \
static void C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, decrement_by)(VECTOR_ITERATOR_NAME* iter, ptrdiff_t diff);              \
static VECTOR_ITERATOR_NAME C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, decrement)(VECTOR_ITERATOR_NAME* iter, ptrdiff_t diff); \
static TYPE* C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, get)(VECTOR_ITERATOR_NAME* iter);                                      \
static bool C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, equals)(VECTOR_ITERATOR_NAME* left, VECTOR_ITERATOR_NAME* right);       \
                                                                                                                            \
static VECTOR_ITERATOR_NAME C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, create)(TYPE* ptr, VECTOR_NAME* vec)                    \
{                                                                                                                           \
    VECTOR_ITERATOR_NAME iter = {                                                                                           \
        .ptr = ptr,                                                                                                         \
        .vec = vec                                                                                                          \
    };                                                                                                                      \
    return iter;                                                                                                            \
}                                                                                                                           \
                                                                                                                            \
static void C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, pre_increment)(VECTOR_ITERATOR_NAME* iter)                              \
{                                                                                                                           \
    _C_CUSTOM_ASSERT(iter->ptr < iter->vec->last, "Cannot increment end iterator.");                                        \
    ++iter->ptr;                                                                                                            \
}                                                                                                                           \
                                                                                                                            \
static VECTOR_ITERATOR_NAME C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, post_increment)(VECTOR_ITERATOR_NAME* iter)             \
{                                                                                                                           \
    VECTOR_ITERATOR_NAME temp = *iter;                                                                                      \
    C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, pre_increment)(iter);                                                           \
    return temp;                                                                                                            \
}                                                                                                                           \
                                                                                                                            \
static void C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, increment_by)(VECTOR_ITERATOR_NAME* iter, ptrdiff_t diff)               \
{                                                                                                                           \
    _C_CUSTOM_ASSERT(iter->ptr + diff < iter->vec->last, "Cannot increment end iterator.");                                 \
    iter->ptr += diff;                                                                                                      \
}                                                                                                                           \
                                                                                                                            \
static VECTOR_ITERATOR_NAME C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, increment)(VECTOR_ITERATOR_NAME* iter, ptrdiff_t diff)  \
{                                                                                                                           \
    VECTOR_ITERATOR_NAME temp = *iter;                                                                                      \
    C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, increment_by)(&temp, diff);                                                     \
    return temp;                                                                                                            \
}                                                                                                                           \
                                                                                                                            \
static void C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, pre_decrement)(VECTOR_ITERATOR_NAME* iter)                              \
{                                                                                                                           \
    _C_CUSTOM_ASSERT(iter->ptr > iter->vec->first, "Cannot decrement begin iterator.");                                     \
    --iter->ptr;                                                                                                            \
}                                                                                                                           \
                                                                                                                            \
static VECTOR_ITERATOR_NAME C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, post_decrement)(VECTOR_ITERATOR_NAME* iter)             \
{                                                                                                                           \
    VECTOR_ITERATOR_NAME temp = *iter;                                                                                      \
    C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, pre_decrement)(iter);                                                           \
    return temp;                                                                                                            \
}                                                                                                                           \
                                                                                                                            \
static void C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, decrement_by)(VECTOR_ITERATOR_NAME* iter, ptrdiff_t diff)               \
{                                                                                                                           \
    _C_CUSTOM_ASSERT(iter->ptr + diff > iter->vec->first, "Cannot decrement begin iterator.");                              \
    iter->ptr -= diff;                                                                                                      \
}                                                                                                                           \
                                                                                                                            \
static VECTOR_ITERATOR_NAME C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, decrement)(VECTOR_ITERATOR_NAME* iter, ptrdiff_t diff)  \
{                                                                                                                           \
    VECTOR_ITERATOR_NAME temp = *iter;                                                                                      \
    C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, decrement_by)(&temp, diff);                                                     \
    return temp;                                                                                                            \
}                                                                                                                           \
                                                                                                                            \
static TYPE* C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, get)(VECTOR_ITERATOR_NAME* iter)                                       \
{                                                                                                                           \
    _C_CUSTOM_ASSERT(iter->ptr < iter->vec->last, "Cannot dereference end iterator.");                                      \
    return iter->ptr;                                                                                                       \
}                                                                                                                           \
                                                                                                                            \
static bool C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, equals)(VECTOR_ITERATOR_NAME* left, VECTOR_ITERATOR_NAME* right)        \
{                                                                                                                           \
    return left->ptr == right->ptr;                                                                                         \
}                                                                                                                           \



/**
 * @def _DEFINE_GENERIC_VECTOR_IMPL
 * @brief Internal macro that defines the complete implementation of a vector.
 *
 * This macro generates a full implementation of a dynamic array.
 * The generated vector operates on elements of type `TYPE`.
 *
 * @param VECTOR_NAME               Public-facing name prefix for struct and functions
 * @param VECTOR_ITERATOR_NAME      Public-facing name prefix for iterator struct and functions
 * @param TYPE                      Type of elements to be stored in the vector.
 * @param TYPE_REF_EQUALS_FUNC      Function comparing two `TYPE*` for equality.
 * @param TYPE_REF_COPY_FUNC        Function that copies from `TYPE*` to `TYPE*`
 * @param TYPE_REF_DELETE_FUNC      Function that deletes/frees the internal data of a `TYPE*`
 */
#define _DEFINE_GENERIC_VECTOR_IMPL(                                                                                    \
    VECTOR_NAME,                                                                                                        \
    VECTOR_ITERATOR_NAME,                                                                                               \
    TYPE,                                                                                                               \
    TYPE_REF_EQUALS_FUNC,                                                                                               \
    TYPE_REF_COPY_FUNC,                                                                                                 \
    TYPE_REF_DELETE_FUNC                                                                                                \
)                                                                                                                       \
                                                                                                                        \
static VECTOR_NAME          C_IDENTIFIER_BIND(VECTOR_NAME, create)(size_t capacity);                                    \
static void                 C_IDENTIFIER_BIND(VECTOR_NAME, destroy)(VECTOR_NAME* vec);                                  \
static void                 C_IDENTIFIER_BIND(VECTOR_NAME, clear)(VECTOR_NAME* vec);                                    \
static void                 C_IDENTIFIER_BIND(VECTOR_NAME, copy)(VECTOR_NAME* dest, const VECTOR_NAME* source);         \
static void                 C_IDENTIFIER_BIND(VECTOR_NAME, move)(VECTOR_NAME* dest, VECTOR_NAME* source);               \
static TYPE*                C_IDENTIFIER_BIND(VECTOR_NAME, data)(const VECTOR_NAME* vec);                               \
static size_t               C_IDENTIFIER_BIND(VECTOR_NAME, size)(const VECTOR_NAME* vec);                               \
static size_t               C_IDENTIFIER_BIND(VECTOR_NAME, capacity)(const VECTOR_NAME* vec);                           \
static bool                 C_IDENTIFIER_BIND(VECTOR_NAME, empty)(const VECTOR_NAME* vec);                              \
static void                 C_IDENTIFIER_BIND(VECTOR_NAME, push_back)(VECTOR_NAME* vec, const TYPE* item);              \
static void                 C_IDENTIFIER_BIND(VECTOR_NAME, pop_back)(VECTOR_NAME* vec);                                 \
static TYPE*                C_IDENTIFIER_BIND(VECTOR_NAME, element_front)(VECTOR_NAME* vec);                            \
static TYPE*                C_IDENTIFIER_BIND(VECTOR_NAME, element_back)(VECTOR_NAME* vec);                             \
static TYPE*                C_IDENTIFIER_BIND(VECTOR_NAME, element_at)(VECTOR_NAME* vec, size_t index);                 \
static bool                 C_IDENTIFIER_BIND(VECTOR_NAME, equals)(const VECTOR_NAME* left, const VECTOR_NAME* right);  \
static VECTOR_ITERATOR_NAME C_IDENTIFIER_BIND(VECTOR_NAME, begin)(VECTOR_NAME* vec);                                    \
static VECTOR_ITERATOR_NAME C_IDENTIFIER_BIND(VECTOR_NAME, end)(VECTOR_NAME* vec);                                      \
                                                                                                                        \
/**                                                                                                                     \
 * @brief Creates a vector with an initial capacity.                                                                    \
 * @param capacity Initial capacity of the vector.                                                                      \
 * @return A new instance of VECTOR_NAME.                                                                               \
 */                                                                                                                     \
static VECTOR_NAME C_IDENTIFIER_BIND(VECTOR_NAME, create)(size_t capacity)                                              \
{                                                                                                                       \
    _C_CUSTOM_ASSERT(0 < capacity, "Vector capacity should be greater than 0");                                         \
    TYPE* arr = (TYPE*)malloc(sizeof(TYPE) * capacity);                                                                 \
    VECTOR_NAME vec = {                                                                                                 \
        .first = arr,                                                                                                   \
        .last = arr,                                                                                                    \
        .final = arr + capacity                                                                                         \
    };                                                                                                                  \
    return vec;                                                                                                         \
}                                                                                                                       \
                                                                                                                        \
/**                                                                                                                     \
 * @brief Destroys a vector and releases allocated memory.                                                              \
 * @param vec Pointer to the vector.                                                                                    \
 */                                                                                                                     \
static void C_IDENTIFIER_BIND(VECTOR_NAME, destroy)(VECTOR_NAME* vec)                                                   \
{                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != vec, "Vector is NULL");                                                                    \
    if (NULL == vec->first) return;                                                                                     \
    size_t vec_size = C_IDENTIFIER_BIND(VECTOR_NAME, size)(vec);                                                        \
    for (size_t i = 0; i < vec_size; ++i)                                                                               \
        TYPE_REF_DELETE_FUNC(vec->first + i);                                                                           \
    free(vec->first);                                                                                                   \
    vec->first = vec->last = vec->final = NULL;                                                                         \
}                                                                                                                       \
                                                                                                                        \
/**                                                                                                                     \
 * @brief Clears the vector contents but retains capacity.                                                              \
 * @param vec Pointer to the vector.                                                                                    \
 */                                                                                                                     \
static void C_IDENTIFIER_BIND(VECTOR_NAME, clear)(VECTOR_NAME* vec)                                                     \
{                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != vec, "Vector is NULL");                                                                    \
    size_t vec_size = C_IDENTIFIER_BIND(VECTOR_NAME, size)(vec);                                                        \
    for (size_t i = 0; i < vec_size; ++i)                                                                               \
        TYPE_REF_DELETE_FUNC(vec->first + i);                                                                           \
    vec->last = vec->first;                                                                                             \
}                                                                                                                       \
                                                                                                                        \
/**                                                                                                                     \
 * @brief Copies contents of one vector to another.                                                                     \
 * @param dest Destination vector pointer.                                                                              \
 * @param source Source vector pointer.                                                                                 \
 */                                                                                                                     \
static void C_IDENTIFIER_BIND(VECTOR_NAME, copy)(VECTOR_NAME* dest, const VECTOR_NAME* source)                          \
{                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != dest, "Vector dest is NULL");                                                              \
    _C_CUSTOM_ASSERT(NULL != source, "Vector source is NULL");                                                          \
    _C_CUSTOM_ASSERT(NULL != source->first, "Vector source array is NULL");                                             \
    if (dest == source) return;                                                                                         \
    C_IDENTIFIER_BIND(VECTOR_NAME, destroy)(dest);                                                                      \
    size_t new_capacity = C_IDENTIFIER_BIND(VECTOR_NAME, capacity)(source);                                             \
    size_t new_size = C_IDENTIFIER_BIND(VECTOR_NAME, size)(source);                                                     \
    *dest = C_IDENTIFIER_BIND(VECTOR_NAME, create)(new_capacity);                                                       \
    for (size_t i = 0; i < new_size; ++i)                                                                               \
        TYPE_REF_COPY_FUNC(dest->first + i, source->first+ i);                                                          \
    dest->last = dest->first + new_size;                                                                                \
}                                                                                                                       \
                                                                                                                        \
/**                                                                                                                     \
 * @brief Moves contents from one vector to another.                                                                    \
 * @param dest Destination vector pointer.                                                                              \
 * @param source Source vector pointer.                                                                                 \
 */                                                                                                                     \
static void C_IDENTIFIER_BIND(VECTOR_NAME, move)(VECTOR_NAME* dest, VECTOR_NAME* source)                                \
{                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != dest, "Vector dest is NULL");                                                              \
    _C_CUSTOM_ASSERT(NULL != source, "Vector source is NULL");                                                          \
    _C_CUSTOM_ASSERT(NULL != source->first, "Vector source array is NULL");                                             \
    if (dest == source) return;                                                                                         \
    C_IDENTIFIER_BIND(VECTOR_NAME, destroy)(dest);                                                                      \
    *dest = *source;                                                                                                    \
    source->first = source->last = source->final = NULL;                                                                \
}                                                                                                                       \
                                                                                                                        \
/**                                                                                                                     \
 * @brief Returns the internal data array.                                                                              \
 * @param vec Pointer to vector.                                                                                        \
 * @return Pointer to the data array.                                                                                   \
 */                                                                                                                     \
static TYPE* C_IDENTIFIER_BIND(VECTOR_NAME, data)(const VECTOR_NAME* vec)                                               \
{                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != vec, "Vector is NULL");                                                                    \
    return vec->first;                                                                                                  \
}                                                                                                                       \
                                                                                                                        \
/**                                                                                                                     \
 * @brief Returns the number of elements in the vector.                                                                 \
 * @param vec Pointer to vector.                                                                                        \
 * @return Number of elements.                                                                                          \
 */                                                                                                                     \
static size_t C_IDENTIFIER_BIND(VECTOR_NAME, size)(const VECTOR_NAME* vec)                                              \
{                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != vec, "Vector is NULL");                                                                    \
    return vec->last - vec->first;                                                                                      \
}                                                                                                                       \
                                                                                                                        \
/**                                                                                                                     \
 * @brief Returns the current capacity of the vector.                                                                   \
 * @param vec Pointer to vector.                                                                                        \
 * @return Current capacity.                                                                                            \
 */                                                                                                                     \
static size_t C_IDENTIFIER_BIND(VECTOR_NAME, capacity)(const VECTOR_NAME* vec)                                          \
{                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != vec, "Vector is NULL");                                                                    \
    return vec->final - vec->first;                                                                                     \
}                                                                                                                       \
                                                                                                                        \
/**                                                                                                                     \
 * @brief Checks if the vector is empty.                                                                                \
 * @param vec Pointer to vector.                                                                                        \
 * @return `true` if empty, `false` otherwise.                                                                          \
 */                                                                                                                     \
static bool C_IDENTIFIER_BIND(VECTOR_NAME, empty)(const VECTOR_NAME* vec)                                               \
{                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != vec, "Vector is NULL");                                                                    \
    return vec->last == vec->first;                                                                                     \
}                                                                                                                       \
                                                                                                                        \
/**                                                                                                                     \
 * @brief Adds an element to the end of the vector.                                                                     \
 * @param vec Pointer to vector.                                                                                        \
 * @param item Pointer to item to push.                                                                                 \
 */                                                                                                                     \
static void C_IDENTIFIER_BIND(VECTOR_NAME, push_back)(VECTOR_NAME* vec, const TYPE* item)                               \
{                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != vec, "Vector is NULL");                                                                    \
    if (vec->last >= vec->final)                                                                                        \
    {                                                                                                                   \
        size_t old_size = C_IDENTIFIER_BIND(VECTOR_NAME, size)(vec);                                                    \
        size_t old_cap = C_IDENTIFIER_BIND(VECTOR_NAME, capacity)(vec);                                                 \
        size_t new_cap = old_cap + old_cap / 2 + 1;                                                                     \
        vec->first = (TYPE*)realloc(vec->first, sizeof(TYPE) * new_cap);                                                \
        vec->last = vec->first + old_size;                                                                              \
        vec->final = vec->first + new_cap;                                                                              \
    }                                                                                                                   \
    TYPE_REF_COPY_FUNC(vec->last++, item);                                                                              \
}                                                                                                                       \
                                                                                                                        \
/**                                                                                                                     \
 * @brief Removes the last element from the vector.                                                                     \
 * @param vec Pointer to vector.                                                                                        \
 */                                                                                                                     \
static void C_IDENTIFIER_BIND(VECTOR_NAME, pop_back)(VECTOR_NAME* vec)                                                  \
{                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != vec, "Vector is NULL");                                                                    \
    if (vec->first == vec->last) return;                                                                                \
    TYPE_REF_DELETE_FUNC(--vec->last);                                                                                  \
}                                                                                                                       \
                                                                                                                        \
/**                                                                                                                     \
 * @brief Returns a pointer to the first element.                                                                       \
 * @param vec Pointer to vector.                                                                                        \
 * @return Pointer to the first element.                                                                                \
 */                                                                                                                     \
static TYPE* C_IDENTIFIER_BIND(VECTOR_NAME, element_front)(VECTOR_NAME* vec)                                            \
{                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != vec, "Vector is NULL");                                                                    \
    _C_CUSTOM_ASSERT(vec->first == vec->last, "Vector is empty");                                                       \
    return vec->first;                                                                                                  \
}                                                                                                                       \
                                                                                                                        \
/**                                                                                                                     \
 * @brief Returns a pointer to the last element.                                                                        \
 * @param vec Pointer to vector.                                                                                        \
 * @return Pointer to the last element.                                                                                 \
 */                                                                                                                     \
static TYPE* C_IDENTIFIER_BIND(VECTOR_NAME, element_back)(VECTOR_NAME* vec)                                             \
{                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != vec, "Vector is NULL");                                                                    \
    _C_CUSTOM_ASSERT(vec->first == vec->last, "Vector is empty");                                                       \
    return vec->last;                                                                                                   \
}                                                                                                                       \
                                                                                                                        \
/**                                                                                                                     \
 * @brief Returns a pointer to the element at index.                                                                    \
 * @param vec Pointer to vector.                                                                                        \
 * @param index Position of the element to get.                                                                         \
 * @return Pointer to the element at index.                                                                             \
 */                                                                                                                     \
static TYPE* C_IDENTIFIER_BIND(VECTOR_NAME, element_at)(VECTOR_NAME* vec, size_t index)                                 \
{                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != vec, "Vector is NULL");                                                                    \
    _C_CUSTOM_ASSERT(vec->first + index < vec->last, "Index out of bounds");                                            \
    return vec->first + index;                                                                                          \
}                                                                                                                       \
                                                                                                                        \
/**                                                                                                                     \
 * @brief Checks whether two vectors are equal by comparing each element.                                               \
 * @param left Left-hand side pointer to a vector.                                                                      \
 * @param right Right-hand side pointer to a vector.                                                                    \
 * @return `true` if equal, `false` otherwise.                                                                          \
 */                                                                                                                     \
static bool C_IDENTIFIER_BIND(VECTOR_NAME, equals)(const VECTOR_NAME* left, const VECTOR_NAME* right)                   \
{                                                                                                                       \
    _C_CUSTOM_ASSERT(NULL != left, "Vector left is NULL");                                                              \
    _C_CUSTOM_ASSERT(NULL != right, "Vector right is NULL");                                                            \
    if (C_IDENTIFIER_BIND(VECTOR_NAME, size)(left) ==                                                                   \
        C_IDENTIFIER_BIND(VECTOR_NAME, size)(right)) return false;                                                      \
    size_t s = C_IDENTIFIER_BIND(VECTOR_NAME, size)(left);                                                              \
    for (size_t i = 0; i < s; ++i)                                                                                      \
        if (!TYPE_REF_EQUALS_FUNC(left->first + i, right->first + i)) return false;                                     \
    return true;                                                                                                        \
}                                                                                                                       \
                                                                                                                        \
/**                                                                                                                     \
 * @brief Returns an iterator to the first element in vector.                                                           \
 * @param vec Pointer to vector.                                                                                        \
 * @return A new instance of VECTOR_ITERATOR_NAME.                                                                      \
 */                                                                                                                     \
static VECTOR_ITERATOR_NAME C_IDENTIFIER_BIND(VECTOR_NAME, begin)(VECTOR_NAME* vec)                                     \
{                                                                                                                       \
    VECTOR_ITERATOR_NAME iter = C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, create)(vec->first, vec);                       \
    return iter;                                                                                                        \
}                                                                                                                       \
                                                                                                                        \
/**                                                                                                                     \
 * @brief Returns an iterator to the last + 1 element in vector.                                                        \
 * @param vec Pointer to vector.                                                                                        \
 * @return A new instance of VECTOR_ITERATOR_NAME.                                                                      \
 */                                                                                                                     \
static VECTOR_ITERATOR_NAME C_IDENTIFIER_BIND(VECTOR_NAME, end)(VECTOR_NAME* vec)                                       \
{                                                                                                                       \
    VECTOR_ITERATOR_NAME iter = C_IDENTIFIER_BIND(VECTOR_ITERATOR_NAME, create)(vec->last, vec);                        \
    return iter;                                                                                                        \
}                                                                                                                       \


/**
 * @brief Public macro to define a generic vector for a given type with all required dependencies.
 *
 * This macro instantiates:
 * 
 * - The vector API (   `_create`, `_destroy`, `_clear`, `_copy`, `_move`, `_data`, `_size`, `_capacity`, `_empty`,
 *                      `_push_back`, `_pop_back`, `_element_front`, `_element_back`, `_element_at`, `_equals`,
 *                      `_begin`, `_end`
 *                  )
 * 
 * @param VECTOR_NAME_PUBLIC_PREFIX     The public name prefix for generated vector (e.g., `MyVec` -> `MyVec_create`, etc.).
 * @param TYPE                          Type stored in the vector.
 * @param TYPE_REF_EQUALS_FUNC          Function comparing two `TYPE*` for equality
 * @param TYPE_REF_COPY_FUNC            Function that copies from `TYPE*` to `TYPE*`
 * @param TYPE_REF_DELETE_FUNC          Function that deletes/frees the internal data of a `TYPE*`
 */
#define DEFINE_GENERIC_VECTOR(                              \
    VECTOR_NAME_PUBLIC_PREFIX,                              \
    TYPE,                                                   \
    TYPE_REF_EQUALS_FUNC,                                   \
    TYPE_REF_COPY_FUNC,                                     \
    TYPE_REF_DELETE_FUNC                                    \
)                                                           \
                                                            \
_DEFINE_GENERIC_VECTOR_DATA(                                \
    VECTOR_NAME_PUBLIC_PREFIX,                              \
    TYPE                                                    \
)                                                           \
                                                            \
_DEFINE_GENERIC_VECTOR_ITERATOR(                            \
    C_IDENTIFIER_BIND(VECTOR_NAME_PUBLIC_PREFIX, ITERATOR), \
    VECTOR_NAME_PUBLIC_PREFIX,                              \
    TYPE                                                    \
)                                                           \
                                                            \
_DEFINE_GENERIC_VECTOR_IMPL(                                \
    VECTOR_NAME_PUBLIC_PREFIX,                              \
    C_IDENTIFIER_BIND(VECTOR_NAME_PUBLIC_PREFIX, ITERATOR), \
    TYPE,                                                   \
    TYPE_REF_EQUALS_FUNC,                                   \
    TYPE_REF_COPY_FUNC,                                     \
    TYPE_REF_DELETE_FUNC                                    \
)                                                           \


#endif  // C_VECTOR_H