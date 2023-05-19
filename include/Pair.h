#pragma once
#include "Tuple.h"
#include "Utility.h"


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

    // (H1) Helper for (4)
    template<class Tuple1, class Tuple2, size_t... Indexes1, size_t... Indexes2>
    Pair(Tuple1&& val1, Tuple2&& val2, IndexSequence<Indexes1...>, IndexSequence<Indexes2...>)
        : First(get<Indexes1>(custom::forward<Tuple1>(val1))...), Second(get<Indexes2>(custom::forward<Tuple2>(val2))...) { /*Empty*/ }     // Can copy or Move

public:
    // Constructors

    // (0) Default constructor
    template<class _Type1 = Type1, class _Type2 = Type2,
    EnableIf_t<Conjunction_v<   IsDefaultConstructible<_Type1>,
                                IsDefaultConstructible<_Type2>>, bool> = true>
    Pair()
        : First(), Second() { /*Empty*/ }

    // (1) Copy/Move obj constructor
    template<class Other1, class Other2,
    EnableIf_t<Conjunction_v<   IsConstructible<Type1, Other1>,
                                IsConstructible<Type2, Other2>>, bool> = true>
    Pair(Other1&& val1, Other2&& val2)
        : First(custom::forward<Other1>(val1)), Second(custom::forward<Other2>(val2)) { /*Empty*/ }

    // (2) Copy convertible constructor
    template<class Other1, class Other2,
    EnableIf_t<Conjunction_v<   Negation<IsSame<Pair, Pair<Other1, Other2>>>,
                                IsConstructible<Type1, const Other1&>,
                                IsConstructible<Type2, const Other2&>,
                                IsConvertible<const Other1&, Type1>,
                                IsConvertible<const Other2&, Type2>>, bool> = true>
    Pair(const Pair<Other1, Other2>& other)
        : First(other.First), Second(other.Second) { /*Empty*/ }

    // (3) Move convertible constructor
    template<class Other1, class Other2,
    EnableIf_t<Conjunction_v<   Negation<IsSame<Pair, Pair<Other1, Other2>>>,
                                IsConstructible<Type1, Other1>,
                                IsConstructible<Type2, Other2>,
                                IsConvertible<Other1, Type1>,
                                IsConvertible<Other2, Type2>>, bool> = true>
    Pair(Pair<Other1, Other2>&& other)
        : First(custom::forward<Other1>(other.First)), Second(custom::forward<Other2>(other.Second)) { /*Empty*/ }

    // (4) Piecewise constructor
    template<class... Types1, class... Types2>
    Pair(PiecewiseConstruct_t, Tuple<Types1...>&& val1, Tuple<Types2...>&& val2)
        : Pair(custom::move(val1), custom::move(val2), IndexSequenceFor<Types1...>{}, IndexSequenceFor<Types2...>{}) { /*Empty*/ }   // Uses (H1)

    Pair(const Pair&)   = default;
    Pair(Pair&&)        = default;
    ~Pair()             = default;

public:
    // Operators

    // Copy convertible operator
    template<class Other1, class Other2,
    EnableIf_t<Conjunction_v<   Negation<IsSame<Pair, Pair<Other1, Other2>>>,
                                IsAssignable<Type1&, const Other1&>,
                                IsAssignable<Type2&, const Other2&>,
                                IsConvertible<const Other1&, Type1>,
                                IsConvertible<const Other2&, Type2>>, bool> = true>
    Pair& operator=(const Pair<Other1, Other2>& other) {
        First  = other.First;
        Second = other.Second;
        return *this;
    }

    // Move convertible operator
    template<class Other1, class Other2,
    EnableIf_t<Conjunction_v<   Negation<IsSame<Pair, Pair<Other1, Other2>>>,
                                IsAssignable<Type1&, Other1>,
                                IsAssignable<Type2&, Other2>,
                                IsConvertible<Other1, Type1>,
                                IsConvertible<Other2, Type2>>, bool> = true>
    Pair& operator=(Pair<Other1, Other2>&& other) {
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
Pair<UnRefWrap_t<Type1>, UnRefWrap_t<Type2>> make_pair(Type1&& first, Type2&& second) {
    return Pair<UnRefWrap_t<Type1>, UnRefWrap_t<Type2>>(custom::forward<Type1>(first), custom::forward<Type2>(second));
}

CUSTOM_END