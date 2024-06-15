#pragma once
#include "x_node.h"
#include "x_memory.h"
#include "c_utility.h"
#include "c_iterator.h"
#include "c_functional.h"


CUSTOM_BEGIN

template<class Type, class Alloc>
struct _ForwardListData
{
	// deduce data types and forward them
	using _AllocTraits		= allocator_traits<Alloc>;
	using _Node				= detail::_ForwardNode<Type>;
	using _AllocNode		= typename _AllocTraits::template rebind_alloc<_Node>;
	using _AllocNodeTraits	= allocator_traits<_AllocNode>;
	using _NodePtr			= typename _AllocNodeTraits::pointer;

	using value_type			= typename _AllocTraits::value_type;
	using DifferenceType 	= typename _AllocTraits::DifferenceType;
	using reference			= typename _AllocTraits::reference;
	using const_reference	= typename _AllocTraits::const_reference;
	using pointer			= typename _AllocTraits::pointer;
	using const_pointer		= typename _AllocTraits::const_pointer;

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
    using iterator_category 	= forward_iterator_tag;
	using value_type			= typename _Data::value_type;
	using DifferenceType 	= typename _Data::DifferenceType;
	using reference			= typename _Data::const_reference;
	using pointer			= typename _Data::const_pointer;

	_NodePtr _Ptr			= nullptr;
	const _Data* _RefData	= nullptr;

public:

	ForwardListConstIterator() noexcept = default;

	explicit ForwardListConstIterator(_NodePtr nodePtr, const _Data* data) noexcept
		:_Ptr(nodePtr), _RefData(data) { /*Empty*/ }

	ForwardListConstIterator& operator++() noexcept {
		CUSTOM_ASSERT(_Ptr != nullptr, "Cannot increment end iterator.");
		_Ptr = _Ptr->_Next;
		return *this;
	}

	ForwardListConstIterator operator++(int) noexcept {
		ForwardListConstIterator temp = *this;
		++(*this);
		return temp;
	}

	pointer operator->() const noexcept {
		return pointer_traits<pointer>::pointer_to(**this);	//return &(**this); calls operator*
	}

	reference operator*() const noexcept {
		CUSTOM_ASSERT(_Ptr != nullptr, "Cannot dereference end iterator.");
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
		return _Ptr == nullptr;
	}

	bool is_last_valid() const noexcept {
		return _Ptr->_Next == nullptr;
	}

	friend void _verify_range(const ForwardListConstIterator& first, const ForwardListConstIterator& last) noexcept {
		CUSTOM_ASSERT(first._RefData == last._RefData, "ForwardList iterators in range are from different containers");
		// No possible way to determine order.
	}
}; // END ForwardListConstIterator

template<class ForwardListData>
class ForwardListIterator : public ForwardListConstIterator<ForwardListData>		// Linked list iterator
{
private:
	using _Base				= ForwardListConstIterator<ForwardListData>;
	using _Data 			= ForwardListData;
	using _NodePtr			= typename _Data::_NodePtr;

public:
    using iterator_category 	= forward_iterator_tag;
	using value_type 		= typename _Data::value_type;
	using DifferenceType 	= typename _Data::DifferenceType;
	using reference 		= typename _Data::reference;
	using pointer 			= typename _Data::pointer;

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

	pointer operator->() const noexcept {
		return const_cast<pointer>(_Base::operator->());
	}

	reference operator*() const noexcept {
		return const_cast<reference>(_Base::operator*());
	}
}; // END ForwardListIterator


template<class Type, class Alloc = custom::allocator<Type>>
class ForwardList				// Singly Linked list
{
private:
	using _Data 				= _ForwardListData<Type, Alloc>;
	using _AllocTraits			= typename _Data::_AllocTraits;
	using _Node					= typename _Data::_Node;
	using _AllocNode			= typename _Data::_AllocNode;
	using _AllocNodeTraits		= typename _Data::_AllocNodeTraits;
	using _NodePtr				= typename _Data::_NodePtr;

public:
	static_assert(is_same_v<Type, typename Alloc::value_type>, "Object type and allocator type must be the same!");
	static_assert(is_object_v<Type>, "Containers require object type!");

