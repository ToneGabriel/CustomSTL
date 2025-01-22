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
template<class Tuple>
struct tuple_size;

template<class... Elements>
struct tuple_size<tuple<Elements...>> : integral_constant<size_t, sizeof...(Elements)> {};

template<class Tuple>
struct tuple_size<const Tuple> : public tuple_size<Tuple> {};

template<class Tuple>
struct tuple_size<volatile Tuple> : public tuple_size<Tuple> {};

template<class Tuple>
struct tuple_size<const volatile Tuple> : public tuple_size<Tuple> {};

template<class Tuple>
constexpr size_t tuple_size_v = tuple_size<Tuple>::value;

// tuple_element
template<size_t Index, class Tuple>
struct tuple_element;	// tuple_element prototype for accessing tuple members

template<size_t Index, class This, class... Rest>	// Recursive tuple_element implementation
struct tuple_element<Index, tuple<This, Rest...>> : public tuple_element<Index - 1, tuple<Rest...>> {};

template<class This, class... Rest>
struct tuple_element<0, tuple<This, Rest...>>	// Default tuple_element implementation
{
	using type			= This;
	using tuple_type	= tuple<This, Rest...>;
};

template<size_t Index, class Tuple>
using tuple_element_t = typename tuple_element<Index, Tuple>::type;

template<size_t Index, class Tuple>
using tuple_element_tuple_t = typename tuple_element<Index, Tuple>::tuple_type;

// get
template<int Index, class... Types>
constexpr tuple_element_t<Index, tuple<Types...>>& get(tuple<Types...>& tupleObj) noexcept;

template<int Index, class... Types>
constexpr const tuple_element_t<Index, tuple<Types...>>& get(const tuple<Types...>& tupleObj) noexcept;

template<int Index, class... Types>
constexpr tuple_element_t<Index, tuple<Types...>>&& get(tuple<Types...>&& tupleObj) noexcept;

template<int Index, class... Types>
constexpr const tuple_element_t<Index, tuple<Types...>>&& get(const tuple<Types...>&& tupleObj) noexcept;

CUSTOM_DETAIL_BEGIN

// _Is_Tuple_Constructible
template<bool SameSize, class Tuple, class... Args>
struct _Is_Tuple_Constructible_Impl : false_type {};

template<class... Types, class... Args>
struct _Is_Tuple_Constructible_Impl<true, tuple<Types...>, Args...> : conjunction<is_constructible<Types, Args>...> {};

template<class Tuple, class... Args>
struct _Is_Tuple_Constructible : _Is_Tuple_Constructible_Impl<tuple_size_v<Tuple> == sizeof...(Args), Tuple, Args...> {};

template<class Tuple, class... Args>
constexpr bool _Is_Tuple_Constructible_v = _Is_Tuple_Constructible<Tuple, Args...>::value;

// _Is_Tuple_Assignable
template<bool SameSize, class Tuple, class... Args>
struct _Is_Tuple_Assignable_Impl : false_type {};

template<class... Types, class... Args>
struct _Is_Tuple_Assignable_Impl<true, tuple<Types...>, Args...> : conjunction<is_assignable<Types&, Args>...> {};

template<class Tuple, class... Args>
struct _Is_Tuple_Assignable : _Is_Tuple_Assignable_Impl<tuple_size_v<Tuple> == sizeof...(Args), Tuple, Args...> {};

template<class Tuple, class... Args>
constexpr bool _Is_Tuple_Assignable_v = _Is_Tuple_Assignable<Tuple, Args...>::value;

// _Is_Tuple_Convertible
template<class ThisTuple, class OtherTuple, class... OtherTypes>
struct _Is_Tuple_Convertible : true_type {};

template<class This, class OtherTuple, class OtherType>
struct _Is_Tuple_Convertible<tuple<This>, OtherTuple, OtherType>
	: bool_constant<!disjunction_v<	is_same<This, OtherType>,
									is_constructible<This, OtherTuple>,
									is_convertible<OtherTuple, This>>> {};

// tuple_cat helper
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

