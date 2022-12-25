#pragma once
#include "Allocator.h"
#include "BaseIterator.h"


// Linked List Node ========================================================
template<class LinkedList>
struct LinkedListNode                                                             // Struct that holds data and references to next and previous struct
{
public:
	using ValueType = typename LinkedList::ValueType;

	ValueType Value;                                                              // Data
	LinkedListNode* Previous	= nullptr;                                        // Reference to previous 
	LinkedListNode* Next		= nullptr;                                        // Reference to next

	LinkedListNode() = default;

	template<class... Args>
	LinkedListNode(Args&&... args) {                                              // Add data using emplace ValueTypepe Constructor
		new(&Value) ValueType(std::forward<Args>(args)...);
	}

	LinkedListNode(const ValueType& value) {                                      // Add data using reference ValueTypepe Constructor
		new(&Value) ValueType(value);
	}

	LinkedListNode(ValueType&& value) {                                           // Add data using temporary ValueTypepe Constructor
		new(&Value) ValueType(std::move(value));
	}
};
// Linked List Node ========================================================
// END


// Linked List Iterator ========================================================
template<class LinkedList>
class LinkedListIterator : public BaseIterator<LinkedList>
{
private:
	using Base = BaseIterator<LinkedList>;

public:

	explicit LinkedListIterator(Base::IterType* nodePtr, Base::Data* data)
		:Base(nodePtr, data) { }

	LinkedListIterator& operator++() {
		if (this->_Ptr == this->_IterationData->_IterEnd)
			throw std::out_of_range("Cannot increment end iterator...");

		this->_Ptr = this->_Ptr->Next;
		return *this;
	}

	LinkedListIterator& operator++(int) {
		LinkedListIterator temp = *this;
		++(*this);
		return temp;
	}

	LinkedListIterator& operator--() {
		if (this->_Ptr == this->_IterationData->_IterBegin)
			throw std::out_of_range("Cannot decrement begin iterator...");

		this->_Ptr = this->_Ptr->Previous;
		return *this;
	}

	LinkedListIterator& operator--(int) {
		LinkedListIterator temp = *this;
		--(*this);
		return temp;
	}

	Base::IterType* operator->() {
		if (this->_Ptr == this->_IterationData->_IterEnd)
			throw std::out_of_range("Cannot access end iterator...");

		return this->_Ptr;
	}

	Base::ValueType& operator*() {
		if (this->_Ptr == this->_IterationData->_IterEnd)
			throw std::out_of_range("Cannot dereference end iterator...");

		return this->_Ptr->Value;
	}

	bool operator==(const LinkedListIterator& other) const {
		return this->_Ptr == other._Ptr;
	}

	bool operator!=(const LinkedListIterator& other) const {
		return !(*this == other);
	}
};
// Linked List Iterator ========================================================
// END


// Linked List ========================================================
template<class Type>
class LinkedList
{
public:
	using ValueType = Type;													// Type for stored values
	using Node		= typename LinkedListNode<LinkedList<ValueType>>;		// Node type
	using IterType	= Node;													// Type of iterating element
	using Iterator	= typename LinkedListIterator<LinkedList<ValueType>>;	// Iterator type
	using Data		= typename Iterator::Data;								// Iteration data

private:
	size_t _size	= 0;                                                    // Number of Nodes held by this
	Node* _head		= nullptr;												// Head of this list

	mutable Data _data;
	mutable Node* _workspaceNode = nullptr;                                 // Auxiliary Node for work

public:
	// Constructors

	LinkedList() {                                                              // Default Constructor
		reset_head();
	}

	template<class... Args>
	LinkedList(const size_t& newSize, Args&&... args) : LinkedList() {          // Emplace type Constructor
		create_until_size(newSize, std::forward<Args>(args)...);
	}

	LinkedList(const size_t& newSize, const ValueType& value) : LinkedList() {  // Reference type Constructor
		create_until_size(newSize, value);
	}

	LinkedList(const size_t& newSize, ValueType&& value) : LinkedList() {       // Temporary type Constructor
		create_until_size(newSize, std::move(value));
	}

	LinkedList(const LinkedList<ValueType>& other) : LinkedList() {				// Copy Constructor
		copy(other);
	}

	LinkedList(LinkedList<ValueType>&& other) noexcept : LinkedList() {			// Move Constructor
		move(std::move(other));
	}

	~LinkedList() {                                                             // Destructor
		clear();
		clear_head();
	}

public:
	// Main functions

