#pragma once
#include "Common.h"
#include "Utility.h"

CUSTOM_BEGIN

template <class... Types>
class Tuple;				// Tuple prototype

template<class This, class... Rest>
class Tuple<This, Rest...> : public Tuple<Rest...> {	// Recursive Tuple implementation
public:
	This First;

public:

	Tuple(const This& first, Rest&&... rest)
		:Tuple<Rest...>(custom::forward<Rest>(rest)...), First(first) { /*Empty*/ }

	Tuple(This&& first, Rest&&... rest)
		:Tuple<Rest...>(custom::forward<Rest>(rest)...), First(custom::move(first)) { /*Empty*/ }

	Tuple(const Tuple&) = default;
	Tuple(Tuple&&)		= default;
    ~Tuple()            = default;
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
typename TupleElement<Index, Tuple<Types...>>::Type& get(Tuple<Types...>& tuple) {      // Function to get Tuple member from reference
	using TupleType = typename TupleElement<Index, Tuple<Types...>>::TupleType;

	return static_cast<TupleType&>(tuple).First;
}

template<int Index, class... Types>
typename TupleElement<Index, Tuple<Types...>>::Type&& get(Tuple<Types...>&& tuple) {    // Function to get Tuple member from rval
	using Type		= typename TupleElement<Index, Tuple<Types...>>::Type;
	using TupleType = typename TupleElement<Index, Tuple<Types...>>::TupleType;

	return static_cast<Type&&>(static_cast<TupleType&>(tuple).First);
}

template <class... Types>
Tuple<Types&&...> forward_as_tuple(Types&&... args) {	    // Forward arguments in a tuple
	return Tuple<Types&&...>(custom::forward<Types>(args)...);
}

CUSTOM_END