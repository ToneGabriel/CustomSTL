#pragma once
#include "Common.h"
#include "Allocator.h"
#include "List.h"
#include "Vector.h"
#include <cassert>


// CUSTOM_BEGIN

// // Headings =================================================================================

// template<class Type>
// class Deque					// Deque Template implemented as circular array (with list as iterator)
// {
// public:
// 	using ValueType 	= Type;										// Type for stored values
// 	using IterList		= List<ValueType>;							// List of ValueType used for iterating
// 	using Node			= typename IterList::Node;					// Node component from List
// 	using CircularArray	= Vector<Node*>;							// Vector of Node* used for quick access
// 	using Iterator		= typename List<ValueType>::Iterator;		// Iterator for this container (identical to List iterator)

// private:
// 	size_t _front		= 0;										// Indicator of first elem (begin())
// 	size_t _back		= 0;										// Indicator of last elem (end() - 1)
// 	IterList _elems;												// Used to iterate through container
// 	CircularArray _array;											// Circular array used for quick access

// public:
// 	// Constructors

// 	Deque();														// Default Constructor
// 	Deque(const size_t& newCapacity, const ValueType& copyValue);	// Add multiple copies Constructor
// 	Deque(const Deque& other);										// Copy Constructor
// 	Deque(Deque&& other) noexcept;									// Move Constructor
// 	~Deque();

// public:
// 	// Main functions

// 	void reserve(const size_t& newCapacity);									// Allocate memory and move values if needed
// 	void shrink_to_fit();														// Allocate memory with capacity equal to size and move values there
// 	void realloc(const size_t& newCapacity);									// Allocate memory and populate it with default values (delete old)
// 	void realloc(const size_t& newCapacity, const ValueType& copyValue);		// Allocate memory and populate it with given reference (delete old)
// 	void resize(const size_t& newSize);											// Change size and Construct/Destruct objects with default value if needed
// 	void resize(const size_t& newSize, const ValueType& copyValue);				// Change size and Construct/Destruct objects with given reference if needed

// 	template<class... Args>
// 	void emplace_back(Args&&... args);											// Construct object using arguments (Args) and add it to the tail
// 	void push_back(const ValueType& copyValue);									// Construct object using reference and add it to the tail
// 	void push_back(ValueType&& moveValue);										// Construct object using temporary and add it to the tail
// 	void pop_back();															// Remove last component

// 	template<class... Args>
// 	void emplace_front(Args&&... args);											// Construct object using arguments (Args) and add it to the front
// 	void push_front(const ValueType& copyValue);								// Construct object using reference and add it to the front
// 	void push_front(ValueType&& moveValue);										// Construct object using temporary and add it to the front
// 	void pop_front();															// Remove first component

// 	template<class... Args>
// 	Iterator emplace(const Iterator& iterator, Args&&... args);					// Emplace object at iterator position with given arguments
// 	Iterator push(const Iterator& iterator, const ValueType& copyValue);		// Push copy object at iterator position
// 	Iterator push(const Iterator& iterator, ValueType&& moveValue);				// Push temporary object at iterator position
// 	Iterator pop(const Iterator& iterator);										// Remove component at iterator position

// 	ValueType& front();                                                     	// Get the value of the first component
// 	const ValueType& front() const;
// 	ValueType& back();                                                      	// Get the value of the last component
// 	const ValueType& back() const;

// 	const size_t capacity() const;												// Get capacity
// 	const size_t size() const;													// Get size
// 	void clear();																// Remove ALL components but keep memory
// 	bool empty() const;															// Check if array is empty
// 	void print_details();

// 	const ValueType& at(const size_t& index) const;								// Acces object at index with check (read only)
// 	ValueType& at(const size_t& index);											// Acces object at index with check

// public:
// 	// Operators

// 	const ValueType& operator[](const size_t& index) const;						// Acces object at index (read only)
// 	ValueType& operator[](const size_t& index);									// Acces object at index

