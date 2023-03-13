#pragma once
#include "Vector.h"
#include "Utility.h"


CUSTOM_BEGIN

// Headings =================================================================================

template<class Type>
class Stack				// Stack Template implemented as Vector wrapper
{
public:
	using ValueType = Type;									// Type for stored values

private:
	Vector<ValueType> _baseContainer;

public:
	// Constructors

	Stack() = default;
	Stack(const Stack& other);
	Stack(Stack&& other);
	~Stack() = default;

public:
	// Main Functions

	template<class... Args>
	void emplace(Args&&... args);
	void push(const ValueType& copyValue);
	void push(ValueType&& moveValue);
	void pop();												// Remove first elem from stack

	const ValueType& top() const;
	ValueType& top();
	const size_t size() const;								// Get size
	bool empty() const;										// Check if container is empty
	void clear();											// Remove ALL components

public:
	// Operators

	Stack& operator=(const Stack& other);					// Assign operator using reference
	Stack& operator=(Stack&& other) noexcept;				// Assign operator using temporary

	bool operator==(const Stack& other) const;
	bool operator!=(const Stack& other) const;
}; // END Stack Template



// Definitions =================================================================================

// Stack Template
template<class Type>
Stack<Type>::Stack(const Stack& other)
	: _baseContainer(other._baseContainer) { /*Empty*/ }

template<class Type>
Stack<Type>::Stack(Stack&& other)
	: _baseContainer(custom::move(other._baseContainer)) { /*Empty*/ }

template<class Type>
template<class... Args>
void Stack<Type>::emplace(Args&&... args) {
	_baseContainer.emplace_back(custom::forward<Args>(args)...);
}

template<class Type>
void Stack<Type>::push(const ValueType& copyValue) {
	emplace(copyValue);
}

template<class Type>
void Stack<Type>::push(ValueType&& moveValue) {
	emplace(custom::move(moveValue));
}

template<class Type>
void Stack<Type>::pop() {
	_baseContainer.pop_back();
}

template<class Type>
const typename Stack<Type>::ValueType& Stack<Type>::top() const {
	return _baseContainer.back();
}

template<class Type>
typename Stack<Type>::ValueType& Stack<Type>::top() {
	return _baseContainer.back();
}

template<class Type>
const size_t Stack<Type>::size() const {
	return _baseContainer.size();
}

template<class Type>
bool Stack<Type>::empty() const {
	return _baseContainer.empty();
}

template<class Type>
void Stack<Type>::clear() {
	_baseContainer.clear();
}

template<class Type>
Stack<Type>& Stack<Type>::operator=(const Stack& other) {
	_baseContainer = other._baseContainer;
	return *this;
}

template<class Type>
Stack<Type>& Stack<Type>::operator=(Stack&& other) noexcept {
	_baseContainer = custom::move(other._baseContainer);
	return *this;
}

template<class Type>
bool Stack<Type>::operator==(const Stack& other) const {
	return _baseContainer == other._baseContainer;
}

template<class Type>
bool Stack<Type>::operator!=(const Stack& other) const {
	return !(*this == other);
}
// END Stack Template

CUSTOM_END