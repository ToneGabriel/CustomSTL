#pragma once
#include "xNode.h"
#include "Utility.h"

CUSTOM_BEGIN

template<class Traits>
class _HashTable;

template<class Type>
struct ListData 
{
	using ValueType			= Type;									// Type for stored values
	using Reference			= ValueType&;
	using ConstReference	= const Reference;
	using Pointer			= ValueType*;
	using ConstPointer		= const Pointer;

	using Node				= node::_DoubleNode<ValueType>;			// Node type
	using NodePtr			= Node*;

	size_t _Size			= 0;									// Number of Nodes held
	NodePtr _Head 			= nullptr;								// Head of list
};

template<class List>
class ListConstIterator
{
public:
	using Data			= typename List::Data;
	using ValueType		= typename List::ValueType;
	using Reference		= typename List::ConstReference;
	using Pointer		= typename List::ConstPointer;
	using NodePtr		= typename List::NodePtr;

	NodePtr _Ptr		= nullptr;
	const Data* _Data	= nullptr;

public:

	ListConstIterator() noexcept = default;

	explicit ListConstIterator(NodePtr nodePtr, const Data* data) noexcept
		:_Ptr(nodePtr), _Data(data) { /*Empty*/ }

	ListConstIterator& operator++() noexcept {
		CUSTOM_ASSERT(_Ptr != _Data->_Head, "Cannot increment end iterator...");
		_Ptr = _Ptr->_Next;
		return *this;
	}

	ListConstIterator operator++(int) noexcept {
		ListConstIterator temp = *this;
		++(*this);
		return temp;
	}

	ListConstIterator& operator--() noexcept {
		CUSTOM_ASSERT(_Ptr != _Data->_Head->_Next, "Cannot decrement begin iterator...");
		_Ptr = _Ptr->_Previous;
		return *this;
	}

	ListConstIterator operator--(int) noexcept {
		ListConstIterator temp = *this;
		--(*this);
		return temp;
	}

	Pointer operator->() const noexcept {
		CUSTOM_ASSERT(_Ptr != _Data->_Head, "Cannot access end iterator...");
		return &(**this);
	}

	Reference operator*() const noexcept {
		CUSTOM_ASSERT(_Ptr != _Data->_Head, "Cannot dereference end iterator...");
		return _Ptr->_Value;
	}

	bool operator==(const ListConstIterator& other) const noexcept {
		return _Ptr == other._Ptr;
	}

	bool operator!=(const ListConstIterator& other) const noexcept {
		return !(*this == other);
	}

public:

	bool is_begin() const noexcept {
		return _Ptr == _Data->_Head->_Next;
	}

	bool is_end() const noexcept {
		return _Ptr == _Data->_Head;
	}
}; // END ListConstIterator

template<class List>
class ListIterator : public ListConstIterator<List>		// Linked List Iterator
{
private:
	using Base		= ListConstIterator<List>;

public:
	using Data 		= typename List::Data;
	using ValueType = typename List::ValueType;
	using Reference = typename List::Reference;
	using Pointer 	= typename List::Pointer;
	using NodePtr	= typename List::NodePtr;

public:

	ListIterator() noexcept = default;

	explicit ListIterator(NodePtr nodePtr, const Data* data) noexcept
		: Base(nodePtr, data) { /*Empty*/ }

	ListIterator& operator++() noexcept {
		Base::operator++();
		return *this;
	}

	ListIterator operator++(int) noexcept {
		ListIterator temp = *this;
		Base::operator++();
		return temp;
	}

	ListIterator& operator--() noexcept {
		Base::operator--();
		return *this;
	}

	ListIterator operator--(int) noexcept {
		ListIterator temp = *this;
		Base::operator--();
		return temp;
	}

	Pointer operator->() const noexcept {
		return const_cast<Pointer>(Base::operator->());
	}

	Reference operator*() const noexcept {
		return const_cast<Reference>(Base::operator*());
	}
}; // END ListIterator


template<class Type, class Alloc = custom::Allocator<Type>>
class List				// Doubly Linked List
{
private:
	template<class>
	friend class _HashTable;												// Needed in _HashTable class

public:
	static_assert(IsSame_v<Type, typename Alloc::ValueType>, "Object type and Allocator type must be the same!");
	static_assert(IsObject_v<Type>, "Containers require object type!");

