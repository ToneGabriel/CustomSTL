#pragma once
#include "custom/type_traits.h"

CUSTOM_BEGIN

// iterator tags
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag         : input_iterator_tag {};
struct bidirectional_iterator_tag   : forward_iterator_tag {};
struct random_access_iterator_tag   : bidirectional_iterator_tag {};

CUSTOM_DETAIL_BEGIN

// normal iterator traits helpers
template<class iterator, class = void>
struct _Iterator_Traits {};

template<class iterator>
struct _Iterator_Traits<iterator, void_t<   typename iterator::iterator_category,
                                            typename iterator::value_type,
                                            typename iterator::difference_type,
                                            typename iterator::pointer,
                                            typename iterator::reference>>
{
    using iterator_category = typename iterator::iterator_category;
    using value_type        = typename iterator::value_type;
    using difference_type   = typename iterator::difference_type;
    using pointer           = typename iterator::pointer;
    using reference         = typename iterator::reference;
};

// pointer iterator traits helpers
template<class Type, bool = is_object_v<Type>>
struct _Iterator_Traits_Ptr
{
    using iterator_category = random_access_iterator_tag;
    using value_type        = remove_cv_t<Type>;
    using difference_type   = ptrdiff_t;
    using pointer           = Type*;
    using reference         = Type&;
};

template<class Type>
struct _Iterator_Traits_Ptr<Type, false> {};

CUSTOM_DETAIL_END

// iterator traits
template<class iterator>
struct iterator_traits : detail::_Iterator_Traits<iterator> {};

template<class Type>
struct iterator_traits<Type*> :  detail::_Iterator_Traits_Ptr<Type> {};

// is iterator
template<class Iter, class = void>
constexpr bool is_iterator_v = false;

template<class Iter>
constexpr bool is_iterator_v<Iter, void_t<typename iterator_traits<Iter>::iterator_category>> = true;

template<class Iter>
struct is_iterator : bool_constant<is_iterator_v<Iter>> {};

// is input iterator
template<class Iter>
constexpr bool is_input_iterator_v = is_convertible_v<typename iterator_traits<Iter>::iterator_category, input_iterator_tag>;

// is output iterator
template<class Iter>
constexpr bool is_output_iterator_v = is_convertible_v<typename iterator_traits<Iter>::iterator_category, output_iterator_tag>;

// is forward iterator
template<class Iter>
constexpr bool is_forward_iterator_v = is_convertible_v<typename iterator_traits<Iter>::iterator_category, forward_iterator_tag>;

// is bidirectional iterator
template<class Iter>
constexpr bool is_bidirectional_iterator_v = is_convertible_v<typename iterator_traits<Iter>::iterator_category, bidirectional_iterator_tag>;

// is random access iterator
template<class Iter>
constexpr bool is_random_access_iterator_v = is_convertible_v<typename iterator_traits<Iter>::iterator_category, random_access_iterator_tag>;

CUSTOM_DETAIL_BEGIN

// helpers
template<class Type>
constexpr void _verify_range(const Type* const first, const Type* const last) noexcept
{
    // ALL containers have this function implemented as friend in their iterators
    // This is a specialization for raw pointers
    CUSTOM_ASSERT(first <= last, "Transposed pointer range.");
}

template<class Iter1, class Iter2>
constexpr void _verify_iteration_range(const Iter1& first, const Iter2& last)
{
    // check that [first, last) forms an iterator range
    _verify_range(first, last);
}

template<class OtherIter, class InputIt, class BinaryPredicate>
constexpr void _verify_iteration_order(InputIt first, InputIt last, BinaryPredicate&& pred)
{
    // test if range is ordered by predicate
    // and the value types match for the 2 iterator types

    using OtherValue    = typename iterator_traits<OtherIter>::value_type;
    using IterValue     = typename iterator_traits<InputIt>::value_type;

    if constexpr (is_same_v<OtherValue, IterValue> && is_forward_iterator_v<InputIt>)
        if (first != last)
            for (auto next = first; ++next != last; first = next)
                CUSTOM_ASSERT(!static_cast<bool>(pred(*next, *first)), "Sequence not ordered");
}

template<class BinaryPredicate, class Type1, class Type2,
enable_if_t<is_same_v<remove_cv_ref_t<Type1>, remove_cv_ref_t<Type2>>, bool> = true>
constexpr bool _verify_predicate_order(BinaryPredicate&& pred, Type1&& left, Type2&& right)
noexcept(noexcept(pred(left, right)) && noexcept(pred(right, left)))
{
    // test if pred is strict weak ordering
    // when the arguments are the cv-same-type

    const bool res = static_cast<bool>(pred(left, right));

    if (res)
        CUSTOM_ASSERT(!pred(right, left), "Invalid comparator");

    return res;
}

CUSTOM_DETAIL_END

CUSTOM_END