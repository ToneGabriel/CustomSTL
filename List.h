#pragma once
#include "Common.h"
#include "Node.h"
#include "BaseIterator.h"

CUSTOM_BEGIN

// Headings =================================================================================

template <class Traits>
class HashTable;

template<class>
class SearchTree;

template<class List>
class ListIterator : public BaseIterator<List>		// Linked List Iterator
{
public:
	using Base		= BaseIterator<List>;
	using Data		= typename Base::Data;
	using IterType	= typename Base::IterType;
	using ValueType = typename Base::ValueType;

public:

	explicit ListIterator(IterType* nodePtr, Data* data);

	ListIterator& operator++();			// ++it
	ListIterator operator++(int);		// it++
	ListIterator& operator--();			// --it
	ListIterator operator--(int);		// it--

	IterType* operator->();
	ValueType& operator*();

	bool operator==(const ListIterator& other) const;
	bool operator!=(const ListIterator& other) const;
}; // END Linked List Iterator


template<class Type>
class List				// Linked List
{
private:
	template<class>
	friend class HashTable;

	template<class>
	friend class SearchTree;

public:
	using ValueType = Type;											// Type for stored values
	using Node		= DoubleNode<ValueType>;						// Node type
	using IterType	= Node;											// Type of iterating element
	using Iterator	= ListIterator<List<ValueType>>;				// Iterator type
	using Data		= typename Iterator::Data;						// Iteration data

private:
	size_t _size	= 0;											// Number of Nodes held by this
	Node* _head		= nullptr;										// Head of this list

	mutable Data _data;
	mutable Node* _workspaceNode = nullptr;							// Auxiliary Node for work

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
	void _reset_head();
	void _clear_head();

	template<class... Args>
	void _create_until_size(const size_t& newSize, Args&&... args);			// Add elements until current size equals newSize
	void _delete_until_size(const size_t& newSize);							// Remove elements until current size equals newSize
	Node* _scroll_node(const size_t& index) const;							// Get object in the list at index position by going through all components
	Data* _update_iteration_data() const;									// Update data and sent it to iterator
}; // END Linked List



// Definitions =================================================================================

// Linked List Iterator
template<class List>
ListIterator<List>::ListIterator(IterType* nodePtr, Data* data)
	:Base(nodePtr, data) { /*Empty*/ }

template<class List>
ListIterator<List>& ListIterator<List>::operator++() {
	if (this->_Ptr == this->_IterationData->_End)
		throw std::out_of_range("Cannot increment end iterator...");

	this->_Ptr = this->_Ptr->Next;
	return *this;
}

template<class List>
ListIterator<List> ListIterator<List>::operator++(int) {	// TODO: "&" missing or not???
	ListIterator temp = *this;
	++(*this);
	return temp;
}

template<class List>
ListIterator<List>& ListIterator<List>::operator--() {
	if (this->_Ptr == this->_IterationData->_Begin)
		throw std::out_of_range("Cannot decrement begin iterator...");

	this->_Ptr = this->_Ptr->Previous;
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
	if (this->_Ptr == this->_IterationData->_End)
		throw std::out_of_range("Cannot access end iterator...");

	return this->_Ptr;
}

template<class List>
typename ListIterator<List>::ValueType& ListIterator<List>::operator*() {
	if (this->_Ptr == this->_IterationData->_End)
		throw std::out_of_range("Cannot dereference end iterator...");

	return this->_Ptr->Value;
}

template<class List>
bool ListIterator<List>::operator==(const ListIterator& other) const {
	return this->_Ptr == other._Ptr;
}

template<class List>
bool ListIterator<List>::operator!=(const ListIterator& other) const {
	return !(*this == other);
}
// END Linked List Iterator


// Linked List
template<class Type>
List<Type>::List() {
	_reset_head();
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
	_move(std::move(other));
}

template<class Type>
List<Type>::~List() {
	clear();
	_clear_head();
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
	Node* newNode = new Node(std::forward<Args>(args)...);
	_insert_node_before(_head, newNode);
}

template<class Type>
void List<Type>::push_back(const ValueType& copyValue) {
	emplace_back(copyValue);
}

template<class Type>
void List<Type>::push_back(ValueType&& moveValue) {
	emplace_back(std::move(moveValue));
}

template<class Type>
void List<Type>::pop_back() {
	if (_size > 0)
		_remove_node(_head->Previous);
}

template<class Type>
template<class... Args>
void List<Type>::emplace_front(Args&&... args) {
	Node* newNode = new Node(std::forward<Args>(args)...);
	_insert_node_before(_head->Next, newNode);
}

