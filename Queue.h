#pragma once
#include "Deque.h"
#include "Vector.h"
#include "Utility.h"

#include <functional>	// std::less


CUSTOM_BEGIN

// Headings =================================================================================

template<class Type>
class Queue			// Queue template implemented as Deque wrapper
{
public:
	using ValueType = Type;									// Type for stored values

private:
	Deque<ValueType> _baseContainer;

public:
	// Constructors

	Queue() = default;
	Queue(const Queue& other);								// Copy Constructor
	Queue(Queue&& other) noexcept;							// Move Constructor
	~Queue() = default;										// Destructor

public:
	// Main functions

	template<class... Args>
	void emplace(Args&&... args);							// Construct object using arguments (Args) and add it to the tail
	void push(const ValueType& copyValue);
	void push(ValueType&& moveValue);
	void pop();												// Remove first elem from queue

	ValueType& front();
	const ValueType& front() const;
	ValueType& back();
	const ValueType& back() const;

	const size_t size() const;								// Get size
	bool empty() const;										// Check if list is empty
	void clear();											// Remove ALL components

public:
	// Operators

	Queue& operator=(const Queue& other);					// Assign operator using reference
	Queue& operator=(Queue&& other) noexcept;				// Assign operator using temporary

	bool operator==(const Queue& other) const;
	bool operator!=(const Queue& other) const;
}; // END Queue Template


template<class Type, class Compare = std::less<Type>>
class PriorityQueue		// Priority Queue Template implemented as array heap
{
public:
	using ValueType 	= Type;									// Type for stored values
	using ValueCompare 	= Compare;

private:
	Vector<ValueType> _baseContainer;
	ValueCompare _less;

public:
	// Constructors

	PriorityQueue() = default;
	PriorityQueue(const PriorityQueue& other);
	PriorityQueue(PriorityQueue&& other) noexcept;
	~PriorityQueue() = default;

public:
	// Main Functions

	template<class... Args>
	void emplace(Args&&... args);
	void push(const ValueType& copyValue);
	void push(ValueType&& moveValue);
	void pop();												// Remove first elem from queue

	const ValueType& front() const;
	const size_t size() const;								// Get size
	bool empty() const;										// Check if container is empty
	void clear();											// Remove ALL components

	void print_details();

public:
	// Operators

	PriorityQueue& operator=(const PriorityQueue& other);					// Assign operator using reference
	PriorityQueue& operator=(PriorityQueue&& other) noexcept;				// Assign operator using temporary

	bool operator==(const PriorityQueue& other) const;
	bool operator!=(const PriorityQueue& other) const;

private:
	// Helpers
	void _print_graph(const size_t& ident, const size_t& index, const custom::String& rlFlag) const;
	void _heap_push_adjust();								// Adjust from last elem
	void _heap_pop_adjust();								// Adjust from first elem
	size_t _heap_get_max_child(const size_t& index);
	size_t _heap_get_parent(const size_t& index);
}; // END PriorityQueue Template



// Definitions =================================================================================

// Queue Template
template<class Type>
Queue<Type>::Queue(const Queue& other)
	: _baseContainer(other._baseContainer) { /*Empty*/ }

template<class Type>
Queue<Type>::Queue(Queue&& other) noexcept 
	: _baseContainer(custom::move(other._baseContainer)) { /*Empty*/ }

template<class Type>
template<class... Args>
void Queue<Type>::emplace(Args&&... args) {
	_baseContainer.emplace_back(custom::forward<Args>(args)...);
}

template<class Type>
void Queue<Type>::push(const ValueType& copyValue) {
	emplace(copyValue);
}

template<class Type>
void Queue<Type>::push(ValueType&& moveValue) {
	emplace(custom::move(moveValue));
}

template<class Type>
void Queue<Type>::pop() {
	_baseContainer.pop_front();
}

template<class Type>
typename Queue<Type>::ValueType& Queue<Type>::front() {
	return _baseContainer.front();
}

template<class Type>
const typename Queue<Type>::ValueType& Queue<Type>::front() const {
	return _baseContainer.front();
}

template<class Type>
typename Queue<Type>::ValueType& Queue<Type>::back() {
	return _baseContainer.back();
}

template<class Type>
const typename Queue<Type>::ValueType& Queue<Type>::back() const {
	return _baseContainer.back();
}

template<class Type>
const size_t Queue<Type>::size() const {
	return _baseContainer.size();
}

template<class Type>
bool Queue<Type>::empty() const {
	return _baseContainer.empty();
}

template<class Type>
void Queue<Type>::clear() {
	return _baseContainer.clear();
}

template<class Type>
Queue<Type>& Queue<Type>::operator=(const Queue& other) {
	_baseContainer = other._baseContainer;
	return *this;
}

template<class Type>
Queue<Type>& Queue<Type>::operator=(Queue&& other) noexcept {
	_baseContainer = custom::move(other._baseContainer);
	return *this;
}

template<class Type>
bool Queue<Type>::operator==(const Queue& other) const {
	return _baseContainer == other._baseContainer;
}

template<class Type>
bool Queue<Type>::operator!=(const Queue& other) const {
	return !(*this == other);
}
// END Queue Template


