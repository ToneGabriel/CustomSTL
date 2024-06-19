#pragma once
#include "x_iterator.h"


CUSTOM_BEGIN

#pragma region back_insert_iterator
// helpers
template<class Container, class = void>
constexpr bool _Has_Push_Back_Function_Member_v = false;

template<class Container>
constexpr bool _Has_Push_Back_Function_Member_v<Container,
void_t< typename Container::value_type,
        decltype(custom::declval<Container>().push_back(custom::declval<typename Container::value_type>())),
        decltype(custom::declval<Container>().push_back(custom::move(custom::declval<typename Container::value_type>())))>> = true;

template<class Container>
class back_insert_iterator
{
public:
    static_assert(_Has_Push_Back_Function_Member_v<Container>,
                    "back_insert_iterator require a container with push_back function member!");

    using container_type    = Container;
    using iterator_category = output_iterator_tag;
    using value_type        = void;
    using difference_type   = void;
    using reference         = void;
	using pointer           = void;

protected:
    Container* _container;

public:

    constexpr explicit back_insert_iterator(Container& cont) noexcept
        : _container(&cont) { /*Empty*/ }

    constexpr back_insert_iterator& operator=(const typename Container::value_type& val) {
        _container->push_back(val);
        return *this;
    }

    constexpr back_insert_iterator& operator=(typename Container::value_type&& val) {
        _container->push_back(custom::move(val));
        return *this;
    }

    constexpr back_insert_iterator& operator*() noexcept {
        return *this;
    }

    constexpr back_insert_iterator& operator++() noexcept {
        return *this;
    }

    constexpr back_insert_iterator operator++(int) noexcept {
        return *this;
    }
};

template<class Container>
constexpr back_insert_iterator<Container> back_inserter(Container& cont) noexcept {
    return back_insert_iterator<Container>(cont);
}
#pragma endregion back_insert_iterator

#pragma region front_insert_iterator
// helpers
template<class Container, class = void>
constexpr bool _Has_Push_Front_Function_Member_v = false;

template<class Container>
constexpr bool _Has_Push_Front_Function_Member_v<Container,
void_t< typename Container::value_type,
        decltype(custom::declval<Container>().push_front(custom::declval<typename Container::value_type>())),
        decltype(custom::declval<Container>().push_front(custom::move(custom::declval<typename Container::value_type>())))>> = true;

template<class Container>
class front_insert_iterator
{
public:
    static_assert(_Has_Push_Front_Function_Member_v<Container>,
                    "front_insert_iterator require a container with push_front function member!");

    using container_type    = Container;
    using iterator_category = output_iterator_tag;
    using value_type        = void;
    using difference_type   = void;
    using reference         = void;
	using pointer           = void;

protected:
    Container* _container;

public:

    constexpr explicit front_insert_iterator(Container& cont) noexcept
        : _container(&cont) { /*Empty*/ }

    constexpr front_insert_iterator& operator=(const typename Container::value_type& val) {
        _container->push_front(val);
        return *this;
    }

    constexpr front_insert_iterator& operator=(typename Container::value_type&& val) {
        _container->push_front(custom::move(val));
        return *this;
    }

    constexpr front_insert_iterator& operator*() noexcept {
        return *this;
    }

    constexpr front_insert_iterator& operator++() noexcept {
        return *this;
    }

    constexpr front_insert_iterator operator++(int) noexcept {
        return *this;
    }
};

template<class Container>
constexpr front_insert_iterator<Container> front_inserter(Container& cont) noexcept {
    return front_insert_iterator<Container>(cont);
}
#pragma endregion front_insert_iterator

#pragma region insert_iterator
// helpers
template<class Container, class = void>
constexpr bool _Has_Insert_Function_Member_v = false;

template<class Container>
constexpr bool _Has_Insert_Function_Member_v<Container,
void_t< typename Container::value_type,
        typename Container::iterator,
        decltype(custom::declval<Container>().insert(   custom::declval<typename Container::iterator>(),
                                                        custom::declval<typename Container::value_type>())),
        decltype(custom::declval<Container>().insert(   custom::declval<typename Container::iterator>(),
                                                        custom::move(custom::declval<typename Container::value_type>())))>> = true;

template<class Container>
class insert_iterator
{
public:
    static_assert(_Has_Insert_Function_Member_v<Container>,
                    "insert_iterator require a container with insert function member!");

    using container_type    = Container;
    using iterator_type     = typename Container::iterator;
    using iterator_category = output_iterator_tag;
    using value_type        = void;
    using difference_type   = void;
    using reference         = void;
	using pointer           = void;

protected:
    Container* _container;
    iterator_type iter;

public:

    constexpr insert_iterator(Container& cont, iterator_type iter)
        : _container(&cont), iter(iter) { /*Empty*/ }

    constexpr insert_iterator& operator=(const typename Container::value_type& val) {
        iter = _container->insert(iter, val);
        ++iter;
        return *this;
    }

    constexpr insert_iterator& operator=(typename Container::value_type&& val) {
        iter = _container->insert(iter, custom::move(val));
        ++iter;
        return *this;
    }

    constexpr insert_iterator& operator*() {
        return *this;
    }

    constexpr insert_iterator& operator++() {
        return *this;
    }

    constexpr insert_iterator& operator++(int) {
        return *this;
    }
};

template<class Container>
constexpr insert_iterator<Container> inserter(Container& cont, typename Container::iterator iter) {
    return insert_iterator<Container>(cont, iter);
}
#pragma endregion insert_iterator

#pragma region reverse_iterator
// helpers for reverse_iterator operator->()
template<class iterator, class pointer, bool = is_pointer_v<remove_cv_ref_t<iterator>>>
constexpr bool _Has_Nothrow_Operator_Arrow_v = is_nothrow_convertible_v<iterator, pointer>;

