#pragma once
#include "xNode.h"
#include "xMemory.h"
#include "cUtility.h"
#include "cIterator.h"
#include "cFunctional.h"


CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN	// used to declare _HashTable for friend req in List

template<class Traits>
class _HashTable;

CUSTOM_DETAIL_END


template<class Type, class Alloc>
struct _ListData
{
	// deduce data types and forward them
	using _AllocTraits		= AllocatorTraits<Alloc>;
	using _Node				= detail::_DoubleNode<Type>;
	using _AllocNode		= typename _AllocTraits::template RebindAlloc<_Node>;
	using _AllocNodeTraits	= AllocatorTraits<_AllocNode>;
	using _NodePtr			= typename _AllocNodeTraits::Pointer;

	using ValueType			= typename _AllocTraits::ValueType;
	using DifferenceType 	= typename _AllocTraits::DifferenceType;
	using Reference			= typename _AllocTraits::Reference;
	using ConstReference	= typename _AllocTraits::ConstReference;
	using Pointer			= typename _AllocTraits::Pointer;
	using ConstPointer		= typename _AllocTraits::ConstPointer;

	size_t _Size			= 0;									// Number of Nodes held
	_NodePtr _Head 			= nullptr;								// Head of list
};

template<class ListData>
class ListConstIterator
{
private:
	using _Data				= ListData;
	using _NodePtr			= typename _Data::_NodePtr;

public:
    using IteratorCategory 	= BidirectionalIteratorTag;
	using ValueType			= typename _Data::ValueType;
	using DifferenceType 	= typename _Data::DifferenceType;
	using Reference			= typename _Data::ConstReference;
	using Pointer			= typename _Data::ConstPointer;

	_NodePtr _Ptr			= nullptr;
	const _Data* _RefData	= nullptr;

public:

	ListConstIterator() noexcept = default;

	explicit ListConstIterator(_NodePtr nodePtr, const _Data* data) noexcept
		:_Ptr(nodePtr), _RefData(data) { /*Empty*/ }

	ListConstIterator& operator++() noexcept {
		CUSTOM_ASSERT(_Ptr != _RefData->_Head, "Cannot increment end iterator.");
		_Ptr = _Ptr->_Next;
		return *this;
	}

	ListConstIterator operator++(int) noexcept {
		ListConstIterator temp = *this;
		++(*this);
		return temp;
	}

	ListConstIterator& operator--() noexcept {
		CUSTOM_ASSERT(_Ptr != _RefData->_Head->_Next, "Cannot decrement begin iterator.");
		_Ptr = _Ptr->_Previous;
		return *this;
	}

	ListConstIterator operator--(int) noexcept {
		ListConstIterator temp = *this;
		--(*this);
		return temp;
	}

	Pointer operator->() const noexcept {
		return PointerTraits<Pointer>::pointer_to(**this);	//return &(**this); calls operator*
	}

	Reference operator*() const noexcept {
		CUSTOM_ASSERT(_Ptr != _RefData->_Head, "Cannot dereference end iterator.");
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
		return _Ptr == _RefData->_Head->_Next;
	}

	bool is_end() const noexcept {
		return _Ptr == _RefData->_Head;
	}

	friend void _verify_range(const ListConstIterator& first, const ListConstIterator& last) noexcept {
		CUSTOM_ASSERT(first._RefData == last._RefData, "List iterators in range are from different containers");
		// No possible way to determine order.
	}
}; // END ListConstIterator

template<class ListData>
class ListIterator : public ListConstIterator<ListData>		// Linked List Iterator
{
private:
	using _Base				= ListConstIterator<ListData>;
	using _Data 			= ListData;
	using _NodePtr			= typename _Data::_NodePtr;

public:
    using IteratorCategory 	= BidirectionalIteratorTag;
	using ValueType 		= typename _Data::ValueType;
	using DifferenceType 	= typename _Data::DifferenceType;
	using Reference 		= typename _Data::Reference;
	using Pointer 			= typename _Data::Pointer;

public:

	ListIterator() noexcept = default;