// 	Deque& operator=(const Deque& other);										// Assign operator using reference
// 	Deque& operator=(Deque&& other) noexcept;									// Assign operator using temporary

// 	bool operator==(const Deque& other) const;
// 	bool operator!=(const Deque& other) const;

// public:
// 	// Iterator specific functions

// 	Iterator begin();
// 	const Iterator begin() const;

// 	Iterator end();
// 	const Iterator end() const;

// private:
// 	// Others
	
// 	void _copy(const Deque& other);												// Generic copy function for vector
// 	void _move(Deque&& other);													// Generic move function for vector
// 	void _extend_if_full();														// Reserve 50% more capacity when full
// 	void _clean_up_array();

// 	const size_t _get_true_index(const size_t& index) const;
// 	size_t& _circular_increment(size_t& offset);
// 	size_t& _circular_decrement(size_t& offset);
// }; // END Deque Template



// // Definitions =================================================================================

// // Deque Template

// template<class Type>
// Deque<Type>::Deque() 
// 	: _array(CircularArray::default_capacity, nullptr), _elems() { /*Empty*/ }

// template<class Type>
// Deque<Type>::Deque(const size_t& newCapacity, const ValueType& copyValue)
// 	: _array(newCapacity, nullptr), _elems() 
// {
// 	// TODO: implement
// }

// template<class Type>
// Deque<Type>::Deque(const Deque& other) {
// 	_copy(other);
// }

// template<class Type>
// Deque<Type>::Deque(Deque&& other) noexcept {
// 	_move(std::move(other));
// }

// template<class Type>
// Deque<Type>::~Deque() {
// 	clear();	// TODO: check
// }

// template<class Type>
// void Deque<Type>::reserve(const size_t& newCapacity) {
	
// }

// template<class Type>
// void Deque<Type>::shrink_to_fit() {

// }

// template<class Type>
// void Deque<Type>::realloc(const size_t& newCapacity) {
	
// }

// template<class Type>
// void Deque<Type>::realloc(const size_t& newCapacity, const ValueType& copyValue) {
	
// }

// template<class Type>
// template<class... Args>
// void Deque<Type>::emplace_back(Args&&... args) {
// 	Node* newNode = new Node(std::forward<Args>(args)...);

// 	if(!empty())
// 		_circular_increment(_back);

// 	_array[_back] = newNode;
// 	_elems._insert_node_before(_elems._head, newNode);
// }

// template<class Type>
// void Deque<Type>::push_back(const ValueType& copyValue) {
// 	emplace_back(copyValue);
// }

// template<class Type>
// void Deque<Type>::push_back(ValueType&& moveValue) {
// 	emplace_back(std::move(moveValue));
// }

// template<class Type>
// void Deque<Type>::pop_back() {
// 	_elems.pop_back();
// 	_array[_back] = nullptr;
// 	_circular_decrement(_back);
// }

// template<class Type>
// template<class... Args>
// void Deque<Type>::emplace_front(Args&&... args) {
// 	Node* newNode = new Node(std::forward<Args>(args)...);

// 	if(!empty())
// 		_circular_decrement(_front);

// 	_array[_front] = newNode;
// 	_elems._insert_node_before(_elems._head->Next, newNode);
// }

// template<class Type>
// void Deque<Type>::push_front(const ValueType& copyValue) {
// 	emplace_front(copyValue);
// }

// template<class Type>
// void Deque<Type>::push_front(ValueType&& moveValue) {
// 	emplace_front(std::move(moveValue));
// }

// template<class Type>
// void Deque<Type>::pop_front() {
// 	_elems.pop_front();
// 	_array[_front] = nullptr;
// 	_circular_increment(_front);
// }

// template<class Type>
// typename Deque<Type>::ValueType& Deque<Type>::front() {
// 	return _elems.front();		// _array[_front]->Value;
// }

