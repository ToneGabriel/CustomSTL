#pragma once
#include "Common.h"
#include "Node.h"
#include "BaseIterator.h"

CUSTOM_BEGIN

template <class Traits>
class HashTable;

// Linked List Iterator ========================================================
template<class List>
class ListIterator : public BaseIterator<List>
{
private:
	using Base = BaseIterator<List>;

public:

	explicit ListIterator(typename Base::IterType* nodePtr, typename Base::Data* data)
		:Base(nodePtr, data) { }

	ListIterator& operator++() {
		if (this->_Ptr == this->_IterationData->_End)
			throw std::out_of_range("Cannot increment end iterator...");

		this->_Ptr = this->_Ptr->Next;
		return *this;
	}

	ListIterator& operator++(int) {
		ListIterator temp = *this;
		++(*this);
		return temp;
	}

	ListIterator& operator--() {
		if (this->_Ptr == this->_IterationData->_Begin)
			throw std::out_of_range("Cannot decrement begin iterator...");

		this->_Ptr = this->_Ptr->Previous;
		return *this;
	}

	ListIterator& operator--(int) {
		ListIterator temp = *this;
		--(*this);
		return temp;
	}

	typename Base::IterType* operator->() {
		if (this->_Ptr == this->_IterationData->_End)
			throw std::out_of_range("Cannot access end iterator...");

		return this->_Ptr;
	}

	typename Base::ValueType& operator*() {
		if (this->_Ptr == this->_IterationData->_End)
			throw std::out_of_range("Cannot dereference end iterator...");

		return this->_Ptr->Value;
	}

	bool operator==(const ListIterator& other) const {
		return this->_Ptr == other._Ptr;
	}

	bool operator!=(const ListIterator& other) const {
		return !(*this == other);
	}
};
// Linked List Iterator ========================================================
// END


// Linked List ========================================================
template<class Type>
class List
{
private:
	template <class>
	friend class HashTable;

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

	List() {														// Default Constructor
		reset_head();
	}

	List(const size_t& newSize, const ValueType& value) : List() {  // Reference type Constructor
		create_until_size(newSize, value);
	}

	List(const List& other) : List() {								// Copy Constructor
		copy(other);
	}

	List(List&& other) noexcept : List() {							// Move Constructor
		move(std::move(other));
	}

	~List() {														// Destructor
		clear();
		clear_head();
	}

public:
	// Main functions

	void resize(const size_t& newSize) {									// Resize the list by removing or adding default elements to the tail
		delete_until_size(newSize);
		create_until_size(newSize);
	}

	void resize(const size_t& newSize, const ValueType& copyValue) {		// Resize the list by removing or adding copy elements to the tail
		delete_until_size(newSize);
		create_until_size(newSize, copyValue);
	}

	template<class... Args>
	void emplace_back(Args&&... args) {                                     // Construct object using arguments (Args) and add it to the tail
		Node* newNode = new Node(std::forward<Args>(args)...);
		insert_node_before(_head, newNode);
	}

	void push_back(const ValueType& copyValue) {                            // Construct object using reference and add it to the tail
		emplace_back(copyValue);
	}

	void push_back(ValueType&& moveValue) {                                 // Construct object using temporary and add it to the tail
		emplace_back(std::move(moveValue));
	}

	void pop_back() {                                                       // Remove last component
		if (_size > 0)
			remove_node(_head->Previous);
	}

	template<class... Args>
	void emplace_front(Args&&... args) {                                    // Construct object using arguments (Args) and add it to the head
		Node* newNode = new Node(std::forward<Args>(args)...);
		insert_node_before(_head->Next, newNode);
	}

	void push_front(const ValueType& copyValue) {                           // Construct object using reference and add it to the head
		emplace_front(copyValue);
	}

	void push_front(ValueType&& moveValue) {                                // Construct object using temporary and add it to the head
		emplace_front(std::move(moveValue));
	}

	void pop_front() {                                                      // Remove first component
		if (_size > 0)
			remove_node(_head->Next);
	}

	template<class... Args>
	Iterator emplace(const Iterator& iterator, Args&&... args) {            // Construct object using arguments (Args) and add it BEFORE the iterator position
		_workspaceNode = iterator._Ptr;
		Node* newNode = new Node(std::forward<Args>(args)...);
		insert_node_before(_workspaceNode, newNode);

		return Iterator(newNode, update_iteration_data());
	}

	Iterator push(const Iterator& iterator, const ValueType& copyValue) {   // Construct object using reference and add it BEFORE the iterator position
		return emplace(iterator, copyValue);
	}

