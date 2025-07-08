#pragma once
#include "stl_cpp/x_memory.h"
#include "stl_cpp/c_utility.h"
#include "stl_cpp/c_iterator.h"
#include "stl_cpp/c_algorithm.h"


CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN

template<class Type, class Alloc>
struct _Deque_Data
{
	using _Alloc_Traits		= allocator_traits<Alloc>;
	using _AllocPtr 		= typename _Alloc_Traits::template rebind_alloc<typename _Alloc_Traits::pointer>;
    using _AllocPtr_Traits	= allocator_traits<_AllocPtr>;
	using _MapPtr 			= typename _AllocPtr_Traits::pointer;

	using value_type		= typename _Alloc_Traits::value_type;
	using difference_type	= typename _Alloc_Traits::difference_type;
	using reference			= typename _Alloc_Traits::reference;
	using const_reference	= typename _Alloc_Traits::const_reference;
	using pointer			= typename _Alloc_Traits::pointer;
	using const_pointer		= typename _Alloc_Traits::const_pointer;

	_MapPtr _Map 			= nullptr;
	size_t _MapCapacity 	= 0;
	size_t _First 			= 0;
	size_t _Size 			= 0;

	static constexpr size_t _BLOCK_SIZE = 4;

	size_t get_block(const size_t offset) const noexcept
	{
		return (offset / _BLOCK_SIZE) % _MapCapacity;
    }
};	// END _Deque_Data

template<class DequeData>
class _Deque_Const_Iterator
{
private:
	using _Data				= DequeData;

public:
    using iterator_category	= random_access_iterator_tag;
	using value_type		= typename _Data::value_type;
	using difference_type 	= typename _Data::difference_type;
	using reference			= typename _Data::const_reference;
	using pointer			= typename _Data::const_pointer;

	size_t _Offset			= 0;
	const _Data* _RefData	= nullptr;

public:

	_Deque_Const_Iterator() noexcept = default;

    explicit _Deque_Const_Iterator(size_t offset, const _Data* data) noexcept
		:_Offset(offset), _RefData(data) { /*Empty*/ }

	_Deque_Const_Iterator& operator++() noexcept
	{
		CUSTOM_ASSERT(_Offset < _RefData->_First + _RefData->_Size, "Cannot increment end iterator.");
		++_Offset;
		return *this;
	}

	_Deque_Const_Iterator operator++(int) noexcept
	{
		_Deque_Const_Iterator temp = *this;
		++(*this);
		return temp;
	}

	_Deque_Const_Iterator& operator+=(const difference_type diff) noexcept
	{
		CUSTOM_ASSERT(	_Offset + static_cast<size_t>(diff) >= _RefData->_First &&
						_Offset + static_cast<size_t>(diff) <= _RefData->_First + _RefData->_Size, 
						"Cannot increment end iterator.");
		_Offset += static_cast<size_t>(diff);
		return *this;
	}

	_Deque_Const_Iterator operator+(const difference_type diff) const noexcept
	{
		_Deque_Const_Iterator temp = *this;
		temp += diff;
		return temp;
	}

	_Deque_Const_Iterator& operator--() noexcept
	{
		CUSTOM_ASSERT(_Offset > _RefData->_First, "Cannot decrement begin iterator.");
		--_Offset;
		return *this;
	}

	_Deque_Const_Iterator operator--(int) noexcept
	{
		_Deque_Const_Iterator temp = *this;
		--(*this);
		return temp;
	}

	_Deque_Const_Iterator& operator-=(const difference_type diff) noexcept
	{
		CUSTOM_ASSERT(	_Offset - static_cast<size_t>(diff) >= _RefData->_First &&
						_Offset - static_cast<size_t>(diff) <= _RefData->_First + _RefData->_Size,
						"Cannot decrement begin iterator.");
		_Offset -= static_cast<size_t>(diff);
		return *this;
	}

	_Deque_Const_Iterator operator-(const difference_type diff) const noexcept
	{
		_Deque_Const_Iterator temp = *this;
		temp -= diff;
		return temp;
	}

