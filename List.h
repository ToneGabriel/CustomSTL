#pragma once
#include "Node.h"
#include "Utility.h"

CUSTOM_BEGIN

// Headings =================================================================================

template <class Traits>
class HashTable;

template<class Type>
struct ListIterationData {		// Data used for iterating List
	Type* _Begin	= nullptr;
	Type* _End		= nullptr;

	ListIterationData() = default;
	~ListIterationData();
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

	explicit ListIterator(IterType* nodePtr, Data* data);
	~ListIterator();

	ListIterator& operator++();			// ++it
	ListIterator operator++(int);		// it++
	ListIterator& operator--();			// --it
	ListIterator operator--(int);		// it--

	IterType* operator->();
	ValueType& operator*();

	bool operator==(const ListIterator& other) const;
	bool operator!=(const ListIterator& other) const;

public:
	const bool is_begin() const;
	const bool is_end() const;
}; // END Linked List Iterator


template<class Type>
class List				// Linked List
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
	size_t _size	= 0;											// Number of Nodes held by this
	Node* _head		= nullptr;										// Head of this list

	mutable Alloc _alloc;											// Allocator
	mutable Data _data;

public:
	// Constructors

	List();															// Default Constructor
	List(const size_t& newSize, const ValueType& value);			// Add multiple copies Constructor
	List(const List& other);										// Copy Constructor
	List(List&& other) noexcept;									// Move Constructor
	~List();														// Destructor

public:
	// Main functions

	void resize(const size_t& newSize);									// Resize the list by removing or adding default elements to the tail
	void resize(const size_t& newSize, const ValueType& copyValue);		// Resize the list by removing or adding copy elements to the tail

	template<class... Args>
	void emplace_back(Args&&... args);									// Construct object using arguments (Args) and add it to the tail
	void push_back(const ValueType& copyValue);							// Construct object using reference and add it to the tail
	void push_back(ValueType&& moveValue);                              // Construct object using temporary and add it to the tail
	void pop_back();                                                    // Remove last component

	template<class... Args>
	void emplace_front(Args&&... args);                                 // Construct object using arguments (Args) and add it to the head
	void push_front(const ValueType& copyValue);                        // Construct object using reference and add it to the head
	void push_front(ValueType&& moveValue);                             // Construct object using temporary and add it to the head
	void pop_front();                                                   // Remove first component

	template<class... Args>
	Iterator emplace(const Iterator& iterator, Args&&... args);				// Construct object using arguments (Args) and add it BEFORE the iterator position
	Iterator push(const Iterator& iterator, const ValueType& copyValue);    // Construct object using reference and add it BEFORE the iterator position
	Iterator push(const Iterator& iterator, ValueType&& moveValue);         // Construct object using temporary and add it BEFORE the iterator position
	Iterator pop(const Iterator& iterator);                                 // Remove component at iterator position

public:
	ValueType& front();                                                     // Get the value of the first component
	const ValueType& front() const;
	ValueType& back();                                                      // Get the value of the last component
	const ValueType& back() const;
	
	const size_t size() const;                                              // Get size
	bool empty() const;                                                     // Check if list is empty
	void clear();                                                           // Remove ALL components

public:
	// Operators

	List& operator=(const List& other);										// Assign operator using reference
	List& operator=(List&& other) noexcept;									// Assign operator using temporary

	bool operator==(const List& other) const;
	bool operator!=(const List& other) const;

public:
	// Iterator specific functions

	Iterator begin();
	const Iterator begin() const;

	Iterator end();
	const Iterator end() const;

	const Iterator at(const size_t& index) const;
	Iterator at(const size_t& index);

private:
	// Others

	void _insert_node_before(Node* beforeNode, Node* newNode);				// Insert Node ferore another
	void _remove_node(Node* junkNode);										// Remove Node and relink
	void _copy(const List& other);											// Generic copy function for list
	void _move(List&& other);												// Generic move function for list

	template<class... Args>
	void _create_until_size(const size_t& newSize, Args&&... args);			// Add elements until current size equals newSize
	void _delete_until_size(const size_t& newSize);							// Remove elements until current size equals newSize
	Node* _scroll_node(const size_t& index) const;							// Get object in the list at index position by going through all components
	Data* _update_iteration_data() const;									// Update data and sent it to iterator
}; // END Linked List



// Definitions =================================================================================

// Linked List Iterator Data
template<class Type>
ListIterationData<Type>::~ListIterationData() {
	_Begin	= nullptr;
	_End	= nullptr;
}

// Linked List Iterator
template<class List>
ListIterator<List>::ListIterator(IterType* nodePtr, Data* data)
	:_Ptr(nodePtr), _Data(data) { /*Empty*/ }