	Iterator push(const Iterator& iterator, ValueType&& moveValue) {        // Construct object using temporary and add it BEFORE the iterator position
		return emplace(iterator, std::move(moveValue));
	}

	Iterator pop(const Iterator& iterator) {                                // Remove component at iterator position
		if (iterator._Ptr == _head)											// Check end()
			throw std::out_of_range("Cannot pop end iterator...");

		_workspaceNode = iterator._Ptr;
		Iterator prevIterator = Iterator(_workspaceNode->Previous, update_iteration_data());
		remove_node(_workspaceNode);

		return prevIterator;
	}

public:
	ValueType& front() {                                                    // Get the value of the first component
		return _head->Next->Value;
	}

	const ValueType& front() const {
		return _head->Next->Value;
	}

	ValueType& back() {                                                     // Get the value of the last component
		return _head->Previous->Value;
	}

	const ValueType& back() const {
		return _head->Previous->Value;
	}

	const size_t size() const {                                             // Get size
		return _size;
	}

	bool empty() const {                                                    // Check if list is empty
		return _size == 0;
	}

	void clear() {                                                          // Remove ALL components
		delete_until_size(0);
	}

public:
	// Operators

	List& operator=(const List& other) {									// Assign operator using reference
		if (_head != other._head)
		{
			clear();
			copy(other);
		}

		return *this;
	}

	List& operator=(List&& other) noexcept {								// Assign operator using temporary
		if (_head != other._head)
		{
			clear();
			move(std::move(other));
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
		return !operator==(other);
	}

public:
	// Iterator specific functions

	Iterator begin() {
		return Iterator(_head->Next, update_iteration_data());
	}

	const Iterator begin() const {
		return Iterator(_head->Next, update_iteration_data());
	}

	Iterator end() {
		return Iterator(_head, update_iteration_data());
	}

	const Iterator end() const {
		return Iterator(_head, update_iteration_data());
	}

	const Iterator at(const size_t& index) const {
		if (index < 0 || index >= _size)
			throw std::out_of_range("Invalid Index...");

		return Iterator(scroll_node(index), update_iteration_data());
	}

	Iterator at(const size_t& index) {
		if (index < 0 || index >= _size)
			throw std::out_of_range("Invalid Index...");

		return Iterator(scroll_node(index), update_iteration_data());
	}

private:
	// Others

	void insert_node_before(Node* beforeNode, Node* newNode) {				// Insert Node ferore another
		newNode->Previous = beforeNode->Previous;
		newNode->Next = beforeNode;

		beforeNode->Previous->Next = newNode;
		beforeNode->Previous = newNode;

		_size++;
	}

	void remove_node(Node* junkNode) {										// Remove Node and relink
		junkNode->Previous->Next = junkNode->Next;
		junkNode->Next->Previous = junkNode->Previous;

		delete junkNode;
		_size--;
	}

	Data* update_iteration_data() const {									// Update data and sent it to iterator
		_data._Begin = _head->Next;
		_data._End = _head;

		return &_data;
	}

	void copy(const List& other) {											// Generic copy function for list
		_workspaceNode = other._head->Next;
		while (_size < other._size) {
			push_back(_workspaceNode->Value);
			_workspaceNode = _workspaceNode->Next;
		}
	}

	void move(List&& other) {												// Generic move function for vector
		// link current head with the other "body"
		_head->Next = other._head->Next;
		_head->Next->Previous = _head;
		_head->Previous = other._head->Previous;
		_head->Previous->Next = _head;
		_size = other._size;
		_data = other._data;

		// link old head with itself
		other.reset_head();
		other._size = 0;
		other.update_iteration_data();
	}

	template<class... Args>
	void create_until_size(const size_t& newSize, Args&&... args) {		// Add elements until current size equals newSize
		while (_size < newSize)
			emplace_back(std::forward<Args>(args)...);
	}

	void delete_until_size(const size_t& newSize) {						// Remove elements until current size equals newSize
		while (_size > newSize)
			pop_back();
	}

	Node* scroll_node(const size_t& index) const {						// Get object in the list at index position by going through all components
		_workspaceNode = _head->Next;
		if (_workspaceNode != _head)
			for (size_t i = 0; i < index; i++)
				_workspaceNode = _workspaceNode->Next;

		return _workspaceNode;
	}

	void reset_head() {
		if (_head == nullptr)
			_head = new Node();

		_head->Next = _head;
		_head->Previous = _head;
	}

	void clear_head() {
		delete _head;
		_head = nullptr;
	}
};
// Linked List ========================================================
// END

CUSTOM_END