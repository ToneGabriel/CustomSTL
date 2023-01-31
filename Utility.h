#pragma once
#include "Common.h"
#include <utility>      // std::index_sequence, std::index_sequence_for

CUSTOM_BEGIN

// Tuple Definitions ==============================================================================

template <class... Types>
class Tuple;				// Tuple prototype

template<class This, class... Rest>
class Tuple<This, Rest...> : public Tuple<Rest...> {	// Recursive Tuple implementation
public:
	This First;

public:

	Tuple(const This& first, Rest&&... rest)
		:Tuple<Rest...>(std::forward<Rest>(rest)...), First(first) { /*Empty*/ }

	Tuple(This&& first, Rest&&... rest)
		:Tuple<Rest...>(std::forward<Rest>(rest)...), First(std::move(first)) { /*Empty*/ }

	Tuple(const Tuple&) = default;
	Tuple(Tuple&&)		= default;
};

template<>
class Tuple<> {				// Default Tuple implementation
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

template <class This, class... Rest>                        // Default TupleElement implementation
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

// END Tuple Definitions ==============================================================================





// Pair Definitions ===================================================================================

struct PiecewiseConstruct { // Tag struct declaration for piecewise construction of custom::Pair objects
	explicit PiecewiseConstruct() = default;
};

// Instance of piecewise tag
constexpr PiecewiseConstruct piecewise_construct = PiecewiseConstruct();

template<class Type1, class Type2>
class Pair								// Pair Template
{
public:
	Type1 First;
	Type2 Second;

public:

	Pair()
		:First(), Second() { /*Empty*/ }

	Pair(const Type1& val1, const Type2& val2)
		:First(val1), Second(val2) { /*Empty*/ }

	Pair(Type1&& val1, Type2&& val2)
		:First(std::move(val1)), Second(std::move(val2)) { /*Empty*/ }

	Pair(const Pair& other)
		:First(other.First), Second(other.Second) { /*Empty*/ }

	Pair(Pair&& other)
		:First(std::move(other.First)), Second(std::move(other.Second)) { /*Empty*/ }

	template <class Tuple1, class Tuple2, size_t... Indexes1, size_t... Indexes2>
	Pair(Tuple1& Val1, Tuple2& Val2, std::index_sequence<Indexes1...>, std::index_sequence<Indexes2...>)
		:First(get<Indexes1>(std::move(Val1))...), Second(get<Indexes2>(std::move(Val2))...) { /*Empty*/ }

	template <class... Types1, class... Types2>
	Pair(PiecewiseConstruct, Tuple<Types1...> Val1, Tuple<Types2...> Val2)
		:Pair(Val1, Val2, std::index_sequence_for<Types1...>{}, std::index_sequence_for<Types2...>{}) { /*Empty*/ }

	~Pair() = default;
}; // END Pair Template

// END Pair Definitions ===================================================================================

CUSTOM_END