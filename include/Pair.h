#pragma once
#include "Tuple.h"
#include "Utility.h"    // IndexSequence, IndexSequenceFor

CUSTOM_BEGIN

template<class Type1, class Type2>
class Pair			        // Pair Template
{
public:
    using FirstType     = Type1;
    using SecondType    = Type2;

    Type1 First;
	Type2 Second;

public:
    // Constructors

    Pair()
        :First(), Second() { /*Empty*/ }

	Pair(const Type1& val1, const Type2& val2)
        :First(val1), Second(val2) { /*Empty*/ }

	Pair(Type1&& val1, Type2&& val2)
        :First(custom::move(val1)), Second(custom::move(val2)) { /*Empty*/ }

	Pair(const Pair& other)
        :First(other.First), Second(other.Second) { /*Empty*/ }

	Pair(Pair&& other)
        :First(custom::move(other.First)), Second(custom::move(other.Second)) { /*Empty*/ }

	template<class Tuple1, class Tuple2, size_t... Indexes1, size_t... Indexes2>
	Pair(Tuple1& Val1, Tuple2& Val2, IndexSequence<Indexes1...>, IndexSequence<Indexes2...>)
        :First(get<Indexes1>(custom::move(Val1))...), Second(get<Indexes2>(custom::move(Val2))...) { /*Empty*/ }

	template<class... Types1, class... Types2>
	Pair(PiecewiseConstruct_t, Tuple<Types1...> Val1, Tuple<Types2...> Val2)
        :Pair(Val1, Val2, IndexSequenceFor<Types1...>{}, IndexSequenceFor<Types2...>{}) { /*Empty*/ }

	~Pair() = default;

public:
    // Operators

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

template<class Type1, class Type2>
Pair<Type1, Type2> make_pair(Type1&& first, Type2&& second) {
    return Pair<Type1, Type2>(custom::forward<Type1>(first), custom::forward<Type2>(second));
}

CUSTOM_END