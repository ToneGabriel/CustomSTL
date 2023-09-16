#pragma once
#include "Deque.h"
#include "Vector.h"
#include "Utility.h"
#include "Functional.h"	// for custom::Less


CUSTOM_BEGIN

template<class Type, class Container = custom::Deque<Type>>
class Queue			// Queue template implemented as Deque wrapper
{
public:
	using ValueType 		= Type;									// Type for stored values
	using ContainerType 	= Container;
	using Reference			= typename ContainerType::Reference;
	using ConstReference	= typename ContainerType::ConstReference;
	using Pointer			= typename ContainerType::Pointer;
	using ConstPointer		= typename ContainerType::ConstPointer;

private:
	ContainerType _baseContainer;

public:
	// Constructors

	Queue() = default;

	Queue(const Queue& other)								// Copy Constructor
	: _baseContainer(other._baseContainer) { /*Empty*/ }

	Queue(Queue&& other) noexcept							// Move Constructor
		: _baseContainer(custom::move(other._baseContainer)) { /*Empty*/ }

	~Queue() noexcept = default;							// Destructor

public:
	// Operators

	Queue& operator=(const Queue& other) {					// Assign operator using reference
		_baseContainer = other._baseContainer;
		return *this;
	}

	Queue& operator=(Queue&& other) noexcept {				// Assign operator using temporary
		_baseContainer = custom::move(other._baseContainer);
		return *this;
	}

	template<class _Type, class _Container>
	friend bool operator==(const Queue<_Type, _Container>& left, const Queue<_Type, _Container>& right);

public:
	// Main functions

	template<class... Args>
	void emplace(Args&&... args) {							// Construct object using arguments (Args) and add it to the tail
		_baseContainer.emplace_back(custom::forward<Args>(args)...);
	}

	void push(const ValueType& copyValue) {
		emplace(copyValue);
	}

	void push(ValueType&& moveValue) {
		emplace(custom::move(moveValue));
	}

	void pop() {											// Remove first elem from queue
		_baseContainer.pop_front();
	}

	void clear() {								// Remove ALL components
		return _baseContainer.clear();
	}

	Reference front() noexcept {
		return _baseContainer.front();
	}

	ConstReference front() const noexcept {
		return _baseContainer.front();
	}

	Reference back() noexcept {
		return _baseContainer.back();
	}

	ConstReference back() const noexcept {
		return _baseContainer.back();
	}

	size_t size() const noexcept {				// Get size
		return _baseContainer.size();
	}

	bool empty() const noexcept {					// Check if list is empty
		return _baseContainer.empty();
	}
}; // END Queue template


// Queue binary operators
template<class _Type, class _Container>
bool operator==(const Queue<_Type, _Container>& left, const Queue<_Type, _Container>& right) {
	return left._baseContainer == right._baseContainer;
}

template<class _Type, class _Container>
bool operator!=(const Queue<_Type, _Container>& left, const Queue<_Type, _Container>& right) {
	return !(left == right);
}


template<class Type, class Container = custom::Vector<Type>, class Compare = custom::Less<Type>>
class PriorityQueue		// Priority Queue Template implemented as array heap
{
public:
	using ValueType 		= Type;									// Type for stored values
	using ContainerType 	= Container;
	using ValueCompare 		= Compare;
	using Reference			= typename ContainerType::Reference;
	using ConstReference	= typename ContainerType::ConstReference;
	using Pointer			= typename ContainerType::Pointer;
	using ConstPointer		= typename ContainerType::ConstPointer;

private:
	ContainerType _baseContainer;
	ValueCompare _less;

public:
	// Constructors

	PriorityQueue() = default;

	PriorityQueue(const PriorityQueue& other)
		: _baseContainer(other._baseContainer) { /*Empty*/ }

	PriorityQueue(PriorityQueue&& other) noexcept
		: _baseContainer(custom::move(other._baseContainer)) { /*Empty*/ }