// template<class Type>
// const typename Deque<Type>::ValueType& Deque<Type>::front() const {
// 	return _elems.front();
// }

// template<class Type>
// typename Deque<Type>::ValueType& Deque<Type>::back() {
// 	return _elems.back();		// _array[_back]->Value;
// }

// template<class Type>
// const typename Deque<Type>::ValueType& Deque<Type>::back() const {
// 	return _elems.back();
// }

// template<class Type>
// const size_t Deque<Type>::capacity() const {
// 	return _array.capacity();
// }

// template<class Type>
// const size_t Deque<Type>::size() const {
// 	return _elems.size();
// }

// template<class Type>
// void Deque<Type>::clear() {
// 	_elems.clear();
// 	_front = _back = 0;
// 	//_array.clear();		// TODO: _array??
// }

// template<class Type>
// bool Deque<Type>::empty() const {
// 	return _elems.empty();
// }

// template<class Type>
// void Deque<Type>::print_details() {
// 	std::cout << "Size= " << size() << '\n';
// 	std::cout << "Capacity= " << capacity() << '\n';
// 	std::cout << "Front= " << _front << '\n';
// 	std::cout << "Back= " << _back << '\n';

// 	for (size_t i = 0; i < capacity(); i++)
// 		if(_array[i] == nullptr)
// 			std::cout << "N ";
// 		else
// 			std::cout << _array[i]->Value << ' ';
	
// 	std::cout << "\n\n";
// }

// template<class Type>
// const typename Deque<Type>::ValueType& Deque<Type>::at(const size_t& index) const {
// 	if (index < 0 || index >= size())
// 		throw std::out_of_range("Invalid Index...");

// 	return _array[_get_true_index(index)]->Value;
// }

// template<class Type>
// typename Deque<Type>::ValueType& Deque<Type>::at(const size_t& index) {
// 	if (index < 0 || index >= size())
// 		throw std::out_of_range("Invalid Index...");

// 	return _array[_get_true_index(index)]->Value;
// }

// template<class Type>
// const typename Deque<Type>::ValueType& Deque<Type>::operator[](const size_t& index) const {
// 	assert(!(index < 0 || index >= size()));
// 	return _array[_get_true_index(index)]->Value;
// }

// template<class Type>
// typename Deque<Type>::ValueType& Deque<Type>::operator[](const size_t& index) {
// 	assert(!(index < 0 || index >= size()));
// 	return _array[_get_true_index(index)]->Value;
// }

// template<class Type>
// Deque<Type>& Deque<Type>::operator=(const Deque& other) {
// 	if (_elems._head != other._elems._head)
// 	{
// 		_elems = other._elems;
// 		//_force_rehash(other.bucket_count());
// 	}

// 	return *this;
// }

// template<class Type>
// Deque<Type>& Deque<Type>::operator=(Deque&& other) noexcept {
// 	if (_elems._head != other._elems._head)
// 	{
// 		_elems = std::move(other._elems);
// 		//_force_rehash(other.bucket_count());
// 	}

// 	return *this;
// }

// template<class Type>
// bool Deque<Type>::operator==(const Deque& other) const {
// 	return _elems == other._elems;
// }

// template<class Type>
// bool Deque<Type>::operator!=(const Deque& other) const {
// 	return !operator==(other);
// }

// template<class Type>
// typename Deque<Type>::Iterator Deque<Type>::begin() {
// 	return _elems.begin();
// }

// template<class Type>
// const typename Deque<Type>::Iterator Deque<Type>::begin() const {
// 	return _elems.begin();
// }

// template<class Type>
// typename Deque<Type>::Iterator Deque<Type>::end() {
// 	return _elems.end();
// }

// template<class Type>
// const typename Deque<Type>::Iterator Deque<Type>::end() const {
// 	return _elems.end();
// }

