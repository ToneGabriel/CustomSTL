#pragma once
#include "Deque.h"
#include "Vector.h"
#include "Utility.h"

#include <functional>	// std::less


CUSTOM_BEGIN

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

	Queue(const Queue& other)								// Copy Constructor
	: _baseContainer(other._baseContainer) { /*Empty*/ }

	Queue(Queue&& other) noexcept							// Move Constructor
		: _baseContainer(custom::move(other._baseContainer)) { /*Empty*/ }

	~Queue() = default;										// Destructor

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

	ValueType& front() {
		return _baseContainer.front();
	}
	const ValueType& front() const {
		return _baseContainer.front();
	}

	ValueType& back() {
		return _baseContainer.back();
	}

	const ValueType& back() const {
		return _baseContainer.back();
	}

	const size_t size() const {								// Get size
		return _baseContainer.size();
	}

	bool empty() const {									// Check if list is empty
		return _baseContainer.empty();
	}

	void clear() {											// Remove ALL components
		return _baseContainer.clear();
	}

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

	bool operator==(const Queue& other) const {
	return _baseContainer == other._baseContainer;
	}

	bool operator!=(const Queue& other) const {
		return !(*this == other);
	}
}; // END Queue template



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

	PriorityQueue(const PriorityQueue& other)
		: _baseContainer(other._baseContainer) { /*Empty*/ }

	PriorityQueue(PriorityQueue&& other) noexcept
		: _baseContainer(custom::move(other._baseContainer)) { /*Empty*/ }

	~PriorityQueue() = default;

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
			std::swap(_baseContainer[0], _baseContainer[_baseContainer.size() - 1]);
			_baseContainer.pop_back();
			_heap_pop_adjust();
		}
	}

	const ValueType& front() const {
		return _baseContainer.front();
	}

	const size_t size() const {
		return _baseContainer.size();
	}

	bool empty() const {
		return _baseContainer.empty();
	}

	void clear() {
		return _baseContainer.clear();
	}

	void print_details() {
		std::cout << "Size= " << size() << '\n';
		_print_graph(0, 0, "HEAD");
	}

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

	bool operator==(const PriorityQueue& other) const {
		return _baseContainer == other._baseContainer;
	}

	bool operator!=(const PriorityQueue& other) const {
		return !(*this == other);
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
			std::swap(_baseContainer[parent], _baseContainer[index]);
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
				std::swap(_baseContainer[index], _baseContainer[maxChild]);
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

CUSTOM_END