	using Data 					= ListData<Type>;							// Members that are modified
	using ValueType 			= typename Data::ValueType;					// Type for stored values
	using Reference				= typename Data::Reference;
	using ConstReference		= typename Data::ConstReference;
	using Pointer				= typename Data::Pointer;
	using ConstPointer			= typename Data::ConstPointer;
	using AllocatorType			= Alloc;

	using Node 					= typename Data::Node;						// Node in list
	using NodePtr				= typename Data::NodePtr;
	using NodeAllocatorType		= custom::Allocator<Node>;

	using Iterator				= ListIterator<List<ValueType>>;			// Iterator type
	using ConstIterator			= ListConstIterator<List<ValueType>>;		// Const Iterator type
	using ReverseIterator		= custom::ReverseIterator<Iterator>;		// Reverse Iterator type
	using ConstReverseIterator	= custom::ReverseIterator<ConstIterator>;	// Const Reverse Iterator type

private:
	Data _data;																// Actual container data
	NodeAllocatorType _alloc;												// Allocator for nodes

public:
	// Constructors

	List() {
		_create_head();
	}

	List(const size_t& newSize, const ValueType& value) : List() {	// Add multiple copies Constructor
		_create_until_size(newSize, value);
	}

	List(const List& other) : List() {
		_copy(other);
	}

	List(List&& other) noexcept : List() {
		_move(custom::move(other));
	}

