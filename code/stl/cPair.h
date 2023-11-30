#pragma once
#include "cTuple.h"
#include "cUtility.h"


CUSTOM_BEGIN

// tag struct declaration for piecewise construction of Pair objects
struct PiecewiseConstruct_t { explicit PiecewiseConstruct_t() = default; };

constexpr PiecewiseConstruct_t PiecewiseConstruct = PiecewiseConstruct_t();

template<class Type1, class Type2>
class Pair                          // Pair Template
{
public:
    using FirstType     = Type1;
    using SecondType    = Type2;

    Type1 First;
    Type2 Second;

public:
    // Constructor Helpers

    // (H1) Helper for (5)
    template<class Tuple1, class Tuple2, size_t... Indexes1, size_t... Indexes2>
    constexpr Pair(Tuple1&& val1, Tuple2&& val2, IndexSequence<Indexes1...>, IndexSequence<Indexes2...>)
        :   First(get<Indexes1>(custom::forward<Tuple1>(val1))...),
            Second(get<Indexes2>(custom::forward<Tuple2>(val2))...) { /*Empty*/ }

public:
    // Constructors

    // (0) Default constructor
    template<EnableIf_t<Conjunction_v<  IsDefaultConstructible<Type1>,
                                        IsDefaultConstructible<Type2>>, bool> = true>
    constexpr explicit(!Conjunction_v<  IsImplicitlyDefaultConstructible<Type1>,
                                        IsImplicitlyDefaultConstructible<Type2>>)
    Pair()
    noexcept(IsNothrowDefaultConstructible_v<Type1> && IsNothrowDefaultConstructible_v<Type2>)
        : First(), Second() { /*Empty*/ }

    // (1) Copy components constructor
    template<class Other1 = Type1, class Other2 = Type2,
    EnableIf_t<Conjunction_v<   IsConstructible<Type1, const Other1&>,
                                IsConstructible<Type2, const Other2&>>, bool> = true>
    constexpr explicit(!Conjunction_v<  IsConvertible<const Other1&, Type1>,
                                        IsConvertible<const Other2&, Type2>>)
    Pair(const Other1& val1, const Other2& val2)
    noexcept(IsNothrowConstructible_v<Type1, const Other1&> && IsNothrowConstructible_v<Type2, const Other2&>)
        : First(val1), Second(val2) { /*Empty*/ }

    // (2) Move components constructor
    template<class Other1 = Type1, class Other2 = Type2,
    EnableIf_t<Conjunction_v<   IsConstructible<Type1, Other1>,
                                IsConstructible<Type2, Other2>>, bool> = true>
    constexpr explicit(!Conjunction_v<  IsConvertible<Other1, Type1>,
                                        IsConvertible<Other2, Type2>>)
    Pair(Other1&& val1, Other2&& val2)
    noexcept(IsNothrowConstructible_v<Type1, Other1> && IsNothrowConstructible_v<Type2, Other2>)
        : First(custom::forward<Other1>(val1)), Second(custom::forward<Other2>(val2)) { /*Empty*/ }

    // (3) Copy pair constructor
    template<class Other1, class Other2,
    EnableIf_t<Conjunction_v<   IsConstructible<Type1, const Other1&>,
                                IsConstructible<Type2, const Other2&>>, bool> = true>
    constexpr explicit(!Conjunction_v<  IsConvertible<const Other1&, Type1>,
                                        IsConvertible<const Other2&, Type2>>)
    Pair(const Pair<Other1, Other2>& other)
    noexcept(IsNothrowConstructible_v<Type1, const Other1&> && IsNothrowConstructible_v<Type2, const Other2&>)
        : First(other.First), Second(other.Second) { /*Empty*/ }

    // (4) Move pair constructor
    template<class Other1, class Other2,
    EnableIf_t<Conjunction_v<   IsConstructible<Type1, Other1>,
                                IsConstructible<Type2, Other2>>, bool> = true>
    constexpr explicit(!Conjunction_v<  IsConvertible<Other1, Type1>,
                                        IsConvertible<Other2, Type2>>)
    Pair(Pair<Other1, Other2>&& other)
    noexcept(IsNothrowConstructible_v<Type1, Other1> && IsNothrowConstructible_v<Type2, Other2>)
        :   First(custom::forward<Other1>(other.First)),
            Second(custom::forward<Other2>(other.Second)) { /*Empty*/ }

    // (5) Piecewise constructor
    template<class... Types1, class... Types2>
    constexpr Pair(PiecewiseConstruct_t, Tuple<Types1...>&& val1, Tuple<Types2...>&& val2)
        : Pair( custom::move(val1),
                custom::move(val2),
                IndexSequenceFor<Types1...>{},
                IndexSequenceFor<Types2...>{}) { /*Empty*/ }   // Uses (H1)

    Pair(const Pair&)   = default;
    Pair(Pair&&)        = default;

public:
    // Operators

    // Copy convertible operator
    template<class Other1, class Other2,
    EnableIf_t<Conjunction_v<   Negation<IsSame<Pair, Pair<Other1, Other2>>>,
                                IsAssignable<Type1&, const Other1&>,
                                IsAssignable<Type2&, const Other2&>>, bool> = true>
    constexpr Pair& operator=(const Pair<Other1, Other2>& other)
    noexcept(IsNothrowAssignable_v<Type1&, const Other1&> && IsNothrowAssignable_v<Type2&, const Other2&>) {
        First  = other.First;
        Second = other.Second;
        return *this;
    }

    // Move convertible operator
    template<class Other1, class Other2,
    EnableIf_t<Conjunction_v<   Negation<IsSame<Pair, Pair<Other1, Other2>>>,
                                IsAssignable<Type1&, Other1>,
                                IsAssignable<Type2&, Other2>>, bool> = true>
    constexpr Pair& operator=(Pair<Other1, Other2>&& other)
    noexcept(IsNothrowAssignable_v<Type1&, Other1> && IsNothrowAssignable_v<Type2&, Other2>) {
        First  = custom::forward<Other1>(other.First);
        Second = custom::forward<Other2>(other.Second);
        return *this;
    }

    Pair& operator=(const Pair&)            = default;
    Pair& operator=(Pair&&)                 = default;
    Pair& operator=(const volatile Pair&)   = delete;
}; // END Pair Template


template<class Type1, class Type2>
constexpr bool operator==(const Pair<Type1, Type2>& left, const Pair<Type1, Type2>& right) {
    return left.First == right.First && left.Second == right.Second;
}

template<class Type1, class Type2>
constexpr bool operator!=(const Pair<Type1, Type2>& left, const Pair<Type1, Type2>& right) {
    return !(left == right);
}

template<class Type1, class Type2>
constexpr Pair<UnRefWrap_t<Type1>, UnRefWrap_t<Type2>> make_pair(Type1&& first, Type2&& second)
noexcept(IsNothrowConstructible_v<UnRefWrap_t<Type1>, Type1> && IsNothrowConstructible_v<UnRefWrap_t<Type2>, Type2>) {
    return Pair<UnRefWrap_t<Type1>, UnRefWrap_t<Type2>>(custom::forward<Type1>(first), custom::forward<Type2>(second));
}

CUSTOM_END