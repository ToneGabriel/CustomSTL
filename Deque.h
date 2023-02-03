#pragma once
#include "Common.h"
#include "Allocator.h"


CUSTOM_BEGIN

// Headings =================================================================================

template<class Type>
struct DequeIterationData {			// Data used for iterating Deque
	Type* _Begin	= nullptr;
	Type* _End		= nullptr;
	Type* _ArrBase	= nullptr;
	size_t _TrueCapacity;

	DequeIterationData() = default;
	~DequeIterationData();
};


template<class Deque>
class DequeIterator			// Deque Iterator
{
public:
	using ValueType = typename Deque::ValueType;
	using IterType	= typename Deque::IterType;
	using Data		= DequeIterationData<IterType>;

	IterType* _Ptr	= nullptr;
	Data* _Data		= nullptr;

public:

	explicit DequeIterator(IterType* ptr, Data* data);
	~DequeIterator();

	DequeIterator& operator++();							// ++it
	DequeIterator operator++(int);							// it++
	DequeIterator& operator+=(const size_t& diff);
	DequeIterator operator+(const size_t& diff) const;

	DequeIterator& operator--();							// --it
	DequeIterator operator--(int);							// it--
	DequeIterator& operator-=(const size_t& diff);
	DequeIterator operator-(const size_t& diff) const;

	IterType* operator->();
	ValueType& operator*();

	bool operator==(const DequeIterator& other) const;
	bool operator!=(const DequeIterator& other) const;

public:

	const size_t get_index() const;							// Get the position for the element in array from iterator
	const bool is_begin() const;
	const bool is_end() const;
}; // END Deque Iterator


template<class Type>
class Deque					// Deque Template implemented as circular array
{
public:
	using ValueType = Type;											// Type for stored values
	using IterType	= ValueType;									// Type for iteration (same as value)
	using Alloc		= Allocator<ValueType>;							// Allocator type
	using Iterator	= DequeIterator<Deque<ValueType>>;				// Iterator type
	using Data		= typename Iterator::Data;						// Iteration data

private:
	size_t _size		= 0;										// Number of components held by this
	size_t _capacity	= 0;										// Allocated momory of type ValueType
	ValueType* _array	= nullptr;									// Actual container array
	
	size_t _front		= 0;										// TODO: take care of -1 situations
	size_t _back		= 0;

	mutable Data _data;												// Stores the ends of the array
	mutable Alloc _alloc;											// Allocator

public:
	// Constructors

	Deque() = default;												// Default Constructor
	Deque(const size_t& newCapacity, const ValueType& copyValue);	// Add multiple copies Constructor
	Deque(const Deque& other);										// Copy Constructor
	Deque(Deque&& other) noexcept;									// Move Constructor
	~Deque();

public:
	// Main functions

	void reserve(const size_t& newCapacity);									// Allocate memory and move values if needed
	void shrink_to_fit();														// Allocate memory with capacity equal to size and move values there
	void realloc(const size_t& newCapacity);									// Allocate memory and populate it with default values (delete old)
	void realloc(const size_t& newCapacity, const ValueType& copyValue);		// Allocate memory and populate it with given reference (delete old)
	void resize(const size_t& newSize);											// Change size and Construct/Destruct objects with default value if needed
	void resize(const size_t& newSize, const ValueType& copyValue);				// Change size and Construct/Destruct objects with given reference if needed

	template<class... Args>
	void emplace_back(Args&&... args);											// Construct object using arguments (Args) and add it to the tail
	void push_back(const ValueType& copyValue);									// Construct object using reference and add it to the tail
	void push_back(ValueType&& moveValue);										// Construct object using temporary and add it to the tail
	void pop_back();															// Remove last component

	template<class... Args>
	void emplace_front(Args&&... args);											// Construct object using arguments (Args) and add it to the front
	void push_front(const ValueType& copyValue);								// Construct object using reference and add it to the front
	void push_front(ValueType&& moveValue);										// Construct object using temporary and add it to the front
	void pop_front();															// Remove first component

	template<class... Args>
	Iterator emplace(const Iterator& iterator, Args&&... args);					// Emplace object at iterator position with given arguments
	Iterator push(const Iterator& iterator, const ValueType& copyValue);		// Push copy object at iterator position
	Iterator push(const Iterator& iterator, ValueType&& moveValue);				// Push temporary object at iterator position
	Iterator pop(const Iterator& iterator);										// Remove component at iterator position

	ValueType& front();                                                     	// Get the value of the first component
	const ValueType& front() const;
	ValueType& back();                                                      	// Get the value of the last component
	const ValueType& back() const;

	const size_t capacity() const;												// Get capacity
	const size_t size() const;													// Get size
	void clear();																// Remove ALL components but keep memory
	bool empty() const;															// Check if array is empty

	const ValueType& at(const size_t& index) const;								// Acces object at index with check (read only)
	ValueType& at(const size_t& index);											// Acces object at index with check

public:
	// Operators

	const ValueType& operator[](const size_t& index) const;						// Acces object at index (read only)
	ValueType& operator[](const size_t& index);									// Acces object at index

