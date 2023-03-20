#pragma once
#include "Node.h"
#include "Utility.h"

CUSTOM_BEGIN

template <class Traits>
class HashTable;

template<class Type>
struct ListData {
	using ValueType		= Type;									// Type for stored values
	using Node			= DoubleNode<ValueType>;				// Node type
	using IterType		= Node;									// Type for iteration
	using Alloc			= Allocator<Node>;						// Allocator type

	size_t _Size		= 0;									// Number of Nodes held
	Node* _Head			= nullptr;								// Head of list
};

template<class List>
class ListConstIterator
{
public:
	using Data			= typename List::Data;
	using ValueType		= typename List::ValueType;
	using IterType		= typename List::IterType;

	IterType* _Ptr		= nullptr;
	const Data* _Data	= nullptr;

public:

	explicit ListConstIterator(IterType* nodePtr, const Data* data)
		:_Ptr(nodePtr), _Data(data) { /*Empty*/ }

	ListConstIterator& operator++() {
		if (_Ptr == _Data->_Head)
			throw std::out_of_range("Cannot increment end iterator...");

		_Ptr = _Ptr->_Next;
		return *this;
	}

	ListConstIterator operator++(int) {
		ListConstIterator temp = *this;
		++(*this);
		return temp;
	}

	ListConstIterator& operator--() {
		if (_Ptr == _Data->_Head->_Next)
			throw std::out_of_range("Cannot decrement begin iterator...");

		_Ptr = _Ptr->_Previous;
		return *this;
	}

	ListConstIterator operator--(int) {
		ListConstIterator temp = *this;
		--(*this);
		return temp;
	}

	const IterType* operator->() const {
		if (_Ptr == _Data->_Head)
			throw std::out_of_range("Cannot access end iterator...");

		return _Ptr;
	}

	const ValueType& operator*() const {
		if (_Ptr == _Data->_Head)
			throw std::out_of_range("Cannot dereference end iterator...");

		return _Ptr->_Value;
	}

	bool operator==(const ListConstIterator& other) const {
		return _Ptr == other._Ptr;
	}

	bool operator!=(const ListConstIterator& other) const {
		return !(*this == other);
	}

public:
	const bool is_begin() const {
		return _Ptr == _Data->_Head->_Next;
	}

	const bool is_end() const {
		return _Ptr == _Data->_Head;
	}
};

template<class List>
class ListIterator : public ListConstIterator<List>		// Linked List Iterator
{
public:
	using Base		= ListConstIterator <List>;
	using Data		= typename List::Data;
	using ValueType = typename List::ValueType;
	using IterType	= typename List::IterType;

public:

	explicit ListIterator(IterType* nodePtr, Data* data)
		:Base(nodePtr, data) { /*Empty*/ }

	ListIterator& operator++() {
		Base::operator++();
		return *this;
	}

	ListIterator operator++(int) {
		ListIterator temp = *this;
		Base::operator++();
		return temp;
	}

	ListIterator& operator--() {
		Base::operator--();
		return *this;
	}

	ListIterator operator--(int) {
		ListIterator temp = *this;
		Base::operator--();
		return temp;
	}

	IterType* operator->() const {
		return const_cast<IterType*>(Base::operator->());
	}

	ValueType& operator*() const {
		return const_cast<ValueType&>(Base::operator*());
	}
}; // END Linked List Iterator


