#pragma once
#include "Common.h"
#include "Allocator.h"
#include "BaseIterator.h"

CUSTOM_BEGIN

// Headings =================================================================================

template<class Vector>
class VectorIterator			// Vector Iterator
{
private:
	template<class Type>
	struct VectorIterationData {
		Type* _Begin	= nullptr;
		Type* _End		= nullptr;

		VectorIterationData() = default;
		~VectorIterationData();
	};

public:
	using ValueType = typename Vector::ValueType;
	using IterType	= typename Vector::IterType;
	using Data		= VectorIterationData<IterType>;

	IterType* _Ptr	= nullptr;
	Data* _Data		= nullptr;

public:

	explicit VectorIterator(IterType* ptr, Data* data);
	~VectorIterator();


	VectorIterator& operator++();							// ++it
	VectorIterator operator++(int);							// it++
	VectorIterator& operator+=(const size_t& diff);
	VectorIterator operator+(const size_t& diff) const;

	VectorIterator& operator--();							// --it
	VectorIterator operator--(int);							// it--
	VectorIterator& operator-=(const size_t& diff);
	VectorIterator operator-(const size_t& diff) const;

	IterType* operator->();
	ValueType& operator*();

	bool operator==(const VectorIterator& other) const;
	bool operator!=(const VectorIterator& other) const;

public:

	const size_t get_index() const;							// Get the position for the element in array from iterator
	const bool is_begin() const;
	const bool is_end() const;
}; // END Vector Iterator


template<class Type>
class Vector			// Vector Template
{
public:
	using ValueType = Type;											// Type for stored values
	using IterType	= ValueType;									// Type for iteration (same as value)
	using Alloc		= Allocator<ValueType>;							// Allocator type
	using Iterator	= VectorIterator<Vector<ValueType>>;			// Iterator type
	using Data		= typename Iterator::Data;						// Iteration data

private:
	size_t _size		= 0;										// Number of components held by this
	size_t _capacity	= 0;										// Allocated momory of type ValueType
	ValueType* _array	= nullptr;									// Actual container array

	mutable Data _data;												// Stores the ends of the array
	mutable Alloc _alloc;											// Allocator

public:
	// Constructors

	Vector() = default;												// Default Constructor
	Vector(const size_t& newCapacity, const ValueType& copyValue);	// Add multiple copies Constructor
	Vector(const Vector& other);									// Copy Constructor
	Vector(Vector&& other) noexcept;								// Move Constructor
	~Vector();														// Destructor

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
	Iterator emplace(const Iterator& iterator, Args&&... args);					// Emplace object at iterator position with given arguments
	Iterator push(const Iterator& iterator, const ValueType& copyValue);		// Push copy object at iterator position
	Iterator push(const Iterator& iterator, ValueType&& moveValue);				// Push temporary object at iterator position
	Iterator pop(const Iterator& iterator);										// Remove component at iterator position

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
	
	Vector& operator=(const Vector& other);										// Assign operator using reference
	Vector& operator=(Vector&& other) noexcept;									// Assign operator using temporary

	bool operator==(const Vector& other) const;
	bool operator!=(const Vector& other) const;

public:
	// Iterator specific functions

	Iterator begin();
	const Iterator begin() const;

	Iterator end();
	const Iterator end() const;

private:
	// Others

	void _copy(const Vector& other);											// Generic copy function for vector
	void _move(Vector&& other);													// Generic move function for vector
	void _extend_if_full();														// Reserve 50% more capacity when full
	void _clean_up_array();														// Clear and Deallocate array
	Data* _update_iteration_data() const;										// Update the data used in Iterator
}; // END Vector Template



// Definitions =================================================================================

// Vector Iterator
template<class Vector>
template<class Type>
VectorIterator<Vector>::VectorIterationData<Type>::~VectorIterationData() {
	_Begin	= nullptr;
	_End	= nullptr;
}

template<class Vector>
VectorIterator<Vector>::VectorIterator(IterType* ptr, Data* data)
	:_Ptr(ptr), _Data(data) { /*Empty*/ }

template<class Vector>
VectorIterator<Vector>::~VectorIterator() {
	_Ptr	= nullptr;
	_Data	= nullptr;
}

template<class Vector>
VectorIterator<Vector>& VectorIterator<Vector>::operator++() {
	if (_Ptr >= _Data->_End)
		throw std::out_of_range("Cannot increment end iterator...");

	++_Ptr;
	return *this;
}

template<class Vector>
VectorIterator<Vector> VectorIterator<Vector>::operator++(int) {
	VectorIterator temp = *this;
	++(*this);
	return temp;
}

template<class Vector>
VectorIterator<Vector>& VectorIterator<Vector>::operator+=(const size_t& diff) {
	if (_Ptr + diff >= _Data->_End)
		throw std::out_of_range("Cannot increment end iterator...");

	_Ptr += diff;
	return *this;
}

template<class Vector>
VectorIterator<Vector> VectorIterator<Vector>::operator+(const size_t& diff) const {
	VectorIterator temp = *this;
	temp += diff;
	return temp;
}

