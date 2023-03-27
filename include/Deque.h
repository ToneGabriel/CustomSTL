#pragma once
#include "Allocator.h"
#include "Utility.h"
#include "Vector.h"


CUSTOM_BEGIN

template<class Type>
struct DequeData
{
    using ValueType		= Type;						// Type for stored values
	using IterType		= ValueType;				// Type for iteration (same as value)
	using Alloc			= Allocator<ValueType>;		// Allocator type

    Vector<ValueType> _FrontContainer;
    Vector<ValueType> _BackContainer;
};


template<class Deque>
class DequeConstIterator
{
public:
	using Data			= typename Deque::Data;
	using ValueType		= typename Deque::ValueType;
	using IterType		= typename Deque::IterType;

	IterType* _Ptr		= nullptr;
	const Data* _Data	= nullptr;

public:

    explicit DequeConstIterator(IterType* ptr, const Data* data)
		:_Ptr(ptr), _Data(data) { /*Empty*/ }

	DequeConstIterator& operator++() {
        if (_Ptr == _Data->_BackContainer._data._Last)
			throw std::out_of_range("Cannot increment end iterator...");

        if (_Ptr == _Data->_FrontContainer._data._End - 1)
            _Ptr = _Data->_BackContainer.data._First;
        else
            ++_Ptr;

		return *this;
	}

	DequeConstIterator operator++(int) {
		DequeConstIterator temp = *this;
		++(*this);
		return temp;
	}

	DequeConstIterator& operator+=(const size_t& diff) {
		// if(get_pos() + diff > _Data->_Size)
		// 	throw std::out_of_range("Cannot increment end iterator...");

		// _Data->increment(_Ptr, diff);
		// return *this;
	}

	DequeConstIterator operator+(const size_t& diff) const {
		DequeConstIterator temp = *this;
		temp += diff;
		return temp;
	}

	DequeConstIterator& operator--() {
		// if (_Ptr == _Data->_Begin)
		// 	throw std::out_of_range("Cannot decrement begin iterator...");

		// _Data->decrement(_Ptr);
		// return *this;
	}

	DequeConstIterator operator--(int) {
		DequeConstIterator temp = *this;
		--(*this);
		return temp;
	}

	DequeConstIterator& operator-=(const size_t& diff) {
		// if(get_pos() < diff)
		// 	throw std::out_of_range("Cannot decrement begin iterator...");

		// _Data->decrement(_Ptr, diff);
		// return *this;
	}

	DequeConstIterator operator-(const size_t& diff) const {
		DequeConstIterator temp = *this;
		temp -= diff;
		return temp;
	}

	const IterType* operator->() const {
		// if (_Ptr == _Data->_End)
		// 	throw std::out_of_range("Cannot access end iterator...");

		// return _Ptr;
	}

	const ValueType& operator*() const {
		// if (_Ptr == _Data->_End)
		// 	throw std::out_of_range("Cannot dereference end iterator...");

		// return *_Ptr;
	}

	bool operator==(const DequeConstIterator& other) const {
		return _Ptr == other._Ptr;
	}

	bool operator!=(const DequeConstIterator& other) const {
		return !(*this == other);
	}

public:

	const size_t get_index() const {						// Get the index for the element in array from iterator (relative to array address, NOT begin)
		//return static_cast<size_t>(_Ptr - _Data->_Base);
	}

	const bool is_begin() const {
		//return _Ptr == _Data->_Front;
	}

	const bool is_end() const {
		//return _Ptr == _Data->_End;
	}
};

template<class Deque>
class DequeIterator : public DequeConstIterator<Deque>			// Deque Iterator
{
private:
	using Base		= DequeConstIterator<Deque>;

public:
	using Data		= typename Deque::Data;
	using ValueType	= typename Deque::ValueType;
	using IterType	= typename Deque::IterType;

public:

	explicit DequeIterator(IterType* ptr, Data* data)
		:Base(ptr, data) { /*Empty*/ }

	DequeIterator& operator++() {
		Base::operator++();
		return *this;
	}

	DequeIterator operator++(int) {
		DequeIterator temp = *this;
		Base::operator++();
		return temp;
	}

	DequeIterator& operator+=(const size_t& diff) {
		Base::operator+=(diff);
		return *this;
	}

	DequeIterator operator+(const size_t& diff) const {
		DequeIterator temp = *this;
		temp += diff;
		return temp;
	}

	DequeIterator& operator--() {
		Base::operator--();
		return *this;
	}

	DequeIterator operator--(int) {
		DequeIterator temp = *this;
		Base::operator--();
		return temp;
	}

	DequeIterator& operator-=(const size_t& diff) {
		Base::operator-=(diff);
		return *this;
	}

	DequeIterator operator-(const size_t& diff) const {
		DequeIterator temp = *this;
		temp -= diff;
		return temp;
	}

	IterType* operator->() const {
		return const_cast<IterType*>(Base::operator->());
	}

	ValueType& operator*() const {
		return const_cast<ValueType&>(Base::operator*());
	}
}; // END Deque Iterator


template<class Type>
class Deque					// Deque Template implemented as circular array
{
public:
	using Data					= DequeData<Type>;							// Members that are modified
	using ValueType 			= typename Data::ValueType;					// Type for stored values
	using IterType				= typename Data::IterType;					// Type for iteration (same as value)
	
	using Iterator				= DequeIterator<Deque<ValueType>>;			// Iterator type
	using ConstIterator			= DequeConstIterator<Deque<ValueType>>;		// Const Iterator type
	using ReverseIterator 		= custom::ReverseIterator<Iterator>;		// ReverseIterator type
	using ConstReverseIterator	= custom::ReverseIterator<ConstIterator>;	// Const Reverse Iterator type

private:
	Data _data;

};

CUSTOM_END