#pragma once
#include "Vector.h"
#include "Utility.h"


CUSTOM_BEGIN

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

	Stack(const Stack& other)
		: _baseContainer(other._baseContainer) { /*Empty*/ }

	Stack(Stack&& other)
		: _baseContainer(custom::move(other._baseContainer)) { /*Empty*/ }

	~Stack() = default;

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

	const ValueType& top() const {
		return _baseContainer.back();
	}

	ValueType& top() {
		return _baseContainer.back();
	}

	const size_t size() const {
		return _baseContainer.size();
	}

	bool empty() const {
		return _baseContainer.empty();
	}

	void clear() {
		_baseContainer.clear();
	}

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

	bool operator==(const Stack& other) const {
		return _baseContainer == other._baseContainer;
	}

	bool operator!=(const Stack& other) const {
		return !(*this == other);
	}
}; // END Stack Template

CUSTOM_END