template<class Callable, class Tuple, size_t... Indices>
constexpr auto _apply_impl(Callable&& func, Tuple&& tuple, index_sequence<Indices...>)
noexcept(noexcept(custom::invoke(custom::forward<Callable>(func), custom::get<Indices>(custom::forward<Tuple>(tuple))...)))
-> decltype(custom::invoke(custom::forward<Callable>(func), custom::get<Indices>(custom::forward<Tuple>(tuple))...))
{
    return custom::invoke(custom::forward<Callable>(func), custom::get<Indices>(custom::forward<Tuple>(tuple))...);
}

template<class Type, class Tuple, size_t... Indices>
constexpr Type _make_from_tuple_impl(Tuple&& tuple, index_sequence<Indices...>)
noexcept(is_nothrow_constructible_v<Type, decltype(custom::get<Indices>(custom::forward<Tuple>(tuple)))...>)
{
	static_assert(is_constructible_v<Type, decltype(custom::get<Indices>(custom::forward<Tuple>(tuple)))...>,
					"The target type must be constructible using the arguments in tuple.");

	return Type(custom::get<Indices>(custom::forward<Tuple>(tuple))...);
}

template<class RetType, size_t... Kx, size_t... Ix, class Tuple>
constexpr RetType _tuple_cat_impl(index_sequence<Kx...>, index_sequence<Ix...>, Tuple tuple)
{
	return RetType(custom::get<Kx>(custom::get<Ix>(custom::move(tuple)))...);
}

CUSTOM_DETAIL_END

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

	bool _equals(const tuple&) const
	{
        return true;
    }
};	// END tuple<>


template<class This, class... Rest>
class tuple<This, Rest...> : public tuple<Rest...>			// Recursive tuple implementation
{
public:
	// Friends

	template<int Index, class... Types>
	friend constexpr tuple_element_t<Index, tuple<Types...>>& get(tuple<Types...>& tupleObj) noexcept;

	template<int Index, class... Types>
	friend constexpr const tuple_element_t<Index, tuple<Types...>>& get(const tuple<Types...>& tupleObj) noexcept;

	template<int Index, class... Types>
	friend constexpr tuple_element_t<Index, tuple<Types...>>&& get(tuple<Types...>&& tupleObj) noexcept;

	template<int Index, class... Types>
	friend constexpr const tuple_element_t<Index, tuple<Types...>>&& get(const tuple<Types...>&& tupleObj) noexcept;

private:
	using _This_Type	= This;
	using _Base			= tuple<Rest...>;

	_This_Type _ThisVal;	// Data stored to this iteration

public:
	// Construction Helpers

	// (H1) Helper for (1), (H2)
	template<class Tag, class _This, class... _Rest, enable_if_t<is_same_v<Tag, _Tuple_Exact_Args_t>, bool> = true>
	tuple(Tag, _This&& first, _Rest&&...rest)
		: _Base(_Tuple_Exact_Args_t{}, custom::forward<_Rest>(rest)...), _ThisVal(custom::forward<_This>(first)) { /*Empty*/ }
	
	// (H2) Helper for (H3)
	template<class Tag, class Tuple, size_t... Indices, enable_if_t<is_same_v<Tag, _Tuple_Unpack_t>, bool> = true>
	tuple(Tag, Tuple&& other, index_sequence<Indices...>)
		: tuple(_Tuple_Exact_Args_t{}, custom::get<Indices>(custom::forward<Tuple>(other))...) { /*Empty*/ }	// Uses (H1)

	// (H3) Helper for (2), (3)
	template<class Tag, class Tuple, enable_if_t<is_same_v<Tag, _Tuple_Unpack_t>, bool> = true>
	tuple(Tag, Tuple&& other)
		: tuple(_Tuple_Unpack_t{}, custom::forward<Tuple>(other),
				make_index_sequence<tuple_size_v<remove_reference_t<Tuple>>>{}) { /*Empty*/ }	// Uses (H2)

public:
	// Constructors

	// (0) Default constructor
	template<class _This = This,
	enable_if_t<conjunction_v<is_default_constructible<_This>, is_default_constructible<Rest>...>, bool> = true>
	tuple()
		: _Base(), _ThisVal() { /*Empty*/ }

	// (1) Copy/Move obj constructor
	template<class _This = This, class... _Rest,
	enable_if_t<detail::_Is_Tuple_Constructible_v<tuple, _This, _Rest...>, bool> = true>
	tuple(_This&& first, _Rest&&... rest)
		: tuple(_Tuple_Exact_Args_t{}, custom::forward<_This>(first), custom::forward<_Rest>(rest)...) { /*Empty*/ }	// Uses (H1)