template<class Type>
class List				// Doubly Linked List
{
private:
	template<class>
	friend class HashTable;

public:
	using Data 					= ListData<Type>;							// Members that are modified
	using ValueType 			= typename Data::ValueType;					// Type for stored values
	using Node 					= typename Data::Node;						// Node in list
	using IterType				= typename Data::IterType;					// Type of iterating element
	using Alloc					= typename Data::Alloc;						// Allocator type
	using Iterator				= ListIterator<List<ValueType>>;			// Iterator type
	using ConstIterator			= ListConstIterator<List<ValueType>>;		// Const Iterator type
	using ReverseIterator		= custom::ReverseIterator<Iterator>;		// Reverse Iterator type
	using ConstReverseIterator	= custom::ReverseIterator<ConstIterator>;	// Const Reverse Iterator type

private:
	Alloc _alloc;													// Allocator
	Data _data;														// Actual container data

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
		_insert_node_before(_data._Head, newNode);
	}

	void push_back(const ValueType& copyValue) {						// Construct object using reference and add it to the tail
		emplace_back(copyValue);
	}

	void push_back(ValueType&& moveValue) {                             // Construct object using temporary and add it to the tail
		emplace_back(custom::move(moveValue));
	}
	
	void pop_back() {													// Remove last component
		if (_data._Size > 0)
			_remove_node(_data._Head->_Previous);
	}

	template<class... Args>
	void emplace_front(Args&&... args) {                                // Construct object using arguments (Args) and add it to the head
		Node* newNode = new Node(custom::forward<Args>(args)...);
		_insert_node_before(_data._Head->_Next, newNode);
	}

	void push_front(const ValueType& copyValue) {                       // Construct object using reference and add it to the head
		emplace_front(copyValue);
	}

	void push_front(ValueType&& moveValue) {                            // Construct object using temporary and add it to the head
		emplace_front(custom::move(moveValue));
	}

	void pop_front() {                                                  // Remove first component
		if (_data._Size > 0)
			_remove_node(_data._Head->_Next);
	}

	template<class... Args>
	Iterator emplace(const Iterator& iterator, Args&&... args) {			// Construct object using arguments (Args) and add it BEFORE the iterator position
		Node* temp = iterator._Ptr;
		Node* newNode = new Node(custom::forward<Args>(args)...);
		_insert_node_before(temp, newNode);

		return Iterator(newNode, &_data);
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
		Iterator prevIterator = Iterator(temp->_Previous, &_data);	// TODO: check iter
		_remove_node(temp);

		return prevIterator;
	}

	ValueType& front() {                                                    // Get the value of the first component
		assert(_data._Size > 0);
		return _data._Head->_Next->_Value;
	}

	const ValueType& front() const {
		assert(_data._Size > 0);
		return _data._Head->_Next->_Value;
	}

	ValueType& back() {                                                     // Get the value of the last component
		assert(_data._Size > 0);
		return _data._Head->_Previous->_Value;
	}

	const ValueType& back() const {
		assert(_data._Size > 0);
		return _data._Head->_Previous->_Value;
	}

	ValueType& at(const size_t& index) {
		if (index >= _data._Size)
			throw std::out_of_range("Invalid Index...");

		return _scroll_node(index)->_Value;
	}

	const ValueType& at(const size_t& index) const {
		if (index >= _data._Size)
			throw std::out_of_range("Invalid Index...");

		return _scroll_node(index)->_Value;
	}

	const size_t size() const {
		return _data._Size;
	}

	bool empty() const {
		return _data._Size == 0;
	}

	void clear() {
		_delete_until_size(0);
	}

public:
	// Operators

	List& operator=(const List& other) {
		if (_data._Head != other._data._Head)
		{
			clear();
			_copy(other);
		}

		return *this;
	}

	List& operator=(List&& other) noexcept {
		if (_data._Head != other._data._Head)
		{
			clear();
			_move(custom::move(other));
		}

		return *this;
	}

	bool operator==(const List& other) const {
		if (size() != other.size())
			return false;

		auto it1 = begin();
		auto it2 = other.begin();
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
		return Iterator(_data._Head->_Next, &_data);
	}

	ConstIterator begin() const {
		return ConstIterator(_data._Head->_Next, &_data);
	}

	ReverseIterator rbegin() {
		return ReverseIterator(end());
	}

	ConstReverseIterator rbegin() const {
		return ReverseIterator(end());
	}

	Iterator end() {
		return Iterator(_data._Head, &_data);
	}

	ConstIterator end() const {
		return ConstIterator(_data._Head, &_data);
	}

	ReverseIterator rend() {
		return ReverseIterator(begin());
	}

	ConstReverseIterator rend() const {
		return ReverseIterator(begin());
	}

private:
	// Others

	void _create_head() {
		_data._Head 				= _alloc.alloc(1);
		_data._Head->_Next 		= _data._Head;
		_data._Head->_Previous 	= _data._Head;
	}

	void _free_head() {
		_data._Head->_Next 		= nullptr;
		_data._Head->_Previous 	= nullptr;
		_alloc.dealloc(_data._Head, 1);
	}

	void _insert_node_before(Node* beforeNode, Node* newNode) {				// Insert Node before another
		newNode->_Previous = beforeNode->_Previous;
		newNode->_Next = beforeNode;

		beforeNode->_Previous->_Next = newNode;
		beforeNode->_Previous = newNode;

		++_data._Size;
	}

	void _remove_node(Node* junkNode) {										// Remove Node and relink
		junkNode->_Previous->_Next = junkNode->_Next;
		junkNode->_Next->_Previous = junkNode->_Previous;

		delete junkNode;
		--_data._Size;
	}

	void _copy(const List& other) {											// Generic copy function for list
		Node* temp = other._data._Head->_Next;
		while (_data._Size < other._data._Size) {
			push_back(temp->_Value);
			temp = temp->_Next;
		}
		
		// &_data;
	}

	void _move(List&& other) {												// Generic move function for list
		std::swap(_data._Head, other._data._Head);

		_data._Size = other._data._Size;
		other._data._Size = 0;

		// &_data;
		// other.&_data;
	}

	template<class... Args>
	void _create_until_size(const size_t& newSize, Args&&... args) {		// Add elements until current size equals newSize
		while (_data._Size < newSize)
			emplace_back(custom::forward<Args>(args)...);
	}

	void _delete_until_size(const size_t& newSize) {						// Remove elements until current size equals newSize
		while (_data._Size > newSize)
			pop_back();
	}

	Node* _scroll_node(const size_t& index) const {							// Get object in the list at index position by going through all components
		if (index < _data._Size)
		{
			Node* temp = _data._Head->_Next;
			for (size_t i = 0; i < index; ++i)
				temp = temp->_Next;

			return temp;
		}

		return _data._Head;
	}
}; // END Linked List

CUSTOM_END