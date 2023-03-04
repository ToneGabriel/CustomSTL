#pragma once
#include "Utility.h"


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
	using Iterator	= ArrayIterator<Array<ValueType, Size>>;		// Iterator type
	using Data		= typename Iterator::Data;						// Iteration data

private:
	ValueType _array[Size];											// Actual container array
	mutable Data _data;

public:
	// Constructors

	Array();														// Default Constructor
	Array(const ValueType& copyValue);								// Add multiple copies Constructor
	Array(const Array& other);									    // Copy Constructor
	Array(Array&& other) noexcept;								    // Move Constructor
	~Array() = default;												// Destructor

public:
	// Main functions

    void fill(const ValueType& copyValue);

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
	void _update_iteration_data() const;										// Update the data used in Iterator
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
template<class Type, size_t Size>
Array<Type, Size>::Array() {
	_update_iteration_data();
}

template<class Type, size_t Size>
Array<Type, Size>::Array(const ValueType& copyValue) : Array() {
	fill(copyValue);
}

template<class Type, size_t Size>
Array<Type, Size>::Array(const Array& other) : Array() {
	_copy(other);
}

template<class Type, size_t Size>
Array<Type, Size>::Array(Array&& other) noexcept : Array() {
	_move(custom::move(other));
}

template<class Type, size_t Size>
void Array<Type, Size>::fill(const ValueType& copyValue) {
	for (size_t i = 0; i < Size; ++i)
		_array[i] = copyValue;
}

template<class Type, size_t Size>
typename Array<Type, Size>::ValueType& Array<Type, Size>::front() {
	return _array[0];
}

template<class Type, size_t Size>
const typename Array<Type, Size>::ValueType& Array<Type, Size>::front() const {
	return _array[0];
}

template<class Type, size_t Size>
typename Array<Type, Size>::ValueType& Array<Type, Size>::back() {
	return _array[Size - 1];
}

template<class Type, size_t Size>
const typename Array<Type, Size>::ValueType& Array<Type, Size>::back() const {
	return _array[Size - 1];
}

template<class Type, size_t Size>
const size_t Array<Type, Size>::size() const {
	return Size;
}

template<class Type, size_t Size>
bool Array<Type, Size>::empty() const {
	return Size == 0;
}

template<class Type, size_t Size>
const typename Array<Type, Size>::ValueType& Array<Type, Size>::at(const size_t& index) const {
	if (index < 0 || index >= Size)
		throw std::out_of_range("Invalid Index...");

	return _array[index];
}

template<class Type, size_t Size>
typename Array<Type, Size>::ValueType& Array<Type, Size>::at(const size_t& index) {
	if (index < 0 || index >= Size)
		throw std::out_of_range("Invalid Index...");

	return _array[index];
}

template<class Type, size_t Size>
const typename Array<Type, Size>::ValueType& Array<Type, Size>::operator[](const size_t& index) const {
	assert(!(index < 0 || index >= Size));
	return _array[index];
}

template<class Type, size_t Size>
typename Array<Type, Size>::ValueType& Array<Type, Size>::operator[](const size_t& index) {
	assert(!(index < 0 || index >= Size));
	return _array[index];
}

template<class Type, size_t Size>
Array<Type, Size>& Array<Type, Size>::operator=(const Array& other) {
	if (_array != other._array)
		_copy(other);

	return *this;
}

template<class Type, size_t Size>
Array<Type, Size>& Array<Type, Size>::operator=(Array&& other) noexcept {
	if (_array != other._array)
		_move(custom::move(other));

	return *this;
}

template<class Type, size_t Size>
bool Array<Type, Size>::operator==(const Array& other) const {
	for (size_t i = 0; i < Size; ++i)
		if (_array[i] != other._array[i])
			return false;

	return true;
}

template<class Type, size_t Size>
bool Array<Type, Size>::operator!=(const Array& other) const {
	return !(*this == other);
}

template<class Type, size_t Size>
typename Array<Type, Size>::Iterator Array<Type, Size>::begin() {
	return Iterator(&_array[0], &_data);
}

template<class Type, size_t Size>
const typename Array<Type, Size>::Iterator Array<Type, Size>::begin() const {
	return Iterator(&_array[0], &_data);
}

template<class Type, size_t Size>
typename Array<Type, Size>::Iterator Array<Type, Size>::end() {
	return Iterator(&_array[Size], &_data);
}

template<class Type, size_t Size>
const typename Array<Type, Size>::Iterator Array<Type, Size>::end() const {
	return Iterator(&_array[Size], &_data);
}

template<class Type, size_t Size>
void Array<Type, Size>::_copy(const Array& other) {
	for(size_t i = 0; i < Size; ++i)
		_array[i] = other._array[i];
}

template<class Type, size_t Size>
void Array<Type, Size>::_move(Array&& other) {
	for(size_t i = 0; i < Size; ++i)
		_array[i] = custom::move(other._array[i]);
}

template<class Type, size_t Size>
void Array<Type, Size>::_update_iteration_data() const {
	_data._Begin = const_cast<ValueType*>(&_array[0]);
	_data._End = const_cast<ValueType*>(&_array[Size]);
}
// END Array Template

CUSTOM_END