	using value_type 			= typename _Data::value_type;
	using DifferenceType 		= typename _Data::DifferenceType;
	using reference				= typename _Data::reference;
	using const_reference		= typename _Data::const_reference;
	using pointer				= typename _Data::pointer;
	using const_pointer			= typename _Data::const_pointer;
	using allocator_type			= Alloc;

	using iterator				= ForwardListIterator<_Data>;				// iterator type
	using const_iterator			= ForwardListConstIterator<_Data>;			// Const iterator type

private:
	_Data _data;															// Actual container data
	_AllocNode _alloc;														// allocator for nodes

public:
	// Constructors

	ForwardList() {
		_create_head();
	}

	ForwardList(const size_t newSize, const value_type& value) : ForwardList() {	// Add multiple copies Constructor
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
				const value_type& copyValue) {					// Resize the list by removing or adding copy elements to the tail
		_delete_until_size(newSize);
		_create_until_size(newSize, copyValue);
	}

	template<class... Args>
	void emplace_front(Args&&... args) {						// Construct object using arguments (Args) and add it to the tail
		_NodePtr newNode = _create_common_node(custom::forward<Args>(args)...);
		_insert_node_after(_data._Head, newNode);
	}

	void push_front(const value_type& copyValue) {				// Construct object using reference and add it to the tail
		emplace_front(copyValue);
	}

	void push_front(value_type&& moveValue) {					// Construct object using temporary and add it to the tail
		emplace_front(custom::move(moveValue));
	}

	void pop_front() {											// Remove last component
		if (_data._Size > 0)
			_remove_node_after(_data._Head);
	}

	template<class... Args>
	iterator emplace_after(const_iterator where, Args&&... args) {	// Construct object using arguments (Args) and add it AFTER the where position
		if (where.is_end())
			throw std::out_of_range("Cannot emplace after end iterator.");

		_NodePtr newNode = _create_common_node(custom::forward<Args>(args)...);
		_insert_node_after(where._Ptr, newNode);

		return iterator(newNode, &_data);
	}

	iterator pop_after(const_iterator where) {						// Remove component after where position
		if (where.is_end() || where.is_last_valid())
			throw std::out_of_range("Cannot pop after end iterator.");

		_NodePtr temp 			= where._Ptr;
		iterator nextIterator 	= iterator(temp->_Next, &_data);
		_remove_node_after(temp);

		return nextIterator;
	}

	reference front() noexcept {								// Get the value of the first component
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Head->_Next->_Value;
	}

	const_reference front() const noexcept {
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Head->_Next->_Value;
	}

	size_t size() const noexcept {
		return _data._Size;
	}

	size_t max_size() const noexcept {
        return (custom::min)(	static_cast<size_t>((numeric_limits<DifferenceType>::max)()),
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
			_NodePtr current 	= _data._Head->_Next;
			_NodePtr prev 		= nullptr;
			_NodePtr next 		= nullptr;
	
			while (current != nullptr)
			{
				next 			= current->_Next;
				current->_Next 	= prev;
				prev 			= current;
				current 		= next;
			}

			_data._Head->_Next 	= prev;
		}
    }