template<class List>
ListIterator<List>::~ListIterator() {
	_Ptr	= nullptr;
	_Data	= nullptr;
}

template<class List>
ListIterator<List>& ListIterator<List>::operator++() {
	if (_Ptr == _Data->_End)
		throw std::out_of_range("Cannot increment end iterator...");

	_Ptr = _Ptr->_Next;
	return *this;
}

template<class List>
ListIterator<List> ListIterator<List>::operator++(int) {
	ListIterator temp = *this;
	++(*this);
	return temp;
}

template<class List>
ListIterator<List>& ListIterator<List>::operator--() {
	if (_Ptr == _Data->_Begin)
		throw std::out_of_range("Cannot decrement begin iterator...");

	_Ptr = _Ptr->_Previous;
	return *this;
}

template<class List>
ListIterator<List> ListIterator<List>::operator--(int) {
	ListIterator temp = *this;
	--(*this);
	return temp;
}

template<class List>
typename ListIterator<List>::IterType* ListIterator<List>::operator->() {
	if (_Ptr == _Data->_End)
		throw std::out_of_range("Cannot access end iterator...");

	return _Ptr;
}

template<class List>
typename ListIterator<List>::ValueType& ListIterator<List>::operator*() {
	if (_Ptr == _Data->_End)
		throw std::out_of_range("Cannot dereference end iterator...");

	return _Ptr->_Value;
}

template<class List>
bool ListIterator<List>::operator==(const ListIterator& other) const {
	return _Ptr == other._Ptr;
}

template<class List>
bool ListIterator<List>::operator!=(const ListIterator& other) const {
	return !(*this == other);
}

template<class List>
const bool ListIterator<List>::is_begin() const {
	return _Ptr == _Data->_Begin;
}

template<class List>
const bool ListIterator<List>::is_end() const {
	return _Ptr == _Data->_End;
}
// END Linked List Iterator


// Linked List
template<class Type>
List<Type>::List() {
	_head = Node::create_head(_alloc);
}

template<class Type>
List<Type>::List(const size_t& newSize, const ValueType& value) : List() {
	_create_until_size(newSize, value);
}

template<class Type>
List<Type>::List(const List& other) : List() {
	_copy(other);
}

template<class Type>
List<Type>::List(List&& other) noexcept : List() {
	_move(custom::move(other));
}

template<class Type>
List<Type>::~List() {
	clear();
	Node::free_head(_alloc, _head);
}

template<class Type>
void List<Type>::resize(const size_t& newSize) {
	_delete_until_size(newSize);
	_create_until_size(newSize);
}

template<class Type>
void List<Type>::resize(const size_t& newSize, const ValueType& copyValue) {
	_delete_until_size(newSize);
	_create_until_size(newSize, copyValue);
}

template<class Type>
template<class... Args>
void List<Type>::emplace_back(Args&&... args) {
	Node* newNode = new Node(custom::forward<Args>(args)...);
	_insert_node_before(_head, newNode);
}

template<class Type>
void List<Type>::push_back(const ValueType& copyValue) {
	emplace_back(copyValue);
}

template<class Type>
void List<Type>::push_back(ValueType&& moveValue) {
	emplace_back(custom::move(moveValue));
}

template<class Type>
void List<Type>::pop_back() {
	if (_size > 0)
		_remove_node(_head->_Previous);
}

template<class Type>
template<class... Args>
void List<Type>::emplace_front(Args&&... args) {
	Node* newNode = new Node(custom::forward<Args>(args)...);
	_insert_node_before(_head->_Next, newNode);
}

template<class Type>
void List<Type>::push_front(const ValueType& copyValue) {
	emplace_front(copyValue);
}

template<class Type>
void List<Type>::push_front(ValueType&& moveValue) {
	emplace_front(custom::move(moveValue));
}

template<class Type>
void List<Type>::pop_front() {
	if (_size > 0)
		_remove_node(_head->_Next);
}

template<class Type>
template<class... Args>
typename List<Type>::Iterator List<Type>::emplace(const Iterator& iterator, Args&&... args) {
	Node* temp = iterator._Ptr;
	Node* newNode = new Node(custom::forward<Args>(args)...);
	_insert_node_before(temp, newNode);

	return Iterator(newNode, _update_iteration_data());
}

template<class Type>
typename List<Type>::Iterator List<Type>::push(const Iterator& iterator, const ValueType& copyValue) {
	return emplace(iterator, copyValue);
}

template<class Type>
typename List<Type>::Iterator List<Type>::push(const Iterator& iterator, ValueType&& moveValue) {
	return emplace(iterator, custom::move(moveValue));
}

