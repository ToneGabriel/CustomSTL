#pragma once
#include "Node.h"
#include "Utility.h"

CUSTOM_BEGIN

template <class Traits>
class HashTable;

template<class Type>
struct ListIterationData {		// Data used for iterating List
	Type* _Begin	= nullptr;
	Type* _End		= nullptr;

	ListIterationData() = default;
	~ListIterationData() {
		_Begin	= nullptr;
		_End	= nullptr;
	}
};

template<class List>
class ListIterator		// Linked List Iterator
{
public:
	using ValueType = typename List::ValueType;
	using IterType	= typename List::IterType;
	using Data		= ListIterationData<IterType>;

	IterType* _Ptr	= nullptr;
	Data* _Data		= nullptr;

public:

	explicit ListIterator(IterType* nodePtr, Data* data)
		:_Ptr(nodePtr), _Data(data) { /*Empty*/ }

	~ListIterator() {
		_Ptr	= nullptr;
		_Data	= nullptr;
	}

	ListIterator& operator++() {
		if (_Ptr == _Data->_End)
			throw std::out_of_range("Cannot increment end iterator...");

		_Ptr = _Ptr->_Next;
		return *this;
	}

	ListIterator operator++(int) {
		ListIterator temp = *this;
		++(*this);
		return temp;
	}

	ListIterator& operator--() {
		if (_Ptr == _Data->_Begin)
			throw std::out_of_range("Cannot decrement begin iterator...");

		_Ptr = _Ptr->_Previous;
		return *this;
	}

	ListIterator operator--(int) {
		ListIterator temp = *this;
		--(*this);
		return temp;
	}

	IterType* operator->() {
		if (_Ptr == _Data->_End)
			throw std::out_of_range("Cannot access end iterator...");

		return _Ptr;
	}

	ValueType& operator*() {
		if (_Ptr == _Data->_End)
			throw std::out_of_range("Cannot dereference end iterator...");

		return _Ptr->_Value;
	}

	bool operator==(const ListIterator& other) const {
		return _Ptr == other._Ptr;
	}

	bool operator!=(const ListIterator& other) const {
		return !(*this == other);
	}

public:
	const bool is_begin() const {
		return _Ptr == _Data->_Begin;
	}

	const bool is_end() const {
		return _Ptr == _Data->_End;
	}
}; // END Linked List Iterator


template<class Type>
class List				// Doubly Linked List
{
private:
	template<class>
	friend class HashTable;

public:
	using ValueType = Type;											// Type for stored values
	using Node		= DoubleNode<ValueType>;						// Node type
	using IterType	= Node;											// Type of iterating element
	using Iterator	= ListIterator<List<ValueType>>;				// Iterator type
	using Alloc		= Allocator<Node>;								// Allocator for Node type
	using Data		= typename Iterator::Data;						// Iteration data

private:
	Alloc _alloc;													// Allocator
	size_t _size	= 0;											// Number of Nodes held by this
	Node* _head		= nullptr;										// Head of this list

	mutable Data _data;

public:
	// Constructors

	List() {
		_create_head();
	}

	List(const size_t& newSize, const ValueType& value)	: List() {	// Add multiple copies Constructor
		_create_until_size(newSize, value);
	}

	List(const List& other)	: List() {
		_copy(other);
	}

	List(List&& other) noexcept : List() {
		_move(custom::move(other));
	}

	~List() {
		clear();
		_free_head();
	}

public:
	// Main functions

	void resize(const size_t& newSize) {								// Resize the list by removing or adding default elements to the tail
		_delete_until_size(newSize);
		_create_until_size(newSize);
	}

	void resize(const size_t& newSize, const ValueType& copyValue) {	// Resize the list by removing or adding copy elements to the tail
		_delete_until_size(newSize);
		_create_until_size(newSize, copyValue);
	}

	template<class... Args>
	void emplace_back(Args&&... args) {									// Construct object using arguments (Args) and add it to the tail
		Node* newNode = new Node(custom::forward<Args>(args)...);
		_insert_node_before(_head, newNode);
	}

	void push_back(const ValueType& copyValue) {						// Construct object using reference and add it to the tail
		emplace_back(copyValue);
	}

	void push_back(ValueType&& moveValue) {                             // Construct object using temporary and add it to the tail
		emplace_back(custom::move(moveValue));
	}
	
	void pop_back() {													// Remove last component
		if (_size > 0)
			_remove_node(_head->_Previous);
	}

	template<class... Args>
	void emplace_front(Args&&... args) {                                // Construct object using arguments (Args) and add it to the head
		Node* newNode = new Node(custom::forward<Args>(args)...);
		_insert_node_before(_head->_Next, newNode);
	}

	void push_front(const ValueType& copyValue) {                       // Construct object using reference and add it to the head
		emplace_front(copyValue);
	}

	void push_front(ValueType&& moveValue) {                            // Construct object using temporary and add it to the head
		emplace_front(custom::move(moveValue));
	}

	void pop_front() {                                                  // Remove first component
		if (_size > 0)
			_remove_node(_head->_Next);
	}

