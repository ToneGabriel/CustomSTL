#pragma once
#include "Utility.h"

CUSTOM_BEGIN

template <class... Types>
class Tuple;											// Tuple prototype

template<class This, class... Rest>
class Tuple<This, Rest...> : public Tuple<Rest...>		// Recursive Tuple implementation
{
public:
	This First;

public:

	// TODO: fix constructors

	Tuple(const This& first, Rest&&... rest)
		:Tuple<Rest...>(custom::forward<Rest>(rest)...), First(first) { /*Empty*/ }

	Tuple(This&& first, Rest&&... rest)
		:Tuple<Rest...>(custom::forward<Rest>(rest)...), First(custom::move(first)) { /*Empty*/ }

	Tuple(const Tuple&) = default;
	Tuple(Tuple&&)		= default;
    ~Tuple()            = default;
};

template<>
class Tuple<>											// Default Tuple implementation
{
public:

	Tuple() 			= default;
	Tuple(const Tuple&) = default;
	Tuple(Tuple&&)		= default;
	~Tuple() 			= default;
};

// tuple size
template<class Tuple>
struct TupleSize;

template<class... Elements>
struct TupleSize<Tuple<Elements...>> : IntegralConstant<size_t, sizeof...(Elements)> {};

template<class Tuple>
constexpr size_t TupleSize_v = TupleSize<Tuple>::Value;

template<class Tuple,
class NoCV_t 	= RemoveCV_t<Tuple>,
class 			= EnableIf_t<IsSame_v<Tuple, NoCV_t>>,
size_t 			= TupleSize_v<Tuple>>
using _EnableTupleSize = Tuple;

template<typename Tuple>
struct TupleSize<const _EnableTupleSize<Tuple>> : public TupleSize<Tuple> {};

template<typename Tuple>
struct TupleSize<volatile _EnableTupleSize<Tuple>> : public TupleSize<Tuple> {};

template<typename Tuple>
struct TupleSize<const volatile _EnableTupleSize<Tuple>> : public TupleSize<Tuple> {};

// tuple element
template<size_t Index, class Tuple>
struct TupleElement;                                        // TupleElement prototype for accessing Tuple members

template<size_t Index, class This, class... Rest>			// Recursive TupleElement implementation
struct TupleElement<Index, Tuple<This, Rest...>>  : public TupleElement<Index - 1, Tuple<Rest...>> {};

template<class This, class... Rest>
struct TupleElement <0, Tuple<This, Rest...>>				// Default TupleElement implementation
{
	using Type = This;
	using TupleType = Tuple<This, Rest...>;
};

template<size_t Index, class Tuple>
using TupleElement_t = typename TupleElement<Index, Tuple>::Type;

template<size_t Index, class Tuple>
using TupleElement_tt = typename TupleElement<Index, Tuple>::TupleType;

// get
template<int Index, class... Types>
TupleElement_t<Index, Tuple<Types...>>& get(Tuple<Types...>& tuple) {					// Function to get Tuple member from reference
	using TupleType = TupleElement_tt<Index, Tuple<Types...>>;

	return static_cast<TupleType&>(tuple).First;
}

template<int Index, class... Types>
const TupleElement_t<Index, Tuple<Types...>>& get(const Tuple<Types...>& tuple) {		// Same but const
	using TupleType = TupleElement_tt<Index, Tuple<Types...>>;

	return static_cast<const TupleType&>(tuple).First;
}

template<int Index, class... Types>
TupleElement_t<Index, Tuple<Types...>>&& get(Tuple<Types...>&& tuple) {					// Function to get Tuple member from rval
	using Type		= TupleElement_t<Index, Tuple<Types...>>;
	using TupleType = TupleElement_tt<Index, Tuple<Types...>>;

	return static_cast<Type&&>(static_cast<TupleType&>(tuple).First);
}

template<int Index, class... Types>
const TupleElement_t<Index, Tuple<Types...>>&& get(const Tuple<Types...>&& tuple) {		// Same but const
	using Type		= TupleElement_t<Index, Tuple<Types...>>;
	using TupleType = TupleElement_tt<Index, Tuple<Types...>>;

	return static_cast<const Type&&>(static_cast<const TupleType&>(tuple).First);
}

