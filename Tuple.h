#pragma once
#include "Common.h"

CUSTOM_BEGIN

template <class... Types>
class Tuple;

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

template<class This>
class Tuple<This> {											// Default implementation
public:
	This First;

public:
	Tuple()
		:First() {}

	Tuple(const This& first) 
		:First(first) { }

	Tuple(This&& first)
		:First(std::move(first)) { }

	Tuple(const Tuple&) = default;
	Tuple(Tuple&&)		= default;
};

template <size_t Index, class Tuple>
struct TupleElement;

template<size_t Index, class This, class... Rest>
struct TupleElement<Index, Tuple<This, Rest...>> : public TupleElement<Index - 1, Tuple<Rest...>> {};

template <class This, class... Rest>
struct TupleElement <0, Tuple<This, Rest...>>
{
	using Type = This;
	using TupleType = Tuple<This, Rest...>;
};

// get
template<int Index, class... Types>
typename TupleElement<Index, Tuple<Types...>>::Type& get(Tuple<Types...>& tuple) {
	using TupleType = typename TupleElement<Index, Tuple<Types...>>::TupleType;

	return static_cast<TupleType&>(tuple).First;
}

template<int Index, class... Types>
typename TupleElement<Index, Tuple<Types...>>::Type&& get(Tuple<Types...>&& tuple) {
	using Type		= typename TupleElement<Index, Tuple<Types...>>::Type;
	using TupleType = typename TupleElement<Index, Tuple<Types...>>::TupleType;

	return static_cast<Type&&>(static_cast<TupleType&>(tuple).First);
}

template <size_t Index, class... Types>
typename auto&& tuple_get(Tuple<Types...>&& tuple) {
	// used by pair's piecewise constructor
	using Type		= typename TupleElement<Index, Tuple<Types...>>::Type;
	using TupleType = typename TupleElement<Index, Tuple<Types...>>::TupleType;

	return static_cast<Type&&>(static_cast<TupleType&>(tuple).First);
}
// end get

template <class... Types>
Tuple<Types&&...> forward_as_tuple(Types&&... args) {				// forward arguments in a tuple
	return Tuple<Types&&...>(std::forward<Types>(args)...);
}


CUSTOM_END