#pragma once
#include "c_utility.h"
#include "x_functional.h"


CUSTOM_BEGIN

// pair prototype
template<class First, class Second>
class pair;

// tag type for construction (from one arg per element)
struct _Tuple_Exact_Args_t { explicit _Tuple_Exact_Args_t() = default; };

// tag type for construction (from unpacking a tuple/pair)
struct _Tuple_Unpack_t { explicit _Tuple_Unpack_t() = default; };

// tuple prototype
template<class... Types>
class tuple;

// tuple size
template<class _Tuple>
struct tuple_size;

template<class... Elements>
struct tuple_size<tuple<Elements...>> : integral_constant<size_t, sizeof...(Elements)> {};

template<class _Tuple>
constexpr size_t tuple_size_v = tuple_size<_Tuple>::value;

template<class _Tuple, class NoCV_t = remove_cv_t<_Tuple>, class = enable_if_t<is_same_v<_Tuple, NoCV_t>>, size_t = tuple_size_v<_Tuple>>
using _EnableTupleSize = _Tuple;

template<class _Tuple>
struct tuple_size<const _EnableTupleSize<_Tuple>> : public tuple_size<_Tuple> {};

template<class _Tuple>
struct tuple_size<volatile _EnableTupleSize<_Tuple>> : public tuple_size<_Tuple> {};

template<class _Tuple>
struct tuple_size<const volatile _EnableTupleSize<_Tuple>> : public tuple_size<_Tuple> {};

// tuple constructible
template<bool SameSize, class _Tuple, class... Args>
constexpr bool _TupleConstructible_v = false;

template<class... Types, class... Args>						// Each type component from tuple (tuple<Types...>) is constructible with the corresponding argument (copy/move obj)
constexpr bool _TupleConstructible_v<true, tuple<Types...>, Args...> = conjunction_v<is_constructible<Types, Args>...>;

template<class _Tuple, class... Args>						// Check tuple size with given construction arguments then above
constexpr bool tuple_constructible_v = _TupleConstructible_v<tuple_size_v<_Tuple> == sizeof...(Args), _Tuple, Args...>;

template<class _Tuple, class... Args>
struct tuple_constructible : bool_constant<tuple_constructible_v<_Tuple, Args...>> {};

// tuple assignable
template<bool SameSize, class _Tuple, class... Args>
constexpr bool _TupleAssignable_v = false;

template<class... Types, class... Args>
constexpr bool _TupleAssignable_v<true, tuple<Types...>, Args...> = conjunction_v<is_assignable<Types&, Args>...>; // note Types& instead of Types

template<class _Tuple, class... Args>
constexpr bool tuple_assignable_v = _TupleAssignable_v<tuple_size_v<_Tuple> == sizeof...(Args), _Tuple, Args...>;

template<class _Tuple, class... Args>
struct tuple_assignable : bool_constant<tuple_assignable_v<_Tuple, Args...>> {};

// tuple convert
template<class ThisTuple, class OtherTuple, class... OtherTypes>
struct tuple_convert : true_type {};

template<class This, class OtherTuple, class OtherType>
struct tuple_convert<tuple<This>, OtherTuple, OtherType>
	: bool_constant<!disjunction_v<	is_same<This, OtherType>,
									is_constructible<This, OtherTuple>,
									IsConvertible<OtherTuple, This>>> {};

// tuple element
template<size_t Index, class _Tuple>
struct tuple_element;                                        // tuple_element prototype for accessing tuple members

template<size_t Index, class This, class... Rest>			// Recursive tuple_element implementation
struct tuple_element<Index, tuple<This, Rest...>> : public tuple_element<Index - 1, tuple<Rest...>> {};

template<class This, class... Rest>
struct tuple_element<0, tuple<This, Rest...>>				// Default tuple_element implementation
{
	using type			= This;
	using tuple_type	= tuple<This, Rest...>;
};

template<size_t Index, class _Tuple>
using tuple_element_t = typename tuple_element<Index, _Tuple>::type;

template<size_t Index, class _Tuple>
using tuple_element_tt = typename tuple_element<Index, _Tuple>::tuple_type;

// tuple cat helper
template<class Type, class KxArg, class IxArg, size_t IxNext, class... Sequences>
struct _Tuple_Cat;

