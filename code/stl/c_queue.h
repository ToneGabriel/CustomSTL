#pragma once
// #include "Deque.h"	// can be used as Container in queue/priority_queue
#include "c_list.h"
#include "c_vector.h"
#include "c_utility.h"
#include "c_functional.h"	// for custom::Less


CUSTOM_BEGIN

template<class Type, class Container = custom::list<Type>>
class queue			// queue template implemented as SequenceContainer wrapper
{
// The Container must satisfy the requirements of SequenceContainer.
// Additionally, it must provide the following functions with the usual semantics:
// back(), front(), push_back() (... or emplace_back()), pop_front()
// custom::Deque and custom::list satisfy these requirements

public:
	using container_type 	= Container;
	using value_type 		= typename container_type::value_type;
	using difference_type 	= typename container_type::difference_type;
	using reference			= typename container_type::reference;
	using const_reference	= typename container_type::const_reference;
	using pointer			= typename container_type::pointer;
	using const_pointer		= typename container_type::const_pointer;

private:
	container_type _baseContainer;

public:
	// Constructors

	queue() = default;

	queue(const queue& other)								// Copy Constructor
	: _baseContainer(other._baseContainer) { /*Empty*/ }

	queue(queue&& other) noexcept							// Move Constructor
		: _baseContainer(custom::move(other._baseContainer)) { /*Empty*/ }

	~queue() noexcept = default;							// Destructor

public:
	// Operators

	queue& operator=(const queue& other) {					// Assign operator using reference
		_baseContainer = other._baseContainer;
		return *this;
	}

	queue& operator=(queue&& other) noexcept {				// Assign operator using temporary
		_baseContainer = custom::move(other._baseContainer);
		return *this;
	}

	template<class _Type, class _Container>
	friend bool operator==(const queue<_Type, _Container>& left, const queue<_Type, _Container>& right);

public:
	// Main functions

	template<class... Args>
	void emplace(Args&&... args) {							// Construct object using arguments (Args) and add it to the tail
		_baseContainer.emplace_back(custom::forward<Args>(args)...);
	}

	void push(const value_type& copyValue) {
		emplace(copyValue);
	}

	void push(value_type&& moveValue) {
		emplace(custom::move(moveValue));
	}

	void pop() {											// Remove first elem from queue
		_baseContainer.pop_front();
	}

	void clear() {								// Remove ALL components
		return _baseContainer.clear();
	}

	reference front() noexcept {
		return _baseContainer.front();
	}

	const_reference front() const noexcept {
		return _baseContainer.front();
	}

	reference back() noexcept {
		return _baseContainer.back();
	}

	const_reference back() const noexcept {
		return _baseContainer.back();
	}

	size_t size() const noexcept {				// Get size
		return _baseContainer.size();
	}

	bool empty() const noexcept {					// Check if list is empty
		return _baseContainer.empty();
	}
}; // END queue template


// queue binary operators
template<class _Type, class _Container>
bool operator==(const queue<_Type, _Container>& left, const queue<_Type, _Container>& right) {
	return left._baseContainer == right._baseContainer;
}

template<class _Type, class _Container>
bool operator!=(const queue<_Type, _Container>& left, const queue<_Type, _Container>& right) {
	return !(left == right);
}


template<class Type, class Container = custom::vector<Type>, class Compare = custom::less<Type>>
class priority_queue		// Priority queue Template implemented as array heap
{
// The Container must satisfy the requirements of SequenceContainer,
// and its iterators must satisfy the requirements of LegacyRandomAccessIterator.
// Additionally, it must provide the following functions with the usual semantics:
// front(), push_back() (... or emplace_back()), pop_back()
// custom::vector and custom::Deque satisfy these requirements

public:
	using container_type 	= Container;
	using value_compare		= Compare;
	using value_type 		= typename container_type::value_type;
	using difference_type 	= typename container_type::difference_type;
	using reference			= typename container_type::reference;
	using const_reference	= typename container_type::const_reference;
	using pointer			= typename container_type::pointer;
	using const_pointer		= typename container_type::const_pointer;

private:
	container_type _baseContainer;
	value_compare _less;

public:
	// Constructors

	priority_queue() = default;

	priority_queue(const priority_queue& other)
		: _baseContainer(other._baseContainer) { /*Empty*/ }

	priority_queue(priority_queue&& other) noexcept
		: _baseContainer(custom::move(other._baseContainer)) { /*Empty*/ }

	~priority_queue() noexcept = default;

public:
	// Operators

	priority_queue& operator=(const priority_queue& other) {
		_baseContainer = other._baseContainer;
		return *this;
	}

	priority_queue& operator=(priority_queue&& other) noexcept {
		_baseContainer = custom::move(other._baseContainer);
		return *this;
	}

	template<class _Type, class _Container, class _Compare>
	friend bool operator==(	const priority_queue<_Type, _Container, _Compare>& left,
							const priority_queue<_Type, _Container, _Compare>& right);

public:
	// Main Functions

	template<class... Args>
	void emplace(Args&&... args) {
		_baseContainer.emplace_back(custom::forward<Args>(args)...);
		_heap_push_adjust();
	}

	void push(const value_type& copyValue) {
		emplace(copyValue);
	}

	void push(value_type&& moveValue) {
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

	reference front() noexcept {
		return _baseContainer.front();
	}

	const_reference front() const noexcept {
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
	void _print_graph(const size_t ident, const size_t index, const custom::String& rlFlag) const {
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

	size_t _heap_get_max_child(const size_t index) {
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

	size_t _heap_get_parent(const size_t index) {
		if (index > 0)
			return static_cast<size_t>((index + 1) / 2) - 1;

		return index;	// 0 (root)
	}
}; // END priority_queue Template


// priority_queue binary operators
template<class _Type, class _Container, class _Compare>
bool operator==(const priority_queue<_Type, _Container, _Compare>& left,
				const priority_queue<_Type, _Container, _Compare>& right) {
	return left._baseContainer == right._baseContainer;
}

template<class _Type, class _Container, class _Compare>
bool operator!=(const priority_queue<_Type, _Container, _Compare>& left,
				const priority_queue<_Type, _Container, _Compare>& right) {
	return !(left == right);
}

CUSTOM_END