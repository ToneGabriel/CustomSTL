#pragma once
#include "Allocator.h"
#include "Utility.h"


CUSTOM_BEGIN

template<class Type>
struct VectorIterationData {		// Data used for iterating Vector
	Type* _Begin	= nullptr;
	Type* _End		= nullptr;

	VectorIterationData() = default;
	~VectorIterationData() {
		_Begin	= nullptr;
		_End	= nullptr;
	}
};

template<class Vector>
class VectorIterator			// Vector Iterator
{
public:
	using ValueType = typename Vector::ValueType;
	using IterType	= typename Vector::IterType;
	using Data		= VectorIterationData<IterType>;

	IterType* _Ptr	= nullptr;
	Data* _Data		= nullptr;

public:

	explicit VectorIterator(IterType* ptr, Data* data)
		:_Ptr(ptr), _Data(data) { /*Empty*/ }

	~VectorIterator() {
		_Ptr	= nullptr;
		_Data	= nullptr;
	}

public:

	VectorIterator& operator++() {
		if (_Ptr >= _Data->_End)
			throw std::out_of_range("Cannot increment end iterator...");

		++_Ptr;
		return *this;
	}

	VectorIterator operator++(int) {
		VectorIterator temp = *this;
		++(*this);
		return temp;
	}

	VectorIterator& operator+=(const size_t& diff) {
		if (_Ptr + diff >= _Data->_End)
			throw std::out_of_range("Cannot increment end iterator...");

		_Ptr += diff;
		return *this;
	}

	VectorIterator operator+(const size_t& diff) const {
		VectorIterator temp = *this;
		temp += diff;
		return temp;
	}

	VectorIterator& operator--() {
		if (_Ptr <= _Data->_Begin)
			throw std::out_of_range("Cannot decrement begin iterator...");

		--_Ptr;
		return *this;
	}

	VectorIterator operator--(int) {
		VectorIterator temp = *this;
		--(*this);
		return temp;
	}

	VectorIterator& operator-=(const size_t& diff) {
		if (_Ptr - diff <= _Data->_Begin)
			throw std::out_of_range("Cannot decrement begin iterator...");

		_Ptr -= diff;
		return *this;
	}

