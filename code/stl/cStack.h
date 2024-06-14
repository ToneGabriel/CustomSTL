#pragma once
#include "c_vector.h"
#include "c_utility.h"


CUSTOM_BEGIN

template<class Type, class Container = custom::vector<Type>>
class Stack				// Stack Template implemented as vector wrapper
{
// The container must satisfy the requirements of SequenceContainer.
// Additionally, it must provide the following functions with the usual semantics:
// back(), push_back() (... or emplace_back()), pop_back()
// custom::vector, custom::Deque and custom::List satisfy these requirements

public:
	using ContainerType 	= Container;
	using ValueType			= typename ContainerType::ValueType;
	using DifferenceType 	= typename ContainerType::DifferenceType;
	using reference			= typename ContainerType::reference;
	using const_reference	= typename ContainerType::const_reference;
	using pointer			= typename ContainerType::pointer;
	using const_pointer		= typename ContainerType::const_pointer;

private:
	ContainerType _baseContainer;

public:
	// Constructors

	Stack() = default;

	Stack(const Stack& other)
		: _baseContainer(other._baseContainer) { /*Empty*/ }

	Stack(Stack&& other) noexcept
		: _baseContainer(custom::move(other._baseContainer)) { /*Empty*/ }

	~Stack() noexcept = default;

public:
	// Operators

	Stack& operator=(const Stack& other) {
		_baseContainer = other._baseContainer;
		return *this;
	}

	Stack& operator=(Stack&& other) noexcept {
		_baseContainer = custom::move(other._baseContainer);
		return *this;
	}

	template<class _Type, class _Container>
	friend bool operator==(const Stack<_Type, _Container>& left, const Stack<_Type, _Container>& right);

public:
	// Main Functions

	template<class... Args>
	void emplace(Args&&... args) {
		_baseContainer.emplace_back(custom::forward<Args>(args)...);
	}

	void push(const ValueType& copyValue) {
		emplace(copyValue);
	}

	void push(ValueType&& moveValue) {
		emplace(custom::move(moveValue));
	}

	void pop() {											// Remove first elem from stack
		_baseContainer.pop_back();
	}

	void clear() {
		_baseContainer.clear();
	}

	const_reference top() const noexcept {
		return _baseContainer.back();
	}

	reference top() noexcept {
		return _baseContainer.back();
	}

	size_t size() const noexcept {
		return _baseContainer.size();
	}

	bool empty() const noexcept {
		return _baseContainer.empty();
	}
}; // END Stack Template


// Stack binary operators
template<class _Type, class _Container>
bool operator==(const Stack<_Type, _Container>& left, const Stack<_Type, _Container>& right) {
	return left._baseContainer == right._baseContainer;
}

template<class _Type, class _Container>
bool operator!=(const Stack<_Type, _Container>& left, const Stack<_Type, _Container>& right) {
	return !(left == right);
}

CUSTOM_END