	Deque& operator=(const Deque& other);										// Assign operator using reference
	Deque& operator=(Deque&& other) noexcept;									// Assign operator using temporary

	bool operator==(const Deque& other) const;
	bool operator!=(const Deque& other) const;

public:
	// Iterator specific functions

	Iterator begin();
	const Iterator begin() const;

	Iterator end();
	const Iterator end() const;

private:
	// Others

	ValueType* _alloc_array(const size_t& capacity);							// Allocate memory +1 null term
	void _dealloc_array(ValueType* address, const size_t& capacity);			// Deallocate memory +1 null term
	void _destroy_array(ValueType* address, const size_t& offset, const size_t& size, const size_t& capacity);
	
	void _copy(const Deque& other);												// Generic copy function for vector
	void _move(Deque&& other);													// Generic move function for vector
	void _extend_if_full();														// Reserve 50% more capacity when full
	void _clean_up_array();

	const size_t _get_true_index(const size_t& index) const;
	size_t& _increment_offset(size_t& offset, const size_t& inc = 1);
	size_t& _decrement_offset(size_t& offset, const size_t& dec = 1);
	Data* _update_iteration_data() const;
}; // END Deque Template



// Definitions =================================================================================

// Deque Iterator Data
template<class Type>
DequeIterationData<Type>::~DequeIterationData() {
	_Begin	= nullptr;
	_End	= nullptr;
}

// Deque Iterator
template<class Deque>
DequeIterator<Deque>::DequeIterator(IterType* ptr, Data* data)
	:_Ptr(ptr), _Data(data) { /*Empty*/ }

template<class Deque>
DequeIterator<Deque>::~DequeIterator() {
	_Ptr	= nullptr;
	_Data	= nullptr;
}

template<class Deque>
DequeIterator<Deque>& DequeIterator<Deque>::operator++() {

}

template<class Deque>
DequeIterator<Deque> DequeIterator<Deque>::operator++(int) {

}

template<class Deque>
DequeIterator<Deque>& DequeIterator<Deque>::operator+=(const size_t& diff) {

}

template<class Deque>
DequeIterator<Deque> DequeIterator<Deque>::operator+(const size_t& diff) const {

}

template<class Deque>
DequeIterator<Deque>& DequeIterator<Deque>::operator--() {

}

template<class Deque>
DequeIterator<Deque> DequeIterator<Deque>::operator--(int) {

}

template<class Deque>
DequeIterator<Deque>& DequeIterator<Deque>::operator-=(const size_t& diff) {

}

template<class Deque>
DequeIterator<Deque> DequeIterator<Deque>::operator-(const size_t& diff) const {

}

template<class Deque>
typename DequeIterator<Deque>::IterType* DequeIterator<Deque>::operator->() {

}

template<class Deque>
typename DequeIterator<Deque>::ValueType& DequeIterator<Deque>::operator*() {

}

template<class Deque>
bool DequeIterator<Deque>::operator==(const DequeIterator& other) const {

}

template<class Deque>
bool DequeIterator<Deque>::operator!=(const DequeIterator& other) const {

}

template<class Deque>
const size_t DequeIterator<Deque>::get_index() const {

}

template<class Deque>
const bool DequeIterator<Deque>::is_begin() const {

}

template<class Deque>
const bool DequeIterator<Deque>::is_end() const {

}
// END Deque Iterator

// Deque Template
template<class Type>
Deque<Type>::Deque(const size_t& newCapacity, const ValueType& copyValue) {
	realloc(newCapacity, copyValue);
}

template<class Type>
Deque<Type>::Deque(const Deque& other) {
	_copy(other);
}

template<class Type>
Deque<Type>::Deque(Deque&& other) noexcept {
	_move(std::move(other));
}

template<class Type>
Deque<Type>::~Deque() {
	_clean_up_array();
}

template<class Type>
void Deque<Type>::reserve(const size_t& newCapacity)
{
	ValueType* newArray = _alloc_array(newCapacity);

	if (newCapacity < _size)
		_size = newCapacity;

	if(_size > 0)
	{
		for(size_t i = 0; i < _size; i++)
		{
			_alloc.construct(&newArray[i], std::move(_array[_front]));
			_increment_offset(_front);
		}

		_front = 0;
		_back = _size - 1;
	}

	_clean_up_array();
	_array = newArray;
	_capacity = newCapacity;
}

template<class Type>
void Deque<Type>::shrink_to_fit() {
	reserve(_size);
}

template<class Type>
void Deque<Type>::realloc(const size_t& newCapacity) {
	_clean_up_array();

	_capacity = newCapacity;
	_size = _capacity;
	_front = 0;
	_back = _size - 1;		// TODO: check back = -1

	_array = _alloc_array(_capacity);
	_alloc.construct_range(_array, _capacity);
}

template<class Type>
void Deque<Type>::realloc(const size_t& newCapacity, const ValueType& copyValue) {
	_clean_up_array();

	_capacity = newCapacity;
	_size = _capacity;
	_front = 0;
	_back = _size - 1;

	_array = _alloc.alloc(_capacity);
	_alloc.construct_range(_array, _capacity, copyValue);
}

