#pragma once
#include "x_node.h"
#include "x_memory.h"
#include "c_utility.h"
#include "c_iterator.h"
#include "c_functional.h"


CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN

template<class Traits>
class _Hash_Table; // declared as friend in list

template<class Type, class Alloc>
struct _List_Data
{
	// deduce data types and forward them
	using _Alloc_Traits			= allocator_traits<Alloc>;
	using _Node					= detail::_Double_Node<Type>;
	using _Alloc_Node			= typename _Alloc_Traits::template rebind_alloc<_Node>;
	using _Alloc_Node_Traits	= allocator_traits<_Alloc_Node>;
	using _NodePtr				= typename _Alloc_Node_Traits::pointer;

	using value_type			= typename _Alloc_Traits::value_type;
	using difference_type		= typename _Alloc_Traits::difference_type;
	using reference				= typename _Alloc_Traits::reference;
	using const_reference		= typename _Alloc_Traits::const_reference;
	using pointer				= typename _Alloc_Traits::pointer;
	using const_pointer			= typename _Alloc_Traits::const_pointer;

	size_t _Size				= 0;									// Number of Nodes held
	_NodePtr _Head				= nullptr;								// Head of list
};	// END _List_Data

template<class ListData>
class _List_Const_Iterator
{
private:
	using _Data				= ListData;
	using _NodePtr			= typename _Data::_NodePtr;

public:
    using iterator_category	= bidirectional_iterator_tag;
	using value_type		= typename _Data::value_type;
	using difference_type	= typename _Data::difference_type;
	using reference			= typename _Data::const_reference;
	using pointer			= typename _Data::const_pointer;

	_NodePtr _Ptr			= nullptr;
	const _Data* _RefData	= nullptr;

public:

	_List_Const_Iterator() noexcept = default;

	explicit _List_Const_Iterator(_NodePtr nodePtr, const _Data* data) noexcept
		:_Ptr(nodePtr), _RefData(data) { /*Empty*/ }

	_List_Const_Iterator& operator++() noexcept
	{
		CUSTOM_ASSERT(_Ptr != _RefData->_Head, "Cannot increment end iterator.");
		_Ptr = _Ptr->_Next;
		return *this;
	}

	_List_Const_Iterator operator++(int) noexcept
	{
		_List_Const_Iterator temp = *this;
		++(*this);
		return temp;
	}

	_List_Const_Iterator& operator--() noexcept
	{
		CUSTOM_ASSERT(_Ptr != _RefData->_Head->_Next, "Cannot decrement begin iterator.");
		_Ptr = _Ptr->_Previous;
		return *this;
	}

	_List_Const_Iterator operator--(int) noexcept 
	{
		_List_Const_Iterator temp = *this;
		--(*this);
		return temp;
	}

	pointer operator->() const noexcept
	{
		return pointer_traits<pointer>::pointer_to(**this);	//return &(**this); calls operator*
	}

	reference operator*() const noexcept
	{
		CUSTOM_ASSERT(_Ptr != _RefData->_Head, "Cannot dereference end iterator.");
		return _Ptr->_Value;
	}

	bool operator==(const _List_Const_Iterator& other) const noexcept
	{
		return _Ptr == other._Ptr;
	}

	bool operator!=(const _List_Const_Iterator& other) const noexcept
	{
		return !(*this == other);
	}

public:

	bool is_begin() const noexcept
	{
		return _Ptr == _RefData->_Head->_Next;
	}

	bool is_end() const noexcept
	{
		return _Ptr == _RefData->_Head;
	}

	friend void _verify_range(const _List_Const_Iterator& first, const _List_Const_Iterator& last) noexcept
	{
		CUSTOM_ASSERT(first._RefData == last._RefData, "list iterators in range are from different containers");
		// No possible way to determine order.
	}
}; // END _List_Const_Iterator

template<class ListData>
class _List_Iterator : public _List_Const_Iterator<ListData>
{
private:
	using _Base				= _List_Const_Iterator<ListData>;
	using _Data 			= ListData;
	using _NodePtr			= typename _Data::_NodePtr;

public:
    using iterator_category	= bidirectional_iterator_tag;
	using value_type 		= typename _Data::value_type;
	using difference_type 	= typename _Data::difference_type;
	using reference 		= typename _Data::reference;
	using pointer 			= typename _Data::pointer;

public:

