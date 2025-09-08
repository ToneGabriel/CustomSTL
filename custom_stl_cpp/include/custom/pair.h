#pragma once
#include "custom/tuple.h"
#include "custom/utility.h"


CUSTOM_BEGIN

// tag struct declaration for piecewise construction of pair objects
struct piecewise_construct_t { explicit piecewise_construct_t() = default; };
constexpr piecewise_construct_t piecewise_construct = piecewise_construct_t();


template<class Type1, class Type2>
struct pair
{
public:
    using first_type    = Type1;
    using second_type   = Type2;

    first_type first;
    second_type second;

protected:
    // Constructor Helpers

    // (H1) Helper for (5)
    template<class Tuple1, class Tuple2, size_t... Indices1, size_t... Indices2>
    constexpr pair(Tuple1& t1, Tuple2& t2, index_sequence<Indices1...>, index_sequence<Indices2...>)
        :   first(get<Indices1>(custom::forward<Tuple1>(t1))...),
            second(get<Indices2>(custom::forward<Tuple2>(t2))...) { /*Empty*/ }

public:
    // Constructors

    // (0) Default constructor
    template<class Other1 = Type1, class Other2 = Type2,
    enable_if_t<conjunction_v<  is_default_constructible<Other1>,
                                is_default_constructible<Other2>>, bool> = true>
    constexpr explicit(!conjunction_v<  is_implicitly_default_constructible<Other1>,
                                        is_implicitly_default_constructible<Other2>>)
    pair()
    noexcept(conjunction_v< is_nothrow_default_constructible<Other1>,
                            is_nothrow_default_constructible<Other2>>)
        :   first(),
            second() { /*Empty*/ }

    // (1) Copy components constructor
    template<class Other1 = Type1, class Other2 = Type2,
    enable_if_t<conjunction_v<  is_copy_constructible<Other1>,
                                is_copy_constructible<Other2>>, bool> = true>
    constexpr explicit(!conjunction_v<  is_convertible<const Other1&, Other1>,
                                        is_convertible<const Other2&, Other2>>)
    pair(const Type1& val1, const Type2& val2)
    noexcept(conjunction_v< is_nothrow_copy_constructible<Other1>,
                            is_nothrow_copy_constructible<Other2>>)
        :   first(val1),
            second(val2) { /*Empty*/ }

    // (2) Move components constructor
    template<class Other1 = Type1, class Other2 = Type2,
    enable_if_t<conjunction_v<  is_constructible<Type1, Other1>,
                                is_constructible<Type2, Other2>>, bool> = true>
    constexpr explicit(!conjunction_v<  is_convertible<Other1, Type1>,
                                        is_convertible<Other2, Type2>>)
    pair(Other1&& val1, Other2&& val2)
    noexcept(conjunction_v< is_nothrow_constructible<Type1, Other1>,
                            is_nothrow_constructible<Type2, Other2>>)
        :   first(custom::forward<Other1>(val1)),
            second(custom::forward<Other2>(val2)) { /*Empty*/ }

    // (3) Copy pair constructor
    template<class Other1, class Other2,
    enable_if_t<conjunction_v<  is_constructible<Type1, const Other1&>,
                                is_constructible<Type2, const Other2&>>, bool> = true>
    constexpr explicit(!conjunction_v<  is_convertible<const Other1&, Type1>,
                                        is_convertible<const Other2&, Type2>>)
    pair(const pair<Other1, Other2>& other)
    noexcept(conjunction_v< is_nothrow_constructible<Type1, const Other1&>,
                            is_nothrow_constructible<Type2, const Other2&>>)
        :   first(other.first),
            second(other.second) { /*Empty*/ }

    // (4) Move pair constructor
    template<class Other1, class Other2,
    enable_if_t<conjunction_v<  is_constructible<Type1, Other1>,
                                is_constructible<Type2, Other2>>, bool> = true>
    constexpr explicit(!conjunction_v<  is_convertible<Other1, Type1>,
                                        is_convertible<Other2, Type2>>)
    pair(pair<Other1, Other2>&& other)
    noexcept(conjunction_v< is_nothrow_constructible<Type1, Other1>,
                            is_nothrow_constructible<Type2, Other2>>)
        :   first(custom::forward<Other1>(other.first)),
            second(custom::forward<Other2>(other.second)) { /*Empty*/ }

    // (5) Piecewise constructor. Uses (H1)
    template<class... Types1, class... Types2>
    constexpr pair(piecewise_construct_t, tuple<Types1...> t1, tuple<Types2...> t2)
        : pair(t1, t2, index_sequence_for<Types1...>{}, index_sequence_for<Types2...>{}) { /*Empty*/ }

    pair(const pair&)   = default;
    pair(pair&&)        = default;

public:
    // Operators

    // Copy convertible operator
    template<class Other1, class Other2,
    enable_if_t<conjunction_v<  negation<is_same<pair, pair<Other1, Other2>>>,
                                is_assignable<Type1&, const Other1&>,
                                is_assignable<Type2&, const Other2&>>, bool> = true>
    constexpr pair& operator=(const pair<Other1, Other2>& other)
    noexcept(conjunction_v< is_nothrow_assignable<Type1&, const Other1&>,
                            is_nothrow_assignable<Type2&, const Other2&>>)
    {
        first  = other.first;
        second = other.second;
        return *this;
    }

    // Move convertible operator
    template<class Other1, class Other2,
    enable_if_t<conjunction_v<  negation<is_same<pair, pair<Other1, Other2>>>,
                                is_assignable<Type1&, Other1>,
                                is_assignable<Type2&, Other2>>, bool> = true>
    constexpr pair& operator=(pair<Other1, Other2>&& other)
    noexcept(conjunction_v< is_nothrow_assignable<Type1&, Other1>,
                            is_nothrow_assignable<Type2&, Other2>>)
    {
        first  = custom::forward<Other1>(other.first);
        second = custom::forward<Other2>(other.second);
        return *this;
    }

    pair& operator=(const pair&)            = default;
    pair& operator=(pair&&)                 = default;
    pair& operator=(const volatile pair&)   = delete;
}; // END pair


template<class Type1, class Type2>
constexpr bool operator==(const pair<Type1, Type2>& left, const pair<Type1, Type2>& right)
{
    return left.first == right.first && left.second == right.second;
}

template<class Type1, class Type2>
constexpr bool operator!=(const pair<Type1, Type2>& left, const pair<Type1, Type2>& right)
{
    return !(left == right);
}

template<class Type1, class Type2>
constexpr pair<unrefwrap_t<Type1>, unrefwrap_t<Type2>> make_pair(Type1&& first, Type2&& second)
noexcept(conjunction_v<is_nothrow_constructible<unrefwrap_t<Type1>, Type1>, is_nothrow_constructible<unrefwrap_t<Type2>, Type2>>)
{
    return pair<unrefwrap_t<Type1>, unrefwrap_t<Type2>>(custom::forward<Type1>(first), custom::forward<Type2>(second));
}

CUSTOM_END