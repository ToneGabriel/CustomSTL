#pragma once
#include "Common.h"
#include "Allocator.h"
#include "BaseIterator.h"


CUSTOM_BEGIN

// Headings =================================================================================

template<class Deque>
class DequeIterator : public BaseIterator<Deque>	// Deque Iterator
{
public:
	using Base		= BaseIterator<Deque>;
	using Data		= typename Base::Data;
	using IterType	= typename Base::IterType;
	using ValueType = typename Base::ValueType;

public:

	explicit DequeIterator(IterType* ptr, Data* data);

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
	size_t _front		= 0;
	size_t _back		= 0;
	ValueType* _array	= nullptr;									// Actual container array

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
	const bool _is_end(const Iterator& iterator) const;
	const size_t _get_index(const Iterator& iterator) const;					// Get the position for the element in array from iterator
	Data* _update_iteration_data() const;
}; // END Deque Template



// Definitions =================================================================================

// Deque Template
template<class Type>
Deque<Type>::Deque(const size_t& newCapacity, const ValueType& copyValue) {

}

template<class Type>
Deque<Type>::Deque(const Deque& other) {

}

template<class Type>
Deque<Type>::Deque(Deque&& other) noexcept {

}

template<class Type>
Deque<Type>::~Deque() {
	_clean_up_array();
}

template<class Type>
void Deque<Type>::reserve(const size_t& newCapacity)
{
	if (newCapacity < _size)
		_size = newCapacity;

	ValueType* newArray = _alloc.alloc(newCapacity);
	for (size_t i = _front; i <= _back; i++)
		_alloc.construct(&newArray[i], std::move(_array[i]));

	_clean_up_array();
	_array = newArray;
	_capacity = newCapacity;
}

template<class Type>
Deque<Type>::ValueType* Deque<Type>::_alloc_array(const size_t& capacity) {
	return _alloc.alloc(capacity + 1);
}

template<class Type>
void Deque<Type>::_dealloc_array(ValueType* address, const size_t& capacity) {
	_alloc.dealloc(address, capacity + 1);
}

template<class Type>
void Deque<Type>::_destroy_array(ValueType* address, const size_t& offset, const size_t& size, const size_t& capacity) {
	for (size_t i = 0; i < size; i++)
		_alloc.destroy(address + (offset + i) % capacity);
}

template<class Type>
void Deque<Type>::_clean_up_array()
{
	if (_array != nullptr)
	{
		_destroy_array(_array, front, _size, _capacity);
		_dealloc_array(_array, _capacity);
		_array = nullptr;
	}
}
// END Deque Template

CUSTOM_END