	pointer operator->() const noexcept
	{
        return pointer_traits<pointer>::pointer_to(**this);	//return &(**this); calls operator*
	}

	reference operator*() const noexcept
	{
		CUSTOM_ASSERT(	_Offset >= _RefData->_First &&
						_Offset < _RefData->_First + _RefData->_Size,
						"Cannot dereference end iterator.");

		size_t block	= _RefData->get_block(_Offset);
		size_t offset	= _Offset % _RefData->_BLOCK_SIZE;
		return _RefData->_Map[block][offset];
	}

	reference operator[](const difference_type diff) const noexcept
	{
        return *(*this + diff);
    }

	bool operator==(const _Deque_Const_Iterator& other) const noexcept
	{
		return _Offset == other._Offset;
	}

	bool operator!=(const _Deque_Const_Iterator& other) const noexcept
	{
		return !(*this == other);
	}

public:

	bool is_begin() const noexcept
	{
		return _Offset == _RefData->_First;
	}

	bool is_end() const noexcept
	{
		return _Offset == _RefData->_First + _RefData->_Size;
	}

	friend void _verify_range(const _Deque_Const_Iterator& first, const _Deque_Const_Iterator& last) noexcept
	{
		CUSTOM_ASSERT(first._RefData == last._RefData, "deque iterators in range are from different containers");
		CUSTOM_ASSERT(first._Offset <= last._Offset, "deque iterator range transposed");
	}
}; // END _Deque_Const_Iterator

template<class DequeData>
class _Deque_Iterator : public _Deque_Const_Iterator<DequeData>			// deque iterator
{
private:
	using _Base				= _Deque_Const_Iterator<DequeData>;
	using _Data				= DequeData;

public:
    using iterator_category	= random_access_iterator_tag;
	using value_type		= typename _Data::value_type;
	using difference_type 	= typename _Data::difference_type;
	using reference			= typename _Data::reference;
	using pointer			= typename _Data::pointer;

public:

	_Deque_Iterator() noexcept = default;

	explicit _Deque_Iterator(size_t offset, const _Data* data) noexcept
		:_Base(offset, data) { /*Empty*/ }

	_Deque_Iterator& operator++() noexcept
	{
		_Base::operator++();
		return *this;
	}

	_Deque_Iterator operator++(int) noexcept
	{
		_Deque_Iterator temp = *this;
		_Base::operator++();
		return temp;
	}

	_Deque_Iterator& operator+=(const difference_type diff) noexcept
	{
		_Base::operator+=(diff);
		return *this;
	}

	_Deque_Iterator operator+(const difference_type diff) const noexcept
	{
		_Deque_Iterator temp = *this;
		temp += diff;
		return temp;
	}

	_Deque_Iterator& operator--() noexcept
	{
		_Base::operator--();
		return *this;
	}

	_Deque_Iterator operator--(int) noexcept
	{
		_Deque_Iterator temp = *this;
		_Base::operator--();
		return temp;
	}

	_Deque_Iterator& operator-=(const difference_type diff) noexcept
	{
		_Base::operator-=(diff);
		return *this;
	}

	_Deque_Iterator operator-(const difference_type diff) const noexcept
	{
		_Deque_Iterator temp = *this;
		temp -= diff;
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

	reference operator[](const difference_type diff) const noexcept
	{
        return const_cast<reference>(_Base::operator[](diff));
    }
}; // END deque iterator

CUSTOM_DETAIL_END

template<class Type, class Alloc = custom::allocator<Type>>
class deque					// deque Template implemented as map of blocks
{
private:
	using _Data						= detail::_Deque_Data<Type, Alloc>;
	using _Alloc_Traits				= typename _Data::_Alloc_Traits;
	using _AllocPtr					= typename _Data::_AllocPtr;
	using _AllocPtr_Traits			= typename _Data::_AllocPtr_Traits;
	using _MapPtr					= typename _Data::_MapPtr;

public:
	static_assert(is_same_v<Type, typename Alloc::value_type>, "Object type and allocator type must be the same!");
	static_assert(is_object_v<Type>, "Containers require object type!");

