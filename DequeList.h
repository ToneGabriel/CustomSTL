#pragma once
#include "Common.h"
#include "Allocator.h"
#include "List.h"
#include "Vector.h"
#include <cassert>


CUSTOM_BEGIN

// Headings =================================================================================

template<class Type>
class Deque					// Deque Template implemented as circular array
{
public:
	using ValueType = Type;									// Type for stored values
	using IterList	= List<ValueType>;						// List of ValueType used for iterating
	using Node		= typename IterList::Node;				// Node component from List
	using Array		= Vector<Node*>;						// Array of Node* used for quick access
	using Iterator	= typename List<ValueType>::Iterator;	// Iterator for this container (identical to List iterator)

private:
	size_t _front	= 0;									// Indicator of first elem (begin())
	size_t _back	= 0;									// Indicator of last elem (end() - 1)
	Array _array;											// Circular array used for quick access
	IterList _elems;										// Used to iterate through container

public:
	// Constructors

	Deque();														// Default Constructor
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

private:
	// Others
	
	void _copy(const Deque& other);												// Generic copy function for vector
	void _move(Deque&& other);													// Generic move function for vector
	void _extend_if_full();														// Reserve 50% more capacity when full
	void _clean_up_array();

	const size_t _get_true_index(const size_t& index) const;
	size_t _increment_offset(const size_t& offset);
	size_t _decrement_offset(const size_t& offset);
}; // END Deque Template



// Definitions =================================================================================

// Deque Template

template<class Type>
Deque<Type>::Deque() {
	_array.resize(12, nullptr);
}

template<class Type>
Deque<Type>::Deque(const size_t& newCapacity, const ValueType& copyValue) {
	_array.resize(newCapacity);
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
	clear();	// TODO: check
}

template<class Type>
void Deque<Type>::reserve(const size_t& newCapacity) {
	
}

template<class Type>
void Deque<Type>::shrink_to_fit() {

}

template<class Type>
void Deque<Type>::realloc(const size_t& newCapacity) {
	
}

template<class Type>
void Deque<Type>::realloc(const size_t& newCapacity, const ValueType& copyValue) {
	
}

template<class Type>
template<class... Args>
void Deque<Type>::emplace_back(Args&&... args) {
	Node* newNode = new Node(std::forward<Args>(args)...);

	if(!empty())
		_back = _increment_offset(_back);

	_array[_back] = newNode;
	_elems._insert_node_before(_elems._head, newNode);
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
	_elems.pop_back();
	_array[_back] = nullptr;
	_back = _decrement_offset(_back);
}

template<class Type>
template<class... Args>
void Deque<Type>::emplace_front(Args&&... args) {
	Node* newNode = new Node(std::forward<Args>(args)...);

	if(!empty())
		_front = _decrement_offset(_front);

	_array[_front] = newNode;
	_elems._insert_node_before(_elems._head->Next, newNode);
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
	_elems.pop_front();
	_array[_front] = nullptr;
	_front = _increment_offset(_front);
}

template<class Type>
typename Deque<Type>::ValueType& Deque<Type>::front() {
	return _elems.front();		// _array[_front]->Value;
}

template<class Type>
const typename Deque<Type>::ValueType& Deque<Type>::front() const {
	return _elems.front();
}

template<class Type>
typename Deque<Type>::ValueType& Deque<Type>::back() {
	return _elems.back();		// _array[_back]->Value;
}

template<class Type>
const typename Deque<Type>::ValueType& Deque<Type>::back() const {
	return _elems.back();
}

template<class Type>
const size_t Deque<Type>::capacity() const {
	return _array.capacity();
}

template<class Type>
const size_t Deque<Type>::size() const {
	return _elems.size();
}

template<class Type>
void Deque<Type>::clear() {
	_elems.clear();
	_front = _back = 0;
	//_array.clear();		// TODO: _array??
}

template<class Type>
bool Deque<Type>::empty() const {
	return _elems.empty();
}

template<class Type>
void Deque<Type>::print_details() {
	std::cout << "Size= " << size() << '\n';
	std::cout << "Capacity= " << capacity() << '\n';
	std::cout << "Front= " << _front << '\n';
	std::cout << "Back= " << _back << '\n';

	for (size_t i = 0; i < capacity(); i++)
		if(_array[i] == nullptr)
			std::cout << "N ";
		else
			std::cout << _array[i]->Value << ' ';
	
	std::cout << "\n\n";
}

template<class Type>
const typename Deque<Type>::ValueType& Deque<Type>::at(const size_t& index) const {
	if (index < 0 || index >= size())
		throw std::out_of_range("Invalid Index...");

	return _array[_get_true_index(index)]->Value;
}

template<class Type>
typename Deque<Type>::ValueType& Deque<Type>::at(const size_t& index) {
	if (index < 0 || index >= size())
		throw std::out_of_range("Invalid Index...");

	return _array[_get_true_index(index)]->Value;
}

template<class Type>
const typename Deque<Type>::ValueType& Deque<Type>::operator[](const size_t& index) const {
	assert(!(index < 0 || index >= size()));
	return _array[_get_true_index(index)]->Value;
}

template<class Type>
typename Deque<Type>::ValueType& Deque<Type>::operator[](const size_t& index) {
	assert(!(index < 0 || index >= size()));
	return _array[_get_true_index(index)]->Value;
}

template<class Type>
Deque<Type>& Deque<Type>::operator=(const Deque& other) {
	if (_elems._head != other._elems._head)
	{
		_elems = other._elems;
		//_force_rehash(other.bucket_count());
	}

	return *this;
}

template<class Type>
Deque<Type>& Deque<Type>::operator=(Deque&& other) noexcept {
	if (_elems._head != other._elems._head)
	{
		_elems = std::move(other._elems);
		//_force_rehash(other.bucket_count());
	}

	return *this;
}

template<class Type>
bool Deque<Type>::operator==(const Deque& other) const {
	return _elems == other._elems;
}

template<class Type>
bool Deque<Type>::operator!=(const Deque& other) const {
	return !operator==(other);
}

template<class Type>
typename Deque<Type>::Iterator Deque<Type>::begin() {
	return _elems.begin();
}

template<class Type>
const typename Deque<Type>::Iterator Deque<Type>::begin() const {
	return _elems.begin();
}

template<class Type>
typename Deque<Type>::Iterator Deque<Type>::end() {
	return _elems.end();
}

template<class Type>
const typename Deque<Type>::Iterator Deque<Type>::end() const {
	return _elems.end();
}

template<class Type>
void Deque<Type>::_extend_if_full() {

}

template<class Type>
const size_t Deque<Type>::_get_true_index(const size_t& index) const {
	return (_front + index) % capacity();
}

template<class Type>
size_t Deque<Type>::_increment_offset(const size_t& offset) {
	return (offset == capacity() - 1) ? 0 : offset + 1;
}

template<class Type>
size_t Deque<Type>::_decrement_offset(const size_t& offset) {
	return (offset == 0) ? capacity() - 1 : offset - 1;
}
// END Deque Template

CUSTOM_END