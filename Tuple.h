#pragma once
#include "Common.h"

CUSTOM_BEGIN

template<class This, class... Rest>
class Tuple : public Tuple<Rest...> {		// Recursive Tuple implementation
public:
	This First;

public:
	Tuple(Rest&&... rest)
		:Tuple<Rest...>(std::forward<Rest>(rest)...), First() { }

	Tuple(const This& first, Rest&&... rest)
		:Tuple<Rest...>(std::forward<Rest>(rest)...), First(first) { }

	Tuple(This&& first, Rest&&... rest)
		:Tuple<Rest...>(std::forward<Rest>(rest)...), First(std::move(first)) { }
};

template<class This>
class Tuple<This> {							// Default implementation
public:
	This First;

public:
	Tuple()
		:First() {}

	Tuple(const This& first) 
		:First(first) { }

	Tuple(This&& first)
		:First(std::move(first)) { }
};


template<size_t Index, class This, class... Rest>
struct GetImpl {
	static auto value(const Tuple<This, Rest...>& tuple) -> decltype(GetImpl<Index - 1, Rest...>::value(tuple)) {
		return GetImpl<Index - 1, Rest...>::value(tuple);
	}
};

template<class This, class... Rest>
struct GetImpl<0, This, Rest...> {
	static This value(const Tuple<This, Rest...>& tuple) {
		return tuple.First;
	}
};

template<int Index, class This, class... Rest>
auto get(const Tuple<This, Rest...>& tuple) -> decltype(GetImpl<Index, This, Rest...>::value(tuple)) { //class Type<index, This, Rest...>::value
	return GetImpl<Index, This, Rest...>::value(tuple);
}

CUSTOM_END