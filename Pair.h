#pragma once
#include "Common.h"
#include "Tuple.h"
#include <utility>      // std::index_sequence, std::index_sequence_for

CUSTOM_BEGIN

// Headings =================================================================================

// Tag struct declaration for piecewise construction of custom::Pair objects
struct PiecewiseConstruct {
	explicit PiecewiseConstruct() = default;
};

template<class Type1, class Type2>
class Pair								// Pair Template
{
public:
	Type1 First;
	Type2 Second;

public:
    // Constructors

	Pair();
	Pair(const Type1& val1, const Type2& val2);
	Pair(Type1&& val1, Type2&& val2);
	Pair(const Pair& other);
	Pair(Pair&& other);

	template <class Tuple1, class Tuple2, size_t... Indexes1, size_t... Indexes2>
	Pair(Tuple1& Val1, Tuple2& Val2, std::index_sequence<Indexes1...>, std::index_sequence<Indexes2...>);

	template <class... Types1, class... Types2>
	Pair(PiecewiseConstruct, Tuple<Types1...> Val1, Tuple<Types2...> Val2);

	~Pair() = default;

public:
    // Operators

	Pair& operator=(const Pair& other);
	Pair& operator=(Pair&& other);

	bool operator==(const Pair& other);
	bool operator!=(const Pair& other);

}; // END Pair Template

template<class Type1, class Type2>
Pair<Type1, Type2> make_pair(Type1&& first, Type2&& second);    // make_pair prototype



// Definitions =================================================================================

// Instance of piecewise tag
constexpr PiecewiseConstruct piecewise_construct = PiecewiseConstruct();

// Pair Template
template<class Type1, class Type2>
Pair<Type1, Type2>::Pair()
    :First(), Second() { /*Empty*/ }

template<class Type1, class Type2>
Pair<Type1, Type2>::Pair(const Type1& val1, const Type2& val2)
    :First(val1), Second(val2) { /*Empty*/ }

template<class Type1, class Type2>
Pair<Type1, Type2>::Pair(Type1&& val1, Type2&& val2)
    :First(std::move(val1)), Second(std::move(val2)) { /*Empty*/ }

template<class Type1, class Type2>
Pair<Type1, Type2>::Pair(const Pair& other)
    :First(other.First), Second(other.Second) { /*Empty*/ }

template<class Type1, class Type2>
Pair<Type1, Type2>::Pair(Pair&& other)
    :First(std::move(other.First)), Second(std::move(other.Second)) { /*Empty*/ }

template<class Type1, class Type2>
template <class Tuple1, class Tuple2, size_t... Indexes1, size_t... Indexes2>
Pair<Type1, Type2>::Pair(Tuple1& Val1, Tuple2& Val2, std::index_sequence<Indexes1...>, std::index_sequence<Indexes2...>)
    :First(get<Indexes1>(std::move(Val1))...), Second(get<Indexes2>(std::move(Val2))...) { /*Empty*/ }

template<class Type1, class Type2>
template <class... Types1, class... Types2>
Pair<Type1, Type2>::Pair(PiecewiseConstruct, Tuple<Types1...> Val1, Tuple<Types2...> Val2)
    :Pair(Val1, Val2, std::index_sequence_for<Types1...>{}, std::index_sequence_for<Types2...>{}) { /*Empty*/ }

template<class Type1, class Type2>
Pair<Type1, Type2>& Pair<Type1, Type2>::operator=(const Pair& other) {
    First = other.First;
    Second = other.Second;

    return *this;
}

template<class Type1, class Type2>
Pair<Type1, Type2>& Pair<Type1, Type2>::operator=(Pair&& other) {
    First = std::move(other.First);
    Second = std::move(other.Second);

    return *this;
}

template<class Type1, class Type2>
bool Pair<Type1, Type2>::operator==(const Pair& other) {
    return (First == other.First && Second == other.Second);
}

template<class Type1, class Type2>
bool Pair<Type1, Type2>::operator!=(const Pair& other) {
    return !operator==(other);
}
// END Pair Template

template<class Type1, class Type2>
Pair<Type1, Type2> make_pair(Type1&& first, Type2&& second) {       // make_pair definition
    return Pair<Type1, Type2>(std::forward<Type1>(first), std::forward<Type2>(second));
}

CUSTOM_END