	~List() noexcept {
		clear();
		_free_head();
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

public:
	// Main functions

	void resize(const size_t& newSize) {						// Resize the list by removing or adding default elements to the tail
		_delete_until_size(newSize);
		_create_until_size(newSize);
	}

	void resize(const size_t& newSize,
				const ValueType& copyValue) {					// Resize the list by removing or adding copy elements to the tail
		_delete_until_size(newSize);
		_create_until_size(newSize, copyValue);
	}

	template<class... Args>
	void emplace_back(Args&&... args) {							// Construct object using arguments (Args) and add it to the tail
		AllocatorType al{};
		Node* newNode = _alloc.allocate(1);
		al.construct(&(newNode->_Value), custom::forward<Args>(args)...);
		_insert_node_before(_data._Head, newNode);
	}

	void push_back(const ValueType& copyValue) {				// Construct object using reference and add it to the tail
		emplace_back(copyValue);
	}

	void push_back(ValueType&& moveValue) {						// Construct object using temporary and add it to the tail
		emplace_back(custom::move(moveValue));
	}
	
	void pop_back() {											// Remove last component
		if (_data._Size > 0)
			_remove_node(_data._Head->_Previous);
	}

	template<class... Args>
	void emplace_front(Args&&... args) {						// Construct object using arguments (Args) and add it to the head
		AllocatorType al{};
		Node* newNode = _alloc.allocate(1);
		al.construct(&(newNode->_Value), custom::forward<Args>(args)...);
		_insert_node_before(_data._Head->_Next, newNode);
	}

	void push_front(const ValueType& copyValue) {				// Construct object using reference and add it to the head
		emplace_front(copyValue);
	}

	void push_front(ValueType&& moveValue) {					// Construct object using temporary and add it to the head
		emplace_front(custom::move(moveValue));
	}

	void pop_front() {											// Remove first component
		if (_data._Size > 0)
			_remove_node(_data._Head->_Next);
	}

	template<class... Args>
	Iterator emplace(ConstIterator iterator, Args&&... args) {				// Construct object using arguments (Args) and add it BEFORE the iterator position
		AllocatorType al{};
		Node* temp		= iterator._Ptr;
		Node* newNode	= _alloc.allocate(1);
		al.construct(&(newNode->_Value), custom::forward<Args>(args)...);
		_insert_node_before(temp, newNode);

		return Iterator(newNode, &_data);
	}

	Iterator push(ConstIterator iterator, const ValueType& copyValue) {		// Construct object using reference and add it BEFORE the iterator position
		return emplace(iterator, copyValue);
	}

	Iterator push(ConstIterator iterator, ValueType&& moveValue) {			// Construct object using temporary and add it BEFORE the iterator position
		return emplace(iterator, custom::move(moveValue));
	}

	Iterator pop(ConstIterator iterator) {									// Remove component at iterator position
		if (iterator.is_end())
			throw std::out_of_range("Cannot pop end iterator...");

		Node* temp 				= iterator._Ptr;
		Iterator prevIterator 	= Iterator(temp->_Previous, &_data);
		_remove_node(temp);

		return prevIterator;
	}

	Reference front() noexcept {								// Get the value of the first component
		CUSTOM_ASSERT(!empty(), "Container is empty...");
		return _data._Head->_Next->_Value;
	}

	ConstReference front() const noexcept {
		CUSTOM_ASSERT(!empty(), "Container is empty...");
		return _data._Head->_Next->_Value;
	}

	Reference back() noexcept {									// Get the value of the last component
		CUSTOM_ASSERT(!empty(), "Container is empty...");
		return _data._Head->_Previous->_Value;
	}

	ConstReference back() const noexcept {
		CUSTOM_ASSERT(!empty(), "Container is empty...");
		return _data._Head->_Previous->_Value;
	}

	Reference at(const size_t& index) {
		if (index >= _data._Size)
			throw std::out_of_range("Index out of bounds...");

		return _scroll_node(index)->_Value;
	}

	ConstReference at(const size_t& index) const {
		if (index >= _data._Size)
			throw std::out_of_range("Index out of bounds...");

		return _scroll_node(index)->_Value;
	}

	size_t size() const noexcept {
		return _data._Size;
	}

	// TODO: implement
	// size_t max_size() const noexcept {
	
	// }

	bool empty() const noexcept {
		return _data._Size == 0;
	}

	void clear() {
		_delete_until_size(0);
	}

public:
	// Iterator specific functions

	Iterator begin() noexcept {
		return Iterator(_data._Head->_Next, &_data);
	}

	ConstIterator begin() const noexcept {
		return ConstIterator(_data._Head->_Next, &_data);
	}

	ReverseIterator rbegin() noexcept {
		return ReverseIterator(end());
	}

	ConstReverseIterator rbegin() const noexcept {
		return ConstReverseIterator(end());
	}

	Iterator end() noexcept {
		return Iterator(_data._Head, &_data);
	}

	ConstIterator end() const noexcept {
		return ConstIterator(_data._Head, &_data);
	}

	ReverseIterator rend() noexcept {
		return ReverseIterator(begin());
	}

	ConstReverseIterator rend() const noexcept {
		return ConstReverseIterator(begin());
	}

private:
	// Others

	void _create_head() {
		_data._Head 			= _alloc.allocate(1);
		_data._Head->_Next 		= _data._Head;
		_data._Head->_Previous 	= _data._Head;
	}

	void _free_head() {
		_data._Head->_Next 		= nullptr;
		_data._Head->_Previous 	= nullptr;
		_alloc.deallocate(_data._Head, 1);
	}

	void _insert_node_before(Node* beforeNode, Node* newNode) {	// Insert Node before another
		newNode->_Previous 				= beforeNode->_Previous;
		newNode->_Next 					= beforeNode;

		beforeNode->_Previous->_Next 	= newNode;
		beforeNode->_Previous 			= newNode;

		++_data._Size;
	}

	void _remove_node(Node* junkNode) {							// Remove Node and relink
		AllocatorType al{};

		junkNode->_Previous->_Next = junkNode->_Next;
		junkNode->_Next->_Previous = junkNode->_Previous;
		--_data._Size;

		al.destroy(&(junkNode->_Value));
		_alloc.deallocate(junkNode, 1);
	}

	void _copy(const List& other) {								// Generic copy function for list
		Node* temp = other._data._Head->_Next;
		while (_data._Size < other._data._Size) {
			push_back(temp->_Value);
			temp = temp->_Next;
		}
	}

	void _move(List&& other) noexcept {							// Generic move function for list
		custom::swap(_data._Head, other._data._Head);

		_data._Size 		= other._data._Size;
		other._data._Size 	= 0;
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


// List binary operators
template<class _Type/*, class _Alloc*/>
bool operator==(const List<_Type/*, class _Alloc*/>& left, const List<_Type/*, class _Alloc*/>& right) {
    if (left.size() != right.size())
		return false;

	return custom::equal(left.begin(), left.end(), right.begin());
}

template<class _Type/*, class _Alloc*/>
bool operator!=(const List<_Type/*, class _Alloc*/>& left, const List<_Type/*, class _Alloc*/>& right) {
	return !(left == right);
}

CUSTOM_END