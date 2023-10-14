#pragma once
#include "xNode.h"
#include "xMemory.h"
#include "Utility.h"
#include "Iterator.h"

//#define FWD_LIST_NOT_IMPLEMENTED

#ifndef FWD_LIST_NOT_IMPLEMENTED

CUSTOM_BEGIN

template<class Type, class Alloc>
struct _ForwardListData
{
	// deduce data types and forward them
	using _AllocTraits		= AllocatorTraits<Alloc>;
	using _Node				= node::_ForwardNode<Type>;
	using _AllocNode		= typename _AllocTraits::template RebindAlloc<_Node>;
	using _AllocNodeTraits	= AllocatorTraits<_AllocNode>;
	using _NodePtr			= _AllocNodeTraits::Pointer;

	using ValueType			= typename _AllocTraits::ValueType;
	using DifferenceType 	= typename _AllocTraits::DifferenceType;
	using Reference			= typename _AllocTraits::Reference;
	using ConstReference	= typename _AllocTraits::ConstReference;
	using Pointer			= typename _AllocTraits::Pointer;
	using ConstPointer		= typename _AllocTraits::ConstPointer;

	size_t _Size			= 0;									// Number of Nodes held
	_NodePtr _Head 			= nullptr;								// Head of list
};

template<class ForwardListData>
class ForwardListConstIterator
{
private:
	using _Data				= ForwardListData;
	using _NodePtr			= typename _Data::_NodePtr;

public:
    using IteratorCategory 	= ForwardIteratorTag;
	using ValueType			= typename _Data::ValueType;
	using DifferenceType 	= typename _Data::DifferenceType;
	using Reference			= typename _Data::ConstReference;
	using Pointer			= typename _Data::ConstPointer;

	_NodePtr _Ptr			= nullptr;
	const _Data* _RefData	= nullptr;

public:

	ForwardListConstIterator() noexcept = default;

	explicit ForwardListConstIterator(_NodePtr nodePtr, const _Data* data) noexcept
		:_Ptr(nodePtr), _RefData(data) { /*Empty*/ }

	ForwardListConstIterator& operator++() noexcept {
		CUSTOM_ASSERT(_Ptr != _RefData->_Head, "Cannot increment end iterator...");
		_Ptr = _Ptr->_Next;
		return *this;
	}

	ForwardListConstIterator operator++(int) noexcept {
		ForwardListConstIterator temp = *this;
		++(*this);
		return temp;
	}

	Pointer operator->() const noexcept {
		return PointerTraits<Pointer>::pointer_to(**this);	//return &(**this); calls operator*
	}

	Reference operator*() const noexcept {
		CUSTOM_ASSERT(_Ptr != _RefData->_Head, "Cannot dereference end iterator...");
		return _Ptr->_Value;
	}

	bool operator==(const ForwardListConstIterator& other) const noexcept {
		return _Ptr == other._Ptr;
	}

	bool operator!=(const ForwardListConstIterator& other) const noexcept {
		return !(*this == other);
	}

public:

	bool is_begin() const noexcept {
		return _Ptr == _RefData->_Head->_Next;
	}

	bool is_end() const noexcept {
		return _Ptr == _RefData->_Head;
	}

	friend void _verify_range(const ForwardListConstIterator& first, const ForwardListConstIterator& last) noexcept {
		CUSTOM_ASSERT(first._RefData == last._RefData, "ForwardList iterators in range are from different containers");
		// No possible way to determine order.
	}
}; // END ForwardListConstIterator

template<class ForwardListData>
class ForwardListIterator : public ForwardListConstIterator<ForwardListData>		// Linked List Iterator
{
private:
	using _Base				= ForwardListConstIterator<ForwardListData>;
	using _Data 			= ForwardListData;
	using _NodePtr			= typename _Data::_NodePtr;

public:
    using IteratorCategory 	= ForwardIteratorTag;
	using ValueType 		= typename _Data::ValueType;
	using DifferenceType 	= typename _Data::DifferenceType;
	using Reference 		= typename _Data::Reference;
	using Pointer 			= typename _Data::Pointer;

public:

	ForwardListIterator() noexcept = default;

	explicit ForwardListIterator(_NodePtr nodePtr, const _Data* data) noexcept
		: _Base(nodePtr, data) { /*Empty*/ }

	ForwardListIterator& operator++() noexcept {
		_Base::operator++();
		return *this;
	}

	ForwardListIterator operator++(int) noexcept {
		ForwardListIterator temp = *this;
		_Base::operator++();
		return temp;
	}

	Pointer operator->() const noexcept {
		return const_cast<Pointer>(_Base::operator->());
	}

	Reference operator*() const noexcept {
		return const_cast<Reference>(_Base::operator*());
	}
}; // END ForwardListIterator


template<class Type, class Alloc = custom::Allocator<Type>>
class ForwardList				// Singly Linked List
{
private:
	using _Data 				= _ForwardListData<Type, Alloc>;
	using _AllocTraits			= typename _Data::_AllocTraits;
	using _Node					= typename _Data::_Node;
	using _AllocNode			= typename _Data::_AllocNode;
	using _AllocNodeTraits		= typename _Data::_AllocNodeTraits;
	using _NodePtr				= typename _Data::_NodePtr;

public:
	static_assert(IsSame_v<Type, typename Alloc::ValueType>, "Object type and Allocator type must be the same!");
	static_assert(IsObject_v<Type>, "Containers require object type!");

	using ValueType 			= typename _Data::ValueType;
	using DifferenceType 		= typename _Data::DifferenceType;
	using Reference				= typename _Data::Reference;
	using ConstReference		= typename _Data::ConstReference;
	using Pointer				= typename _Data::Pointer;
	using ConstPointer			= typename _Data::ConstPointer;
	using AllocatorType			= Alloc;