	explicit ListIterator(_NodePtr nodePtr, const _Data* data) noexcept
		: _Base(nodePtr, data) { /*Empty*/ }

	ListIterator& operator++() noexcept {
		_Base::operator++();
		return *this;
	}

	ListIterator operator++(int) noexcept {
		ListIterator temp = *this;
		_Base::operator++();
		return temp;
	}

	ListIterator& operator--() noexcept {
		_Base::operator--();
		return *this;
	}

	ListIterator operator--(int) noexcept {
		ListIterator temp = *this;
		_Base::operator--();
		return temp;
	}

	Pointer operator->() const noexcept {
		return const_cast<Pointer>(_Base::operator->());
	}

	Reference operator*() const noexcept {
		return const_cast<Reference>(_Base::operator*());
	}
}; // END ListIterator


template<class Type, class Alloc = custom::Allocator<Type>>
class List				// Doubly Linked List
{
private:
	template<class>
	friend class detail::_HashTable;												// Needed in _HashTable class

	using _Data 				= _ListData<Type, Alloc>;					// Members that are modified
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
	using ReverseIterator		= custom::ReverseIterator<Iterator>;		// Reverse Iterator type
	using ConstReverseIterator	= custom::ReverseIterator<ConstIterator>;	// Const Reverse Iterator type

private:
	_Data _data;															// Actual container data
	_AllocNode _alloc;														// Allocator for nodes

public:
	// Constructors

	List() {
		_create_head();
	}