template<class Type, size_t... Kx, size_t... Ix, size_t IxNext>
struct _Tuple_Cat<Type, index_sequence<Kx...>, index_sequence<Ix...>, IxNext>
{
	using _Ret_Type		= tuple<tuple_element_t<Kx, remove_cv_ref_t<tuple_element_t<Ix, Type>>>...>;
	using _KxSeq		= index_sequence<Kx...>;
	using _IxSeq		= index_sequence<Ix...>;
};

template<class Type, size_t... Kx, size_t... Ix, size_t IxNext, size_t... KxNext, class... Rest>
struct _Tuple_Cat<Type, index_sequence<Kx...>, index_sequence<Ix...>, IxNext, index_sequence<KxNext...>, Rest...>
	: _Tuple_Cat<Type, index_sequence<Kx..., KxNext...>, index_sequence<Ix..., (IxNext + 0 * KxNext)...>, // repeat IxNext, ignoring the elements of KxNext
	IxNext + 1, Rest...> {};


template<>
class tuple<>												// Default tuple implementation
{
public:
	// Constructors

	template<class Tag, enable_if_t<is_same_v<Tag, _Tuple_Exact_Args_t>, bool> = true>
	tuple(Tag) { /*Empty*/ }

	tuple()				= default;
	tuple(const tuple&) = default;
	tuple(tuple&&)		= default;
	~tuple()			= default;

public:
	// Operators

	tuple& operator=(const tuple&) 	= default;
	tuple& operator=(tuple&&) 		= default;

public:
	// Helpers

	bool _equals(const tuple&) const {
        return true;
    }
};


template<class This, class... Rest>
class tuple<This, Rest...> : public tuple<Rest...>			// Recursive tuple implementation
{
public:
	using ThisType	= This;
	using Base		= tuple<Rest...>;

	ThisType ThisVal;											// Data stored to this iteration

public:
	// Construction Helpers

	// (H1) Helper for (1), (H2)
	template<class Tag, class _This, class... _Rest, enable_if_t<is_same_v<Tag, _Tuple_Exact_Args_t>, bool> = true>
	tuple(Tag, _This&& first, _Rest&&...rest)
		: Base(_Tuple_Exact_Args_t{}, custom::forward<_Rest>(rest)...), ThisVal(custom::forward<_This>(first)) { /*Empty*/ }
	
	// (H2) Helper for (H3)
	template<class Tag, class _Tuple, size_t... Indices, enable_if_t<is_same_v<Tag, _Tuple_Unpack_t>, bool> = true>
	tuple(Tag, _Tuple&& other, index_sequence<Indices...>);		// defined after custom::get()

	// (H3) Helper for (2), (3)
	template<class Tag, class _Tuple, enable_if_t<is_same_v<Tag, _Tuple_Unpack_t>, bool> = true>
	tuple(Tag, _Tuple&& other)
		: tuple(_Tuple_Unpack_t{}, custom::forward<_Tuple>(other),
				make_index_sequence<tuple_size_v<remove_reference_t<_Tuple>>>{}) { /*Empty*/ }	// Uses (H2)

public:
	// Constructors

	// (0) Default constructor
	template<class _This = This,
	enable_if_t<conjunction_v<is_default_constructible<_This>, is_default_constructible<Rest>...>, bool> = true>
	tuple()
		: Base(), ThisVal() { /*Empty*/ }		// Uses itself

	// (1) Copy/Move obj constructor
	template<class _This = This, class... _Rest,
	enable_if_t<tuple_constructible_v<tuple, _This, _Rest...>, bool> = true>
	tuple(_This&& first, _Rest&&... rest)
		: tuple(_Tuple_Exact_Args_t{}, custom::forward<_This>(first), custom::forward<_Rest>(rest)...) { /*Empty*/ }	// Uses (H1)

	// (2) Copy convertible constructor
	template<class... OtherTypes,
	enable_if_t<conjunction_v<	tuple_constructible<tuple, const OtherTypes&...>,
								tuple_convert<tuple, const tuple<OtherTypes...>&, OtherTypes...>>, bool> = true>
	tuple(const tuple<OtherTypes...>& other)
		: tuple(_Tuple_Unpack_t{}, other) { /*Empty*/ }	// Uses (H3)

	// (3) Move convertible constructor
	template<class... OtherTypes,
	enable_if_t<conjunction_v<	tuple_constructible<tuple, OtherTypes...>,
								tuple_convert<tuple, tuple<OtherTypes...>, OtherTypes...>>, bool> = true>
	tuple(tuple<OtherTypes...>&& other)
		: tuple(_Tuple_Unpack_t{}, custom::move(other)) { /*Empty*/ }		// Uses (H3)