// template<class Type>
// void Deque<Type>::_extend_if_full() {
// 	if (size() >= capacity())
// 		reserve(capacity() + capacity() / 2 + 1);
// }

// template<class Type>
// const size_t Deque<Type>::_get_true_index(const size_t& index) const {
// 	return (_front + index) % capacity();
// }

// template<class Type>
// size_t& Deque<Type>::_circular_increment(size_t& offset) {
// 	return offset = (offset == capacity() - 1) ? 0 : ++offset;
// }

// template<class Type>
// size_t& Deque<Type>::_circular_decrement(size_t& offset) {
// 	return offset = (offset == 0) ? capacity() - 1 : --offset;
// }
// // END Deque Template

// CUSTOM_END



#pragma once
#include "Common.h"
#include "Allocator.h"
#include <cassert>


CUSTOM_BEGIN

// Headings =================================================================================

template<class Type>
struct DequeIterationData {		// Data used for iterating Vector
	Type* _Begin		= nullptr;
	Type* _End			= nullptr;
	Type* _Base 		= nullptr;
	size_t _Capacity 	= 0;

	DequeIterationData() = default;
	~DequeIterationData();
};

template<class Deque>
class DequeIterator			// Deque Iterator
{
public:
	using ValueType = typename Deque::ValueType;
	using IterType 	= typename Deque::IterType;
	using Data 		= DequeIterationData<IterType>;

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
	using ValueType = Type;									// Type for stored values
	using IterType 	= ValueType;							// Type for iteration (same as value)
	using Alloc 	= Allocator<ValueType>;					// Allocator type
	using Iterator 	= DequeIterator<Deque<ValueType>>;		// Iterator type
	using Data		= typename Iterator::Data;				// Iteration data

private:
	size_t _size = 0;										// Number of components held by this
	size_t _capacity = 0;									// Allocated momory of type ValueType
	size_t _front = 0;										// Indicator of first elem (begin())
	size_t _back = 0;										// Indicator of last elem (end() - 1)
	ValueType* _array = nullptr;							// Actual container array

	mutable Alloc _alloc;									// Allocator
	mutable Data _data;										// Stores the ends of the array + others

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
	void print_details();

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

	static size_t& circular_increment(size_t& index, const size_t& capacity);	// increment and return index
	static size_t& circular_decrement(size_t& index, const size_t& capacity);	// decrement and return index

private:
	// Others

	ValueType* _alloc_array(const size_t& capacity);							// Allocate memory +1 null term
	void _dealloc_array();														// Deallocate memory +1 null term
	void _destroy_array();

	void _copy(const Deque& other);												// Generic copy function for vector
	void _move(Deque&& other);													// Generic move function for vector
	void _extend_if_full();														// Reserve 50% more capacity when full
	void _clean_up_array();

	const size_t _get_true_index(const size_t& index) const;
	Data* _update_iteration_data(const size_t& index) const;										// Update the data used in Iterator
}; // END Deque Template



// Definitions =================================================================================


// Deque Iterator
template<class Type>
DequeIterationData<Type>::~DequeIterationData() {
	_Begin 	= nullptr;
	_End 	= nullptr;
	_Base	= nullptr;
}

template<class Deque>
DequeIterator<Deque>::DequeIterator(IterType* ptr, Data* data)
	:_Ptr(ptr), _Data(data) { /*Empty*/ }

template<class Deque>
DequeIterator<Deque>::~DequeIterator() {
	_Ptr 	= nullptr;
	_Data 	= nullptr;
}

template<class Deque>
DequeIterator<Deque>& DequeIterator<Deque>::operator++() {
	if (is_end())
		throw std::out_of_range("Cannot increment end iterator...");

	size_t index = get_index();
	_Ptr = _Data->_Base + Deque::circular_increment(index, _Data->_Capacity);

	return *this;
}

template<class Deque>
DequeIterator<Deque> DequeIterator<Deque>::operator++(int) {
	DequeIterator temp = *this;
	++(*this);
	return temp;
}

