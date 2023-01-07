#pragma once
#include "Common.h"

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

	//template<class... ArgsType1, class... ArgsType2>
	//Pair(PiecewiseConstruct, ArgsType1&&... args1, ArgsType2&&... args2)
	//	:First(std::forward<ArgsType1>(args1)...), Second(std::forward<ArgsType2>(args2)...) { }

	~Pair() = default;
};

CUSTOM_END