	using value_type				= typename _Data::value_type;				// Type for stored values
	using difference_type			= typename _Data::difference_type;
	using reference					= typename _Data::reference;
	using const_reference			= typename _Data::const_reference;
	using pointer					= typename _Data::pointer;
	using const_pointer				= typename _Data::const_pointer;
	using allocator_type			= Alloc;									// allocator for block

	using iterator					= detail::_Deque_Iterator<_Data>;
	using const_iterator			= detail::_Deque_Const_Iterator<_Data>;
	using reverse_iterator			= custom::reverse_iterator<iterator>;
	using const_reverse_iterator	= custom::reverse_iterator<const_iterator>;

private:
	_Data 			_data;
	allocator_type 	_alloc;

	static constexpr size_t _DEFAULT_CAPACITY = 8;

public:
	// Constructors

	deque()
	{
		_init_map(_DEFAULT_CAPACITY);
	}

	deque(const size_t newSize)
	{
		resize(newSize);
	}

	deque(const size_t newSize, const value_type& copyValue)
	{
		resize(newSize, copyValue);
	}

	deque(std::initializer_list<value_type> list)
	{
		for (const auto& val : list)
			push_back(val);
	}

	deque(const deque& other)
	{
		_copy(other);
	}

	deque(deque&& other) noexcept
	{
		_move(custom::move(other));
	}

	~deque() noexcept
	{
		_clean_up_map();
	}

public:
	// Operators

	const value_type& operator[](const size_t index) const
	{
		CUSTOM_ASSERT(index < size(), "Index out of bounds.");
		return *(begin() + static_cast<difference_type>(index));
	}

	value_type& operator[](const size_t index)
	{
		CUSTOM_ASSERT(index < size(), "Index out of bounds.");
		return *(begin() + static_cast<difference_type>(index));
	}

	deque& operator=(const deque& other)
	{
		if (_data._Map != other._data._Map)
		{
			_clean_up_map();
			_copy(other);
		}

		return *this;
	}

	deque& operator=(deque&& other) noexcept
	{
		if (_data._Map != other._data._Map)
		{
			_clean_up_map();
			_move(custom::move(other));
		}

		return *this;
	}

public:
	// Main functions

	void resize(const size_t newSize)
	{
		while (_data._Size < newSize)
			emplace_back();

        while (_data._Size > newSize)
            pop_back();
	}

	void resize(const size_t newSize, const value_type& copyValue)
	{
		while (_data._Size < newSize)
			emplace_back(copyValue);

        while (_data._Size > newSize)
            pop_back();
	}

	template<class... Args>
	void emplace_back(Args&&... args)
	{
		_extend_if_full();
		
		size_t backOffset	= _data._First + _data._Size;
		size_t block		= _data.get_block(backOffset);

		if (_data._Map[block] == nullptr)
			_data._Map[block] = _alloc.allocate(_data._BLOCK_SIZE);

		_Alloc_Traits::construct(_alloc, _data._Map[block] + backOffset % _data._BLOCK_SIZE, custom::forward<Args>(args)...);
		++_data._Size;
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
		if (!empty())
		{
			size_t backOffset	= _data._First + _data._Size - 1;
			size_t block		= _data.get_block(backOffset);

			_Alloc_Traits::destroy(_alloc, _data._Map[block] + backOffset % _data._BLOCK_SIZE);
			if (--_data._Size == 0)
				_data._First = 0;
		}
	}

	template<class... Args>
	void emplace_front(Args&&... args)
	{
		_extend_if_full();

		if (_data._First == 0)
			_data._First = _data._MapCapacity * _data._BLOCK_SIZE;

		size_t block = _data.get_block(--_data._First);
		if (_data._Map[block] == nullptr)
			_data._Map[block] = _alloc.allocate(_data._BLOCK_SIZE);

		_Alloc_Traits::construct(_alloc, _data._Map[block] + _data._First % _data._BLOCK_SIZE, custom::forward<Args>(args)...);
		++_data._Size;
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
		if (!empty())
		{
			size_t block = _data.get_block(_data._First);

			_Alloc_Traits::destroy(_alloc, _data._Map[block] + _data._First % _data._BLOCK_SIZE);
			if (--_data._Size == 0)
				_data._First = 0;
			else
				++_data._First;
		}
	}

