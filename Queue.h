#pragma once
#include "Common.h"
#include "Node.h"

CUSTOM_BEGIN

// Headings =================================================================================

template<class Type>
class Queue				// Queue Template
{
public:
	using ValueType = Type;									// Type for stored values
	using Node		= ForwardNode<ValueType>;				// Node type

private:
	size_t _size	= 0;									// Number of Nodes held by this
	Node* _head		= nullptr;								// Head of this list
	Node* _tail		= nullptr;								// Tail of this list

	mutable Node* _workspaceNode = nullptr;					// Auxiliary Node for work

public:
	// Constructors

	Queue() = default;
	Queue(const size_t& newSize, const ValueType& value);	// Add multiple copies Constructor
	Queue(const Queue& other);								// Copy Constructor
	Queue(Queue&& other) noexcept;							// Move Constructor
	~Queue();												// Destructor

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

private:
	// Others

	void _copy(const Queue& other);							// Generic copy function for queue
	void _move(Queue&& other);								// Generic move function for queue

	template<class... Args>
	void _create_until_size(const size_t& newSize, Args&&... args); // Add elements until current size equals newSize
}; // END Queue Template



// Definitions =================================================================================

// Queue Template
template<class Type>
Queue<Type>::Queue(const size_t& newSize, const ValueType& value) {
	_create_until_size(newSize, value);
}

template<class Type>
Queue<Type>::Queue(const Queue& other) {
	_copy(other);
}

template<class Type>
Queue<Type>::Queue(Queue&& other) noexcept {
	_move(std::move(other));
}

template<class Type>
Queue<Type>::~Queue() {
	clear();
}

template<class Type>
template<class... Args>
void Queue<Type>::emplace(Args&&... args) {
	Node* newNode = new Node(std::forward<Args>(args)...);

	if (_head == nullptr)
		_head = _tail = newNode;	// Here head and tail are not important and can be used to store value
	else {
		_tail->_Next = newNode;
		_tail = newNode;
	}
	_size++;
}

template<class Type>
void Queue<Type>::push(const ValueType& copyValue) {
	emplace(copyValue);
}

template<class Type>
void Queue<Type>::push(ValueType&& moveValue) {
	emplace(std::move(moveValue));
}

template<class Type>
void Queue<Type>::pop() {
	if (_head) {
		_workspaceNode = _head;
		_head = _head->_Next;

		if (_head == nullptr)
			_tail = nullptr;

		delete _workspaceNode;
		_size--;
	}
}

template<class Type>
typename Queue<Type>::ValueType& Queue<Type>::front() {
	return _head->_Value;
}

template<class Type>
const typename Queue<Type>::ValueType& Queue<Type>::front() const {
	return _head->_Value;
}

template<class Type>
typename Queue<Type>::ValueType& Queue<Type>::back() {
	return _tail->_Value;
}

template<class Type>
const typename Queue<Type>::ValueType& Queue<Type>::back() const {
	return _tail->_Value;
}

template<class Type>
const size_t Queue<Type>::size() const {
	return _size;
}

template<class Type>
bool Queue<Type>::empty() const {
	return _size == 0;
}

template<class Type>
void Queue<Type>::clear() {
	while (_size) {
		_workspaceNode = _head;
		_head = _head->_Next;

		delete _workspaceNode;
		_size--;
	}

	_head = _tail = nullptr;
}

template<class Type>
Queue<Type>& Queue<Type>::operator=(const Queue& other) {
	if (_head != other._head)
	{
		clear();
		_copy(other);
	}

	return *this;
}

template<class Type>
Queue<Type>& Queue<Type>::operator=(Queue&& other) noexcept {
	if (_head != other._head)
	{
		clear();
		_move(std::move(other));
	}

	return *this;
}

template<class Type>
void Queue<Type>::_copy(const Queue& other) {
	_workspaceNode = other._head;
	while (_size < other._size) {
		enqueue(_workspaceNode->_Value);
		_workspaceNode = _workspaceNode->_Next;
	}
}

template<class Type>
void Queue<Type>::_move(Queue&& other) {
	_head = other._head;
	_tail = other._tail;
	_size = other._size;

	other._head = other._tail = nullptr;
	other._size = 0;
}

template<class Type>
template<class... Args>
void Queue<Type>::_create_until_size(const size_t& newSize, Args&&... args) {
	while (_size < newSize)
		emplace(std::forward<Args>(args)...);
}
// END Queue Template

CUSTOM_END