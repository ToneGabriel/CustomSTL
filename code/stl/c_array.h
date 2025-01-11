#pragma once
#include "c_utility.h"
#include "c_iterator.h"
#include "c_algorithm.h"


CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN

template<class Type, size_t Size>
class _Array_Const_Iterator
{
public:
    using iterator_category	= random_access_iterator_tag;
	using value_type		= Type;
	using difference_type	= ptrdiff_t;
	using reference			= const value_type&;
	using pointer			= const value_type*;

	value_type* _Ptr		= nullptr;
	size_t _Index			= 0;

public:

	constexpr _Array_Const_Iterator() noexcept = default;

	constexpr explicit _Array_Const_Iterator(value_type* ptr, size_t index) noexcept
		:_Ptr(ptr), _Index(index) { /*Empty*/ }

	constexpr _Array_Const_Iterator& operator++() noexcept
	{
		CUSTOM_ASSERT(_Index < Size, "Cannot increment end iterator.");
		++_Index;
		return *this;
	}

	constexpr _Array_Const_Iterator operator++(int) noexcept
	{
		_Array_Const_Iterator temp = *this;
		++(*this);
		return temp;
	}

	constexpr _Array_Const_Iterator& operator+=(const difference_type diff) noexcept
	{
		CUSTOM_ASSERT(_Index + static_cast<size_t>(diff) <= Size, "Cannot increment end iterator.");
		_Index += static_cast<size_t>(diff);
		return *this;
	}

	constexpr _Array_Const_Iterator operator+(const difference_type diff) const noexcept
	{
		_Array_Const_Iterator temp = *this;
		temp += diff;
		return temp;
	}

	constexpr _Array_Const_Iterator& operator--() noexcept
	{
		CUSTOM_ASSERT(_Index > 0, "Cannot decrement begin iterator.");
		--_Index;
		return *this;
	}

	constexpr _Array_Const_Iterator operator--(int) noexcept
	{
		_Array_Const_Iterator temp = *this;
		--(*this);
		return temp;
	}

	constexpr _Array_Const_Iterator& operator-=(const difference_type diff) noexcept
	{
		CUSTOM_ASSERT(_Index >= static_cast<size_t>(diff), "Cannot decrement begin iterator.");
		_Index -= static_cast<size_t>(diff);
		return *this;
	}

	constexpr _Array_Const_Iterator operator-(const difference_type diff) const noexcept
	{
		_Array_Const_Iterator temp = *this;
		temp -= diff;
		return temp;
	}

	constexpr pointer operator->() const noexcept
	{
		CUSTOM_ASSERT(_Index < Size, "Cannot access end iterator.");
		return _Ptr + _Index;
	}

	constexpr reference operator*() const noexcept
	{
		CUSTOM_ASSERT(_Index < Size, "Cannot dereference end iterator.");
		return *(_Ptr + _Index);
	}

    constexpr reference operator[](const difference_type diff) const noexcept
	{
        return *(*this + diff);
    }

	constexpr bool operator==(const _Array_Const_Iterator& other) const noexcept
	{
		return (_Ptr == other._Ptr && _Index == other._Index);
	}

	constexpr bool operator!=(const _Array_Const_Iterator& other) const noexcept
	{
		return !(*this == other);
	}

public:

	// Get the position for the element in array from iterator
	constexpr size_t get_index() const noexcept
	{
		return _Index;
	}

	constexpr bool is_begin() const noexcept
	{
		return _Index == 0;
	}

	constexpr bool is_end() const noexcept
	{
		return _Index == Size;
	}

	friend constexpr void _verify_range(const _Array_Const_Iterator& first, const _Array_Const_Iterator& last) noexcept
	{
		CUSTOM_ASSERT(first._Ptr == last._Ptr, "array iterators in range are from different containers");
		CUSTOM_ASSERT(first._Index <= last._Index, "array iterator range transposed");
	}
}; // END _Array_Const_Iterator

template<class Type, size_t Size>
class _Array_Iterator : public _Array_Const_Iterator<Type, Size>
{
private:
	using _Base				= _Array_Const_Iterator<Type, Size>;
	
public:
    using iterator_category	= random_access_iterator_tag;
	using value_type		= Type;
	using difference_type	= ptrdiff_t;
	using reference			= value_type&;
	using pointer			= value_type*;

public:

	constexpr _Array_Iterator() noexcept = default;

	constexpr explicit _Array_Iterator(value_type* ptr, size_t index) noexcept
		:_Base(ptr, index) { /*Empty*/ }

	constexpr _Array_Iterator& operator++() noexcept
	{
		_Base::operator++();
		return *this;
	}

	constexpr _Array_Iterator operator++(int) noexcept
	{
		_Array_Iterator temp = *this;
		_Base::operator++();
		return temp;
	}

	constexpr _Array_Iterator& operator+=(const difference_type diff) noexcept
	{
		_Base::operator+=(diff);
		return *this;
	}

	constexpr _Array_Iterator operator+(const difference_type diff) const noexcept
	{
		_Array_Iterator temp = *this;
		temp += diff;
		return temp;
	}

	constexpr _Array_Iterator& operator--() noexcept
	{
		_Base::operator--();
		return *this;
	}

	constexpr _Array_Iterator operator--(int) noexcept
	{
		_Array_Iterator temp = *this;
		_Base::operator--();
		return temp;
	}

	constexpr _Array_Iterator& operator-=(const difference_type diff) noexcept
	{
		_Base::operator-=(diff);
		return *this;
	}

