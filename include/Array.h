#pragma once
#include "Utility.h"
#include "Allocator.h"


CUSTOM_BEGIN

template<class Type, size_t Size>
struct ArrayData
{
	using ValueType		= Type;									// Type for stored values
	using IterType		= ValueType;							// Type for iteration (same as value)
	using Alloc			= Allocator<ValueType>;					// Allocator type

	size_t _Size = Size;
	ValueType _Array[Size];										// Actual container array
};

template<class Array>
class ArrayConstIterator
{
public:
	using Data			= typename Array::Data;
	using ValueType		= typename Array::ValueType;
	using IterType		= typename Array::IterType;

	IterType* _Ptr		= nullptr;
	const Data* _Data	= nullptr;

public:

	explicit ArrayConstIterator(IterType* ptr, const Data* data) noexcept
		:_Ptr(ptr), _Data(data) { /*Empty*/ }

	ArrayConstIterator& operator++() {
		if (_Ptr >= &_Data->_Array[_Data->_Size])
			throw std::out_of_range("Cannot increment end iterator...");

		++_Ptr;
		return *this;
	}

	ArrayConstIterator operator++(int) {
		ArrayConstIterator temp = *this;
		++(*this);
		return temp;
	}

	ArrayConstIterator& operator+=(const size_t& diff) {
		if (_Ptr + diff >= &_Data->_Array[_Data->_Size])
			throw std::out_of_range("Cannot increment end iterator...");

		_Ptr += diff;
		return *this;
	}

	ArrayConstIterator operator+(const size_t& diff) const {
		ArrayConstIterator temp = *this;
		temp += diff;
		return temp;
	}

	ArrayConstIterator& operator--() {
		if (_Ptr <= &_Data->_Array[0])
			throw std::out_of_range("Cannot decrement begin iterator...");

		--_Ptr;
		return *this;
	}

	ArrayConstIterator operator--(int) {
		ArrayConstIterator temp = *this;
		--(*this);
		return temp;
	}

	ArrayConstIterator& operator-=(const size_t& diff) {
		if (_Ptr - diff <= &_Data->_Array[0])
			throw std::out_of_range("Cannot decrement begin iterator...");

		_Ptr -= diff;
		return *this;
	}

	ArrayConstIterator operator-(const size_t& diff) const {
		ArrayConstIterator temp = *this;
		temp -= diff;
		return temp;
	}

	const IterType* operator->() const {
		if (_Ptr >= &_Data->_Array[_Data->_Size])
			throw std::out_of_range("Cannot access end iterator...");

		return _Ptr;
	}

	const ValueType& operator*() const {
		if (_Ptr >= &_Data->_Array[_Data->_Size])
			throw std::out_of_range("Cannot dereference end iterator...");

		return *_Ptr;
	}

	bool operator==(const ArrayConstIterator& other) const {
		return _Ptr == other._Ptr;
	}

	bool operator!=(const ArrayConstIterator& other) const {
		return !(*this == other);
	}

public:

	const size_t get_index() const {						// Get the position for the element in array from iterator
		return static_cast<size_t>(_Ptr - &_Data->_Array[0]);
	}

	const bool is_begin() const {
		return _Ptr == &_Data->_Array[0];
	}

	const bool is_end() const {
		return _Ptr == &_Data->_Array[_Data->_Size];
	}

};

template<class Array>
class ArrayIterator : public ArrayConstIterator<Array>
{
private:
	using Base		= ArrayConstIterator<Array>;
	
public:
	using Data		= typename Array::Data;
	using ValueType = typename Array::ValueType;
	using IterType	= typename Array::IterType;

public:

	explicit ArrayIterator(IterType* ptr, const Data* data) noexcept
		:Base(ptr, data) { /*Empty*/ }

	ArrayIterator& operator++() {
		Base::operator++();
		return *this;
	}

	ArrayIterator operator++(int) {
		ArrayIterator temp = *this;
		Base::operator++();
		return temp;
	}

	ArrayIterator& operator+=(const size_t& diff) {
		Base::operator+=(diff);
		return *this;
	}

	ArrayIterator operator+(const size_t& diff) const {
		ArrayIterator temp = *this;
		temp += diff;
		return temp;
	}

	ArrayIterator& operator--() {
		Base::operator--();
		return *this;
	}

	ArrayIterator operator--(int) {
		ArrayIterator temp = *this;
		Base::operator--();
		return temp;
	}

	ArrayIterator& operator-=(const size_t& diff) {
		Base::operator-=(diff);
		return *this;
	}