	_List_Iterator() noexcept = default;

	explicit _List_Iterator(_NodePtr nodePtr, const _Data* data) noexcept
		: _Base(nodePtr, data) { /*Empty*/ }

	_List_Iterator& operator++() noexcept
	{
		_Base::operator++();
		return *this;
	}

	_List_Iterator operator++(int) noexcept
	{
		_List_Iterator temp = *this;
		_Base::operator++();
		return temp;
	}

	_List_Iterator& operator--() noexcept
	{
		_Base::operator--();
		return *this;
	}

	_List_Iterator operator--(int) noexcept
	{
		_List_Iterator temp = *this;
		_Base::operator--();
		return temp;
	}

	pointer operator->() const noexcept
	{
		return const_cast<pointer>(_Base::operator->());
	}

	reference operator*() const noexcept
	{
		return const_cast<reference>(_Base::operator*());
	}
}; // END _List_Iterator

CUSTOM_DETAIL_END

template<class Type, class Alloc = custom::allocator<Type>>
class list				// Doubly Linked list
{
private:
	template<class>
	friend class detail::_Hash_Table;												// Needed in _Hash_Table class

	using _Data 				= detail::_List_Data<Type, Alloc>;					// Members that are modified
	using _Alloc_Traits			= typename _Data::_Alloc_Traits;
	using _Node					= typename _Data::_Node;
	using _Alloc_Node			= typename _Data::_Alloc_Node;
	using _Alloc_Node_Traits	= typename _Data::_Alloc_Node_Traits;
	using _NodePtr				= typename _Data::_NodePtr;

public:
	static_assert(is_same_v<Type, typename Alloc::value_type>, "Object type and allocator type must be the same!");
	static_assert(is_object_v<Type>, "Containers require object type!");

	using value_type				= typename _Data::value_type;
	using difference_type			= typename _Data::difference_type;
	using reference					= typename _Data::reference;
	using const_reference			= typename _Data::const_reference;
	using pointer					= typename _Data::pointer;
	using const_pointer				= typename _Data::const_pointer;
	using allocator_type			= Alloc;

	using iterator					= detail::_List_Iterator<_Data>;
	using const_iterator			= detail::_List_Const_Iterator<_Data>;
	using reverse_iterator			= custom::reverse_iterator<iterator>;
	using const_reverse_iterator	= custom::reverse_iterator<const_iterator>;

private:
	_Data _data;															// Actual container data
	_Alloc_Node _alloc;														// allocator for nodes

public:
	// Constructors

	list()
	{
		_create_head();
	}

	list(const size_t newSize, const value_type& value) : list()
	{
		_create_until_size(newSize, value);
	}

	list(std::initializer_list<value_type> list)
	{
		for (const auto& val : list)
			push_back(val);
	}

	list(const list& other) : list()
	{
		_copy(other);
	}

	list(list&& other) noexcept : list()
	{
		_move(custom::move(other));
	}

	~list() noexcept
	{
		clear();
		_free_head();
	}

public:
	// Operators

	list& operator=(const list& other)
	{
		if (_data._Head != other._data._Head)
		{
			clear();
			_copy(other);
		}

		return *this;
	}

	list& operator=(list&& other) noexcept
	{
		if (_data._Head != other._data._Head)
		{
			clear();
			_move(custom::move(other));
		}

		return *this;
	}

public:
	// Main functions

	void resize(const size_t newSize)	// Resize the list by removing or adding default elements to the tail
	{
		_delete_until_size(newSize);
		_create_until_size(newSize);
	}

	void resize(const size_t newSize,
				const value_type& copyValue)
	{
		_delete_until_size(newSize);
		_create_until_size(newSize, copyValue);
	}

	template<class... Args>
	void emplace_back(Args&&... args)
	{
		_NodePtr newNode = _create_common_node(custom::forward<Args>(args)...);
		_link_node_before(_data._Head, newNode);
	}

	void push_back(const value_type& copyValue)
	{
		emplace_back(copyValue);
	}

	void push_back(value_type&& moveValue)
	{
		emplace_back(custom::move(moveValue));
	}
	
	void pop_back()
	{
		if (_data._Size > 0)
			_remove_node(_data._Head->_Previous);
	}