	template<class UnaryPredicate>
	size_t remove_if(UnaryPredicate pred) {
		size_t oldSize 		= _data._Size;
		_NodePtr current 	= _data._Head->_Next;
		_NodePtr prev 		= _data._Head;
		_NodePtr next 		= nullptr;

		while (current != nullptr)
		{
			next = current->_Next;

			if (pred(current->_Value))
				_remove_node_after(prev);	// remove current
			else
				prev = prev->_Next;

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

		while (next != nullptr)
		{
			if (pred(current->_Value, next->_Value))
				_remove_node_after(current);
			else
				current = next;

			next = current->_Next;
		}

		return oldSize - _data._Size;
	}

	size_t unique() {
		return unique(equal_to<>{});
	}

	void splice_after(const_iterator where, ForwardList& other, const_iterator otherFirst, const_iterator otherLast) {
		// splice (otherFirst, otherLast) AFTER where

		if (where._RefData->_Head == otherFirst._RefData->_Head ||
			otherFirst._RefData->_Head != otherLast._RefData->_Head)
			throw std::domain_error("ForwardList provided by otherFirst and otherLast must be the same, but different from the one provided by where");

		size_t count = static_cast<size_t>(custom::distance(otherFirst, otherLast) - 1);

		if (max_size() - _data._Size < count)
			throw std::out_of_range("ForwardList too long");

		if (count != 0)		// worth splicing
		{
			// do sizes
			_data._Size 		+= count;
			other._data._Size 	-= count;

			// to node ptrs from iterators
			_NodePtr thisFirst 	= where._Ptr;					// here otherFirst->_Next should link to (AFTER where)
			_NodePtr oFirst 	= otherFirst._Ptr;				// otherFirst node ptr (not included)
			_NodePtr oLast 		= otherLast._Ptr;				// otherLast node ptr (not included)
			
			// take nodes from other list and link them to this one
			_splice_after(thisFirst, oFirst, oLast);
		}
	}

	void splice_after(const_iterator where, ForwardList& other, const_iterator otherFirst) {
		splice_after(where, other, otherFirst, other.end());
	}

	void splice_after(const_iterator where, ForwardList& other) {
		splice_after(where, other, other.before_begin(), other.end());
	}

	template<class Compare>
	void merge(ForwardList& other, Compare comp) {
		// TODO: implement
	}

	void merge(ForwardList& other) {
		merge(other, less<>{});
	}

	template<class Compare>
	void sort(Compare comp) {
		// TODO: implement
	}

	void sort() {
		sort(less<>{});
	}

public:
	// iterator specific functions

	iterator before_begin() noexcept {
        return iterator(_data._Head, &_data);
    }

    const_iterator before_begin() const noexcept {
        return const_iterator(_data._Head, &_data);
    }

	iterator begin() noexcept {
		return iterator(_data._Head->_Next, &_data);
	}

	const_iterator begin() const noexcept {
		return const_iterator(_data._Head->_Next, &_data);
	}

	iterator end() noexcept {
		return iterator(nullptr, &_data);
	}

	const_iterator end() const noexcept {
		return const_iterator(nullptr, &_data);
	}

private:
	// Others

	void _create_head() {
		_data._Head 			= _alloc.allocate(1);
		_data._Head->_Next 		= nullptr;
	}

	void _free_head() {
		_alloc.deallocate(_data._Head, 1);
	}

	template<class... Args>
	_NodePtr _create_common_node(Args&&... args) {
		// don't increase size here
		_NodePtr newNode = _alloc.allocate(1);
		_AllocNodeTraits::construct(_alloc, &(newNode->_Value), custom::forward<Args>(args)...);
		return newNode;
	}

	void _insert_node_after(_NodePtr afterNode, _NodePtr newNode) {
		newNode->_Next 		= afterNode->_Next;
		afterNode->_Next 	= newNode;
		++_data._Size;
	}

	void _remove_node_after(_NodePtr afterNode) {
		_NodePtr junkNode 	= afterNode->_Next;
		afterNode->_Next 	= junkNode->_Next;
		--_data._Size;

		_AllocNodeTraits::destroy(_alloc, &(junkNode->_Value));
		_alloc.deallocate(junkNode, 1);
	}

	void _copy(const ForwardList& other) {								// Generic copy function for list
		_NodePtr temp = other._data._Head->_Next;
		while (_data._Size < other._data._Size)
		{
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

	void _splice_after(_NodePtr thisFirst, _NodePtr otherFirst, _NodePtr otherLast) {
		// here beforeOtherLast should link to
		_NodePtr thisLast				= thisFirst->_Next;

		// get node before otherLast
		_NodePtr beforeOtherLast = otherFirst;
		while (beforeOtherLast->_Next != otherLast)
			beforeOtherLast = beforeOtherLast->_Next;

		// open this list and link to other ends
		thisFirst->_Next 				= otherFirst->_Next;
		beforeOtherLast->_Next 			= thisLast;

		// close other list
		otherFirst->_Next 				= otherLast;
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