#pragma once
#include "stl_cpp/c_vector.h"
#include "stl_cpp/c_utility.h"


CUSTOM_BEGIN

template<class Type, class Container = custom::vector<Type>>
class stack				// stack Template implemented as vector wrapper
{
// The container must satisfy the requirements of SequenceContainer.
// Additionally, it must provide the following functions with the usual semantics:
// back(), push_back() (... or emplace_back()), pop_back()
// custom::vector, custom::deque and custom::list satisfy these requirements

public:
	using container_type	= Container;
	using value_type		= typename container_type::value_type;
	using difference_type 	= typename container_type::difference_type;
	using reference			= typename container_type::reference;
	using const_reference	= typename container_type::const_reference;
	using pointer			= typename container_type::pointer;
	using const_pointer		= typename container_type::const_pointer;

private:
	container_type _baseContainer;

public:
	// Constructors

	stack() = default;

	stack(const stack& other)
		: _baseContainer(other._baseContainer) { /*Empty*/ }

	stack(stack&& other) noexcept
		: _baseContainer(custom::move(other._baseContainer)) { /*Empty*/ }

	~stack() noexcept = default;

public:
	// Operators

	stack& operator=(const stack& other)
	{
		_baseContainer = other._baseContainer;
		return *this;
	}

	stack& operator=(stack&& other) noexcept
	{
		_baseContainer = custom::move(other._baseContainer);
		return *this;
	}

	template<class _Type, class _Container>
	friend bool operator==(const stack<_Type, _Container>& left, const stack<_Type, _Container>& right);

public:
	// Main Functions

	template<class... Args>
	void emplace(Args&&... args)
	{
		_baseContainer.emplace_back(custom::forward<Args>(args)...);
	}

	void push(const value_type& copyValue)
	{
		emplace(copyValue);
	}

	void push(value_type&& moveValue)
	{
		emplace(custom::move(moveValue));
	}

	void pop()	// Remove first elem from stack
	{
		_baseContainer.pop_back();
	}

	void clear()
	{
		_baseContainer.clear();
	}

	const_reference top() const noexcept
	{
		return _baseContainer.back();
	}

	reference top() noexcept
	{
		return _baseContainer.back();
	}

	size_t size() const noexcept
	{
		return _baseContainer.size();
	}

	bool empty() const noexcept
	{
		return _baseContainer.empty();
	}
}; // END stack


// stack binary operators
template<class _Type, class _Container>
bool operator==(const stack<_Type, _Container>& left, const stack<_Type, _Container>& right)
{
	return left._baseContainer == right._baseContainer;
}

template<class _Type, class _Container>
bool operator!=(const stack<_Type, _Container>& left, const stack<_Type, _Container>& right)
{
	return !(left == right);
}

CUSTOM_END