template<class Type>
template<class... Args>
void Deque<Type>::emplace_back(Args&&... args) {
	_extend_if_full();

	if(_size == 0)
		_alloc.construct(&_array[_back], std::forward<Args>(args)...);
	else
		_alloc.construct(&_array[_increment_offset(_back)], std::forward<Args>(args)...);

	++_size;
}

template<class Type>
void Deque<Type>::push_back(const ValueType& copyValue) {
	emplace_back(copyValue);
}

template<class Type>
void Deque<Type>::push_back(ValueType&& moveValue) {
	emplace_back(std::move(moveValue));
}

template<class Type>
void Deque<Type>::pop_back() {
	if (_size > 0)
		{
			_alloc.destroy(&_array[_back]);
			_decrement_offset(_back);
			--_size;
		}
}

template<class Type>
template<class... Args>
void Deque<Type>::emplace_front(Args&&... args) {
	_extend_if_full();

	if(_size == 0)
		_alloc.construct(&_array[_front], std::forward<Args>(args)...);
	else
		_alloc.construct(&_array[_decrement_offset(_front)], std::forward<Args>(args)...);

	++_size;
}

template<class Type>
void Deque<Type>::push_front(const ValueType& copyValue) {
	emplace_front(copyValue);
}

template<class Type>
void Deque<Type>::push_front(ValueType&& moveValue) {
	emplace_front(std::move(moveValue));
}

template<class Type>
void Deque<Type>::pop_front() {
	if (_size > 0)
		{
			_alloc.destroy(&_array[_front]);
			_increment_offset(_front);
			--_size;
		}
}

template<class Type>
typename Deque<Type>::ValueType& Deque<Type>::front() {
	return _array[_front];
}


template<class Type>
const typename Deque<Type>::ValueType& Deque<Type>::front() const {
	return _array[_front];
}

template<class Type>
typename Deque<Type>::ValueType& Deque<Type>::back() {
	return _array[_back];
}

template<class Type>
const typename Deque<Type>::ValueType& Deque<Type>::back() const {
	return _array[_back];
}

template<class Type>
const size_t Deque<Type>::capacity() const {
	return _capacity;
}

template<class Type>
const size_t Deque<Type>::size() const {
	return _size;
}

template<class Type>
void Deque<Type>::clear() {
	_destroy_array(_array, _front, _size, _capacity);
	_size = 0;
}

template<class Type>
bool Deque<Type>::empty() const {
	return _size == 0;
}

template<class Type>
typename Deque<Type>::ValueType& Deque<Type>::operator[](const size_t& index) {
	return _array[_get_true_index(index)];
}

template<class Type>
typename Deque<Type>::Iterator Deque<Type>::begin() {
	return Iterator(_array + _front, _update_iteration_data());
}

template<class Type>
const typename Deque<Type>::Iterator Deque<Type>::begin() const {
	return Iterator(_array + _front, _update_iteration_data());
}

template<class Type>
typename Deque<Type>::Iterator Deque<Type>::end() {
	return Iterator(_array + _back + 1, _update_iteration_data());
}

template<class Type>
const typename Deque<Type>::Iterator Deque<Type>::end() const {
	return Iterator(_array + _back + 1, _update_iteration_data());
}

template<class Type>
typename Deque<Type>::ValueType* Deque<Type>::_alloc_array(const size_t& capacity) {
	return _alloc.alloc(capacity);
}

template<class Type>
void Deque<Type>::_dealloc_array(ValueType* address, const size_t& capacity) {
	_alloc.dealloc(address, capacity);
}

template<class Type>
void Deque<Type>::_destroy_array(ValueType* address, const size_t& offset, const size_t& size, const size_t& capacity) {
	for (size_t i = 0; i < size; i++)
		_alloc.destroy(address + (offset + i) % (capacity));	// TODO: replace with true_index
}

template<class Type>
void Deque<Type>::_extend_if_full() {
	if (_size >= _capacity)
		reserve(_capacity + _capacity / 2 + 1);
}

template<class Type>
const size_t Deque<Type>::_get_true_index(const size_t& index) const {
	return (_front + index) % (_capacity + 1);
}

template<class Type>
size_t& Deque<Type>::_increment_offset(size_t& offset, const size_t& inc) {
	offset = (offset + inc) % (_capacity + 1);
	return offset;
}

template<class Type>
size_t& Deque<Type>::_decrement_offset(size_t& offset, const size_t& dec) {
	if(offset - dec < 0)
		offset = _capacity + 1 - (offset - dec);
	else
		offset = (offset - dec) % (_capacity + 1);
	
	return offset;
}

template<class Type>
void Deque<Type>::_clean_up_array()
{
	if (_array != nullptr)
	{
		_destroy_array(_array, _front, _size, _capacity);
		_dealloc_array(_array, _capacity);
		_array = nullptr;
	}
}

template<class Type>
typename Deque<Type>::Data* Deque<Type>::_update_iteration_data() const {
	_data._Begin = _array + _front;
	_data._End = _array + _back + 1;
	_data._ArrBase = _array;
	_data._TrueCapacity = _capacity + 1;

	return &_data;
}
// END Deque Template

CUSTOM_END