	List(const size_t newSize, const ValueType& value) : List() {	// Add multiple copies Constructor
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
	void emplace_back(Args&&... args) {							// Construct object using arguments (Args) and add it to the tail
		_NodePtr newNode = _create_common_node(custom::forward<Args>(args)...);
		_link_node_before(_data._Head, newNode);
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
		_NodePtr newNode = _create_common_node(custom::forward<Args>(args)...);
		_link_node_before(_data._Head->_Next, newNode);
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
	Iterator emplace(ConstIterator where, Args&&... args) {				// Construct object using arguments (Args) and add it BEFORE the where position
		_NodePtr newNode = _create_common_node(custom::forward<Args>(args)...);
		_link_node_before(where._Ptr, newNode);

		return Iterator(newNode, &_data);
	}

	Iterator push(ConstIterator where, const ValueType& copyValue) {		// Construct object using reference and add it BEFORE the where position
		return emplace(where, copyValue);
	}

	Iterator push(ConstIterator where, ValueType&& moveValue) {			// Construct object using temporary and add it BEFORE the where position
		return emplace(where, custom::move(moveValue));
	}

	Iterator pop(ConstIterator where) {									// Remove component at where position
		if (where.is_end())
			throw std::out_of_range("Cannot pop end iterator.");

		_NodePtr temp 			= where._Ptr;
		Iterator prevIterator 	= Iterator(temp->_Previous, &_data);
		_remove_node(temp);

		return prevIterator;
	}

	Reference front() noexcept {								// Get the value of the first component
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Head->_Next->_Value;
	}

	ConstReference front() const noexcept {
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Head->_Next->_Value;
	}

	Reference back() noexcept {									// Get the value of the last component
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Head->_Previous->_Value;
	}

	ConstReference back() const noexcept {
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Head->_Previous->_Value;
	}

	Reference at(const size_t index) {
		if (index >= _data._Size)
			throw std::out_of_range("Index out of bounds.");

		return _scroll_node(index)->_Value;
	}

	ConstReference at(const size_t index) const {
		if (index >= _data._Size)
			throw std::out_of_range("Index out of bounds.");

		return _scroll_node(index)->_Value;
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
	// Main Operations

	void reverse() noexcept {
		if (_data._Size > 1)
		{
			_NodePtr current = _data._Head;

			do
			{
				custom::swap(current->_Next, current->_Previous);
				current = current->_Next;
			} while (current != _data._Head);
		}
	}

	template<class UnaryPredicate>
	size_t remove_if(UnaryPredicate pred) {
		size_t oldSize 		= _data._Size;
		_NodePtr current 	= _data._Head->_Next;
		_NodePtr next 		= nullptr;

		while (current != _data._Head)
		{
			next = current->_Next;

			if (pred(current->_Value))
				_remove_node(current);

			current = next;
		}

		return oldSize - _data._Size;
	}

	size_t remove(const Type& val) {
		return remove_if([&](const Type& other) -> bool { return other == val; });
	}

	template<class BinaryPredicate>
	size_t unique(BinaryPredicate pred) {
		size_t oldSize 		= _data._Size;
		_NodePtr current 	= _data._Head->_Next;
		_NodePtr next 		= current->_Next;

		while (next != _data._Head)
		{
			if (pred(current->_Value, next->_Value))
				_remove_node(next);
			else
				current = next;

			next = current->_Next;
		}

		return oldSize - _data._Size;
	}

	size_t unique() {
		return unique(EqualTo<>{});
	}

	void splice(ConstIterator where, List& other, ConstIterator otherFirst, ConstIterator otherLast) {
		// splice [otherFirst, otherLast) BEFORE where

		if (where._RefData->_Head == otherFirst._RefData->_Head ||
			otherFirst._RefData->_Head != otherLast._RefData->_Head)
			throw std::domain_error("List provided by otherFirst and otherLast must be the same, but different from the one provided by where");

		size_t count = static_cast<size_t>(custom::distance(otherFirst, otherLast));

		if (max_size() - _data._Size < count)
			throw std::out_of_range("List too long");

		if (count != 0)		// worth splicing
		{
			// do sizes
			_data._Size 		+= count;
			other._data._Size 	-= count;

			// to node ptrs from iterators
			_NodePtr thisFirst 	= where._Ptr->_Previous;		// here otherFirst should link to (BEFORE where)
			_NodePtr oFirst 	= otherFirst._Ptr;				// otherFirst node ptr
			_NodePtr oLast 		= otherLast._Ptr->_Previous;	// otherLast - 1 node ptr
			
			// take nodes from other list and link them to this one
			_splice(thisFirst, oFirst, oLast);
		}
	}

	void splice(ConstIterator where, List& other, ConstIterator otherFirst) {
		// splice [otherFirst, ...) BEFORE where
		splice(where, other, otherFirst, other.end());
	}

	void splice(ConstIterator where, List& other) {
		// spice ALL other list BEFORE where
		splice(where, other, other.begin(), other.end());
	}

	template<class Compare>
	void merge(List& other, Compare comp) {
		// both lists should be sorted
		if (_data._Head != other._data._Head && other._data._Size > 0)
		{
			_NodePtr midNode = other._data._Head->_Next;

			// add ALL other elems to the end of this list
			splice(end(), other);

			// merge the two sorted lists (now spliced in this) using midNode as delimiter
			_merge_internal(_data._Head->_Next, midNode, _data._Head, comp);
		}
	}

	void merge(List& other) {
		merge(other, Less<>{});
	}

	template<class Compare>
	void sort(Compare comp) {
		(void)_sort(_data._Head->_Next, _data._Size, comp);
	}

	void sort() {
		sort(Less<>{});
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

	template<class... Args>
	_NodePtr _create_common_node(Args&&... args) {
		// don't increase size here
		_NodePtr newNode = _alloc.allocate(1);
		_AllocNodeTraits::construct(_alloc, &(newNode->_Value), custom::forward<Args>(args)...);
		return newNode;
	}

	void _link_node_before(_NodePtr beforeNode, _NodePtr newNode) {	// Insert Node before another
		newNode->_Previous 				= beforeNode->_Previous;
		newNode->_Next 					= beforeNode;

		beforeNode->_Previous->_Next 	= newNode;
		beforeNode->_Previous 			= newNode;

		++_data._Size;
	}

	void _unlink_node(_NodePtr targetNode) {						// Unlink node from list
		CUSTOM_ASSERT(	targetNode->_Next != nullptr && targetNode->_Previous != nullptr,
						"Unlink only if linked");

		targetNode->_Previous->_Next	= targetNode->_Next;
		targetNode->_Next->_Previous	= targetNode->_Previous;

		targetNode->_Next				= nullptr;
		targetNode->_Previous			= nullptr;

		--_data._Size;
	}

	void _remove_node(_NodePtr junkNode) {						// Remove Node and relink
		_unlink_node(junkNode);
		_AllocNodeTraits::destroy(_alloc, &(junkNode->_Value));
		_alloc.deallocate(junkNode, 1);
	}

	void _copy(const List& other) {								// Generic copy function for list
		_NodePtr temp = other._data._Head->_Next;
		while (_data._Size < other._data._Size) {
			push_back(temp->_Value);
			temp = temp->_Next;
		}
	}

	void _move(List&& other) noexcept {							// Generic move function for list
		custom::swap(_data._Head, other._data._Head);
		_data._Size = custom::exchange(other._data._Size, 0);
	}

	template<class... Args>
	void _create_until_size(const size_t newSize, Args&&... args) {		// Add elements until current size equals newSize
		while (_data._Size < newSize)
			emplace_back(custom::forward<Args>(args)...);
	}

	void _delete_until_size(const size_t newSize) {						// Remove elements until current size equals newSize
		while (_data._Size > newSize)
			pop_back();
	}

	void _splice(_NodePtr thisFirst, _NodePtr otherFirst, _NodePtr otherLast) {
		// thisFirst, thisLast, otherFirst, otherLast are the EXACT ends of the lists

		// here otherLast should link to
		_NodePtr thisLast				= thisFirst->_Next;

		// open this list and link to other ends
		thisFirst->_Next 				= otherFirst;
		thisLast->_Previous 			= otherLast;

		// close other list
		otherFirst->_Previous->_Next 	= otherLast->_Next;
		otherLast->_Next->_Previous 	= otherFirst->_Previous;

		// link other ends to this list
		otherFirst->_Previous 			= thisFirst;
		otherLast->_Next 				= thisLast;
	}

	template<class Compare>
	void _merge_internal(_NodePtr first, _NodePtr mid, _NodePtr last, Compare comp) {
        // merge the sorted ranges [first, mid) and [mid, last)

		_NodePtr runStart = nullptr;

		for (;;)
		{
			runStart = mid;

			// mid becomes first element >= first, starting from runStart
			while (mid != last && comp(mid->_Value, first->_Value))
				mid = mid->_Next;

			// [runStart, mid) goes before first
			if (runStart != mid)
				_splice(first->_Previous, runStart, mid->_Previous);

			// check end of list
			if (mid == last)
				return;

			// increase first so that next range [runStart, mid) goes before it
			do
			{
				first = first->_Next;
				if (first == mid)
					return;
			} while (!comp(mid->_Value, first->_Value));
		}
	}

	template<class Compare>
	_NodePtr _sort(_NodePtr first, size_t size, Compare comp) {
		// order [first, first + size), return first + size

		switch (size)
		{
			case 0:
				return first;
			case 1:
				return first->_Next;
			default:
				break;
		}

		_NodePtr mid	= _sort(first, size / 2, comp);
		_NodePtr last	= _sort(mid, size - size / 2, comp);
		_merge_internal(_data._Head->_Next, mid, last, comp);

		return last;
	}

	_NodePtr _scroll_node(const size_t index) const {							// Get object in the list at index position by going through all components
		if (index < _data._Size)
		{
			_NodePtr temp = _data._Head->_Next;
			for (size_t i = 0; i < index; ++i)
				temp = temp->_Next;

			return temp;
		}

		return _data._Head;
	}
}; // END Linked List


// List binary operators
template<class _Type, class _Alloc>
bool operator==(const List<_Type, _Alloc>& left, const List<_Type, _Alloc>& right) {
    if (left.size() != right.size())
		return false;

	return custom::equal(left.begin(), left.end(), right.begin());
}

template<class _Type, class _Alloc>
bool operator!=(const List<_Type, _Alloc>& left, const List<_Type, _Alloc>& right) {
	return !(left == right);
}

CUSTOM_END