// PriorityQueue Template
template<class Type, class Compare>
PriorityQueue<Type, Compare>::PriorityQueue(const PriorityQueue& other)
	: _baseContainer(other._baseContainer) { /*Empty*/ }

template<class Type, class Compare>
PriorityQueue<Type, Compare>::PriorityQueue(PriorityQueue&& other) noexcept 
	: _baseContainer(custom::move(other._baseContainer)) { /*Empty*/ }

template<class Type, class Compare>
template<class... Args>
void PriorityQueue<Type, Compare>::emplace(Args&&... args) {
	_baseContainer.emplace_back(custom::forward<Args>(args)...);
	_heap_push_adjust();
}

template<class Type, class Compare>
void PriorityQueue<Type, Compare>::push(const ValueType& copyValue) {
	emplace(copyValue);
}

template<class Type, class Compare>
void PriorityQueue<Type, Compare>::push(ValueType&& moveValue) {
	emplace(custom::move(moveValue));
}

template<class Type, class Compare>
void PriorityQueue<Type, Compare>::pop() {
	if (_baseContainer.size() > 0)
	{
		std::swap(_baseContainer[0], _baseContainer[_baseContainer.size() - 1]);
		_baseContainer.pop_back();
		_heap_pop_adjust();
	}
}

template<class Type, class Compare>
const typename PriorityQueue<Type, Compare>::ValueType& PriorityQueue<Type, Compare>::front() const {
	return _baseContainer.front();
}

template<class Type, class Compare>
const size_t PriorityQueue<Type, Compare>::size() const {
	return _baseContainer.size();
}

template<class Type, class Compare>
bool PriorityQueue<Type, Compare>::empty() const {
	return _baseContainer.empty();
}

template<class Type, class Compare>
void PriorityQueue<Type, Compare>::clear() {
	return _baseContainer.clear();
}

template<class Type, class Compare>
void PriorityQueue<Type, Compare>::print_details() {	
	std::cout << "Size= " << size() << '\n';
	_print_graph(0, 0, "HEAD");
}

template<class Type, class Compare>
PriorityQueue<Type, Compare>& PriorityQueue<Type, Compare>::operator=(const PriorityQueue& other) {
	_baseContainer = other._baseContainer;
	return *this;
}

template<class Type, class Compare>
PriorityQueue<Type, Compare>& PriorityQueue<Type, Compare>::operator=(PriorityQueue&& other) noexcept {
	_baseContainer = custom::move(other._baseContainer);
	return *this;
}

template<class Type, class Compare>
bool PriorityQueue<Type, Compare>::operator==(const PriorityQueue& other) const {
	return _baseContainer == other._baseContainer;
}

template<class Type, class Compare>
bool PriorityQueue<Type, Compare>::operator!=(const PriorityQueue& other) const {
	return !(*this == other);
}

template<class Type, class Compare>
void PriorityQueue<Type, Compare>::_print_graph(const size_t& ident, const size_t& index, const custom::String& rlFlag) const {
	custom::String str;
	str.append(ident, '\t');

	if (index < size())
		std::cout << str << _baseContainer[index] << ' ' << rlFlag << '\n';

	if ((index + 1) * 2 - 1 < size())
		_print_graph(ident + 1, (index + 1) * 2 - 1, "LEFT");

	if ((index + 1) * 2 < size())
		_print_graph(ident + 1, (index + 1) * 2, "RIGHT");
}

template<class Type, class Compare>
void PriorityQueue<Type, Compare>::_heap_push_adjust() {
	if (size() == 0)
		return;

	size_t index	= size() - 1;
	size_t parent	= _heap_get_parent(index);

	while (index > 0 && _less(_baseContainer[parent], _baseContainer[index]))
	{
		std::swap(_baseContainer[parent], _baseContainer[index]);
		index	= parent;
		parent	= _heap_get_parent(index);
	}
}

template<class Type, class Compare>
void PriorityQueue<Type, Compare>::_heap_pop_adjust() {
	if (size() == 0)
		return;

	size_t index = 0;
	size_t maxChild;

	while (index < size())
	{
		maxChild = _heap_get_max_child(index);

		if (_less(_baseContainer[index], _baseContainer[maxChild]))
		{
			std::swap(_baseContainer[index], _baseContainer[maxChild]);
			index = maxChild;
		}
		else
			return;
	}
}

template<class Type, class Compare>
size_t PriorityQueue<Type, Compare>::_heap_get_max_child(const size_t& index) {
	size_t leftChild	= (index + 1) * 2 - 1;
	size_t rightChild	= leftChild + 1;

	if (leftChild + rightChild >= 2 * size())								// no child exists
		return index;
	else if (rightChild >= size())											// just left exists
		return leftChild;
	else if (leftChild >= size())											// just right exists
		return rightChild;
	else if (_less(_baseContainer[rightChild], _baseContainer[leftChild]))	// compare values
		return leftChild;
	else
		return rightChild;
}

template<class Type, class Compare>
size_t PriorityQueue<Type, Compare>::_heap_get_parent(const size_t& index) {
	if (index > 0)
		return static_cast<size_t>((index + 1) / 2) - 1;

	return index;	// 0 (root)
}
// END PriorityQueue Template

CUSTOM_END