	template<class... Args>
	void emplace_front(Args&&... args)
	{
		_NodePtr newNode = _create_common_node(custom::forward<Args>(args)...);
		_link_node_before(_data._Head->_Next, newNode);
	}

	void push_front(const value_type& copyValue)
	{
		emplace_front(copyValue);
	}

	void push_front(value_type&& moveValue)
	{
		emplace_front(custom::move(moveValue));
	}

	void pop_front()
	{
		if (_data._Size > 0)
			_remove_node(_data._Head->_Next);
	}

	template<class... Args>
	iterator emplace(const_iterator where, Args&&... args) // Construct object using arguments (Args) and add it BEFORE the where position
	{
		_NodePtr newNode = _create_common_node(custom::forward<Args>(args)...);
		_link_node_before(where._Ptr, newNode);

		return iterator(newNode, &_data);
	}

	iterator insert(const_iterator where, const value_type& copyValue)
	{
		return emplace(where, copyValue);
	}

	iterator insert(const_iterator where, value_type&& moveValue)
	{
		return emplace(where, custom::move(moveValue));
	}

	iterator erase(const_iterator where)	// Remove component at where position
	{
		if (where.is_end())
			throw std::out_of_range("Cannot erase end iterator.");

		_NodePtr temp 			= where._Ptr;
		iterator prevIterator 	= iterator(temp->_Previous, &_data);
		_remove_node(temp);

		return prevIterator;
	}

	reference front() noexcept
	{
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Head->_Next->_Value;
	}

	const_reference front() const noexcept
	{
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Head->_Next->_Value;
	}

	reference back() noexcept
	{
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Head->_Previous->_Value;
	}

	const_reference back() const noexcept
	{
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Head->_Previous->_Value;
	}

	reference at(const size_t index)
	{
		if (index >= _data._Size)
			throw std::out_of_range("Index out of bounds.");

		return _scroll_node(index)->_Value;
	}

	const_reference at(const size_t index) const
	{
		if (index >= _data._Size)
			throw std::out_of_range("Index out of bounds.");

		return _scroll_node(index)->_Value;
	}

	size_t size() const noexcept
	{
		return _data._Size;
	}

	size_t max_size() const noexcept
	{
        return (custom::min)(	static_cast<size_t>((numeric_limits<difference_type>::max)()),
								_Alloc_Node_Traits::max_size(_alloc));
    }

	bool empty() const noexcept
	{
		return _data._Size == 0;
	}

	void clear()
	{
		_delete_until_size(0);
	}

public:
	// Main Operations