	template<class... Args>
	Iterator emplace(const Iterator& iterator, Args&&... args) {			// Construct object using arguments (Args) and add it BEFORE the iterator position
		Node* temp = iterator._Ptr;
		Node* newNode = new Node(custom::forward<Args>(args)...);
		_insert_node_before(temp, newNode);

		return Iterator(newNode, _update_iteration_data());
	}

	Iterator push(const Iterator& iterator, const ValueType& copyValue) {   // Construct object using reference and add it BEFORE the iterator position
		return emplace(iterator, copyValue);
	}

	Iterator push(const Iterator& iterator, ValueType&& moveValue) {        // Construct object using temporary and add it BEFORE the iterator position
		return emplace(iterator, custom::move(moveValue));
	}

	Iterator pop(const Iterator& iterator) {                                // Remove component at iterator position
		if (iterator.is_end())
			throw std::out_of_range("Cannot pop end iterator...");

		Node* temp = iterator._Ptr;
		Iterator prevIterator = Iterator(temp->_Previous, _update_iteration_data());	// TODO: check iter
		_remove_node(temp);

		return prevIterator;
	}

	ValueType& front() {                                                    // Get the value of the first component
		return _head->_Next->_Value;
	}

	const ValueType& front() const {
		return _head->_Next->_Value;
	}

	ValueType& back() {                                                     // Get the value of the last component
		return _head->_Previous->_Value;
	}

	const ValueType& back() const {
		return _head->_Previous->_Value;
	}

	ValueType& at(const size_t& index) {
		if (index < 0 || index >= _size)
			throw std::out_of_range("Invalid Index...");

		return _scroll_node(index)->_Value;
	}

	const ValueType& at(const size_t& index) const {
		if (index < 0 || index >= _size)
			throw std::out_of_range("Invalid Index...");

		return _scroll_node(index)->_Value;
	}

	const size_t size() const {
		return _size;
	}

	bool empty() const {
		return _size == 0;
	}

	void clear() {
		_delete_until_size(0);
	}

public:
	// Operators

	List& operator=(const List& other) {
	if (_head != other._head)
	{
		clear();
		_copy(other);
	}

	return *this;
	}

	List& operator=(List&& other) noexcept {
	if (_head != other._head)
	{
		clear();
		_move(custom::move(other));
	}

	return *this;
	}

	bool operator==(const List& other) const {
		if (size() != other.size())
			return false;

		Iterator it1 = begin();
		Iterator it2 = other.begin();
		while (it1 != end())
		{
			if (*it1 != *it2)
				return false;

			++it1;
			++it2;
		}

		return true;
	}

	bool operator!=(const List& other) const {
		return !(*this == other);
	}

public:
	// Iterator specific functions

	Iterator begin() {
		return Iterator(_head->_Next, _update_iteration_data());
	}

	const Iterator begin() const {
		return Iterator(_head->_Next, _update_iteration_data());
	}

	Iterator end() {
		return Iterator(_head, _update_iteration_data());
	}

	const Iterator end() const {
		return Iterator(_head, _update_iteration_data());
	}

private:
	// Others

	void _create_head() {
		_head 				= _alloc.alloc(1);
		_head->_Next 		= _head;
		_head->_Previous 	= _head;
	}

	void _free_head() {
		_head->_Next 		= nullptr;
		_head->_Previous 	= nullptr;
		_alloc.dealloc(_head, 1);
	}

	void _insert_node_before(Node* beforeNode, Node* newNode) {				// Insert Node before another
		newNode->_Previous = beforeNode->_Previous;
		newNode->_Next = beforeNode;

		beforeNode->_Previous->_Next = newNode;
		beforeNode->_Previous = newNode;

		++_size;
	}

	void _remove_node(Node* junkNode) {										// Remove Node and relink
		junkNode->_Previous->_Next = junkNode->_Next;
		junkNode->_Next->_Previous = junkNode->_Previous;

		delete junkNode;
		--_size;
	}

	void _copy(const List& other) {											// Generic copy function for list
		Node* temp = other._head->_Next;
		while (_size < other._size) {
			push_back(temp->_Value);
			temp = temp->_Next;
		}
		
		_update_iteration_data();
	}

	void _move(List&& other) {												// Generic move function for list
		std::swap(_head, other._head);		// Current _head is free of memory

		_size = other._size;
		other._size = 0;

		_update_iteration_data();
		other._update_iteration_data();
	}

	template<class... Args>
	void _create_until_size(const size_t& newSize, Args&&... args) {		// Add elements until current size equals newSize
		while (_size < newSize)
			emplace_back(custom::forward<Args>(args)...);
	}

	void _delete_until_size(const size_t& newSize) {						// Remove elements until current size equals newSize
		while (_size > newSize)
			pop_back();
	}

	Node* _scroll_node(const size_t& index) const {							// Get object in the list at index position by going through all components
		if (index < _size)
		{
			Node* temp = _head->_Next;
			for (size_t i = 0; i < index; ++i)
				temp = temp->_Next;

			return temp;
		}

		return _head;
	}

	Data* _update_iteration_data() const {									// Update data and sent it to iterator
		_data._Begin 	= _head->_Next;
		_data._End 		= _head;

		return &_data;
	}
}; // END Linked List

CUSTOM_END