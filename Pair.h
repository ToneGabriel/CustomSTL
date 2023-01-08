#pragma once
#include "Common.h"
#include "Tuple.h"

CUSTOM_BEGIN

struct PiecewiseConstruct { // tag type
	explicit PiecewiseConstruct() = default;
};


template<class Type1, class Type2>
class Pair
{
public:
	Type1 First;
	Type2 Second;

public:

	Pair()
		:First(), Second() { }

	Pair(const Type1& val1, const Type2& val2)
		:First(val1), Second(val2) { }

	Pair(Type1&& val1, Type2&& val2)
		:First(std::move(val1)), Second(std::move(val2)) { }

	Pair(const Pair<Type1, Type2>& other)
		:First(other.First), Second(other.Second) { }

	Pair(Pair<Type1, Type2>&& other)
		:First(std::move(other.First)), Second(std::move(other.Second)) { }

	template <class... Types1, class... Types2>
	Pair(PiecewiseConstruct, Tuple<Types1...> Val1, Tuple<Types2...> Val2)
		:Pair(Val1, Val2, std::index_sequence_for<Types1...>{}, std::index_sequence_for<Types2...>{}) { }

	~Pair() = default;

private:
	template <class Tuple1, class Tuple2, size_t... Indexes1, size_t... Indexes2>
	Pair(Tuple1& Val1, Tuple2& Val2, std::index_sequence<Indexes1...>, std::index_sequence<Indexes2...>)
		:First(tuple_get<Indexes1>(std::move(Val1))...), Second(tuple_get<Indexes2>(std::move(Val1))...) { }
};

CUSTOM_END