	constexpr _Array_Iterator operator-(const difference_type diff) const noexcept
	{
		_Array_Iterator temp = *this;
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
}; // END _Array_Iterator

CUSTOM_DETAIL_END


template<class Type, size_t Size>
class array							// array Template (follows aggregate rules)
{
//Aggregate Rules:
//Is a class type(a struct, class, or union), or an array type;
//Has no private or protected non-static data members;
//Has no user-declared or inherited constructors;
//Has no base classes;
//Has no virtual member functions;

public:
	using value_type				= Type;
	using difference_type			= ptrdiff_t;
	using reference					= value_type&;
	using const_reference			= const value_type&;
	using pointer					= value_type*;
	using const_pointer				= const value_type*;

	using iterator					= detail::_Array_Iterator<value_type, Size>;
	using const_iterator			= detail::_Array_Const_Iterator<value_type, Size>;
	using reverse_iterator			= custom::reverse_iterator<iterator>;
	using const_reverse_iterator	= custom::reverse_iterator<const_iterator>;

	value_type _array[Size];	// Actual container array

public:
	// Operators

	constexpr const_reference operator[](const size_t index) const noexcept
	{
		CUSTOM_ASSERT(index < size(), "Index out of bounds.");
		return _array[index];
	}

	constexpr reference operator[](const size_t index) noexcept
	{
		CUSTOM_ASSERT(index < size(), "Index out of bounds.");
		return _array[index];
	}
	
public:
	// Main functions

    constexpr void fill(const value_type& copyValue)
	{
		(void)custom::fill_n(begin(), Size, copyValue);
	}

	constexpr void swap(array& other)
	{
		custom::swap(_array, other._array);
		//(void)custom::swap_ranges(_array, _array + Size, other._array);
    }

    constexpr reference front() noexcept
	{
		CUSTOM_ASSERT(Size > 0, "Container is empty.");
		return _array[0];
	}

	constexpr const_reference front() const noexcept
	{
		CUSTOM_ASSERT(Size > 0, "Container is empty.");
		return _array[0];
	}

	constexpr reference back() noexcept
	{
		CUSTOM_ASSERT(Size > 0, "Container is empty.");
		return _array[Size - 1];
	}

	constexpr const_reference back() const noexcept
	{
		CUSTOM_ASSERT(Size > 0, "Container is empty.");
		return _array[Size - 1];
	}

	constexpr size_t size() const noexcept
	{
		return Size;
	}

    constexpr size_t max_size() const noexcept
	{
        return Size;
    }

	constexpr bool empty() const noexcept
	{
		return Size == 0;
	}

	constexpr const_reference at(const size_t index) const
	{
		if (index >= Size)
			throw std::out_of_range("Index out of bounds.");

		return _array[index];
	}

	constexpr reference at(const size_t index)
	{
		if (index >= Size)
			throw std::out_of_range("Index out of bounds.");

		return _array[index];
	}

	constexpr pointer data() noexcept
	{
		return _array;
	}

	constexpr const_pointer data() const noexcept
	{
		return _array;
	}

public:
	// iterator specific functions

	constexpr iterator begin() noexcept
	{
		return iterator(_array, 0);
	}

	constexpr const_iterator begin() const noexcept
	{
		return const_iterator(_array, 0);
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
		return iterator(_array, Size);
	}

	constexpr const_iterator end() const noexcept
	{
		return const_iterator(_array, Size);
	}

	constexpr reverse_iterator rend() noexcept
	{
		return reverse_iterator(begin());
	}

	constexpr const_reverse_iterator rend() const noexcept
	{
		return const_reverse_iterator(begin());
	}
}; // END array

// array binary operators
template<class Ty, size_t Size>
constexpr bool operator==(const array<Ty, Size>& left, const array<Ty, Size>& right)
{
    return custom::equal(left.begin(), left.end(), right.begin());
}

template<class Ty, size_t Size>
constexpr bool operator!=(const array<Ty, Size>& left, const array<Ty, Size>& right)
{
	return !(left == right);
}


CUSTOM_DETAIL_BEGIN

template<class Ty, size_t Size, size_t... Idx>
constexpr array<remove_cv_t<Ty>, Size> _to_array_copy_impl(Ty (&builtInArray)[Size], index_sequence<Idx...>)
{
	return { builtInArray[Idx]... };
}

template<class Ty, size_t Size, size_t... Idx>
constexpr array<remove_cv_t<Ty>, Size> _to_array_move_impl(Ty (&&builtInArray)[Size], index_sequence<Idx...>)
{
	return { custom::move(builtInArray[Idx])... };
}

CUSTOM_DETAIL_END


// Deduction guide (ex: custom::array arr {1, 2, 3};)
// All types must be the same. Otherwise the program is ill-formed
template<class first, class... Rest>
array(first, Rest...) -> array<	enable_if_t<(conjunction_v<is_same<first, Rest>...>), first>,
								1 + sizeof...(Rest)>;

// to array
template<class Ty, size_t Size>
constexpr array<remove_cv_t<Ty>, Size> to_array(Ty (&builtInArray)[Size])
{
	static_assert(!is_array_v<Ty>, "to_array does not accept multidimensional arrays.");
	static_assert(is_copy_constructible_v<Ty>, "to_array requires copy constructible elements.");

	return detail::_to_array_copy_impl(builtInArray, make_index_sequence<Size>{});
}

template<class Ty, size_t Size>
constexpr array<remove_cv_t<Ty>, Size> to_array(Ty (&&builtInArray)[Size])
{
	static_assert(!is_array_v<Ty>, "to_array does not accept multidimensional arrays.");
	static_assert(is_move_constructible_v<Ty>, "to_array requires move constructible elements.");

	return detail::_to_array_move_impl(custom::move(builtInArray), make_index_sequence<Size>{});
}

CUSTOM_END