	template<class... Args>
	iterator emplace(const_iterator where, Args&&... args)
	{
		size_t off = where._Offset - _data._First;

		if (off <= _data._Size / 2)		// closer to front
		{
			emplace_front(custom::forward<Args>(args)...);
			
			iterator current 	= begin() + static_cast<difference_type>(off);
			value_type val 		= custom::move(*begin());

			for (iterator it = begin(); it != current; ++it)
				*it = custom::move(*(it + 1));

			*current = val;
		}
		else							// closer to back
		{
			emplace_back(custom::forward<Args>(args)...);

			iterator current 	= begin() + static_cast<difference_type>(off);
			value_type val 		= custom::move(*--end());

			for (iterator it = --end(); it != current; --it)
				*it = custom::move(*(it - 1));

			*current = val;
		}
		
		return begin() + static_cast<difference_type>(off);
	}

	iterator insert(const_iterator where, const value_type& copyValue)
	{
		return emplace(where, copyValue);
	}

	iterator insert(const_iterator where, value_type&& moveValue)
	{
		return emplace(where, custom::move(moveValue));
	}

	iterator erase(const_iterator where)
	{
		if (where.is_end())
			throw std::out_of_range("array erase iterator outside range.");
			
		size_t off = where._Offset - _data._First;

		if (off <= _data._Size / 2)		// closer to front
		{
			iterator current 	= begin() + static_cast<difference_type>(off);
			iterator first 		= begin();

			for (iterator it = current; it != first; --it)
				*it = custom::move(*(it - 1));

			pop_front();
		}
		else							// closer to back
		{
			iterator current 	= begin() + static_cast<difference_type>(off);
			iterator last 		= --end();

			for (iterator it = current; it != last; ++it)
				*it = custom::move(*(it + 1));
			
			pop_back();
		}

		return begin() + static_cast<difference_type>(off);
	}

	void clear()
	{
		while (!empty())
			pop_back();

		for (size_t i = 0; i < _data._MapCapacity; ++i)
			if (_data._Map[i] != nullptr)
			{
				_alloc.deallocate(_data._Map[i], _data._BLOCK_SIZE);
				_data._Map[i] = nullptr;
			}
	}

	size_t size() const noexcept
	{
		return _data._Size;
	}

    size_t max_size() const noexcept
	{
        return (custom::min)(	static_cast<size_t>((numeric_limits<difference_type>::max)()),
								_Alloc_Traits::max_size(_alloc));
    }

	bool empty() const noexcept
	{
		return (_data._Size == 0);
	}

	const_reference at(const size_t index) const
	{
		if (index >= size())
			throw std::out_of_range("Index out of bounds.");

		return *(begin() + static_cast<difference_type>(index));
	}

	reference at(const size_t index)
	{
		if (index >= size())
			throw std::out_of_range("Index out of bounds.");

		return *(begin() + static_cast<difference_type>(index));
	}

	const_reference front() const noexcept
	{
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return *begin();
	}

	reference front() noexcept
	{
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return *begin();
	}

	const_reference back() const noexcept
	{
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return *(--end());
	}

	reference back() noexcept
	{
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return *(--end());
	}

	void print_details()
	{
		std::cout << "Map Capacity= " << _data._MapCapacity << '\n';
		std::cout << "Size= " << _data._Size << '\n';
		std::cout << "Init Offset= " << _data._First << '\n';

		for (size_t i = 0; i < _data._MapCapacity; ++i)
		{
			std::cout << "Block " << i << " = ";
			if (_data._Map[i] == nullptr)
				std::cout << "NULL\n";
			else
			{
				for (size_t j = 0; j < _data._BLOCK_SIZE; ++j)
					std::cout << _data._Map[i][j] << ' ';
				std::cout << '\n';
			}
		}
	}

public:
	// iterator specific functions