template<class Deque>
DequeIterator<Deque>& DequeIterator<Deque>::operator+=(const size_t& diff) {

}

template<class Deque>
DequeIterator<Deque> DequeIterator<Deque>::operator+(const size_t& diff) const {

}

template<class Deque>
DequeIterator<Deque>& DequeIterator<Deque>::operator--() {			// TODO: check
	if (is_begin())
		throw std::out_of_range("Cannot decrement begin iterator...");

	size_t index = get_index();
	_Ptr = _Data->_Base + Deque::circular_decrement(index, _Data->_Capacity);

	return *this;
}

template<class Deque>
DequeIterator<Deque> DequeIterator<Deque>::operator--(int) {
	DequeIterator temp = *this;
	--(*this);
	return temp;
}

template<class Deque>
DequeIterator<Deque>& DequeIterator<Deque>::operator-=(const size_t& diff) {

}

template<class Deque>
DequeIterator<Deque> DequeIterator<Deque>::operator-(const size_t& diff) const {

}

template<class Deque>
typename DequeIterator<Deque>::IterType* DequeIterator<Deque>::operator->() {
	if (is_end())
		throw std::out_of_range("Cannot access end iterator...");

	return _Ptr;
}

template<class Deque>
typename DequeIterator<Deque>::ValueType& DequeIterator<Deque>::operator*() {
	if (is_end())
		throw std::out_of_range("Cannot dereference end iterator...");

	return *_Ptr;
}

template<class Deque>
bool DequeIterator<Deque>::operator==(const DequeIterator& other) const {
	return _Ptr == other._Ptr;
}

template<class Deque>
bool DequeIterator<Deque>::operator!=(const DequeIterator& other) const {
	return !(*this == other);
}

template<class Deque>
const size_t DequeIterator<Deque>::get_index() const {
	return static_cast<size_t>(_Ptr - _Data->_Base);
}

template<class Deque>
const bool DequeIterator<Deque>::is_begin() const {
	return _Ptr == _Data->_Begin;
}