template<class Type>
typename List<Type>::Iterator List<Type>::pop(const Iterator& iterator) {
	if (iterator.is_end())
		throw std::out_of_range("Cannot pop end iterator...");

	Node* temp = iterator._Ptr;
	Iterator prevIterator = Iterator(temp->_Previous, _update_iteration_data());
	_remove_node(temp);

	return prevIterator;
}

template<class Type>
typename List<Type>::ValueType& List<Type>::front() {
	return _head->_Next->_Value;
}

template<class Type>
const typename List<Type>::ValueType& List<Type>::front() const {
	return _head->_Next->_Value;
}

template<class Type>
typename List<Type>::ValueType& List<Type>::back() {
	return _head->_Previous->_Value;
}

template<class Type>
const typename List<Type>::ValueType& List<Type>::back() const {
	return _head->_Previous->_Value;
}

template<class Type>
const size_t List<Type>::size() const {
	return _size;
}

template<class Type>
bool List<Type>::empty() const {
	return _size == 0;
}

template<class Type>
void List<Type>::clear() {
	_delete_until_size(0);
}

template<class Type>
List<Type>& List<Type>::operator=(const List& other) {
	if (_head != other._head)
	{
		clear();
		_copy(other);
	}

	return *this;
}

template<class Type>
List<Type>& List<Type>::operator=(List&& other) noexcept {
	if (_head != other._head)
	{
		clear();
		_move(custom::move(other));
	}

	return *this;
}

template<class Type>
bool List<Type>::operator==(const List& other) const {
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

template<class Type>
bool List<Type>::operator!=(const List& other) const {
	return !(*this == other);
}

template<class Type>
typename List<Type>::Iterator List<Type>::begin() {
	return Iterator(_head->_Next, _update_iteration_data());
}

template<class Type>
const typename List<Type>::Iterator List<Type>::begin() const {
	return Iterator(_head->_Next, _update_iteration_data());
}

template<class Type>
typename List<Type>::Iterator List<Type>::end() {
	return Iterator(_head, _update_iteration_data());
}

template<class Type>
const typename List<Type>::Iterator List<Type>::end() const {
	return Iterator(_head, _update_iteration_data());
}

template<class Type>
const typename List<Type>::Iterator List<Type>::at(const size_t& index) const {
	if (index < 0 || index >= _size)
		throw std::out_of_range("Invalid Index...");

	return Iterator(_scroll_node(index), _update_iteration_data());
}

template<class Type>
typename List<Type>::Iterator List<Type>::at(const size_t& index) {
	if (index < 0 || index >= _size)
		throw std::out_of_range("Invalid Index...");

	return Iterator(_scroll_node(index), _update_iteration_data());
}

template<class Type>
void List<Type>::_insert_node_before(Node* beforeNode, Node* newNode) {
	newNode->_Previous = beforeNode->_Previous;
	newNode->_Next = beforeNode;

	beforeNode->_Previous->_Next = newNode;
	beforeNode->_Previous = newNode;

	++_size;
}

template<class Type>
void List<Type>::_remove_node(Node* junkNode) {
	junkNode->_Previous->_Next = junkNode->_Next;
	junkNode->_Next->_Previous = junkNode->_Previous;

	delete junkNode;
	--_size;
}

template<class Type>
void List<Type>::_copy(const List& other) {
	Node* temp = other._head->_Next;
	while (_size < other._size) {
		push_back(temp->_Value);
		temp = temp->_Next;
	}
}

template<class Type>
void List<Type>::_move(List&& other) {
	// link current head with the other "body"
	_head->_Next = other._head->_Next;
	_head->_Next->_Previous = _head;
	_head->_Previous = other._head->_Previous;
	_head->_Previous->_Next = _head;
	_size = other._size;
	_data = other._data;

	// link old head with itself
	other._head->_Next = other._head;
	other._head->_Previous = other._head;
	other._size = 0;
	other._update_iteration_data();
}

template<class Type>
template<class... Args>
void List<Type>::_create_until_size(const size_t& newSize, Args&&... args) {
	while (_size < newSize)
		emplace_back(custom::forward<Args>(args)...);
}

template<class Type>
void List<Type>::_delete_until_size(const size_t& newSize) {
	while (_size > newSize)
		pop_back();
}

template<class Type>
typename List<Type>::Node* List<Type>::_scroll_node(const size_t& index) const {
	if (index < _size)
	{
		Node* temp = _head->_Next;
		for (size_t i = 0; i < index; ++i)
			temp = temp->_Next;

		return temp;
	}

	return _head;
}

template<class Type>
typename List<Type>::Data* List<Type>::_update_iteration_data() const {
	_data._Begin = _head->_Next;
	_data._End = _head;

	return &_data;
}
// END Linked List

CUSTOM_END