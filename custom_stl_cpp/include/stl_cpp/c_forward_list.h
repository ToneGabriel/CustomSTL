#pragma once
#include <x_node.h>
#include <x_memory.h>
#include <c_utility.h>
#include <c_iterator.h>
#include <c_functional.h>


CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN

template<class Type, class Alloc>
struct _Forward_List_Data
{
	// deduce data types and forward them
	using _Alloc_Traits			= allocator_traits<Alloc>;
	using _Node					= detail::_Forward_Node<Type>;
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
};	// END _Forward_List_Data

template<class ForwardListData>
class _Forward_List_Const_Iterator
{
private:
	using _Data				= ForwardListData;
	using _NodePtr			= typename _Data::_NodePtr;

public:
    using iterator_category	= forward_iterator_tag;
	using value_type		= typename _Data::value_type;
	using difference_type 	= typename _Data::difference_type;
	using reference			= typename _Data::const_reference;
	using pointer			= typename _Data::const_pointer;

	_NodePtr _Ptr			= nullptr;
	const _Data* _RefData	= nullptr;

public:

	_Forward_List_Const_Iterator() noexcept = default;

	explicit _Forward_List_Const_Iterator(_NodePtr nodePtr, const _Data* data) noexcept
		:_Ptr(nodePtr), _RefData(data) { /*Empty*/ }

	_Forward_List_Const_Iterator& operator++() noexcept
	{
		CUSTOM_ASSERT(_Ptr != nullptr, "Cannot increment end iterator.");
		_Ptr = _Ptr->_Next;
		return *this;
	}

	_Forward_List_Const_Iterator operator++(int) noexcept
	{
		_Forward_List_Const_Iterator temp = *this;
		++(*this);
		return temp;
	}

	pointer operator->() const noexcept
	{
		return pointer_traits<pointer>::pointer_to(**this);	//return &(**this); calls operator*
	}

	reference operator*() const noexcept
	{
		CUSTOM_ASSERT(_Ptr != nullptr, "Cannot dereference end iterator.");
		return _Ptr->_Value;
	}

	bool operator==(const _Forward_List_Const_Iterator& other) const noexcept
	{
		return _Ptr == other._Ptr;
	}

	bool operator!=(const _Forward_List_Const_Iterator& other) const noexcept
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
		return _Ptr == nullptr;
	}

	bool is_last_valid() const noexcept
	{
		return _Ptr->_Next == nullptr;
	}

	friend void _verify_range(const _Forward_List_Const_Iterator& first, const _Forward_List_Const_Iterator& last) noexcept
	{
		CUSTOM_ASSERT(first._RefData == last._RefData, "forward_list iterators in range are from different containers");
		// No possible way to determine order.
	}
}; // END _Forward_List_Const_Iterator

template<class ForwardListData>
class _Forward_List_Iterator : public _Forward_List_Const_Iterator<ForwardListData>		// Linked list iterator
{
private:
	using _Base				= _Forward_List_Const_Iterator<ForwardListData>;
	using _Data 			= ForwardListData;
	using _NodePtr			= typename _Data::_NodePtr;

public:
    using iterator_category	= forward_iterator_tag;
	using value_type 		= typename _Data::value_type;
	using difference_type 	= typename _Data::difference_type;
	using reference 		= typename _Data::reference;
	using pointer 			= typename _Data::pointer;

public:

	_Forward_List_Iterator() noexcept = default;

	explicit _Forward_List_Iterator(_NodePtr nodePtr, const _Data* data) noexcept
		: _Base(nodePtr, data) { /*Empty*/ }

	_Forward_List_Iterator& operator++() noexcept
	{
		_Base::operator++();
		return *this;
	}

	_Forward_List_Iterator operator++(int) noexcept
	{
		_Forward_List_Iterator temp = *this;
		_Base::operator++();
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
}; // END _Forward_List_Iterator

CUSTOM_DETAIL_END

template<class Type, class Alloc = custom::allocator<Type>>
class forward_list				// Singly Linked list
{
private:
	using _Data 				= detail::_Forward_List_Data<Type, Alloc>;
	using _Alloc_Traits			= typename _Data::_Alloc_Traits;
	using _Node					= typename _Data::_Node;
	using _Alloc_Node			= typename _Data::_Alloc_Node;
	using _Alloc_Node_Traits	= typename _Data::_Alloc_Node_Traits;
	using _NodePtr				= typename _Data::_NodePtr;

public:
	static_assert(is_same_v<Type, typename Alloc::value_type>, "Object type and allocator type must be the same!");
	static_assert(is_object_v<Type>, "Containers require object type!");

	using value_type 			= typename _Data::value_type;
	using difference_type 		= typename _Data::difference_type;
	using reference				= typename _Data::reference;
	using const_reference		= typename _Data::const_reference;
	using pointer				= typename _Data::pointer;
	using const_pointer			= typename _Data::const_pointer;
	using allocator_type		= Alloc;

	using iterator				= detail::_Forward_List_Iterator<_Data>;		// iterator type
	using const_iterator		= detail::_Forward_List_Const_Iterator<_Data>;	// Const iterator type

private:
	_Data _data;			// Actual container data
	_Alloc_Node _alloc;		// allocator for nodes

public:
	// Constructors

	forward_list()
	{
		_create_head();
	}

	forward_list(const size_t newSize, const value_type& value) : forward_list()	// Add multiple copies Constructor
	{
		_create_until_size(newSize, value);
	}

	forward_list(std::initializer_list<value_type> list) : forward_list()
	{
		for (const auto& val : list)
			push_back(val);
	}

