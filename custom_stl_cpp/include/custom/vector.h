#pragma once
#include "custom/_memory_utils.h"
#include "custom/utility.h"
#include "custom/algorithm.h"
#include "custom/iterator.h"


CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN

template<class Type, class Alloc>
struct _Vector_Data
{
	using _Alloc_Traits		= allocator_traits<Alloc>;

	using value_type		= typename _Alloc_Traits::value_type;
	using difference_type	= typename _Alloc_Traits::difference_type;
	using reference			= typename _Alloc_Traits::reference;
	using const_reference	= typename _Alloc_Traits::const_reference;
	using pointer			= typename _Alloc_Traits::pointer;
	using const_pointer		= typename _Alloc_Traits::const_pointer;

	pointer _First			= nullptr;			// Actual container array
	pointer _Last			= nullptr;			// pointer to end
	pointer _Final			= nullptr;			// pointer to capacity end
};	// END _Vector_Data

template<class VecData>
class _Vector_Const_Iterator
{
private:
	using _Data				= VecData;

public:
    using iterator_category	= random_access_iterator_tag;
	using value_type		= typename _Data::value_type;
	using difference_type 	= typename _Data::difference_type;
	using reference 		= typename _Data::const_reference;
	using pointer			= typename _Data::const_pointer;

	value_type* _Ptr		= nullptr;
	const _Data* _RefData	= nullptr;

public:

	constexpr _Vector_Const_Iterator() noexcept : _Ptr() { /*Empty*/ }

	constexpr explicit _Vector_Const_Iterator(value_type* ptr, const _Data* data) noexcept
		:_Ptr(ptr), _RefData(data) { /*Empty*/ }

	constexpr _Vector_Const_Iterator& operator++() noexcept
	{
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot increment end iterator.");
		++_Ptr;
		return *this;
	}

	constexpr _Vector_Const_Iterator operator++(int) noexcept
	{
		_Vector_Const_Iterator temp = *this;
		++(*this);
		return temp;
	}

	constexpr _Vector_Const_Iterator& operator+=(const difference_type diff) noexcept
	{
		CUSTOM_ASSERT(_Ptr + diff <= _RefData->_Last, "Cannot increment end iterator.");
		_Ptr += diff;
		return *this;
	}

	constexpr _Vector_Const_Iterator operator+(const difference_type diff) const noexcept
	{
		_Vector_Const_Iterator temp = *this;
		temp += diff;
		return temp;
	}

	constexpr _Vector_Const_Iterator& operator--() noexcept
	{
		CUSTOM_ASSERT(_Ptr > _RefData->_First, "Cannot decrement begin iterator.");
		--_Ptr;
		return *this;
	}

	constexpr _Vector_Const_Iterator operator--(int) noexcept
	{
		_Vector_Const_Iterator temp = *this;
		--(*this);
		return temp;
	}

	constexpr _Vector_Const_Iterator& operator-=(const difference_type diff) noexcept
	{
		CUSTOM_ASSERT(_Ptr - diff >= _RefData->_First, "Cannot decrement begin iterator.");
		_Ptr -= diff;
		return *this;
	}

	constexpr _Vector_Const_Iterator operator-(const difference_type diff) const noexcept
	{
		_Vector_Const_Iterator temp = *this;
		temp -= diff;
		return temp;
	}

	constexpr pointer operator->() const noexcept
	{
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot access end iterator.");
		return _Ptr;
	}

	constexpr reference operator*() const noexcept
	{
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot dereference end iterator.");
		return *_Ptr;
	}

    constexpr reference operator[](const difference_type diff) const noexcept
	{
        return *(*this + diff);
    }

	constexpr bool operator==(const _Vector_Const_Iterator& other) const noexcept
	{
		return _Ptr == other._Ptr;
	}

	constexpr bool operator!=(const _Vector_Const_Iterator& other) const noexcept
	{
		return !(*this == other);
	}

public:

	// Get the position for the element in array from iterator
	constexpr size_t get_index() const noexcept
	{
		return static_cast<size_t>(_Ptr - _RefData->_First);
	}

	constexpr bool is_begin() const noexcept
	{
		return _Ptr == _RefData->_First;
	}

	constexpr bool is_end() const noexcept
	{
		return _Ptr == _RefData->_Last;
	}

	friend constexpr void _verify_range(const _Vector_Const_Iterator& first, const _Vector_Const_Iterator& last) noexcept
	{
		CUSTOM_ASSERT(first._RefData == last._RefData, "vector iterators in range are from different containers");
		CUSTOM_ASSERT(first._Ptr <= last._Ptr, "vector iterator range transposed");
	}
}; // END _Vector_Const_Iterator