template<class Vector>
VectorIterator<Vector>& VectorIterator<Vector>::operator--() {
	if (_Ptr <= _Data->_Begin)
		throw std::out_of_range("Cannot decrement begin iterator...");

	--_Ptr;
	return *this;
}

template<class Vector>
VectorIterator<Vector> VectorIterator<Vector>::operator--(int) {
	VectorIterator temp = *this;
	--(*this);
	return temp;
}

template<class Vector>
VectorIterator<Vector>& VectorIterator<Vector>::operator-=(const size_t& diff) {
	if (_Ptr - diff <= _Data->_Begin)
		throw std::out_of_range("Cannot decrement begin iterator...");

	_Ptr -= diff;
	return *this;
}

template<class Vector>
VectorIterator<Vector> VectorIterator<Vector>::operator-(const size_t& diff) const {
	VectorIterator temp = *this;
	temp -= diff;
	return temp;
}

template<class Vector>
typename VectorIterator<Vector>::IterType* VectorIterator<Vector>::operator->() {
	if (_Ptr >= _Data->_End)
		throw std::out_of_range("Cannot access end iterator...");

	return _Ptr;
}

template<class Vector>
typename VectorIterator<Vector>::ValueType& VectorIterator<Vector>::operator*() {
	if (_Ptr >= _Data->_End)
		throw std::out_of_range("Cannot dereference end iterator...");

	return *_Ptr;
}

template<class Vector>
bool VectorIterator<Vector>::operator==(const VectorIterator& other) const {
	return _Ptr == other._Ptr;
}

template<class Vector>
bool VectorIterator<Vector>::operator!=(const VectorIterator& other) const {
	return !(*this == other);
}

template<class Vector>
const size_t VectorIterator<Vector>::get_index() const {
	return _Ptr - _Data->_Begin;
}

template<class Vector>
const bool VectorIterator<Vector>::is_begin() const {
	return _Ptr == _Data->_Begin;
}

template<class Vector>
const bool VectorIterator<Vector>::is_end() const {
	return _Ptr == _Data->_End;
}
// END Vector Iterator


// Vector Template
template<class Type>
Vector<Type>::Vector(const size_t& newCapacity, const ValueType& copyValue) {
	realloc(newCapacity, copyValue);
}

template<class Type>
Vector<Type>::Vector(const Vector& other) {
	_copy(other);
}

template<class Type>
Vector<Type>::Vector(Vector&& other) noexcept {
	_move(std::move(other));
}

template<class Type>
Vector<Type>::~Vector() {
	_clean_up_array();
}

template<class Type>
void Vector<Type>::reserve(const size_t& newCapacity) {
	if (newCapacity < _size)
		_size = newCapacity;

	ValueType* newArray = _alloc.alloc(newCapacity);
	for (size_t i = 0; i < _size; i++)
		_alloc.construct(&newArray[i], std::move(_array[i]));

	_clean_up_array();
	_array = newArray;
	_capacity = newCapacity;
}

template<class Type>
void Vector<Type>::shrink_to_fit() {
	reserve(_size);
}

template<class Type>
void Vector<Type>::realloc(const size_t& newCapacity) {
	_clean_up_array();

	_capacity = newCapacity;
	_size = _capacity;

	_array = _alloc.alloc(_capacity);
	_alloc.construct_range(_array, _capacity);
}

template<class Type>
void Vector<Type>::realloc(const size_t& newCapacity, const ValueType& copyValue) {
	_clean_up_array();

	_capacity = newCapacity;
	_size = _capacity;

	_array = _alloc.alloc(_capacity);
	_alloc.construct_range(_array, _capacity, copyValue);
}

template<class Type>
void Vector<Type>::resize(const size_t& newSize) {
	if (newSize < _size)
		_alloc.destroy_range(&_array[newSize], _size - newSize);
	else {
		if (newSize > _capacity)
			reserve(newSize);

		_alloc.construct_range(&_array[_size], newSize - _size);
	}

	_size = newSize;
}

template<class Type>
void Vector<Type>::resize(const size_t& newSize, const ValueType& copyValue) {
	if (newSize < _size)
		_alloc.destroy_range(&_array[newSize], _size - newSize);
	else {
		if (newSize > _capacity)
			reserve(newSize);

		_alloc.construct_range(&_array[_size], newSize - _size, copyValue);
	}

	_size = newSize;
}

template<class Type>
template<class... Args>
void Vector<Type>::emplace_back(Args&&... args) {
	_extend_if_full();
	_alloc.construct(&_array[_size++], std::forward<Args>(args)...);
}

template<class Type>
void Vector<Type>::push_back(const ValueType& copyValue) {
	emplace_back(copyValue);
}

template<class Type>
void Vector<Type>::push_back(ValueType&& moveValue) {
	emplace_back(std::move(moveValue));
}

template<class Type>
void Vector<Type>::pop_back() {
	if (_size > 0)
		_alloc.destroy(&_array[--_size]);
}