	// (2) Copy convertible constructor
	template<class... OtherTypes,
	enable_if_t<conjunction_v<	detail::_Is_Tuple_Constructible<tuple, const OtherTypes&...>,
								detail::_Is_Tuple_Convertible<tuple, const tuple<OtherTypes...>&, OtherTypes...>>, bool> = true>
	tuple(const tuple<OtherTypes...>& other)
		: tuple(_Tuple_Unpack_t{}, other) { /*Empty*/ }	// Uses (H3)

	// (3) Move convertible constructor
	template<class... OtherTypes,
	enable_if_t<conjunction_v<	detail::_Is_Tuple_Constructible<tuple, OtherTypes...>,
								detail::_Is_Tuple_Convertible<tuple, tuple<OtherTypes...>, OtherTypes...>>, bool> = true>
	tuple(tuple<OtherTypes...>&& other)
		: tuple(_Tuple_Unpack_t{}, custom::move(other)) { /*Empty*/ }		// Uses (H3)

	// (4) pair copy constructor
	template<class first, class second,
    enable_if_t<detail::_Is_Tuple_Constructible_v<tuple, const first&, const second&>, bool> = true>
    tuple(const pair<first, second>& pair)
		: tuple(_Tuple_Exact_Args_t{}, pair.first, pair.second) { /*Empty*/ }	// Uses (H1)

	// (5) pair move constructor
    template<class first, class second,
	enable_if_t<detail::_Is_Tuple_Constructible_v<tuple, first, second>, bool> = true>
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
                            	detail::_Is_Tuple_Assignable<tuple, const OtherTypes&...>>, bool> = true>
    tuple& operator=(const tuple<OtherTypes...>& other)
	{
        _ThisVal 		= other._ThisVal;
        _get_rest()   	= other._get_rest();

        return *this;
    }

	// Move convertible operator
    template<class... OtherTypes,
	enable_if_t<conjunction_v<	negation<is_same<tuple, tuple<OtherTypes...>>>,
                              	detail::_Is_Tuple_Assignable<tuple, OtherTypes...>>, bool> = true>
    tuple& operator=(tuple<OtherTypes...>&& other)
	{
        _ThisVal 		= custom::forward<typename tuple<OtherTypes...>::_This_Type>(other._ThisVal);
        _get_rest()   	= custom::forward<typename tuple<OtherTypes...>::_Base>(other._get_rest());

        return *this;
    }

	template<class first, class second,
	enable_if_t<detail::_Is_Tuple_Assignable_v<tuple, const first&, const second&>, bool> = true>
	tuple& operator=(const pair<first, second>& pair)
	{
		_ThisVal				= pair.first;
		_get_rest()._ThisVal	= pair.second;

		return *this;
	}

	template<class first, class second,
	enable_if_t<detail::_Is_Tuple_Assignable_v<tuple, first, second>, bool> = true>
	tuple& operator=(pair<first, second>&& pair)
	{
		_ThisVal				= custom::forward<first>(pair.first);
		_get_rest()._ThisVal	= custom::forward<second>(pair.second);

		return *this;
	}

	tuple& operator=(const tuple&) 			= default;
	tuple& operator=(tuple&&) 				= default;
	tuple& operator=(const volatile tuple&) = delete;

public:
	// Helpers

    _Base& _get_rest() noexcept	// get reference to rest of elements
	{
        return *this;
    }

    const _Base& _get_rest() const noexcept
	{
        return *this;
    }

	template<class... OtherTypes>
    bool _equals(const tuple<OtherTypes...>& other) const
	{
        return _ThisVal == other._ThisVal && _Base::_equals(other._get_rest());
    }
};	// END tuple<This, Rest...>

template<class... Types1, class... Types2>
constexpr bool operator==(const tuple<Types1...>& left, const tuple<Types2...>& right)
{
    static_assert(sizeof...(Types1) == sizeof...(Types2), "cannot compare tuples of different sizes");
    return left._equals(right);
}


// get
template<int Index, class... Types>
constexpr tuple_element_t<Index, tuple<Types...>>& get(tuple<Types...>& tupleObj) noexcept	// Function to get Tuple member from reference
{
	using _Tuple_Type = tuple_element_tuple_t<Index, tuple<Types...>>;

	return static_cast<_Tuple_Type&>(tupleObj)._ThisVal;
}

