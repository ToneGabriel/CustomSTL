#pragma once
#include "Common.h"
#include "Allocator.h"


CUSTOM_BEGIN

// Headings =================================================================================

template<class Type>
struct ArrayIterationData {
    Type* _Begin		= nullptr;
	Type* _End			= nullptr;

    ArrayIterationData() = default;
    ~ArrayIterationData();
};

template<class Array>
class ArrayIterator			// Array Iterator
{
public:
	using ValueType = typename Array::ValueType;
	using IterType	= typename Array::IterType;
	using Data		= ArrayIterationData<IterType>;

	IterType* _Ptr	= nullptr;
	Data* _Data		= nullptr;

public:

	explicit ArrayIterator(IterType* ptr, Data* data);
	~ArrayIterator();

public:

	ArrayIterator& operator++();							// ++it
	ArrayIterator operator++(int);							// it++
	ArrayIterator& operator+=(const size_t& diff);
	ArrayIterator operator+(const size_t& diff) const;

	ArrayIterator& operator--();							// --it
	ArrayIterator operator--(int);							// it--
	ArrayIterator& operator-=(const size_t& diff);
	ArrayIterator operator-(const size_t& diff) const;

	IterType* operator->();
	ValueType& operator*();

	bool operator==(const ArrayIterator& other) const;
	bool operator!=(const ArrayIterator& other) const;

public:

	const size_t get_index() const;							// Get the position for the element in array from iterator
	const bool is_begin() const;
	const bool is_end() const;
}; // END Array Iterator


template<class Type, size_t Size>
class Array			// Array Template
{
public:
	using ValueType = Type;											// Type for stored values
	using IterType	= ValueType;									// Type for iteration (same as value)
	using Alloc		= Allocator<ValueType>;							// Allocator type
	using Iterator	= ArrayIterator<Array<ValueType>>;			    // Iterator type
	using Data		= typename Iterator::Data;						// Iteration data

private:
	ValueType* _array	= nullptr;									// Actual container array

	mutable Data _data;												// Stores the ends of the array
	mutable Alloc _alloc;											// Allocator

public:
	// Constructors

	Array();														// Default Constructor
	Array(const size_t& newCapacity, const ValueType& copyValue);	// Add multiple copies Constructor
	Array(const Array& other);									    // Copy Constructor
	Array(Array&& other) noexcept;								    // Move Constructor
	~Array();														// Destructor

public:
	// Main functions

    void fill(const ValueType& copyValue);
    ValueType* data();
    const ValueType* data() const;

    ValueType& front();                                                     	// Get the value of the first component
	const ValueType& front() const;
	ValueType& back();                                                      	// Get the value of the last component
	const ValueType& back() const;

	const size_t size() const;													// Get size
	bool empty() const;															// Check if array is empty

	const ValueType& at(const size_t& index) const;								// Acces object at index with check (read only)
	ValueType& at(const size_t& index);											// Acces object at index with check

public:
	// Operators

	const ValueType& operator[](const size_t& index) const;						// Acces object at index (read only)
	ValueType& operator[](const size_t& index);									// Acces object at index
	
	Array& operator=(const Array& other);										// Assign operator using reference
	Array& operator=(Array&& other) noexcept;									// Assign operator using temporary

	bool operator==(const Array& other) const;
	bool operator!=(const Array& other) const;

public:
	// Iterator specific functions

	Iterator begin();
	const Iterator begin() const;

	Iterator end();
	const Iterator end() const;

private:
	// Others

	void _copy(const Array& other);											    // Generic copy function for vector
	void _move(Array&& other);													// Generic move function for vector
	void _clean_up_array();														// Clear and Deallocate array
	Data* _update_iteration_data() const;										// Update the data used in Iterator
}; // END Array Template



// Definitions =================================================================================

// Array Iterator Data
template<class Type>
ArrayIterationData<Type>::~ArrayIterationData() {
	_Begin	= nullptr;
	_End	= nullptr;
}

// Array Iterator
template<class Array>
ArrayIterator<Array>::ArrayIterator(IterType* ptr, Data* data)
	:_Ptr(ptr), _Data(data) { /*Empty*/ }

template<class Array>
ArrayIterator<Array>::~ArrayIterator() {
	_Ptr	= nullptr;
	_Data	= nullptr;
}

template<class Array>
ArrayIterator<Array>& ArrayIterator<Array>::operator++() {
	if (_Ptr >= _Data->_End)
		throw std::out_of_range("Cannot increment end iterator...");

	++_Ptr;
	return *this;
}

template<class Array>
ArrayIterator<Array> ArrayIterator<Array>::operator++(int) {
	ArrayIterator temp = *this;
	++(*this);
	return temp;
}

template<class Array>
ArrayIterator<Array>& ArrayIterator<Array>::operator+=(const size_t& diff) {
	if (_Ptr + diff >= _Data->_End)
		throw std::out_of_range("Cannot increment end iterator...");

	_Ptr += diff;
	return *this;
}

template<class Array>
ArrayIterator<Array> ArrayIterator<Array>::operator+(const size_t& diff) const {
	ArrayIterator temp = *this;
	temp += diff;
	return temp;
}

template<class Array>
ArrayIterator<Array>& ArrayIterator<Array>::operator--() {
	if (_Ptr <= _Data->_Begin)
		throw std::out_of_range("Cannot decrement begin iterator...");

	--_Ptr;
	return *this;
}

template<class Array>
ArrayIterator<Array> ArrayIterator<Array>::operator--(int) {
	ArrayIterator temp = *this;
	--(*this);
	return temp;
}

template<class Array>
ArrayIterator<Array>& ArrayIterator<Array>::operator-=(const size_t& diff) {
	if (_Ptr - diff <= _Data->_Begin)
		throw std::out_of_range("Cannot decrement begin iterator...");

	_Ptr -= diff;
	return *this;
}

template<class Array>
ArrayIterator<Array> ArrayIterator<Array>::operator-(const size_t& diff) const {
	ArrayIterator temp = *this;
	temp -= diff;
	return temp;
}

template<class Array>
typename ArrayIterator<Array>::IterType* ArrayIterator<Array>::operator->() {
	if (_Ptr >= _Data->_End)
		throw std::out_of_range("Cannot access end iterator...");

	return _Ptr;
}

template<class Array>
typename ArrayIterator<Array>::ValueType& ArrayIterator<Array>::operator*() {
	if (_Ptr >= _Data->_End)
		throw std::out_of_range("Cannot dereference end iterator...");

	return *_Ptr;
}

template<class Array>
bool ArrayIterator<Array>::operator==(const ArrayIterator& other) const {
	return _Ptr == other._Ptr;
}

template<class Array>
bool ArrayIterator<Array>::operator!=(const ArrayIterator& other) const {
	return !(*this == other);
}

template<class Array>
const size_t ArrayIterator<Array>::get_index() const {
	return static_cast<size_t>(_Ptr - _Data->_Begin);
}

template<class Array>
const bool ArrayIterator<Array>::is_begin() const {
	return _Ptr == _Data->_Begin;
}

template<class Array>
const bool ArrayIterator<Array>::is_end() const {
	return _Ptr == _Data->_End;
}
// END Array Iterator

// Array Template
// END Array Template

CUSTOM_END