template<class VecData>
class _Vector_Iterator : public _Vector_Const_Iterator<VecData>			// vector iterator
{
private:
	using _Base				= _Vector_Const_Iterator<VecData>;
	using _Data				= VecData;
	
public:
    using iterator_category	= random_access_iterator_tag;
	using value_type 		= typename _Data::value_type;
	using difference_type 	= typename _Data::difference_type;
	using reference			= typename _Data::reference;
	using pointer			= typename _Data::pointer;

public:

	constexpr _Vector_Iterator() noexcept = default;

	constexpr explicit _Vector_Iterator(value_type* ptr, const _Data* data) noexcept
		:_Base(ptr, data) { /*Empty*/ }

	constexpr _Vector_Iterator& operator++() noexcept
	{
		_Base::operator++();
		return *this;
	}

	constexpr _Vector_Iterator operator++(int) noexcept
	{
		_Vector_Iterator temp = *this;
		_Base::operator++();
		return temp;
	}

	constexpr _Vector_Iterator& operator+=(const difference_type diff) noexcept
	{
		_Base::operator+=(diff);
		return *this;
	}

	constexpr _Vector_Iterator operator+(const difference_type diff) const noexcept
	{
		_Vector_Iterator temp = *this;
		temp += diff;
		return temp;
	}

	constexpr _Vector_Iterator& operator--() noexcept
	{
		_Base::operator--();
		return *this;
	}

	constexpr _Vector_Iterator operator--(int) noexcept
	{
		_Vector_Iterator temp = *this;
		_Base::operator--();
		return temp;
	}

	constexpr _Vector_Iterator& operator-=(const difference_type diff) noexcept
	{
		_Base::operator-=(diff);
		return *this;
	}

	constexpr _Vector_Iterator operator-(const difference_type diff) const noexcept
	{
		_Vector_Iterator temp = *this;
		temp -= diff;
		return temp;
	}

	constexpr pointer operator->() const noexcept
	{
		return const_cast<pointer>(_Base::operator->());
	}

	constexpr reference operator*() const noexcept
	{
		return const_cast<reference>(_Base::operator*());
	}

    constexpr reference operator[](const difference_type diff) const noexcept
	{
        return const_cast<reference>(_Base::operator[](diff));
    }
}; // END _Vector_Iterator

CUSTOM_DETAIL_END


template<class Type, class Alloc = custom::allocator<Type>>
class vector			// vector Template
{
private:
	using _Data						= detail::_Vector_Data<Type, Alloc>;		// Members that are modified
	using _Alloc_Traits				= typename _Data::_Alloc_Traits;

public:
	static_assert(is_same_v<Type, typename Alloc::value_type>, "Object type and allocator type must be the same!");
	static_assert(is_object_v<Type>, "Containers require object type!");

	using value_type				= typename _Data::value_type;				// Type for stored values
	using difference_type			= typename _Data::difference_type;
	using reference					= typename _Data::reference;
	using const_reference			= typename _Data::const_reference;
	using pointer					= typename _Data::pointer;
	using const_pointer				= typename _Data::const_pointer;
	using allocator_type			= Alloc;
	
	using iterator					= detail::_Vector_Iterator<_Data>;
	using const_iterator			= detail::_Vector_Const_Iterator<_Data>;
	using reverse_iterator			= custom::reverse_iterator<iterator>;
	using const_reverse_iterator	= custom::reverse_iterator<const_iterator>;

private:
	_Data _data;
	allocator_type _alloc;
	
	static constexpr size_t _DEFAULT_CAPACITY = 8;

public:
	// Constructors

	constexpr vector()
	{
		reserve(_DEFAULT_CAPACITY);
	}

	constexpr vector(const size_t newCapacity)								// Add multiple default copies Constructor
	{
		realloc(newCapacity);
	}

	constexpr vector(	const size_t newCapacity,
						const value_type& copyValue)						// Add multiple copies Constructor
	{
		realloc(newCapacity, copyValue);
	}

	constexpr vector(std::initializer_list<value_type> list)
	{
		reserve(list.size());
		for (const auto& val : list)
			push_back(val);
	}

	constexpr vector(const vector& other)
	{
		_copy(other);
	}

	constexpr vector(vector&& other) noexcept
	{
		_move(custom::move(other));
	}

	constexpr ~vector() noexcept
	{
		_clean_up_array();
	}

public:
	// Operators

	constexpr const_reference operator[](const size_t index) const noexcept
	{
		CUSTOM_ASSERT(index < size(), "Index out of bounds.");
		return _data._First[index];
	}

	constexpr reference operator[](const size_t index) noexcept
	{
		CUSTOM_ASSERT(index < size(), "Index out of bounds.");
		return _data._First[index];
	}