	void reverse() noexcept
	{
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
	size_t remove_if(UnaryPredicate pred)
	{
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

	size_t remove(const Type& val)
	{
		return remove_if([&](const Type& other) -> bool { return other == val; });
	}

	template<class BinaryPredicate>
	size_t unique(BinaryPredicate pred)
	{
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

	size_t unique()
	{
		return unique(equal_to<>{});
	}

	void splice(const_iterator where, list& other, const_iterator otherFirst, const_iterator otherLast)
	{
		// splice [otherFirst, otherLast) BEFORE where

		if (where._RefData->_Head == otherFirst._RefData->_Head ||
			otherFirst._RefData->_Head != otherLast._RefData->_Head)
			throw std::domain_error("list provided by otherFirst and otherLast must be the same, but different from the one provided by where");

		size_t count = static_cast<size_t>(custom::distance(otherFirst, otherLast));

		if (max_size() - _data._Size < count)
			throw std::out_of_range("list too long");

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

	void splice(const_iterator where, list& other, const_iterator otherFirst)
	{
		// splice [otherFirst, ...) BEFORE where
		splice(where, other, otherFirst, other.end());
	}

	void splice(const_iterator where, list& other)
	{
		// spice ALL other list BEFORE where
		splice(where, other, other.begin(), other.end());
	}

	template<class Compare>
	void merge(list& other, Compare comp)
	{
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

	void merge(list& other)
	{
		merge(other, less<>{});
	}

	template<class Compare>
	void sort(Compare comp)
	{
		(void)_sort(_data._Head->_Next, _data._Size, comp);
	}

	void sort()
	{
		sort(less<>{});
	}

public:
	// iterator specific functions

	iterator begin() noexcept
	{
		return iterator(_data._Head->_Next, &_data);
	}

	const_iterator begin() const noexcept
	{
		return const_iterator(_data._Head->_Next, &_data);
	}

	reverse_iterator rbegin() noexcept
	{
		return reverse_iterator(end());
	}

	const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator(end());
	}

	iterator end() noexcept
	{
		return iterator(_data._Head, &_data);
	}

	const_iterator end() const noexcept
	{
		return const_iterator(_data._Head, &_data);
	}

	reverse_iterator rend() noexcept
	{
		return reverse_iterator(begin());
	}

	const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator(begin());
	}

private:
	// Others

	void _create_head()
	{
		_data._Head 			= _alloc.allocate(1);
		_data._Head->_Next 		= _data._Head;
		_data._Head->_Previous 	= _data._Head;
	}

	void _free_head()
	{
		_data._Head->_Next 		= nullptr;
		_data._Head->_Previous 	= nullptr;
		_alloc.deallocate(_data._Head, 1);
	}

	template<class... Args>
	_NodePtr _create_common_node(Args&&... args)
	{
		// don't increase size here
		_NodePtr newNode = _alloc.allocate(1);
		_Alloc_Node_Traits::construct(_alloc, &(newNode->_Value), custom::forward<Args>(args)...);
		return newNode;
	}

	void _link_node_before(_NodePtr beforeNode, _NodePtr newNode)	// Insert Node before another
	{
		newNode->_Previous 				= beforeNode->_Previous;
		newNode->_Next 					= beforeNode;

		beforeNode->_Previous->_Next 	= newNode;
		beforeNode->_Previous 			= newNode;

		++_data._Size;
	}

	void _unlink_node(_NodePtr targetNode)
	{
		CUSTOM_ASSERT(	targetNode->_Next != nullptr && targetNode->_Previous != nullptr,
						"Unlink only if linked");

		targetNode->_Previous->_Next	= targetNode->_Next;
		targetNode->_Next->_Previous	= targetNode->_Previous;

		targetNode->_Next				= nullptr;
		targetNode->_Previous			= nullptr;

		--_data._Size;
	}

	void _remove_node(_NodePtr junkNode)	// Remove Node and relink
	{
		_unlink_node(junkNode);
		_Alloc_Node_Traits::destroy(_alloc, &(junkNode->_Value));
		_alloc.deallocate(junkNode, 1);
	}

	void _copy(const list& other)
	{
		for (	_NodePtr temp = other._data._Head->_Next;
				_data._Size < other._data._Size;
				temp = temp->_Next)
			push_back(temp->_Value);
	}

	void _move(list&& other) noexcept
	{
		custom::swap(_data._Head, other._data._Head);
		_data._Size = custom::exchange(other._data._Size, 0);	// other list is empty before this
	}

	template<class... Args>
	void _create_until_size(const size_t newSize, Args&&... args)
	{
		while (_data._Size < newSize)
			emplace_back(custom::forward<Args>(args)...);
	}

	void _delete_until_size(const size_t newSize)
	{
		while (_data._Size > newSize)
			pop_back();
	}

	void _splice(_NodePtr thisFirst, _NodePtr otherFirst, _NodePtr otherLast)
	{
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
	void _merge_internal(_NodePtr first, _NodePtr mid, _NodePtr last, Compare comp)
	{
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
	_NodePtr _sort(_NodePtr first, size_t size, Compare comp)
	{
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

	_NodePtr _scroll_node(const size_t index) const	// Get object in the list at index position by going through all components
	{
		if (index < _data._Size)
		{
			_NodePtr temp = _data._Head->_Next;
			for (size_t i = 0; i < index; ++i)
				temp = temp->_Next;

			return temp;
		}

		return _data._Head;
	}
}; // END Linked list


// list binary operators
template<class _Type, class _Alloc>
bool operator==(const list<_Type, _Alloc>& left, const list<_Type, _Alloc>& right)
{
    if (left.size() != right.size())
		return false;

	return custom::equal(left.begin(), left.end(), right.begin());
}

template<class _Type, class _Alloc>
bool operator!=(const list<_Type, _Alloc>& left, const list<_Type, _Alloc>& right)
{
	return !(left == right);
}

CUSTOM_END