	VectorIterator operator-(const size_t& diff) const {
		VectorIterator temp = *this;
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

	bool operator==(const VectorIterator& other) const {
		return _Ptr == other._Ptr;
	}

	bool operator!=(const VectorIterator& other) const {
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
}; // END Vector Iterator


template<class Type>
class Vector			// Vector Template
{
public:
	using ValueType 		= Type;									// Type for stored values
	using IterType			= ValueType;							// Type for iteration (same as value)
	using Alloc				= Allocator<ValueType>;					// Allocator type
	using Iterator			= VectorIterator<Vector<ValueType>>;	// Iterator type
	using Data				= typename Iterator::Data;				// Iteration data
	using ReverseIterator 	= ReverseIterator<Iterator>;			// ReverseIterator type

private:
	Alloc _alloc;													// Allocator
	size_t _size		= 0;										// Number of components held by this
	size_t _capacity	= 0;										// Allocated momory of type ValueType
	ValueType* _array	= nullptr;									// Actual container array
	mutable Data _data;

	static constexpr size_t default_capacity = 8;

public:
	// Constructors

	Vector() {														// Default Constructor
		reserve(default_capacity);
	}

	Vector(const size_t& newCapacity, const ValueType& copyValue) {	// Add multiple copies Constructor
		realloc(newCapacity, copyValue);
	}

	Vector(const Vector& other) {									// Copy Constructor
		_copy(other);
	}

	Vector(Vector&& other) noexcept {								// Move Constructor
		_move(custom::move(other));
	}

	~Vector() {														// Destructor
		_clean_up_array();
	}

public:
	// Main functions

	void reserve(const size_t& newCapacity) {									// Allocate memory and move values if needed
		if (newCapacity < _size)
			_size = newCapacity;

		ValueType* newArray = _alloc.alloc(newCapacity);
		for (size_t i = 0; i < _size; ++i)
			_alloc.construct(&newArray[i], custom::move(_array[i]));

		_clean_up_array();
		_array = newArray;
		_capacity = newCapacity;
	}

	void shrink_to_fit() {														// Allocate memory with capacity equal to size and move values there
		reserve(_size);
	}

	void realloc(const size_t& newCapacity) {									// Allocate memory and populate it with default values (delete old)
		_clean_up_array();

		_capacity = newCapacity;
		_size = _capacity;

		_array = _alloc.alloc(_capacity);
		_alloc.construct_range(_array, _capacity);
	}

	void realloc(const size_t& newCapacity, const ValueType& copyValue) {		// Allocate memory and populate it with given reference (delete old)
		_clean_up_array();

		_capacity = newCapacity;
		_size = _capacity;

		_array = _alloc.alloc(_capacity);
		_alloc.construct_range(_array, _capacity, copyValue);
	}
	
	void resize(const size_t& newSize) {										// Change size and Construct/Destruct objects with default value if needed
		if (newSize < _size)
			_alloc.destroy_range(&_array[newSize], _size - newSize);
		else {
			if (newSize > _capacity)
				reserve(newSize);

			_alloc.construct_range(&_array[_size], newSize - _size);
		}

		_size = newSize;
	}

	void resize(const size_t& newSize, const ValueType& copyValue) {			// Change size and Construct/Destruct objects with given reference if needed
		if (newSize < _size)
			_alloc.destroy_range(&_array[newSize], _size - newSize);
		else {
			if (newSize > _capacity)
				reserve(newSize);

			_alloc.construct_range(&_array[_size], newSize - _size, copyValue);
		}

		_size = newSize;
	}

	template<class... Args>
	void emplace_back(Args&&... args) {											// Construct object using arguments (Args) and add it to the tail
		_extend_if_full();
		_alloc.construct(&_array[_size++], custom::forward<Args>(args)...);
	}

	void push_back(const ValueType& copyValue) {								// Construct object using reference and add it to the tail
		emplace_back(copyValue);
	}

	void push_back(ValueType&& moveValue) {										// Construct object using temporary and add it to the tail
		emplace_back(custom::move(moveValue));
	}

	void pop_back() {															// Remove last component
		if (_size > 0)
			_alloc.destroy(&_array[--_size]);
	}

	template<class... Args>
	Iterator emplace(const Iterator& iterator, Args&&... args) {				// Emplace object at iterator position with given arguments
		size_t index = iterator.get_index();				// Don't check end()
		emplace_back();
		for (size_t i = _size - 1; i > index; --i)
			_array[i] = custom::move(_array[i - 1]);

		_alloc.destroy(&_array[index]);
		_alloc.construct(&_array[index], custom::forward<Args>(args)...);

		return Iterator(&_array[index], _update_iteration_data());
	}

	Iterator push(const Iterator& iterator, const ValueType& copyValue) {		// Push copy object at iterator position
		return emplace(iterator, copyValue);
	}

	Iterator push(const Iterator& iterator, ValueType&& moveValue) {			// Push temporary object at iterator position
		return emplace(iterator, custom::move(moveValue));
	}

	Iterator pop(const Iterator& iterator) {									// Remove component at iterator position
		if (iterator.is_end())
			throw std::out_of_range("Array pop iterator outside range...");

		size_t index = iterator.get_index();
		for (size_t i = index; i < _size - 1; ++i)
			_array[i] = custom::move(_array[i + 1]);
		pop_back();

		return Iterator(&_array[index], _update_iteration_data());
	}

	const size_t capacity() const {												// Get capacity
		return _capacity;
	}

	const size_t size() const {													// Get size
		return _size;
	}

	bool empty() const {														// Check if array is empty
		return _size == 0;
	}
	
	void clear() {																// Remove ALL components but keep memory
		_alloc.destroy_range(_array, _size);
		_size = 0;
	}

	const ValueType& at(const size_t& index) const {							// Acces object at index with check (read only)
		if (index < 0 || index >= _size)
			throw std::out_of_range("Invalid Index...");

		return _array[index];
	}

	ValueType& at(const size_t& index) {										// Acces object at index with check
		if (index < 0 || index >= _size)
			throw std::out_of_range("Invalid Index...");

		return _array[index];
	}

	const ValueType& front() const {
		assert(_size > 0);
		return _array[0];
	}

	ValueType& front() {                                                     	// Get the value of the first component
		assert(_size > 0);
		return _array[0];
	}

	const ValueType& back() const {
		assert(_size > 0);
		return _array[_size - 1];
	}

	ValueType& back() {                                                      	// Get the value of the last component
		assert(_size > 0);
		return _array[_size - 1];
	}

public:
	// Operators

	const ValueType& operator[](const size_t& index) const {					// Acces object at index (read only)
		assert(!(index < 0 || index >= _size));
		return _array[index];
	}

	ValueType& operator[](const size_t& index) {								// Acces object at index
		assert(!(index < 0 || index >= _size));
		return _array[index];
	}

	Vector& operator=(const Vector& other) {									// Assign operator using reference
		if (_array != other._array)
		{
			_clean_up_array();
			_copy(other);
		}

		return *this;
	}

	Vector& operator=(Vector&& other) noexcept {								// Assign operator using temporary
		if (_array != other._array)
		{
			_clean_up_array();
			_move(custom::move(other));
		}

		return *this;
	}

	bool operator==(const Vector& other) const {
		if (size() != other.size())
			return false;

		for (size_t i = 0; i < _size; ++i)
			if (_array[i] != other._array[i])
				return false;

		return true;
	}

	bool operator!=(const Vector& other) const {
		return !(*this == other);
	}

public:
	// Iterator specific functions

	Iterator begin() {
		return Iterator(_array, _update_iteration_data());
	}

	const Iterator begin() const {
		return Iterator(_array, _update_iteration_data());
	}

	ReverseIterator rbegin() {
		return ReverseIterator(end());
	}

	const ReverseIterator rbegin() const {
		return ReverseIterator(end());
	}

	Iterator end() {
		return Iterator(_array + _size, _update_iteration_data());
	}

	const Iterator end() const {
		return Iterator(_array + _size, _update_iteration_data());
	}

	ReverseIterator rend() {
		return ReverseIterator(begin());
	}

	const ReverseIterator rend() const {
		return ReverseIterator(begin());
	}

private:
	// Helpers

	void _copy(const Vector& other) {											// Generic copy function for vector
		_array = _alloc.alloc(other._capacity);
		for (size_t i = 0; i < other._size; ++i)
			_alloc.construct(&_array[i], other._array[i]);

		_size 		= other._size;
		_capacity 	= other._capacity;
	}

	void _move(Vector&& other) {												// Generic move function for vector
		_array 		= other._array;
		_size 		= other._size;
		_capacity 	= other._capacity;

		other._array 	= nullptr;
		other._size 	= 0;
		other._capacity = 0;
	}

	void _extend_if_full() {													// Reserve 50% more capacity when full
		if (_size >= _capacity)
			reserve(_capacity + _capacity / 2 + 1);
	}

	void _clean_up_array() {													// Clear and Deallocate array
		if (_array != nullptr)
		{
			_alloc.destroy_range(_array, _size);
			_alloc.dealloc(_array, _capacity);
			_array = nullptr;
		}
	}

	Data* _update_iteration_data() const {										// Update the data used in Iterator
		_data._Begin 	= _array;
		_data._End 		= _array + _size;

		return &_data;
	}
}; // END Vector Template

CUSTOM_END