	constexpr vector& operator=(const vector& other)
	{
		if (_data._First != other._data._First)
		{
			_clean_up_array();
			_copy(other);
		}

		return *this;
	}

	constexpr vector& operator=(vector&& other) noexcept
	{
		if (_data._First != other._data._First)
		{
			_clean_up_array();
			_move(custom::move(other));
		}

		return *this;
	}

public:
	// Main functions

	// Allocate memory and move values if needed
	constexpr void reserve(const size_t newCapacity)
	{
		size_t newSize 			= std::min(newCapacity, size());
		value_type* newArray 	= _alloc.allocate(newCapacity);

		for (size_t i = 0; i < newSize; ++i)
			_Alloc_Traits::construct(_alloc, &newArray[i], custom::move(_data._First[i]));

		_clean_up_array();
		_data._First	= newArray;
		_data._Last		= _data._First + newSize;
		_data._Final	= _data._First + newCapacity;
	}

	// Allocate memory with capacity equal to size and move values there
	constexpr void shrink_to_fit()
	{
		reserve(size());
	}

	// Allocate memory and populate it with default values (delete old)
	constexpr void realloc(const size_t newCapacity)
	{
		_clean_up_array();

		_data._First	= _alloc.allocate(newCapacity);
		_data._Last		= _data._First + newCapacity;
		_data._Final	= _data._First + newCapacity;
		_construct_range(_data._First, newCapacity);
	}

	// Allocate memory and populate it with given reference (delete old)
	constexpr void realloc(	const size_t newCapacity,
							const value_type& copyValue)
	{
		_clean_up_array();

		_data._First	= _alloc.allocate(newCapacity);
		_data._Last		= _data._First + newCapacity;
		_data._Final	= _data._First + newCapacity;
		_construct_range(_data._First, newCapacity, copyValue);
	}
	
	// Change size and Construct/Destruct objects with default value if needed
	constexpr void resize(const size_t newSize)
	{
		if (newSize < size())
			_destroy_range(_data._First + newSize, size() - newSize);
		else
		{
			if (newSize > capacity())
				reserve(newSize);

			_construct_range(_data._Last, newSize - size());
		}

		_data._Last = _data._First + newSize;
	}

	// Change size and Construct/Destruct objects with given reference if needed
	constexpr void resize(const size_t newSize, const value_type& copyValue)
	{
		if (newSize < size())
			_destroy_range(_data._First + newSize, size() - newSize);
		else
		{
			if (newSize > capacity())
				reserve(newSize);

			_construct_range(_data._Last, newSize - size(), copyValue);
		}

		_data._Last = _data._First + newSize;
	}

	// Construct object using arguments (Args) and add it to the tail
	template<class... Args>
	constexpr void emplace_back(Args&&... args)
	{
		_extend_if_full();
		_Alloc_Traits::construct(_alloc, _data._Last++, custom::forward<Args>(args)...);
	}

	// Construct object using reference and add it to the tail
	constexpr void push_back(const value_type& copyValue)
	{
		emplace_back(copyValue);
	}

	// Construct object using temporary and add it to the tail
	constexpr void push_back(value_type&& moveValue)
	{
		emplace_back(custom::move(moveValue));
	}

	// Remove last component
	constexpr void pop_back()
	{
		if (!empty())
			_Alloc_Traits::destroy(_alloc, --_data._Last);
	}

	// Emplace object at where position with given arguments
	template<class... Args>
	constexpr iterator emplace(const_iterator where, Args&&... args)
	{
		size_t index = where.get_index();	// Don't check end()
		emplace_back();

		for (size_t i = size() - 1; i > index; --i)
			_data._First[i] = custom::move(_data._First[i - 1]);

		_Alloc_Traits::destroy(_alloc, _data._First + index);
		_Alloc_Traits::construct(_alloc, _data._First + index, custom::forward<Args>(args)...);

		return iterator(_data._First + index, &_data);
	}

	// Push copy object at iterator position
	constexpr iterator insert(const_iterator where, const value_type& copyValue)
	{
		return emplace(where, copyValue);
	}

	// Push temporary object at iterator position
	constexpr iterator insert(const_iterator where, value_type&& moveValue)
	{
		return emplace(where, custom::move(moveValue));
	}

	// Remove component at iterator position
	constexpr iterator erase(const_iterator where)
	{
		if (where.is_end())
			throw std::out_of_range("vector erase iterator outside range.");

		size_t index = where.get_index();
		size_t beforeLast = size() - 1;

		for (size_t i = index; i < beforeLast; ++i)
			_data._First[i] = custom::move(_data._First[i + 1]);
		pop_back();

		return iterator(_data._First + index, &_data);
	}