// forward as tuple
template<class... Types>
Tuple<Types&&...> forward_as_tuple(Types&&... args) {									// Forward arguments in a tuple
	return Tuple<Types&&...>(custom::forward<Types>(args)...);
}

// apply
template<class Callable, class Tuple, size_t... Indices>
decltype(auto) _apply_impl(Callable&& func, Tuple&& tuple, IndexSequence<Indices...>) noexcept {
    return custom::invoke(custom::forward<Callable>(func), custom::get<Indices>(custom::forward<Tuple>(tuple))...);
}

template<class Callable, class Tuple>
decltype(auto) apply(Callable&& func, Tuple&& tuple) noexcept {							// Invoke Callable obj with args as Tuple
    return _apply_impl(custom::forward<Callable>(func), custom::forward<Tuple>(tuple), MakeIndexSequence<TupleSize_v<RemoveReference_t<Tuple>>>{});
}

// make tuple / tie
template<class... Args>
Tuple<UnRefWrap_t<Args>...> make_tuple(Args&&... args) {								// Create Tuple with args
	return Tuple<UnRefWrap_t<Args>...>(custom::forward<Args>(args)...);
}

template<class... Args>
Tuple<Args&...> tie(Args&... args) noexcept {											// Create Tuple with references of args
	return Tuple<Args&...>(args...);
}

// make from tuple
template<class Type, class Tuple, size_t... Indices>
Type _make_from_tuple_impl(Tuple&& tuple, IndexSequence<Indices...>) {
	static_assert(IsConstructible_v<Type, decltype(custom::get<Indices>(custom::forward<Tuple>(tuple)))...>, "The target type must be constructible using the arguments in tuple.");
	return Type(custom::get<Indices>(custom::forward<Tuple>(tuple))...);
}

template<class Type, class Tuple>
Type make_from_tuple(Tuple&& tuple) {													// construct Type from the elements of tuple
	return _make_from_tuple_impl<Type>(custom::forward<Tuple>(tuple), MakeIndexSequence<TupleSize_v<RemoveReference_t<Tuple>>>{});
}

// tuple cat
template<class Type, class KxArg, class IxArg, size_t IxNext, class... Sequences>
struct _TupleCat;

template<class Type, size_t... Kx, size_t... Ix, size_t IxNext>
struct _TupleCat<Type, IndexSequence<Kx...>, IndexSequence<Ix...>, IxNext>
{
	using RetType	= Tuple<TupleElement_t<Kx, RemoveCVRef_t<TupleElement_t<Ix, Type>>>...>;
	using KxSeq		= IndexSequence<Kx...>;
	using IxSeq		= IndexSequence<Ix...>;
};

template<class Type, size_t... Kx, size_t... Ix, size_t IxNext, size_t... KxNext, class... Rest>
struct _TupleCat<Type, IndexSequence<Kx...>, IndexSequence<Ix...>, IxNext, IndexSequence<KxNext...>, Rest...>
	: _TupleCat<Type, IndexSequence<Kx..., KxNext...>, IndexSequence<Ix..., (IxNext + 0 * KxNext)...>, // repeat IxNext, ignoring the elements of KxNext
	IxNext + 1, Rest...> {};

template<class... Tuples>
using TupleCat = _TupleCat<Tuple<Tuples&&...>, IndexSequence<>, IndexSequence<>, 0, MakeIndexSequence<TupleSize_v<RemoveCVRef_t<Tuples>>>...>;

template<class RetType, size_t... Kx, size_t... Ix, class Tuple>
RetType _tuple_cat_impl(IndexSequence<Kx...>, IndexSequence<Ix...>, Tuple tuple) {		// tuple as copy
	return RetType(custom::get<Kx>(custom::get<Ix>(custom::move(tuple)))...);
}

template<class... Tuples>
typename TupleCat<Tuples...>::RetType tuple_cat(Tuples&&... tuples) {					// concatenate tuples
	using Cat		= TupleCat<Tuples...>;
	using RetType	= typename Cat::RetType;
	using KxSeq		= typename Cat::KxSeq;
	using IxSeq		= typename Cat::IxSeq;

	return _tuple_cat_impl<RetType>(KxSeq{}, IxSeq{}, custom::forward_as_tuple(custom::forward<Tuples>(tuples)...));
}

CUSTOM_END