template<int Index, class... Types>
constexpr const tuple_element_t<Index, tuple<Types...>>& get(const tuple<Types...>& tupleObj) noexcept
{
	using _Tuple_Type = tuple_element_tuple_t<Index, tuple<Types...>>;

	return static_cast<const _Tuple_Type&>(tupleObj)._ThisVal;
}

template<int Index, class... Types>
constexpr tuple_element_t<Index, tuple<Types...>>&& get(tuple<Types...>&& tupleObj) noexcept	// Function to get tuple member from rval
{
	using _Type			= tuple_element_t<Index, tuple<Types...>>;
	using _Tuple_Type	= tuple_element_tuple_t<Index, tuple<Types...>>;

	return static_cast<_Type&&>(static_cast<_Tuple_Type&>(tupleObj)._ThisVal);
}

template<int Index, class... Types>
constexpr const tuple_element_t<Index, tuple<Types...>>&& get(const tuple<Types...>&& tupleObj) noexcept
{
	using _Type			= tuple_element_t<Index, tuple<Types...>>;
	using _Tuple_Type	= tuple_element_tuple_t<Index, tuple<Types...>>;

	return static_cast<const _Type&&>(static_cast<const _Tuple_Type&>(tupleObj)._ThisVal);
}

// forward as tuple
template<class... Types>
constexpr tuple<Types&&...> forward_as_tuple(Types&&... args) noexcept	// Forward arguments in a tuple
{
	return tuple<Types&&...>(custom::forward<Types>(args)...);
}

// apply
template<class Callable, class Tuple>
constexpr auto apply(Callable&& func, Tuple&& tuple)	// Invoke Callable obj with args as tuple
noexcept(noexcept(detail::_apply_impl(custom::forward<Callable>(func), custom::forward<Tuple>(tuple), make_index_sequence<tuple_size_v<remove_reference_t<Tuple>>>{})))
-> decltype(detail::_apply_impl(custom::forward<Callable>(func), custom::forward<Tuple>(tuple), make_index_sequence<tuple_size_v<remove_reference_t<Tuple>>>{}))
{
    return detail::_apply_impl(custom::forward<Callable>(func), custom::forward<Tuple>(tuple), make_index_sequence<tuple_size_v<remove_reference_t<Tuple>>>{});
}

// make tuple / tie
template<class... Args>
tuple<unrefwrap_t<Args>...> make_tuple(Args&&... args)	// Create tuple with args
{
	return tuple<unrefwrap_t<Args>...>(custom::forward<Args>(args)...);
}

// tie
template<class... Args>
tuple<Args&...> tie(Args&... args) noexcept	// Create tuple with references of args
{
	return tuple<Args&...>(args...);
}

// make_from_tuple
template<class Type, class Tuple>
constexpr Type make_from_tuple(Tuple&& tuple)	// construct Type from the elements of Tuple
noexcept(noexcept(detail::_make_from_tuple_impl<Type>(custom::forward<Tuple>(tuple), make_index_sequence<tuple_size_v<remove_reference_t<Tuple>>>{})))
{
	return detail::_make_from_tuple_impl<Type>(custom::forward<Tuple>(tuple), make_index_sequence<tuple_size_v<remove_reference_t<Tuple>>>{});
}

// tuple_cat
template<class... Tuples>	// concatenate tuples
constexpr typename detail::_Tuple_Cat<	tuple<Tuples&&...>, index_sequence<>, index_sequence<>, 0,
										make_index_sequence<tuple_size_v<remove_cv_ref_t<Tuples>>>...>::_Ret_Type tuple_cat(Tuples&&... tuples)
{	
	using _Cat		= detail::_Tuple_Cat<	tuple<Tuples&&...>, index_sequence<>, index_sequence<>, 0,
											make_index_sequence<tuple_size_v<remove_cv_ref_t<Tuples>>>...>;

	using _Ret_Type	= typename _Cat::_Ret_Type;
	using _KxSeq	= typename _Cat::_KxSeq;
	using _IxSeq	= typename _Cat::_IxSeq;

	return detail::_tuple_cat_impl<_Ret_Type>(_KxSeq{}, _IxSeq{}, custom::forward_as_tuple(custom::forward<Tuples>(tuples)...));
}

CUSTOM_END