	using Iterator				= ListIterator<_Data>;						// Iterator type
	using ConstIterator			= ListConstIterator<_Data>;					// Const Iterator type

private:
	_Data _data;															// Actual container data
	_AllocNode _alloc;														// Allocator for nodes

public:
	// Constructors

	ForwardList() {
		_create_head();
	}

	ForwardList(const size_t newSize, const ValueType& value) : ForwardList() {	// Add multiple copies Constructor
		_create_until_size(newSize, value);
	}

	ForwardList(const ForwardList& other) : ForwardList() {
		_copy(other);
	}

	ForwardList(ForwardList&& other) noexcept : ForwardList() {
		_move(custom::move(other));
	}

	~ForwardList() noexcept {
		clear();
		_free_head();
	}

public:
	// Operators

	ForwardList& operator=(const ForwardList& other) {
		if (_data._Head != other._data._Head)
		{
			clear();
			_copy(other);
		}

		return *this;
	}

	ForwardList& operator=(ForwardList&& other) noexcept {
		if (_data._Head != other._data._Head)
		{
			clear();
			_move(custom::move(other));
		}

		return *this;
	}

public:
	// Main functions

	void resize(const size_t newSize) {						// Resize the list by removing or adding default elements to the tail
		_delete_until_size(newSize);
		_create_until_size(newSize);
	}

	void resize(const size_t newSize,
				const ValueType& copyValue) {					// Resize the list by removing or adding copy elements to the tail
		_delete_until_size(newSize);
		_create_until_size(newSize, copyValue);
	}

	template<class... Args>
	void emplace_front(Args&&... args) {						// Construct object using arguments (Args) and add it to the tail
		_NodePtr newNode = _alloc.allocate(1);
		_AllocNodeTraits::construct(_alloc, &(newNode->_Value), custom::forward<Args>(args)...);
		_insert_node_after(_data._Head, newNode);
	}

	void push_front(const ValueType& copyValue) {				// Construct object using reference and add it to the tail
		emplace_front(copyValue);
	}

	void push_front(ValueType&& moveValue) {					// Construct object using temporary and add it to the tail
		emplace_front(custom::move(moveValue));
	}

	void pop_front() {											// Remove last component
		if (_data._Size > 0)
			_remove_node_after(_data._Head);
	}

	template<class... Args>
	Iterator emplace_after(ConstIterator iterator, Args&&... args) {	// Construct object using arguments (Args) and add it AFTER the iterator position
		if (iterator.is_end())
			throw std::out_of_range("Cannot emplace after end iterator...");

		_NodePtr newNode = _alloc.allocate(1);
		_AllocNodeTraits::construct(_alloc, &(newNode->_Value), custom::forward<Args>(args)...);
		_insert_node_after(iterator._Ptr, newNode);

		return Iterator(newNode, &_data);
	}

	Iterator pop_after(ConstIterator iterator) {						// Remove component after iterator position
		if (iterator.is_end())
			throw std::out_of_range("Cannot pop after end iterator...");

		_NodePtr temp 			= iterator._Ptr;
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

	size_t size() const noexcept {
		return _data._Size;
	}

	size_t max_size() const noexcept {
        return (custom::min)(	static_cast<size_t>((NumericLimits<DifferenceType>::max)()),
								_AllocNodeTraits::max_size(_alloc));
    }

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

	Iterator end() noexcept {
		return Iterator(_data._Head, &_data);
	}

	ConstIterator end() const noexcept {
		return ConstIterator(_data._Head, &_data);
	}

private:
	// Others

	void _create_head() {
		_data._Head 			= _alloc.allocate(1);
		_data._Head->_Next 		= _data._Head;
	}

	void _free_head() {
		_data._Head->_Next 		= nullptr;
		_alloc.deallocate(_data._Head, 1);
	}

	void _insert_node_after(_NodePtr afterNode, _NodePtr newNode) {
		newNode->_Next = afterNode->_Next;
		afterNode->_Next = newNode;
		++_data._Size;
	}

	void _remove_node_after(_NodePtr afterNode) {
		_NodePtr junkNode = afterNode->_Next;
		afterNode->_Next = junkNode->_Next;
		--_data._Size;

		_AllocNodeTraits::destroy(_alloc, &(junkNode->_Value));
		_alloc.deallocate(junkNode, 1);
	}

	void _copy(const ForwardList& other) {								// Generic copy function for list
		_NodePtr temp = other._data._Head->_Next;
		while (_data._Size < other._data._Size) {
			push_front(temp->_Value);
			temp = temp->_Next;
		}
	}

	void _move(ForwardList&& other) noexcept {							// Generic move function for list
		custom::swap(_data._Head, other._data._Head);
		_data._Size = custom::exchange(other._data._Size, 0);
	}

	template<class... Args>
	void _create_until_size(const size_t newSize, Args&&... args) {		// Add elements until current size equals newSize
		while (_data._Size < newSize)
			emplace_front(custom::forward<Args>(args)...);
	}

	void _delete_until_size(const size_t newSize) {						// Remove elements until current size equals newSize
		while (_data._Size > newSize)
			pop_front();
	}
};	// END ForwardList


// ForwardList binary operators
template<class _Type, class _Alloc>
bool operator==(const ForwardList<_Type, _Alloc>& left, const ForwardList<_Type, _Alloc>& right) {
    if (left.size() != right.size())
		return false;

	return custom::equal(left.begin(), left.end(), right.begin());
}

template<class _Type, class _Alloc>
bool operator!=(const ForwardList<_Type, _Alloc>& left, const ForwardList<_Type, _Alloc>& right) {
	return !(left == right);
}

CUSTOM_END

#endif	// not implemented