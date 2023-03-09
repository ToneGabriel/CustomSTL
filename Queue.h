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
class PriorityQueue		// Priority Queue Template implemented as array heap (probably)
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

	ValueType& front();
	const ValueType& front() const;

	const size_t size() const;								// Get size
	bool empty() const;										// Check if list is empty
	void clear();											// Remove ALL components

public:
	// Operators

	PriorityQueue& operator=(const PriorityQueue& other);					// Assign operator using reference
	PriorityQueue& operator=(PriorityQueue&& other) noexcept;				// Assign operator using temporary

	bool operator==(const PriorityQueue& other) const;
	bool operator!=(const PriorityQueue& other) const;

private:
	// Helpers
	void _heap_push();
	void _heap_pop();
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
	_heap_push();
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
	_heap_pop();
	_baseContainer.pop_back();
}

template<class Type, class Compare>
typename PriorityQueue<Type, Compare>::ValueType& PriorityQueue<Type, Compare>::front() {
	return _baseContainer.front();
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
void PriorityQueue<Type, Compare>::_heap_push() {	// TODO: implement
	if (_baseContainer.size() == 0)
		return;

	size_t i 		= _baseContainer.size() - 1;
	size_t parent 	= static_cast<size_t>(i/2);  

	while (i > 0 && _less(_baseContainer[parent], _baseContainer[i]))
	{
		std::swap(_baseContainer[parent], _baseContainer[i]);
		i = parent;
		parent = static_cast<size_t>(i/2);
	}
}

template<class Type, class Compare>
void PriorityQueue<Type, Compare>::_heap_pop() {	// TODO: implement
	
}
// END PriorityQueue Template

CUSTOM_END