	~PriorityQueue() noexcept = default;

public:
	// Operators

	PriorityQueue& operator=(const PriorityQueue& other) {
		_baseContainer = other._baseContainer;
		return *this;
	}

	PriorityQueue& operator=(PriorityQueue&& other) noexcept {
		_baseContainer = custom::move(other._baseContainer);
		return *this;
	}

	template<class _Type, class _Container, class _Compare>
	friend bool operator==(	const PriorityQueue<_Type, _Container, _Compare>& left,
										const PriorityQueue<_Type, _Container, _Compare>& right);

public:
	// Main Functions

	template<class... Args>
	void emplace(Args&&... args) {
		_baseContainer.emplace_back(custom::forward<Args>(args)...);
		_heap_push_adjust();
	}

	void push(const ValueType& copyValue) {
		emplace(copyValue);
	}

	void push(ValueType&& moveValue) {
		emplace(custom::move(moveValue));
	}

	void pop() {											// Remove first elem from queue
		if (_baseContainer.size() > 0)
		{
			custom::swap(_baseContainer[0], _baseContainer[_baseContainer.size() - 1]);
			_baseContainer.pop_back();
			_heap_pop_adjust();
		}
	}
	
	void clear() {
		return _baseContainer.clear();
	}

	Reference front() noexcept {
		return _baseContainer.front();
	}

	ConstReference front() const noexcept {
		return _baseContainer.front();
	}

	size_t size() const noexcept {
		return _baseContainer.size();
	}

	bool empty() const noexcept {
		return _baseContainer.empty();
	}

	void print_details() {
		std::cout << "Size= " << size() << '\n';
		_print_graph(0, 0, "HEAD");
	}

private:
	// Helpers
	void _print_graph(const size_t& ident, const size_t& index, const custom::String& rlFlag) const {
		custom::String str;
		str.append(ident, '\t');

		if (index < size())
			std::cout << str << _baseContainer[index] << ' ' << rlFlag << '\n';

		if ((index + 1) * 2 - 1 < size())
			_print_graph(ident + 1, (index + 1) * 2 - 1, "LEFT");

		if ((index + 1) * 2 < size())
			_print_graph(ident + 1, (index + 1) * 2, "RIGHT");
	}

	void _heap_push_adjust() {								// Adjust from last elem
		if (size() == 0)
			return;

		size_t index	= size() - 1;
		size_t parent	= _heap_get_parent(index);

		while (index > 0 && _less(_baseContainer[parent], _baseContainer[index]))
		{
			custom::swap(_baseContainer[parent], _baseContainer[index]);
			index	= parent;
			parent	= _heap_get_parent(index);
		}
	}

	void _heap_pop_adjust() {								// Adjust from first elem
		if (size() == 0)
			return;

		size_t index = 0;
		size_t maxChild;

		while (index < size())
		{
			maxChild = _heap_get_max_child(index);

			if (_less(_baseContainer[index], _baseContainer[maxChild]))
			{
				custom::swap(_baseContainer[index], _baseContainer[maxChild]);
				index = maxChild;
			}
			else
				return;
		}
	}

	size_t _heap_get_max_child(const size_t& index) {
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

	size_t _heap_get_parent(const size_t& index) {
		if (index > 0)
			return static_cast<size_t>((index + 1) / 2) - 1;

		return index;	// 0 (root)
	}
}; // END PriorityQueue Template


// PriorityQueue binary operators
template<class _Type, class _Container, class _Compare>
bool operator==(	const PriorityQueue<_Type, _Container, _Compare>& left,
							const PriorityQueue<_Type, _Container, _Compare>& right) {
	return left._baseContainer == right._baseContainer;
}

template<class _Type, class _Container, class _Compare>
bool operator!=(	const PriorityQueue<_Type, _Container, _Compare>& left,
							const PriorityQueue<_Type, _Container, _Compare>& right) {
	return !(left == right);
}

CUSTOM_END