template<class iterator, class pointer>
constexpr bool _Has_Nothrow_Operator_Arrow_v<iterator, pointer, false> =
noexcept(_fake_copy_init<pointer>(custom::declval<iterator>().operator->()));

template<class iterator>
class reverse_iterator                           // Adaptor for backwards iteration
{
public:
    static_assert(is_bidirectional_iterator_v<iterator>,
                    "reverse_iterator require Bidirectional iterator type!");

    using iterator_type     = iterator;
    using iterator_category = typename iterator_traits<iterator>::iterator_category;
    using value_type        = typename iterator_traits<iterator>::value_type;
    using difference_type   = typename iterator_traits<iterator>::difference_type;
    using reference         = typename iterator_traits<iterator>::reference;
	using pointer           = typename iterator_traits<iterator>::pointer;

    iterator_type Current;

public:

    constexpr reverse_iterator()                             = default;
    constexpr reverse_iterator(const reverse_iterator& other) = default;

    constexpr explicit reverse_iterator(iterator_type base)
    noexcept(is_nothrow_copy_constructible_v<iterator_type>)
        : Current(base) { /*Empty*/ }

    constexpr reverse_iterator& operator=(const reverse_iterator& other) = default;

    constexpr reverse_iterator& operator++() noexcept(noexcept(--Current)) {
        --Current;
        return *this;
    }

    constexpr reverse_iterator operator++(int)
    noexcept(is_nothrow_copy_constructible_v<iterator_type> && noexcept(--Current)) {
        reverse_iterator temp = *this;
        --Current;
        return temp;
    }

    constexpr reverse_iterator& operator--() noexcept(noexcept(++Current)) {
        ++Current;
        return *this;
    }

    constexpr reverse_iterator operator--(int)
    noexcept(is_nothrow_copy_constructible_v<iterator_type> && noexcept(++Current)) {
        reverse_iterator temp = *this;
        ++Current;
        return temp;
    }

    constexpr pointer operator->() const
    noexcept(   is_nothrow_copy_constructible_v<iterator_type> &&
                noexcept(--(custom::declval<iterator_type&>())) &&
                _Has_Nothrow_Operator_Arrow_v<iterator_type&, pointer>) {

        iterator_type temp = Current;
        --temp;

        if constexpr (is_pointer_v<iterator_type>)
            return temp;

        return temp.operator->();
    }

    constexpr reference operator*() const
    noexcept(is_nothrow_copy_constructible_v<iterator_type> && noexcept(*--(custom::declval<iterator_type&>()))) {
        iterator_type temp = Current;
        return *(--temp);
    }

    constexpr iterator_type base() const
    noexcept(is_nothrow_copy_constructible_v<iterator_type>) {
        return Current;
    }

    constexpr const iterator_type& const_ref_base() const noexcept {
        return Current;
    }
}; // END reverse_iterator


// reverse_iterator binary operators
template<class Iterator1, class Iterator2>
constexpr bool operator==(const reverse_iterator<Iterator1>& left, const reverse_iterator<Iterator2>& right)
noexcept(noexcept(_fake_copy_init<bool>(left.const_ref_base() == right.const_ref_base()))) {
    return left.const_ref_base() == right.const_ref_base();
}

template<class Iterator1, class Iterator2>
constexpr bool operator!=(const reverse_iterator<Iterator1>& left, const reverse_iterator<Iterator2>& right)
noexcept(noexcept(_fake_copy_init<bool>(left.const_ref_base() != right.const_ref_base()))) {
    return left.const_ref_base() != right.const_ref_base();
}

template<class iterator>
constexpr reverse_iterator<iterator> make_reverse_iterator(iterator base)
noexcept(is_nothrow_copy_constructible_v<iterator>) {
    return reverse_iterator<iterator>(base);
}
#pragma endregion reverse_iterator

#pragma region Iterator Functions
template<class InputIt>
constexpr typename custom::iterator_traits<InputIt>::difference_type distance(InputIt first, InputIt last) {
    _verify_iteration_range(first, last);
    
    if constexpr (is_random_access_iterator_v<InputIt>)
        return last - first; // assume the iterator will do debug checking
    else
    {
        typename custom::iterator_traits<InputIt>::difference_type dist = 0;

        for (/*Empty*/; first != last; ++first)
            ++dist;

        return dist;
    }
}

template<class InputIt, class Offset>
constexpr void advance(InputIt& first, Offset off) {
    if constexpr (is_random_access_iterator_v<InputIt>)
        first += off;
    else
    {
        if constexpr (is_signed_v<Offset> && !is_bidirectional_iterator_v<InputIt>)
            CUSTOM_ASSERT(off >= 0, "Negative advance of non-bidirectional iterator!");

        for (/*Empty*/; off > 0; --off)  // happens for both input and bidirectional iterator
            ++first;

        if constexpr (is_signed_v<Offset> && is_bidirectional_iterator_v<InputIt>)
            for (/*Empty*/; off < 0; ++off)
                --first;
    }
}

template<class InputIt>
constexpr InputIt next(InputIt first, typename custom::iterator_traits<InputIt>::difference_type off = 1) {
    static_assert(is_input_iterator_v<InputIt>, "next requires input iterator!");

    custom::advance(first, off);
    return first;
}

template<class InputIt>
constexpr InputIt prev(InputIt first, typename custom::iterator_traits<InputIt>::difference_type off = 1) {
    static_assert(is_bidirectional_iterator_v<InputIt>, "prev requires bidirectional iterator!");

    custom::advance(first, -off);
    return first;
}
#pragma endregion Iterator Functions

CUSTOM_END