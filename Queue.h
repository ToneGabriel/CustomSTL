#pragma once
#include "Deque.h"
#include "Vector.h"
#include "Utility.h"


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


template<class Type>
class PriorityQueue		// Priority Queue Template implemented as array heap (probably)
{
	// TODO: implement

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

// END PriorityQueue Template
CUSTOM_END