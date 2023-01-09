#pragma once
#include "Common.h"
#include <utility>      // std::index_sequence, std::index_sequence_for

CUSTOM_BEGIN

// Tuple Template =======================================================
template <class... Types>
class Tuple;                                                // Tuple prototype

template<class This, class... Rest>
class Tuple<This, Rest...> : public Tuple<Rest...> {		// Recursive Tuple implementation
public:
	This First;

public:

	Tuple(const This& first, Rest&&... rest)
		:Tuple<Rest...>(std::forward<Rest>(rest)...), First(first) { }

	Tuple(This&& first, Rest&&... rest)
		:Tuple<Rest...>(std::forward<Rest>(rest)...), First(std::move(first)) { }

	Tuple(const Tuple&) = default;
	Tuple(Tuple&&)		= default;
};

template<>
class Tuple<> {												// Default implementation
public:

	Tuple() 			= default;
	Tuple(const Tuple&) = default;
	Tuple(Tuple&&)		= default;
	~Tuple() 			= default;
};

template <size_t Index, class Tuple>
struct TupleElement;                                        // TupleElement prototype for accessing Tuple members

template<size_t Index, class This, class... Rest>
struct TupleElement<Index, Tuple<This, Rest...>> 
: public TupleElement<Index - 1, Tuple<Rest...>> {};        // Recursive TupleElement implementation

template <class This, class... Rest>                        // Default implementation
struct TupleElement <0, Tuple<This, Rest...>>
{
	using Type = This;
	using TupleType = Tuple<This, Rest...>;
};

template<int Index, class... Types>
typename TupleElement<Index, Tuple<Types...>>::Type& get(Tuple<Types...>& tuple) {      // Function to get Tuple member
	using TupleType = typename TupleElement<Index, Tuple<Types...>>::TupleType;

	return static_cast<TupleType&>(tuple).First;
}

template<int Index, class... Types>
typename TupleElement<Index, Tuple<Types...>>::Type&& get(Tuple<Types...>&& tuple) {
	using Type		= typename TupleElement<Index, Tuple<Types...>>::Type;
	using TupleType = typename TupleElement<Index, Tuple<Types...>>::TupleType;

	return static_cast<Type&&>(static_cast<TupleType&>(tuple).First);
}

template <class... Types>
Tuple<Types&&...> forward_as_tuple(Types&&... args) {	    // Forward arguments in a tuple
	return Tuple<Types&&...>(std::forward<Types>(args)...);
}
// Tuple Template =======================================================
// END


// Pair Template =======================================================
struct PiecewiseConstruct { // tag type
	explicit PiecewiseConstruct() = default;
};

constexpr PiecewiseConstruct piecewise_construct = PiecewiseConstruct();    // Tag for piecewise construction of custom::Pair objects.

template<class Type1, class Type2>
class Pair                                                  // Pair class
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

	template <class Tuple1, class Tuple2, size_t... Indexes1, size_t... Indexes2>
	Pair(Tuple1& Val1, Tuple2& Val2, std::index_sequence<Indexes1...>, std::index_sequence<Indexes2...>)
		:First(get<Indexes1>(std::move(Val1))...), Second(get<Indexes2>(std::move(Val1))...) { }

	template <class... Types1, class... Types2>
	Pair(PiecewiseConstruct, Tuple<Types1...> Val1, Tuple<Types2...> Val2)
		:Pair(Val1, Val2, std::index_sequence_for<Types1...>{}, std::index_sequence_for<Types2...>{}) { }

	~Pair() = default;
};
// Pair Template =======================================================
// END

CUSTOM_END