	template<class... Args>
	void resize(const size_t& newSize, Args&&... args) {                    // Resize the list by removing or adding elements to the tail
		resize_emplace(newSize, std::forward<Args>(args)...);               // Emplace type addition
	}

	void resize(const size_t& newSize, const ValueType& copyValue) {
		resize_emplace(newSize, copyValue);                                 // Reference type addition
	}

	void resize(const size_t& newSize, ValueType&& moveValue) {
		resize_emplace(newSize, std::move(moveValue));                      // Temporary type addition
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
		_workspaceNode = iterator._NodePtr;
		Node* newNode = new Node(std::forward<Args>(args)...);
		insert_node_before(_workspaceNode, newNode);

		return Iterator(newNode, update_iteration_data());
	}

	Iterator push(const Iterator& iterator, const ValueType& copyValue) {    // Construct object using reference and add it BEFORE the iterator position
		return emplace(iterator, copyValue);
	}

	Iterator push(const Iterator& iterator, ValueType&& moveValue) {         // Construct object using temporary and add it BEFORE the iterator position
		return emplace(iterator, std::move(moveValue));
	}

	Iterator pop(const Iterator& iterator) {                                 // Remove component at iterator position
		if (iterator == end())
			throw std::out_of_range("Cannot pop end iterator...");

		_workspaceNode = iterator._NodePtr;
		Iterator nextIterator = Iterator(_workspaceNode->Next, update_iteration_data());
		remove_node(_workspaceNode);

		return nextIterator;
	}

	void insert_node_before(Node* beforeNode, Node* newNode) {
		newNode->Previous = beforeNode->Previous;
		newNode->Next = beforeNode;

		beforeNode->Previous->Next = newNode;
		beforeNode->Previous = newNode;

		_size++;
	}

	void remove_node(Node* junkNode) {
		junkNode->Previous->Next = junkNode->Next;
		junkNode->Next->Previous = junkNode->Previous;

		delete junkNode;
		junkNode = nullptr;
		_size--;
	}

	Data* update_iteration_data() const {
		_data._IterBegin = _head->Next;
		_data._IterEnd = _head;

		return &_data;
	}

	ValueType& front() {                                                     // Get the value of the first component
		return _head->Next->Value;
	}

	ValueType& back() {                                                      // Get the value of the last component
		return _head->Previous->Value;
	}

	const size_t size() const {                                              // Get size
		return _size;
	}

	bool empty() const {                                                     // Check if list is empty
		return _size == 0;
	}

	void clear() {                                                           // Remove ALL components
		delete_until_size(0);
	}

public:
	// Operators

	LinkedList& operator=(const LinkedList<ValueType>& other) {              // Assign operator using reference
		clear();
		copy(other);

		return *this;
	}

	LinkedList& operator=(LinkedList<ValueType>&& other) noexcept {          // Assign operator using temporary
		clear();
		move(std::move(other));

		return *this;
	}

public:
	// Iterator specific functions

	Iterator begin() {
		return Iterator(_head->Next, update_iteration_data());
	}

	Iterator end() {
		return Iterator(_head, update_iteration_data());
	}

	Iterator at(const size_t& index) {
		if (index < 0 || index >= _size)
			throw std::out_of_range("Invalid Index...");

		return Iterator(scroll_node(index), update_iteration_data());
	}

private:
	// Others

	void copy(const LinkedList<ValueType>& other) {
		_workspaceNode = other._head->Next;
		while (_size < other._size) {
			push_back(_workspaceNode->Value);
			_workspaceNode = _workspaceNode->Next;
		}
	}

	void move(LinkedList<ValueType>&& other) {
		// link current head with the other "body"
		_head->Next = other._head->Next;
		_head->Next->Previous = _head;
		_head->Previous = other._head->Previous;
		_head->Previous->Next = _head;
		_size = other._size;
		_data = other._data;

		// link old head with itself
		other.reset_head();
		other._data = nullptr;
		other._size = 0;
	}

	template<class... Args>
	void resize_emplace(const size_t& newSize, Args&&... args) {             // Resize the list by removing or adding elements to the tail
		delete_until_size(newSize);
		create_until_size(newSize, std::forward<Args>(args)...);
	}

	template<class... Args>
	void create_until_size(const size_t& newSize, Args&&... args) {			 // Add elements until current size equals newSize
		while (_size < newSize)
			emplace_back(std::forward<Args>(args)...);                       // Emplace type addition
	}

	void delete_until_size(const size_t& newSize) {							 // Remove elements until current size equals newSize
		while (_size > newSize)
			pop_back();
	}

	Node* scroll_node(const size_t& index) const {                           // Get object in the list at index position by going through all components
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