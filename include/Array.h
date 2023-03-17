#pragma once
#include "Utility.h"


CUSTOM_BEGIN

template<class Type>
struct ArrayIterationData {
    Type* _Begin		= nullptr;
	Type* _End			= nullptr;

    ArrayIterationData() = default;
    ~ArrayIterationData() {
		_Begin	= nullptr;
		_End	= nullptr;
	}
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

	explicit ArrayIterator(IterType* ptr, Data* data)
		:_Ptr(ptr), _Data(data) { /*Empty*/ }

	~ArrayIterator() {
		_Ptr	= nullptr;
		_Data	= nullptr;
	}

public:

	ArrayIterator& operator++() {
		if (_Ptr >= _Data->_End)
			throw std::out_of_range("Cannot increment end iterator...");

		++_Ptr;
		return *this;
	}

	ArrayIterator operator++(int) {
		ArrayIterator temp = *this;
		++(*this);
		return temp;
	}

	ArrayIterator& operator+=(const size_t& diff) {
		if (_Ptr + diff >= _Data->_End)
			throw std::out_of_range("Cannot increment end iterator...");

		_Ptr += diff;
		return *this;
	}

	ArrayIterator operator+(const size_t& diff) const {
		ArrayIterator temp = *this;
		temp += diff;
		return temp;
	}

	ArrayIterator& operator--() {
		if (_Ptr <= _Data->_Begin)
			throw std::out_of_range("Cannot decrement begin iterator...");

		--_Ptr;
		return *this;
	}

	ArrayIterator operator--(int) {
		ArrayIterator temp = *this;
		--(*this);
		return temp;
	}

	ArrayIterator& operator-=(const size_t& diff) {
		if (_Ptr - diff <= _Data->_Begin)
			throw std::out_of_range("Cannot decrement begin iterator...");

		_Ptr -= diff;
		return *this;
	}

	ArrayIterator operator-(const size_t& diff) const {
		ArrayIterator temp = *this;
		temp -= diff;
		return temp;
	}

	IterType* operator->() {
		if (_Ptr >= _Data->_End)
			throw std::out_of_range("Cannot access end iterator...");

		return _Ptr;
	}

	ValueType& operator*() {
		if (_Ptr >= _Data->_End)
			throw std::out_of_range("Cannot dereference end iterator...");

		return *_Ptr;
	}

	bool operator==(const ArrayIterator& other) const {
		return _Ptr == other._Ptr;
	}

	bool operator!=(const ArrayIterator& other) const {
		return !(*this == other);
	}

public:

	const size_t get_index() const {						// Get the position for the element in array from iterator
		return static_cast<size_t>(_Ptr - _Data->_Begin);
	}

	const bool is_begin() const {
		return _Ptr == _Data->_Begin;
	}

	const bool is_end() const {
		return _Ptr == _Data->_End;
	}
}; // END Array Iterator


template<class Type, size_t Size>
class Array							// Array Template
{
public:
	using ValueType 		= Type;											// Type for stored values
	using IterType			= ValueType;									// Type for iteration (same as value)
	using Alloc				= Allocator<ValueType>;							// Allocator type
	using Iterator			= ArrayIterator<Array<ValueType, Size>>;		// Iterator type
	using Data				= typename Iterator::Data;						// Iteration data
	using ReverseIterator 	= ReverseIterator<Iterator>;					// ReverseIterator type

private:
	ValueType _array[Size];											// Actual container array
	mutable Data _data;

public:
	// Constructors

	Array() {														// Default Constructor
		_update_iteration_data();
	}

	Array(const ValueType& copyValue) : Array() {					// Add multiple copies Constructor
		fill(copyValue);
	}

	Array(const Array& other) : Array()	{							// Copy Constructor
		_copy(other);
	}

	Array(Array&& other) noexcept : Array()	{						// Move Constructor
		_move(custom::move(other));
	}

	~Array() = default;												// Destructor

public:
	// Main functions

    void fill(const ValueType& copyValue) {
		for (size_t i = 0; i < Size; ++i)
			_array[i] = copyValue;
	}

    ValueType& front() {											// Get the value of the first component
		assert(Size > 0);
		return _array[0];
	}

	const ValueType& front() const {
		assert(Size > 0);
		return _array[0];
	}

	ValueType& back() {												// Get the value of the last component
		assert(Size > 0);
		return _array[Size - 1];
	}

	const ValueType& back() const {
		assert(Size > 0);
		return _array[Size - 1];
	}

	const size_t size() const {										// Get size
		return Size;
	}

	bool empty() const {											// Check if array is empty
		return Size = 0;
	}

	const ValueType& at(const size_t& index) const {				// Acces object at index with check (read only)
		if (index >= Size)
			throw std::out_of_range("Invalid Index...");

		return _array[index];
	}

	ValueType& at(const size_t& index) {							// Acces object at index with check
		if (index >= Size)
			throw std::out_of_range("Invalid Index...");

		return _array[index];
	}

public:
	// Operators

	const ValueType& operator[](const size_t& index) const {		// Acces object at index (read only)
		assert(!(index >= Size));
		return _array[index];
	}

	ValueType& operator[](const size_t& index) {					// Acces object at index
		assert(!(index >= Size));
		return _array[index];
	}
	
	Array& operator=(const Array& other) {							// Assign operator using reference
		if (_array != other._array)
			_copy(other);

		return *this;
	}

	Array& operator=(Array&& other) noexcept {						// Assign operator using temporary
		if (_array != other._array)
			_move(custom::move(other));

		return *this;
	}

	bool operator==(const Array& other) const {
		for (size_t i = 0; i < Size; ++i)
			if (_array[i] != other._array[i])
				return false;

		return true;
	}

	bool operator!=(const Array& other) const {
		return !(*this == other);
	}

public:
	// Iterator specific functions

	Iterator begin() {
		return Iterator(&_array[0], &_data);
	}

	const Iterator begin() const {
		return Iterator(&_array[0], &_data);
	}

	ReverseIterator rbegin() {
		return ReverseIterator(end());
	}

	const ReverseIterator rbegin() const {
		return ReverseIterator(end());
	}

	Iterator end() {
		return Iterator(&_array[Size], &_data);
	}

	const Iterator end() const {
		return Iterator(&_array[Size], &_data);
	}

	ReverseIterator rend() {
		return ReverseIterator(begin());
	}

	const ReverseIterator rend() const {
		return ReverseIterator(begin());
	}

private:
	// Others

	void _copy(const Array& other) {								// Generic copy function for vector
		for(size_t i = 0; i < Size; ++i)
			_array[i] = other._array[i];
	}

	void _move(Array&& other) {										// Generic move function for vector
		for(size_t i = 0; i < Size; ++i)
			_array[i] = custom::move(other._array[i]);
	}

	void _update_iteration_data() const {							// Update the data used in Iterator
		_data._Begin 	= const_cast<ValueType*>(&_array[0]);
		_data._End 		= const_cast<ValueType*>(&_array[Size]);
	}
}; // END Array Template

CUSTOM_END