	ArrayIterator operator-(const size_t& diff) const {
		ArrayIterator temp = *this;
		temp -= diff;
		return temp;
	}

	IterType* operator->() const {
		return const_cast<IterType*>(Base::operator->());
	}

	ValueType& operator*() const {
		return const_cast<ValueType&>(Base::operator*());
	}
}; // END Array Iterator


template<class Type, size_t Size>
class Array							// Array Template
{
public:
	using Data					= ArrayData<Type, Size>;					// Members that are modified
	using ValueType 			= typename Data::ValueType;					// Type for stored values
	using IterType				= typename Data::IterType;					// Type for iteration (same as value)
	using Alloc					= typename Data::Alloc;						// Allocator type
	
	using Iterator				= ArrayIterator<Array<ValueType, Size>>;		// Iterator type
	using ConstIterator			= ArrayConstIterator<Array<ValueType, Size>>;	// Const Iterator type
	using ReverseIterator 		= custom::ReverseIterator<Iterator>;			// ReverseIterator type
	using ConstReverseIterator	= custom::ReverseIterator<ConstIterator>;		// Const Reverse Iterator type

private:
	Data _data;

public:
	// Constructors

	Array() = default;												// Default Constructor

	Array(const ValueType& copyValue) {								// Add multiple copies Constructor
		fill(copyValue);
	}

	Array(const Array& other) {										// Copy Constructor
		_copy(other);
	}

	Array(Array&& other) noexcept {									// Move Constructor
		_move(custom::move(other));
	}

	~Array() = default;												// Destructor

public:
	// Main functions

    void fill(const ValueType& copyValue) {
		for (size_t i = 0; i < _data._Size; ++i)
			_data._Array[i] = copyValue;
	}

    ValueType& front() {											// Get the value of the first component
		assert(_data._Size > 0);
		return _data._Array[0];
	}

	const ValueType& front() const {
		assert(_data._Size > 0);
		return _data._Array[0];
	}

	ValueType& back() {												// Get the value of the last component
		assert(_data._Size > 0);
		return _data._Array[_data._Size - 1];
	}

	const ValueType& back() const {
		assert(_data._Size > 0);
		return _data._Array[_data._Size - 1];
	}

	const size_t size() const {										// Get size
		return _data._Size;
	}

	bool empty() const {											// Check if array is empty
		return _data._Size == 0;
	}

	const ValueType& at(const size_t& index) const {				// Acces object at index with check (read only)
		if (index >= _data._Size)
			throw std::out_of_range("Invalid Index...");

		return _data._Array[index];
	}

	ValueType& at(const size_t& index) {							// Acces object at index with check
		if (index >= _data._Size)
			throw std::out_of_range("Invalid Index...");

		return _data._Array[index];
	}

public:
	// Operators

	const ValueType& operator[](const size_t& index) const {		// Acces object at index (read only)
		assert(!(index >= _data._Size));
		return _data._Array[index];
	}

	ValueType& operator[](const size_t& index) {					// Acces object at index
		assert(!(index >= _data._Size));
		return _data._Array[index];
	}
	
	Array& operator=(const Array& other) {							// Assign operator using reference
		if (_data._Array != other._data._Array)
			_copy(other);

		return *this;
	}

	Array& operator=(Array&& other) noexcept {						// Assign operator using temporary
		if (_data._Array != other._data._Array)
			_move(custom::move(other));

		return *this;
	}

	bool operator==(const Array& other) const {
		for (size_t i = 0; i < _data._Size; ++i)
			if (_data._Array[i] != other._data._Array[i])
				return false;

		return true;
	}

	bool operator!=(const Array& other) const {
		return !(*this == other);
	}

public:
	// Iterator specific functions

	Iterator begin() {
		return Iterator(&_data._Array[0], &_data);
	}

	const Iterator begin() const {
		return Iterator(&_data._Array[0], &_data);
	}

	ReverseIterator rbegin() {
		return ReverseIterator(end());
	}

	const ReverseIterator rbegin() const {
		return ReverseIterator(end());
	}

	Iterator end() {
		return Iterator(&_data._Array[_data._Size], &_data);
	}

	const Iterator end() const {
		return Iterator(&_data._Array[_data._Size], &_data);
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
		for(size_t i = 0; i < _data._Size; ++i)
			_data._Array[i] = other._data._Array[i];
	}

	void _move(Array&& other) {										// Generic move function for vector
		for(size_t i = 0; i < _data._Size; ++i)
			_data._Array[i] = custom::move(other._data._Array[i]);
	}
}; // END Array Template

CUSTOM_END