template<class Deque>
const bool DequeIterator<Deque>::is_end() const {
	return _Ptr == _Data->_End;
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
void Deque<Type>::reserve(const size_t& newCapacity) {
	ValueType* newArray = _alloc_array(newCapacity);

	if (newCapacity < _size)
		_size = newCapacity;
	if (_size > 0)
	{
		for (size_t i = 0; i < _size; i++)
		{
			_alloc.construct(&newArray[i], std::move(_array[_front]));
			circular_increment(_front, _capacity);
		}

		_front = 0;
		_back = (_size == 0) ? 0 : _size - 1;		// TODO: check back = -1
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
	_back = (_size == 0) ? 0 : _size - 1;		// TODO: check back = -1

	_array = _alloc_array(_capacity);
	_alloc.construct_range(_array, _capacity);
}

template<class Type>
void Deque<Type>::realloc(const size_t& newCapacity, const ValueType& copyValue) {
	_clean_up_array();

	_capacity = newCapacity;
	_size = _capacity;
	_front = 0;
	_back = (_size == 0) ? 0 : _size - 1;

	_array = _alloc.alloc(_capacity);
	_alloc.construct_range(_array, _capacity, copyValue);
}

template<class Type>
template<class... Args>
void Deque<Type>::emplace_back(Args&&... args) {
	_extend_if_full();

	if(!empty())
		circular_increment(_back, _capacity);

	_alloc.construct(&_array[_back], std::forward<Args>(args)...);
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
		circular_decrement(_back, _capacity);
		--_size;
	}
}

template<class Type>
template<class... Args>
void Deque<Type>::emplace_front(Args&&... args) {
	_extend_if_full();

	if(!empty())
		circular_decrement(_front, _capacity);

	_alloc.construct(&_array[_front], std::forward<Args>(args)...);
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
		circular_increment(_front, _capacity);
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
	_destroy_array(_array, _size, _capacity);
	_size = 0;
}

template<class Type>
bool Deque<Type>::empty() const {
	return _size == 0;
}

template<class Type>
void Deque<Type>::print_details() {
	std::cout << "Size= " << _size << '\n';
	std::cout << "Capacity= " << _capacity << '\n';
	std::cout << "Front= " << _front << '\n';
	std::cout << "Back= " << _back << '\n';
	std::cout << "Address= " << _array << '\n';

	for (size_t i = 0; i <= _capacity; i++)
		std::cout << _array[i] << ' ';

	std::cout << "\n\n";
}

template<class Type>
const typename Deque<Type>::ValueType& Deque<Type>::at(const size_t& index) const {
	if (index < 0 || index >= _size)
		throw std::out_of_range("Invalid Index...");

	return _array[_get_true_index(index)];
}

template<class Type>
typename Deque<Type>::ValueType& Deque<Type>::at(const size_t& index) {
	if (index < 0 || index >= _size)
		throw std::out_of_range("Invalid Index...");

	return _array[_get_true_index(index)];
}

template<class Type>
const typename Deque<Type>::ValueType& Deque<Type>::operator[](const size_t& index) const {
	assert(!(index < 0 || index >= _size));
	return _array[_get_true_index(index)];
}

template<class Type>
typename Deque<Type>::ValueType& Deque<Type>::operator[](const size_t& index) {
	assert(!(index < 0 || index >= _size));
	return _array[_get_true_index(index)];
}

template<class Type>
typename Deque<Type>::Iterator Deque<Type>::begin() {
	return Iterator(_array + _front, _update_iteration_data(_front));
}

template<class Type>
const typename Deque<Type>::Iterator Deque<Type>::begin() const {
	return Iterator(_array + _front, _update_iteration_data(_front));
}

template<class Type>
typename Deque<Type>::Iterator Deque<Type>::end() {
	return Iterator(_array + _back + 1, _update_iteration_data(_back + 1));
}

template<class Type>
const typename Deque<Type>::Iterator Deque<Type>::end() const {
	return Iterator(_array + _back + 1, _update_iteration_data(_back + 1));
}

template<class Type>
size_t& Deque<Type>::circular_increment(size_t& index, const size_t& capacity) {
	return index = (index == capacity) ? 0 : index + 1;
}

template<class Type>
size_t& Deque<Type>::circular_decrement(size_t& index, const size_t& capacity) {
	return index = (index == 0) ? capacity : index - 1;
}

template<class Type>
typename Deque<Type>::ValueType* Deque<Type>::_alloc_array(const size_t& capacity) {
	return _alloc.alloc(capacity + 1);
}

template<class Type>
void Deque<Type>::_dealloc_array() {
	_alloc.dealloc(_array, _capacity + 1);
}

template<class Type>
void Deque<Type>::_destroy_array() {
	for (size_t i = _front; i != _back + 1; circular_increment(i, _capacity))
		_alloc.destroy(_array + i);
}

template<class Type>
void Deque<Type>::_extend_if_full() {
	if (_size >= _capacity)
		reserve(_capacity + _capacity / 2 + 1);
}

template<class Type>
void Deque<Type>::_clean_up_array()
{
	if (_array != nullptr)
	{
		_destroy_array();
		_dealloc_array();
		_array = nullptr;
	}
}

template<class Type>
const size_t Deque<Type>::_get_true_index(const size_t& index) const {
	return (_front + index) % (_capacity + 1);
}

template<class Type>
typename Deque<Type>::Data* Deque<Type>::_update_iteration_data(const size_t& index) const {
	_data._Begin = _array + _front;
	_data._End = _array + _back + 1;
	_data._Base = _array;
	_data._Capacity = _capacity;

	return &_data;
}
// END Deque Template

CUSTOM_END