	// (4) pair copy constructor
	template<class first, class second,
    enable_if_t<tuple_constructible_v<tuple, const first&, const second&>, bool> = true>
    tuple(const pair<first, second>& pair)
		: tuple(_Tuple_Exact_Args_t{}, pair.first, pair.second) { /*Empty*/ }	// Uses (H1)

	// (5) pair move constructor
    template<class first, class second,
	enable_if_t<tuple_constructible_v<tuple, first, second>, bool> = true>
	tuple(pair<first, second>&& pair)
        : tuple(_Tuple_Exact_Args_t{}, custom::move(pair.first), custom::move(pair.second)) { /*Empty*/ }	// Uses (H1)

	tuple(const tuple&) = default;
	tuple(tuple&&)		= default;
    ~tuple()            = default;

public:
	// Operators

	// Copy convertible operator
    template<class... OtherTypes,
	enable_if_t<conjunction_v<	negation<is_same<tuple, tuple<OtherTypes...>>>,
                            	tuple_assignable<tuple, const OtherTypes&...>>, bool> = true>
    tuple& operator=(const tuple<OtherTypes...>& other) {
        ThisVal 		= other.ThisVal;
        _get_rest()   	= other._get_rest();

        return *this;
    }

	// Move convertible operator
    template<class... OtherTypes,
	enable_if_t<conjunction_v<	negation<is_same<tuple, tuple<OtherTypes...>>>,
                              	tuple_assignable<tuple, OtherTypes...>>, bool> = true>
    tuple& operator=(tuple<OtherTypes...>&& other) {
        ThisVal 		= custom::forward<typename tuple<OtherTypes...>::ThisType>(other.ThisVal);
        _get_rest()   	= custom::forward<typename tuple<OtherTypes...>::Base>(other._get_rest());

        return *this;
    }

	template<class first, class second,
	enable_if_t<tuple_assignable_v<tuple, const first&, const second&>, bool> = true>
	tuple& operator=(const pair<first, second>& pair) {
		ThisVal				= pair.first;
		_get_rest().ThisVal	= pair.second;

		return *this;
	}

	template<class first, class second,
	enable_if_t<tuple_assignable_v<tuple, first, second>, bool> = true>
	tuple& operator=(pair<first, second>&& pair) {
		ThisVal				= custom::forward<first>(pair.first);
		_get_rest().ThisVal	= custom::forward<second>(pair.second);

		return *this;
	}

	tuple& operator=(const tuple&) 			= default;
	tuple& operator=(tuple&&) 				= default;
	tuple& operator=(const volatile tuple&) = delete;

public:
	// Helpers

    Base& _get_rest() noexcept { 							// get reference to rest of elements
        return *this;
    }

    const Base& _get_rest() const noexcept { 				// get const reference to rest of elements
        return *this;
    }

	template<class... OtherTypes>
    bool _equals(const tuple<OtherTypes...>& other) const {
        return ThisVal == other.ThisVal && Base::_equals(other._get_rest());
    }
};

template<class... Types1, class... Types2>
constexpr bool operator==(const tuple<Types1...>& left, const tuple<Types2...>& right) {
    static_assert(sizeof...(Types1) == sizeof...(Types2), "cannot compare tuples of different sizes");
    return left._equals(right);
}


// get
template<int Index, class... Types>
tuple_element_t<Index, tuple<Types...>>& get(tuple<Types...>& tuple) {					// Function to get Tuple member from reference
	using tuple_type = tuple_element_tt<Index, tuple<Types...>>;

	return static_cast<tuple_type&>(tuple).ThisVal;
}

template<int Index, class... Types>
const tuple_element_t<Index, tuple<Types...>>& get(const tuple<Types...>& tuple) {		// Same but const
	using tuple_type = tuple_element_tt<Index, tuple<Types...>>;

	return static_cast<const tuple_type&>(tuple).ThisVal;
}

template<int Index, class... Types>
tuple_element_t<Index, tuple<Types...>>&& get(tuple<Types...>&& tuple) {					// Function to get tuple member from rval
	using type			= tuple_element_t<Index, tuple<Types...>>;
	using tuple_type 	= tuple_element_tt<Index, tuple<Types...>>;

	return static_cast<Type&&>(static_cast<tuple_type&>(tuple).ThisVal);
}

