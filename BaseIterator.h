#pragma once
#include "Common.h"

CUSTOM_BEGIN

template<class Container>
class BaseIterator
{
private:
	template<class Type>
	struct IterationData {
		Type* _Begin	= nullptr;
		Type* _End		= nullptr;

		IterationData() = default;

		~IterationData() {
			_Begin	= nullptr;
			_End	= nullptr;
		}
	};

public:
	using ValueType = typename Container::ValueType;
	using IterType	= typename Container::IterType;
	using Data		= IterationData<IterType>;

	IterType* _Ptr			= nullptr;
	Data* _IterationData	= nullptr;

public:

	explicit BaseIterator(IterType* ptr, Data* data)
		:_Ptr(ptr), _IterationData(data) { }

	virtual ~BaseIterator() {
		_Ptr			= nullptr;
		_IterationData	= nullptr;
	}
};

CUSTOM_END