template<class Type>
void List<Type>::push_front(const ValueType& copyValue) {
	emplace_front(copyValue);
}

template<class Type>
void List<Type>::push_front(ValueType&& moveValue) {
	emplace_front(std::move(moveValue));
}

template<class Type>
void List<Type>::pop_front() {
	if (_size > 0)
		_remove_node(_head->Next);
}

template<class Type>
template<class... Args>
typename List<Type>::Iterator List<Type>::emplace(const Iterator& iterator, Args&&... args) {
	_workspaceNode = iterator._Ptr;
	Node* newNode = new Node(std::forward<Args>(args)...);
	_insert_node_before(_workspaceNode, newNode);

	return Iterator(newNode, _update_iteration_data());
}

template<class Type>
typename List<Type>::Iterator List<Type>::push(const Iterator& iterator, const ValueType& copyValue) {
	return emplace(iterator, copyValue);
}

template<class Type>
typename List<Type>::Iterator List<Type>::push(const Iterator& iterator, ValueType&& moveValue) {
	return emplace(iterator, std::move(moveValue));
}

template<class Type>
typename List<Type>::Iterator List<Type>::pop(const Iterator& iterator) {
	if (iterator._Ptr == _head)											// Check end()
		throw std::out_of_range("Cannot pop end iterator...");

	_workspaceNode = iterator._Ptr;
	Iterator prevIterator = Iterator(_workspaceNode->Previous, _update_iteration_data());
	_remove_node(_workspaceNode);

	return prevIterator;
}

template<class Type>
typename List<Type>::ValueType& List<Type>::front() {
	return _head->Next->Value;
}

template<class Type>
const typename List<Type>::ValueType& List<Type>::front() const {
	return _head->Next->Value;
}

template<class Type>
typename List<Type>::ValueType& List<Type>::back() {
	return _head->Previous->Value;
}

template<class Type>
const typename List<Type>::ValueType& List<Type>::back() const {
	return _head->Previous->Value;
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
		_move(std::move(other));
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
	return !operator==(other);
}

template<class Type>
typename List<Type>::Iterator List<Type>::begin() {
	return Iterator(_head->Next, _update_iteration_data());
}

template<class Type>
const typename List<Type>::Iterator List<Type>::begin() const {
	return Iterator(_head->Next, _update_iteration_data());
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
	newNode->Previous = beforeNode->Previous;
	newNode->Next = beforeNode;

	beforeNode->Previous->Next = newNode;
	beforeNode->Previous = newNode;

	_size++;
}

template<class Type>
void List<Type>::_remove_node(Node* junkNode) {
	junkNode->Previous->Next = junkNode->Next;
	junkNode->Next->Previous = junkNode->Previous;

	delete junkNode;
	_size--;
}

template<class Type>
typename List<Type>::Data* List<Type>::_update_iteration_data() const {
	_data._Begin = _head->Next;
	_data._End = _head;

	return &_data;
}

template<class Type>
void List<Type>::_copy(const List& other) {
	_workspaceNode = other._head->Next;
	while (_size < other._size) {
		push_back(_workspaceNode->Value);
		_workspaceNode = _workspaceNode->Next;
	}
}

template<class Type>
void List<Type>::_move(List&& other) {
	// link current head with the other "body"
	_head->Next = other._head->Next;
	_head->Next->Previous = _head;
	_head->Previous = other._head->Previous;
	_head->Previous->Next = _head;
	_size = other._size;
	_data = other._data;

	// link old head with itself
	other._reset_head();
	other._size = 0;
	other._update_iteration_data();
}

template<class Type>
template<class... Args>
void List<Type>::_create_until_size(const size_t& newSize, Args&&... args) {
	while (_size < newSize)
		emplace_back(std::forward<Args>(args)...);
}

template<class Type>
void List<Type>::_delete_until_size(const size_t& newSize) {
	while (_size > newSize)
		pop_back();
}

template<class Type>
typename List<Type>::Node* List<Type>::_scroll_node(const size_t& index) const {
	_workspaceNode = _head->Next;
	if (_workspaceNode != _head)
		for (size_t i = 0; i < index; i++)
			_workspaceNode = _workspaceNode->Next;

	return _workspaceNode;
}

template<class Type>
void List<Type>::_reset_head() {
	if (_head == nullptr)
		_head = new Node();

	_head->Next = _head;
	_head->Previous = _head;
}

template<class Type>
void List<Type>::_clear_head() {
	delete _head;
	_head = nullptr;
}
// END Linked List

CUSTOM_END