template<int Index, class... Types>
const tuple_element_t<Index, tuple<Types...>>&& get(const tuple<Types...>&& tuple) {		// Same but const
	using type		= tuple_element_t<Index, tuple<Types...>>;
	using tuple_type = tuple_element_tt<Index, tuple<Types...>>;

	return static_cast<const Type&&>(static_cast<const tuple_type&>(tuple).ThisVal);
}

// (H2) Constructor helper defined here because of custom::get()
template<class This, class... Rest>
template<class Tag, class _Tuple, size_t... Indices, enable_if_t<is_same_v<Tag, _Tuple_Unpack_t>, bool>>
tuple<This, Rest...>::tuple(Tag, _Tuple&& other, index_sequence<Indices...>)
	: tuple(_Tuple_Exact_Args_t{}, custom::get<Indices>(custom::forward<_Tuple>(other))...) { /*Empty*/ }	// Uses (H1)


CUSTOM_DETAIL_BEGIN		// apply, make_from_tuple, tuple_cat helpers

// apply
template<class Callable, class _Tuple, size_t... Indices>
decltype(auto) _apply_impl(Callable&& func, _Tuple&& tuple, index_sequence<Indices...>) noexcept {
    return custom::invoke(custom::forward<Callable>(func), custom::get<Indices>(custom::forward<_Tuple>(tuple))...);
}

// make from tuple
template<class Type, class _Tuple, size_t... Indices>
Type _make_from_tuple_impl(_Tuple&& tuple, index_sequence<Indices...>) {
	static_assert(is_constructible_v<Type, decltype(custom::get<Indices>(custom::forward<_Tuple>(tuple)))...>, "The target type must be constructible using the arguments in tuple.");
	return Type(custom::get<Indices>(custom::forward<_Tuple>(tuple))...);
}

// tuple cat
template<class RetType, size_t... Kx, size_t... Ix, class _Tuple>
RetType _tuple_cat_impl(index_sequence<Kx...>, index_sequence<Ix...>, _Tuple tuple) {		// tuple as copy
	return RetType(custom::get<Kx>(custom::get<Ix>(custom::move(tuple)))...);
}

CUSTOM_DETAIL_END	// apply, make_from_tuple, tuple_cat helpers


// forward as tuple
template<class... Types>
tuple<Types&&...> forward_as_tuple(Types&&... args) {									// Forward arguments in a tuple
	return tuple<Types&&...>(custom::forward<Types>(args)...);
}

// apply
template<class Callable, class _Tuple>
decltype(auto) apply(Callable&& func, _Tuple&& tuple) noexcept {						// Invoke Callable obj with args as tuple
    return detail::_apply_impl(custom::forward<Callable>(func), custom::forward<_Tuple>(tuple), make_index_sequence<tuple_size_v<remove_reference_t<_Tuple>>>{});
}

// make tuple / tie
template<class... Args>
tuple<unrefwrap_t<Args>...> make_tuple(Args&&... args) {								// Create tuple with args
	return tuple<unrefwrap_t<Args>...>(custom::forward<Args>(args)...);
}

// tie
template<class... Args>
tuple<Args&...> tie(Args&... args) noexcept {											// Create tuple with references of args
	return tuple<Args&...>(args...);
}

// make_from_tuple
template<class Type, class _Tuple>
Type make_from_tuple(_Tuple&& tuple) {													// construct Type from the elements of _Tuple
	return detail::_make_from_tuple_impl<Type>(custom::forward<_Tuple>(tuple), make_index_sequence<tuple_size_v<remove_reference_t<_Tuple>>>{});
}

// tuple_cat
template<class... Tuples>																// concatenate tuples
typename _Tuple_Cat<tuple<Tuples&&...>, index_sequence<>, index_sequence<>, 0,
					make_index_sequence<tuple_size_v<remove_cv_ref_t<Tuples>>>...>::_Ret_Type tuple_cat(Tuples&&... tuples) {
	
	using Cat		= _Tuple_Cat<tuple<Tuples&&...>, index_sequence<>, index_sequence<>, 0,
								make_index_sequence<tuple_size_v<remove_cv_ref_t<Tuples>>>...>;
	using _Ret_Type	= typename Cat::_Ret_Type;
	using _KxSeq	= typename Cat::_KxSeq;
	using _IxSeq	= typename Cat::_IxSeq;

	return detail::_tuple_cat_impl<RetType>(KxSeq{}, IxSeq{}, custom::forward_as_tuple(custom::forward<Tuples>(tuples)...));
}

CUSTOM_END