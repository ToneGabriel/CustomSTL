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

    // (H1) Helper for (5)
    template<class Tuple1, class Tuple2, size_t... Indexes1, size_t... Indexes2>
    Pair(Tuple1& val1, Tuple2& val2, IndexSequence<Indexes1...>, IndexSequence<Indexes2...>)
        : First(get<Indexes1>(custom::move(val1))...), Second(get<Indexes2>(custom::move(val2))...) { /*Empty*/ }

public:
    // Constructors

    // (0) Default constructor
    template<class _Type1 = Type1, class _Type2 = Type2,
    EnableIf_t<Conjunction_v<IsDefaultConstructible<_Type1>, IsDefaultConstructible<_Type2>>, bool> = true>
    Pair()
        : First(), Second() { /*Empty*/ }
    
    // TODO: check WTF
    // (1) Copy obj constructor
    template<class _Type1 = Type1, class _Type2 = Type2,
    EnableIf_t<Conjunction_v<   IsCopyConstructible<_Type1>,
                                IsCopyConstructible<_Type2>>, bool> = true>
    explicit(!Conjunction_v<    IsConvertible<const _Type1&, _Type1>,
                                IsConvertible<const _Type2&, _Type2>>)
    Pair(const Type1& val1, const Type2& val2)
        : First(val1), Second(val2) { /*Empty*/ }

    // (2) Move obj constructor
    template<class Other1, class Other2,
    EnableIf_t<Conjunction_v<   IsConstructible<Type1, Other1>,
                                IsConstructible<Type2, Other2>>, bool> = true>
    explicit(!Conjunction_v<    IsConvertible<Other1, Type1>,
                                IsConvertible<Other2, Type2>>)
    Pair(Other1&& val1, Other2&& val2)
        : First(custom::forward<Other1>(val1)), Second(custom::forward<Other2>(val2)) { /*Empty*/ }

    // (3) Copy constructor
    template<class Other1, class Other2,
    EnableIf_t<Conjunction_v<   IsConstructible<Type1, const Other1&>,
                                IsConstructible<Type2, const Other2&>>, bool> = true>
    explicit(!Conjunction_v<    IsConvertible<const Other1&, Type1>,
                                IsConvertible<const Other2&, Type2>>)
    Pair(const Pair<Other1, Other2>& other)
        : First(other.First), Second(other.Second) { /*Empty*/ }

    // (4) Move constructor
    template<class Other1, class Other2,
    EnableIf_t<Conjunction_v<   IsConstructible<Type1, Other1>,
                                IsConstructible<Type2, Other2>>, bool> = true>
    explicit(!Conjunction_v<    IsConvertible<Other1, Type1>,
                                IsConvertible<Other2, Type2>>)
    Pair(Pair<Other1, Other2>&& other)
        : First(custom::forward<Other1>(other.First)), Second(custom::forward<Other2>(other.Second)) {/*Empty*/ }

    // (5) Piecewise constructor
    template<class... Types1, class... Types2>
    Pair(PiecewiseConstruct_t, Tuple<Types1...> val1, Tuple<Types2...> val2)
        : Pair(val1, val2, IndexSequenceFor<Types1...>{}, IndexSequenceFor<Types2...>{}) { /*Empty*/ }   // Uses (H1)

    Pair(const Pair&)   = default;
    Pair(Pair&&)        = default;
    ~Pair()             = default;

public:
    // Operators

    // TODO: complete

    Pair& operator=(const Pair& other) {
        First   = other.First;
        Second  = other.Second;

        return *this;
    }

    Pair& operator=(Pair&& other) {
        First   = custom::move(other.First);
        Second  = custom::move(other.Second);
    }

    bool operator==(const Pair& other) {
        return (First == other.First && Second == other.Second);
    }

    bool operator!=(const Pair& other) {
        return !(*this == other);
    }
}; // END Pair Template

template<class Type1, class Type2>  // TODO: check
Pair<UnRefWrap_t<Type1>, UnRefWrap_t<Type2>> make_pair(Type1&& first, Type2&& second) {
    return Pair<UnRefWrap_t<Type1>, UnRefWrap_t<Type2>>(custom::forward<Type1>(first), custom::forward<Type2>(second));
}

CUSTOM_END