	iterator begin() noexcept
	{
		return iterator(_data._First, &_data);
	}

	const_iterator begin() const noexcept
	{
		return const_iterator(_data._First, &_data);
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
		return iterator(_data._First + _data._Size, &_data);
	}

	const_iterator end() const noexcept
	{
		return const_iterator(_data._First + _data._Size, &_data);
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
	// Helpers

	void _reserve(const size_t newMapCapacity)
	{
		size_t newSize			= _data._Size;
		size_t newFirst			= _data._First % _data._BLOCK_SIZE;
		_MapPtr newMap 			= _create_empty_map(newMapCapacity);
		size_t firstBlock		= _data.get_block(_data._First);					// block to find first elem
		size_t lastBlock		= _data.get_block(_data._First + _data._Size - 1);	// block to find last elem (always != firstBlock)

		size_t newBlockIndex	= 0;
		size_t oldBlockIndex	= firstBlock;

		while (oldBlockIndex != lastBlock)								// copy pointers to blocks and nullify the old ones
		{
			newMap[newBlockIndex++]		= _data._Map[oldBlockIndex];
			_data._Map[oldBlockIndex]	= nullptr;

			if (oldBlockIndex == _data._MapCapacity - 1)
				oldBlockIndex = 0;
			else
				++oldBlockIndex;
		}
		newMap[newBlockIndex]		= _data._Map[oldBlockIndex];		// for lastBlock
		_data._Map[oldBlockIndex]	= nullptr;

		_data._Size = 0;												// clear the remaining empty blocks and map
		_clean_up_map();

		_data._Map				= newMap;
		_data._MapCapacity		= newMapCapacity;
		_data._Size				= newSize;
		_data._First			= newFirst;
	}

	void _extend_if_full()
	{
		if (_data._Map == nullptr)												// after custom::move()
			_init_map(_DEFAULT_CAPACITY);
		else if (_data._Size >= (_data._MapCapacity - 1) * _data._BLOCK_SIZE)	// ensure first and last elem are not in the same block...
			_reserve(2 * _data._MapCapacity);									// ...with last < first
	}

	void _copy(const deque& other)
	{
		_init_map(other._data._MapCapacity);

		for (auto& val : other)
			push_back(val);
	}

	void _move(deque&& other) noexcept
	{
		_data._Map 			= custom::exchange(other._data._Map, nullptr);
		_data._MapCapacity 	= custom::exchange(other._data._MapCapacity, 0);
		_data._Size 		= custom::exchange(other._data._Size, 0);
		_data._First 		= custom::exchange(other._data._First, 0);
	}

	_MapPtr _create_empty_map(const size_t newMapCapacity)
	{
		_MapPtr newMap = _AllocPtr().allocate(newMapCapacity);
		for(size_t i = 0; i < newMapCapacity; ++i)
			newMap[i] = nullptr;

		return newMap;
	}

	void _init_map(const size_t newMapCapacity)
	{
		size_t newCapacity 	= (newMapCapacity < _DEFAULT_CAPACITY) ? _DEFAULT_CAPACITY : newMapCapacity;

		_data._Map 			= _create_empty_map(newCapacity);
		_data._MapCapacity	= newCapacity;
		_data._Size			= 0;
		_data._First		= 0;
	}

	void _clean_up_map()
	{
		if (_data._Map != nullptr)
		{
			clear();
			_AllocPtr().deallocate(_data._Map, _data._MapCapacity);
			_data._Map = nullptr;
		}
	}
}; // END deque Template


// deque binary operators
template<class _Type, class _Alloc>
bool operator==(const deque<_Type, _Alloc>& left, const deque<_Type, _Alloc>& right)
{
	if (left.size() != right.size())
		return false;

	return custom::equal(left.begin(), left.end(), right.begin());
}

template<class _Type, class _Alloc>
bool operator!=(const deque<_Type, _Alloc>& left, const deque<_Type, _Alloc>& right)
{
	return !(left == right);
}

CUSTOM_END