	forward_list(const forward_list& other) : forward_list()
	{
		_copy(other);
	}

	forward_list(forward_list&& other) noexcept : forward_list()
	{
		_move(custom::move(other));
	}

	~forward_list() noexcept
	{
		clear();
		_free_head();
	}

public:
	// Operators

	forward_list& operator=(const forward_list& other)
	{
		if (_data._Head != other._data._Head)
		{
			clear();
			_copy(other);
		}

		return *this;
	}

	forward_list& operator=(forward_list&& other) noexcept
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
	void emplace_front(Args&&... args)
	{
		_NodePtr newNode = _create_common_node(custom::forward<Args>(args)...);
		_insert_node_after(_data._Head, newNode);
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
			_remove_node_after(_data._Head);
	}

	template<class... Args>
	iterator emplace_after(const_iterator where, Args&&... args)
	{
		if (where.is_end())
			throw std::out_of_range("Cannot emplace after end iterator.");

		_NodePtr newNode = _create_common_node(custom::forward<Args>(args)...);
		_insert_node_after(where._Ptr, newNode);

		return iterator(newNode, &_data);
	}

	iterator erase_after(const_iterator where)
	{
		if (where.is_end() || where.is_last_valid())
			throw std::out_of_range("Cannot erase after end iterator.");

		_NodePtr temp 			= where._Ptr;
		iterator nextIterator 	= iterator(temp->_Next, &_data);
		_remove_node_after(temp);

		return nextIterator;
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
	size_t remove_if(UnaryPredicate pred)
	{
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

	size_t unique()
	{
		return unique(equal_to<>{});
	}

	void splice_after(const_iterator where, forward_list& other, const_iterator otherFirst, const_iterator otherLast)
	{
		// splice (otherFirst, otherLast) AFTER where

		if (where._RefData->_Head == otherFirst._RefData->_Head ||
			otherFirst._RefData->_Head != otherLast._RefData->_Head)
			throw std::domain_error("forward_list provided by otherFirst and otherLast must be the same, but different from the one provided by where");

		size_t count = static_cast<size_t>(custom::distance(otherFirst, otherLast) - 1);

		if (max_size() - _data._Size < count)
			throw std::out_of_range("forward_list too long");

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

	void splice_after(const_iterator where, forward_list& other, const_iterator otherFirst)
	{
		splice_after(where, other, otherFirst, other.end());
	}

	void splice_after(const_iterator where, forward_list& other)
	{
		splice_after(where, other, other.before_begin(), other.end());
	}

	template<class Compare>
	void merge(forward_list& other, Compare comp)
	{
		// TODO: implement
	}

	void merge(forward_list& other)
	{
		merge(other, less<>{});
	}

	template<class Compare>
	void sort(Compare comp)
	{
		// TODO: implement
	}

	void sort()
	{
		sort(less<>{});
	}

public:
	// iterator specific functions

	iterator before_begin() noexcept
	{
        return iterator(_data._Head, &_data);
    }

    const_iterator before_begin() const noexcept
	{
        return const_iterator(_data._Head, &_data);
    }

	iterator begin() noexcept
	{
		return iterator(_data._Head->_Next, &_data);
	}

	const_iterator begin() const noexcept
	{
		return const_iterator(_data._Head->_Next, &_data);
	}

	iterator end() noexcept
	{
		return iterator(nullptr, &_data);
	}

	const_iterator end() const noexcept
	{
		return const_iterator(nullptr, &_data);
	}

private:
	// Others

	void _create_head()
	{
		_data._Head 			= _alloc.allocate(1);
		_data._Head->_Next 		= nullptr;
	}

	void _free_head()
	{
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

	void _insert_node_after(_NodePtr afterNode, _NodePtr newNode)
	{
		newNode->_Next 		= afterNode->_Next;
		afterNode->_Next 	= newNode;
		++_data._Size;
	}

	void _remove_node_after(_NodePtr afterNode)
	{
		_NodePtr junkNode 	= afterNode->_Next;
		afterNode->_Next 	= junkNode->_Next;
		--_data._Size;

		_Alloc_Node_Traits::destroy(_alloc, &(junkNode->_Value));
		_alloc.deallocate(junkNode, 1);
	}

	void _copy(const forward_list& other)
	{
		_NodePtr temp = other._data._Head->_Next;
		while (_data._Size < other._data._Size)
		{
			push_front(temp->_Value);
			temp = temp->_Next;
		}
	}

	void _move(forward_list&& other) noexcept
	{
		custom::swap(_data._Head, other._data._Head);
		_data._Size = custom::exchange(other._data._Size, 0);
	}

	template<class... Args>
	void _create_until_size(const size_t newSize, Args&&... args)
	{
		while (_data._Size < newSize)
			emplace_front(custom::forward<Args>(args)...);
	}

	void _delete_until_size(const size_t newSize)
	{
		while (_data._Size > newSize)
			pop_front();
	}

	void _splice_after(_NodePtr thisFirst, _NodePtr otherFirst, _NodePtr otherLast)
	{
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
};	// END forward_list


// forward_list binary operators
template<class _Type, class _Alloc>
bool operator==(const forward_list<_Type, _Alloc>& left, const forward_list<_Type, _Alloc>& right)
{
    if (left.size() != right.size())
		return false;

	return custom::equal(left.begin(), left.end(), right.begin());
}

template<class _Type, class _Alloc>
bool operator!=(const forward_list<_Type, _Alloc>& left, const forward_list<_Type, _Alloc>& right)
{
	return !(left == right);
}

CUSTOM_END