template<class Type>
template<class... Args>
typename Vector<Type>::Iterator Vector<Type>::emplace(const Iterator& iterator, Args&&... args) {
	size_t index = iterator.get_index();				// Don't check end()
	emplace_back();
	for (size_t i = _size - 1; i > index; i--)
		_array[i] = std::move(_array[i - 1]);

	_alloc.destroy(&_array[index]);
	_alloc.construct(&_array[index], std::forward<Args>(args)...);

	return Iterator(&_array[index], _update_iteration_data());
}

template<class Type>
typename Vector<Type>::Iterator Vector<Type>::push(const Iterator& iterator, const ValueType& copyValue) {
	return emplace(iterator, copyValue);
}

template<class Type>
typename Vector<Type>::Iterator Vector<Type>::push(const Iterator& iterator, ValueType&& moveValue) {
	return emplace(iterator, std::move(moveValue));
}

template<class Type>
typename Vector<Type>::Iterator Vector<Type>::pop(const Iterator& iterator) {
	if (iterator.is_end())
		throw std::out_of_range("Array pop iterator outside range...");

	size_t index = iterator.get_index();
	for (size_t i = index; i < _size - 1; i++)
		_array[i] = std::move(_array[i + 1]);
	pop_back();

	return Iterator(&_array[index], _update_iteration_data());
}

template<class Type>
const size_t Vector<Type>::capacity() const {
	return _capacity;
}

template<class Type>
const size_t Vector<Type>::size() const {
	return _size;
}

template<class Type>
void Vector<Type>::clear() {
	_alloc.destroy_range(_array, _size);
	_size = 0;
}

template<class Type>
bool Vector<Type>::empty() const {
	return _size == 0;
}

template<class Type>
const typename Vector<Type>::ValueType& Vector<Type>::at(const size_t& index) const {
	if (index < 0 || index >= _size)
		throw std::out_of_range("Invalid Index...");

	return _array[index];
}

template<class Type>
typename Vector<Type>::ValueType& Vector<Type>::at(const size_t& index) {
	if (index < 0 || index >= _size)
		throw std::out_of_range("Invalid Index...");

	return _array[index];
}

template<class Type>
const typename Vector<Type>::ValueType& Vector<Type>::operator[](const size_t& index) const {
	return _array[index];
}

template<class Type>
typename Vector<Type>::ValueType& Vector<Type>::operator[](const size_t& index) {
	return _array[index];
}

template<class Type>
Vector<Type>& Vector<Type>::operator=(const Vector& other) {
	if (_array != other._array)
	{
		_clean_up_array();
		_copy(other);
	}

	return *this;
}

template<class Type>
Vector<Type>& Vector<Type>::operator=(Vector&& other) noexcept {
	if (_array != other._array)
	{
		_clean_up_array();
		_move(std::move(other));
	}

	return *this;
}

template<class Type>
bool Vector<Type>::operator==(const Vector& other) const {
	if (size() != other.size())
		return false;

	for (size_t i = 0; i < _size; i++)
		if (_array[i] != other._array[i])
			return false;

	return true;
}

template<class Type>
bool Vector<Type>::operator!=(const Vector& other) const {
	return !operator==(other);
}

template<class Type>
typename Vector<Type>::Iterator Vector<Type>::begin() {
	return Iterator(_array, _update_iteration_data());
}

template<class Type>
const typename Vector<Type>::Iterator Vector<Type>::begin() const {
	return Iterator(_array, _update_iteration_data());
}

template<class Type>
typename Vector<Type>::Iterator Vector<Type>::end() {
	return Iterator(_array + _size, _update_iteration_data());
}

template<class Type>
const typename Vector<Type>::Iterator Vector<Type>::end() const {
	return Iterator(_array + _size, _update_iteration_data());
}

template<class Type>
void Vector<Type>::_copy(const Vector& other) {
	_array = _alloc.alloc(other._capacity);
	for (size_t i = 0; i < other._size; i++)
		_alloc.construct(&_array[i], other._array[i]);

	_size = other._size;
	_capacity = other._capacity;
}

template<class Type>
void Vector<Type>::_move(Vector&& other) {
	_array = other._array;
	_size = other._size;
	_capacity = other._capacity;

	other._size = 0;
	other._capacity = 0;
	other._array = nullptr;
}

template<class Type>
typename Vector<Type>::Data* Vector<Type>::_update_iteration_data() const {
	_data._Begin = _array;
	_data._End = _array + _size;

	return &_data;
}

template<class Type>
void Vector<Type>::_clean_up_array() {
	if (_array != nullptr)
	{
		_alloc.destroy_range(_array, _size);
		_alloc.dealloc(_array, _capacity);
		_array = nullptr;
	}
}

template<class Type>
void Vector<Type>::_extend_if_full() {
	if (_size >= _capacity)
		reserve(_capacity + _capacity / 2 + 1);
}
// END Vector Template

CUSTOM_END