	constexpr size_t capacity() const noexcept
	{
		return static_cast<size_t>(_data._Final - _data._First);
	}

	constexpr size_t size() const noexcept
	{
		return static_cast<size_t>(_data._Last - _data._First);
	}

	constexpr size_t max_size() const noexcept
	{
		return (custom::min)(	static_cast<size_t>((numeric_limits<difference_type>::max)()),
								_Alloc_Traits::max_size(_alloc));
	}

	// Check if array is empty
	constexpr bool empty() const noexcept
	{
		return (_data._First == _data._Last);
	}
	
	// Remove ALL components but keep memory
	constexpr void clear()
	{
		_destroy_range(_data._First, size());
		_data._Last = _data._First;
	}

	// Acces object at index with check (read only)
	constexpr const_reference at(const size_t index) const
	{
		if (index >= size())
			throw std::out_of_range("Index out of bounds.");

		return _data._First[index];
	}

	// Acces object at index with check
	constexpr reference at(const size_t index)
	{
		if (index >= size())
			throw std::out_of_range("Index out of bounds.");

		return _data._First[index];
	}

	constexpr const_reference front() const noexcept
	{
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._First[0];
	}

	constexpr reference front() noexcept
	{
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._First[0];
	}

	constexpr const_reference back() const noexcept
	{
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Last[-1];
	}

	constexpr reference back() noexcept
	{
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Last[-1];
	}

	constexpr const_reference data() const noexcept
	{
		return _data._First;
	}

	constexpr reference data() noexcept
	{
		return _data._First;
	}

public:
	// iterator specific functions

	constexpr iterator begin() noexcept
	{
		return iterator(_data._First, &_data);
	}

	constexpr const_iterator begin() const noexcept
	{
		return const_iterator(_data._First, &_data);
	}

	constexpr reverse_iterator rbegin() noexcept
	{
		return reverse_iterator(end());
	}

	constexpr const_reverse_iterator rbegin() const noexcept
	{
		return const_reverse_iterator(end());
	}

	constexpr iterator end() noexcept
	{
		return iterator(_data._Last, &_data);
	}

	constexpr const_iterator end() const noexcept
	{
		return const_iterator(_data._Last, &_data);
	}

	constexpr reverse_iterator rend() noexcept
	{
		return reverse_iterator(begin());
	}

	constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator(begin());
	}

private:
	// Helpers

	constexpr void _copy(const vector& other)
	{
		_data._First	= _alloc.allocate(other.capacity());
		size_t newSize	= other.size();

		for (size_t i = 0; i < newSize; ++i)
			_Alloc_Traits::construct(_alloc, &_data._First[i], other._data._First[i]);

		_data._Last		= _data._First + other.size();
		_data._Final	= _data._First + other.capacity();
	}

	constexpr void _move(vector&& other) noexcept
	{
		_data._First 	= custom::exchange(other._data._First, nullptr);
		_data._Last 	= custom::exchange(other._data._Last, nullptr);
		_data._Final 	= custom::exchange(other._data._Final, nullptr);
	}

	constexpr void _construct_range(value_type* const address, const size_t length)
	{
		for (size_t i = 0; i < length; ++i)
			_Alloc_Traits::construct(_alloc, address + i);
	}

	constexpr void _construct_range(value_type* const address, const size_t length, const value_type& value)
	{
		for (size_t i = 0; i < length; ++i)
			_Alloc_Traits::construct(_alloc, address + i, value);
	}

	constexpr void _destroy_range(value_type* address, const size_t length)
	{
		for (size_t i = 0; i < length; ++i)
			_Alloc_Traits::destroy(_alloc, address + i);
	}

	// Reserve 50% more capacity when full
	constexpr void _extend_if_full()
	{
		if (_data._Last == _data._Final)
			reserve(capacity() + capacity() / 2 + 1);
	}

	// Clear and Deallocate array
	constexpr void _clean_up_array()
	{
		if (_data._First != nullptr)
		{
			_destroy_range(_data._First, size());
			_alloc.deallocate(_data._First, capacity());
			_data._First	= nullptr;
			_data._Last		= nullptr;
			_data._Final	= nullptr;
		}
	}
}; // END vector Template


// vector binary operators
template<class _Type, class _Alloc>
constexpr bool operator==(const vector<_Type, _Alloc>& left, const vector<_Type, _Alloc>& right)
{
	if (left.size() != right.size())
		return false;

	return custom::equal(left.begin(), left.end(), right.begin());
}

template<class _Type, class _Alloc>
constexpr bool operator!=(const vector<_Type, _Alloc>& left, const vector<_Type, _Alloc>& right)
{
	return !(left == right);
}

CUSTOM_END