#pragma once
#include "x_char_traits.h"
#include "x_memory.h"
#include "c_iterator.h"
#include "c_algorithm.h"


CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN

template<class Type>
struct _Basic_String_View_Data
{
	using value_type		= Type;
	using difference_type	= ptrdiff_t;
	using reference			= value_type&;
	using const_reference	= const value_type&;
	using pointer			= value_type*;
	using const_pointer		= const value_type*;


	const_pointer _First	= nullptr;
	const_pointer _Last		= nullptr;
};  // END _Basic_String_View_Data


template<class BasicStrVData>
class _Basic_String_View_Iterator	// This is only const
{
private:
	using _Data				= BasicStrVData;

public:
	using iterator_category	= random_access_iterator_tag;
	using value_type		= typename _Data::value_type;
	using difference_type	= typename _Data::difference_type;
	using reference			= typename _Data::const_reference;
	using pointer			= typename _Data::const_pointer;

	pointer _Ptr			= nullptr;
	const _Data* _RefData	= nullptr;

public:

	constexpr _Basic_String_View_Iterator() noexcept = default;

	constexpr explicit _Basic_String_View_Iterator(pointer ptr, const _Data* data) noexcept
		: _Ptr(ptr), _RefData(data) { /*Empty*/ }

	constexpr _Basic_String_View_Iterator& operator++() noexcept
	{
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot increment end iterator.");
		++_Ptr;
		return *this;
	}

	constexpr _Basic_String_View_Iterator operator++(int) noexcept
	{
		_Basic_String_View_Iterator temp = *this;
		++(*this);
		return temp;
	}

	constexpr _Basic_String_View_Iterator& operator+=(const difference_type diff) noexcept
	{
		CUSTOM_ASSERT(_Ptr + diff <= _RefData->_Last, "Cannot increment end iterator.");
		_Ptr += diff;
		return *this;
	}

	constexpr _Basic_String_View_Iterator operator+(const difference_type diff) const noexcept
	{
		_Basic_String_View_Iterator temp = *this;
		temp += diff;
		return temp;
	}

	constexpr _Basic_String_View_Iterator& operator--() noexcept
	{
		CUSTOM_ASSERT(_Ptr > _RefData->_First, "Cannot decrement begin iterator.");
		--_Ptr;
		return *this;
	}

	constexpr _Basic_String_View_Iterator operator--(int) noexcept
	{
		_Basic_String_View_Iterator temp = *this;
		--(*this);
		return temp;
	}

	constexpr _Basic_String_View_Iterator& operator-=(const difference_type diff) noexcept
	{
		CUSTOM_ASSERT(_Ptr - diff >= _RefData->_First, "Cannot decrement begin iterator.");
		_Ptr -= diff;
		return *this;
	}

	constexpr _Basic_String_View_Iterator operator-(const difference_type diff) const noexcept
	{
		_Basic_String_View_Iterator temp = *this;
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
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot access end iterator.");
		return *_Ptr;
	}

	constexpr reference operator[](const difference_type diff) const noexcept
	{
		return *(*this + diff);
	}

	constexpr bool operator==(const _Basic_String_View_Iterator& other) const noexcept
	{
		return _Ptr == other._Ptr;
	}

	constexpr bool operator!=(const _Basic_String_View_Iterator& other) const noexcept
	{
		return !(*this == other);
	}

public:

	friend constexpr void _verify_range(const _Basic_String_View_Iterator& first,
										const _Basic_String_View_Iterator& last) noexcept
	{
		CUSTOM_ASSERT(first._RefData == last._RefData, "string_view iterators in range are from different containers");
		CUSTOM_ASSERT(first._Ptr <= last._Ptr, "string_view iterator range transposed");
	}
};  // END _Basic_String_View_Iterator

CUSTOM_DETAIL_END

template<class Type, class Traits = custom::char_traits<Type>>
class basic_string_view		// wrapper for any kind of contiguous character buffer
{
private:
	using _Data						= detail::_Basic_String_View_Data<Type>;

public:
	static_assert(is_same_v<Type, typename Traits::char_type>,
		"The program is ill-formed if Traits::char_type is not the same type as Type.");

	static_assert(!is_array_v<Type> && is_trivial_v<Type> && is_standard_layout_v<Type>,
		"The character type of basic_string_view must be a non-array trivial standard-layout type.");

	using traits_type				= Traits;
	using value_type				= typename _Data::value_type;
	using difference_type			= typename _Data::difference_type;
	using reference					= typename _Data::reference;
	using const_reference			= typename _Data::const_reference;
	using pointer					= typename _Data::pointer;
	using const_pointer				= typename _Data::const_pointer;

	using const_iterator			= detail::_Basic_String_View_Iterator<_Data>;
	using iterator					= const_iterator;		// only const
	using const_reverse_iterator	= custom::reverse_iterator<const_iterator>;
	using reverse_iterator			= const_reverse_iterator;	// only const

	static constexpr size_t npos = static_cast<size_t>(-1);

private:
	_Data _data;

public:
	// Constructors

	constexpr basic_string_view() noexcept							= default;
	constexpr basic_string_view(const basic_string_view&) noexcept	= default;
	constexpr basic_string_view(std::nullptr_t)						= delete;

	constexpr basic_string_view(const_pointer cstring) noexcept
	{
		_data._First = cstring;
		_data._Last = cstring + traits_type::length(cstring);
	}

	constexpr basic_string_view(const_pointer cstring, const size_t len) noexcept
	{
		_data._First = cstring;
		_data._Last = cstring + len;
	}

public:
	// Operators

	constexpr basic_string_view& operator=(const basic_string_view&) noexcept = default;

	constexpr const_reference operator[](const size_t index) const noexcept
	{
		CUSTOM_ASSERT(index < size(), "Index out of bounds.");
		return _data._First[index];
	}

public:
	// Main functions

	constexpr size_t size() const noexcept
	{
		return static_cast<size_t>(_data._Last - _data._First);
	}

	constexpr size_t max_size() const noexcept
	{
		return (custom::min)(	static_cast<size_t>((numeric_limits<difference_type>::max)()),
								static_cast<size_t>(-1) / sizeof(value_type));
	}

	constexpr bool empty() const noexcept
	{
		return (_data._First == _data._Last);
	}

	constexpr const_pointer data() const noexcept
	{
		return _data._First;
	}

	constexpr const_reference at(const size_t index) const	// Acces char at index with check (read only)
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

	constexpr const_reference back() const noexcept
	{
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Last[-1];
	}

	constexpr void remove_prefix(size_t len) noexcept
	{
		CUSTOM_ASSERT(size() >= len, "Cannot remove prefix longer than total size");
		_data._First += len;
	}

	constexpr void remove_suffix(size_t len) noexcept
	{
		CUSTOM_ASSERT(size() >= len, "Cannot remove suffix longer than total size");
		_data._Last -= len;
	}

	constexpr void swap(basic_string_view& other) noexcept
	{
		if (_data._First != other._data._First)
		{
			custom::swap(_data._First, other._data._First);
			custom::swap(_data._Last, other._data._Last);
		}
	}

	constexpr size_t copy(value_type* dest, size_t len, size_t pos = 0) const
	{
		CUSTOM_ASSERT(size() >= pos, "Offset longer than total size.");
		len = _clamp_suffix_size(pos, len);		// max number of elems that can be copied
		traits_type::copy(dest, _data._First + pos, len);
		return len;
	}

	constexpr basic_string_view substr(const size_t pos = 0, size_t len = npos) const
	{
		// return a new basic_string_view moved forward by pos and trimmed to len elements
		CUSTOM_ASSERT(size() >= pos, "Offset longer than total size.");
		len = _clamp_suffix_size(pos, len);
		return basic_string_view(_data._First + pos, len);
	}

	// Compare
	constexpr int compare(const basic_string_view& other) const noexcept
	{
		return _compare_with_cstring(0, size(), other._data._First, 0, other.size());
	}

	constexpr int compare(const size_t pos, const size_t len, const basic_string_view& other) const
	{
		return _compare_with_cstring(pos, len, other._data._First, 0, other.size());
	}

	constexpr int compare(	const size_t pos, const size_t len,
							const basic_string_view& other,
							const size_t subpos, const size_t sublen) const
	{
		return _compare_with_cstring(pos, len, other._data._First, subpos, sublen);
	}

	constexpr int compare(const_pointer cstring) const
	{
		return _compare_with_cstring(0, size(), cstring, 0, traits_type::length(cstring));
	}

	constexpr int compare(const size_t pos, const size_t len, const_pointer cstring) const
	{
		return _compare_with_cstring(pos, len, cstring, 0, traits_type::length(cstring));
	}

	constexpr int compare(	const size_t pos, const size_t len,
							const_pointer cstring, const size_t cstringCount) const
	{
		return _compare_with_cstring(pos, len, cstring, 0, cstringCount);
	}
	// END Compare

	// Find
	constexpr size_t find(const basic_string_view& other, const size_t pos = 0) const noexcept
	{
		return _find_cstring(other._data._First, pos, other.size());
	}

	constexpr size_t find(const_pointer cstring, const size_t pos = 0) const noexcept
	{
		return _find_cstring(cstring, pos, traits_type::length(cstring));
	}

	constexpr size_t find(const_pointer cstring, size_t pos, size_t len) const noexcept
	{
		return _find_cstring(cstring, pos, len);
	}

	constexpr size_t find(const value_type chr, const size_t pos = 0) const noexcept
	{
		return _find_cstring(&chr, pos, 1);
	}
	// END Find

	// Rfind function
	constexpr size_t rfind(const basic_string_view& other, size_t pos = npos) const
	{
		return _rfind_cstring(other._data._First, pos, other.size());
	}

	constexpr size_t rfind(const_pointer cstring, size_t pos = npos) const
	{
		return _rfind_cstring(cstring, pos, traits_type::length(cstring));
	}

	constexpr size_t rfind(const_pointer cstring, size_t pos, size_t len) const
	{
		return _rfind_cstring(cstring, pos, len);
	}

	constexpr size_t rfind(value_type chr, size_t pos = npos) const
	{
		return _rfind_cstring(&chr, pos, 1);
	}
	// end Rfind

	// Contains
	constexpr bool contains(const basic_string_view& other) const noexcept
	{
		return find(other) != npos;
	}

	constexpr bool contains(const_pointer cstring) const noexcept
	{
		return find(cstring) != npos;
	}

	constexpr bool contains(const value_type chr) const noexcept
	{
		return find(chr) != npos;
	}
	// END Contains

	// Starts With
	constexpr bool starts_with(const basic_string_view& other) const noexcept
	{
		const size_t otherSize = other.size();

		if (size() < otherSize)
			return false;

		return traits_type::compare(_data._First, other._data._First, otherSize) == 0;
	}

	constexpr bool starts_with(const value_type chr) const noexcept
	{
		return !empty() && traits_type::eq(front(), chr);
	}

	constexpr bool starts_with(const_pointer cstring) const noexcept
	{
		return starts_with(basic_string_view(cstring));
	}
	// END Starts With

	// Ends With
	constexpr bool ends_with(const basic_string_view& other) const noexcept
	{
		const size_t otherSize = other.size();

		if (size() < otherSize)
			return false;

		return traits_type::compare(_data._Last - otherSize, other._data._First, otherSize) == 0;
	}

	constexpr bool ends_with(const value_type chr) const noexcept
	{
		return !empty() && traits_type::eq(back(), chr);
	}

	constexpr bool ends_with(const_pointer cstring) const noexcept
	{
		return ends_with(basic_string_view(cstring));
	}
	// END Ends With

public:
	// iterator specific

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

	constexpr size_t _clamp_suffix_size(const size_t pos, const size_t len) const noexcept
	{
		// trims len to the longest it can be assuming a string at/after pos
		return (custom::min)(len, size() - pos);
	}

	constexpr int _compare_with_cstring(size_t pos, size_t len,
										const_pointer cstring,
										size_t subpos, size_t sublen) const
	{
		if (pos + len > size() || subpos + sublen > traits_type::length(cstring))
			throw std::out_of_range("Invalid length or starting position.");

		return detail::_traits_cstring_compare<traits_type>(_data._First, pos, len,
			cstring, subpos, sublen);
	}

	constexpr size_t _find_cstring(const_pointer cstring, size_t pos, size_t len) const
	{
		if (pos > size() || len > traits_type::length(cstring))
			throw std::out_of_range("Invalid length or starting position.");

		return detail::_traits_cstring_find<traits_type>(_data._First, cstring, pos, len);
	}

	constexpr size_t _rfind_cstring(const_pointer cstring, size_t pos, size_t len) const
	{
		if (len > traits_type::length(cstring))
			throw std::out_of_range("Invalid length.");

		return detail::_traits_cstring_rfind<traits_type>(_data._First, cstring, pos, len);
	}
};  // END basic_string_view


// basic_string_view binary operators
template<class Type, class Traits>
constexpr bool operator==(	const basic_string_view<Type, Traits>& left,
							const basic_string_view<Type, Traits>& right)
{
	return left.compare(right) == 0;
}

template<class Type, class Alloc, class Traits>
constexpr bool operator!=(	const basic_string_view<Type, Traits>& left,
							const basic_string_view<Type, Traits>& right)
{
	return !(left == right);
}


CUSTOM_DETAIL_BEGIN

template<class Type, class Alloc>
struct _Basic_String_Data
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
};	// END _Basic_String_Data


template<class BasicStrData>
class _Basic_String_Const_Iterator
{
private:
	using _Data				= BasicStrData;

public:
    using iterator_category	= random_access_iterator_tag;
	using value_type 		= typename _Data::value_type;
	using difference_type	= typename _Data::difference_type;
	using reference 		= typename _Data::const_reference;
	using pointer			= typename _Data::const_pointer;

	value_type* _Ptr		= nullptr;
	const _Data* _RefData	= nullptr;

public:

	constexpr _Basic_String_Const_Iterator() noexcept : _Ptr() { /*Empty*/ }

	constexpr explicit _Basic_String_Const_Iterator(value_type* ptr, const _Data* data) noexcept
		:_Ptr(ptr), _RefData(data) { /*Empty*/ }

	constexpr _Basic_String_Const_Iterator& operator++() noexcept
	{
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot increment end iterator.");
		++_Ptr;
		return *this;
	}

	constexpr _Basic_String_Const_Iterator operator++(int) noexcept
	{
		_Basic_String_Const_Iterator temp = *this;
		++(*this);
		return temp;
	}

	constexpr _Basic_String_Const_Iterator& operator+=(const difference_type diff) noexcept
	{
		CUSTOM_ASSERT(_Ptr + diff <= _RefData->_Last, "Cannot increment end iterator.");
		_Ptr += diff;
		return *this;
	}

	constexpr _Basic_String_Const_Iterator operator+(const difference_type diff) const noexcept
	{
		_Basic_String_Const_Iterator temp = *this;
		temp += diff;
		return temp;
	}

	constexpr _Basic_String_Const_Iterator& operator--() noexcept
	{
		CUSTOM_ASSERT(_Ptr > _RefData->_First, "Cannot decrement begin iterator.");
		--_Ptr;
		return *this;
	}

	constexpr _Basic_String_Const_Iterator operator--(int) noexcept
	{
		_Basic_String_Const_Iterator temp = *this;
		--(*this);
		return temp;
	}

	constexpr _Basic_String_Const_Iterator& operator-=(const difference_type diff) noexcept
	{
		CUSTOM_ASSERT(_Ptr - diff >= _RefData->_First, "Cannot decrement begin iterator.");
		_Ptr -= diff;
		return *this;
	}

	constexpr _Basic_String_Const_Iterator operator-(const difference_type diff) const noexcept
	{
		_Basic_String_Const_Iterator temp = *this;
		temp -= diff;
		return temp;
	}

	constexpr pointer operator->() const noexcept
	{
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot access end iterator.");
		return pointer_traits<pointer>::pointer_to(**this);
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

	constexpr bool operator==(const _Basic_String_Const_Iterator& other) const noexcept
	{
		return _Ptr == other._Ptr;
	}

	constexpr bool operator!=(const _Basic_String_Const_Iterator& other) const noexcept
	{
		return !(*this == other);
	}

public:

	constexpr size_t get_index() const noexcept	// Get the position for the element in array from iterator
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

	friend constexpr void _verify_range(const _Basic_String_Const_Iterator& first, const _Basic_String_Const_Iterator& last) noexcept
	{
		CUSTOM_ASSERT(first._RefData == last._RefData, "basic_string iterators in range are from different containers");
		CUSTOM_ASSERT(first._Ptr <= last._Ptr, "basic_string iterator range transposed");
	}
}; // END _Basic_String_Const_Iterator


template<class BasicStrData>
class _Basic_String_Iterator : public _Basic_String_Const_Iterator<BasicStrData>		// basic_string iterator
{
private:
	using _Base				= _Basic_String_Const_Iterator<BasicStrData>;
	using _Data				= BasicStrData;
	
public:
    using iterator_category	= random_access_iterator_tag;
	using value_type 		= typename _Data::value_type;
	using difference_type	= typename _Data::difference_type;
	using reference			= typename _Data::reference;
	using pointer			= typename _Data::pointer;

public:

	constexpr _Basic_String_Iterator() noexcept  = default;

	constexpr explicit _Basic_String_Iterator(value_type* ptr, const _Data* data) noexcept
		: _Base(ptr, data) { /*Empty*/ }

	constexpr _Basic_String_Iterator& operator++() noexcept
	{
		_Base::operator++();
		return *this;
	}

	constexpr _Basic_String_Iterator operator++(int) noexcept
	{
		_Basic_String_Iterator temp = *this;
		_Base::operator++();
		return temp;
	}

	constexpr _Basic_String_Iterator& operator+=(const difference_type diff) noexcept
	{
		_Base::operator+=(diff);
		return *this;
	}

	constexpr _Basic_String_Iterator operator+(const difference_type diff) const noexcept
	{
		_Basic_String_Iterator temp = *this;
		temp += diff;
		return temp;
	}

	constexpr _Basic_String_Iterator& operator--() noexcept
	{
		_Base::operator--();
		return *this;
	}

	constexpr _Basic_String_Iterator operator--(int) noexcept
	{
		_Basic_String_Iterator temp = *this;
		_Base::operator--();
		return temp;
	}

	constexpr _Basic_String_Iterator& operator-=(const difference_type diff) noexcept
	{
		_Base::operator-=(diff);
		return *this;
	}

	constexpr _Basic_String_Iterator operator-(const difference_type diff) const noexcept
	{
		_Basic_String_Iterator temp = *this;
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
}; // END _Basic_String_Iterator

CUSTOM_DETAIL_END

template<class Type, class Alloc = custom::allocator<Type>, class Traits = custom::char_traits<Type>>
class basic_string		// null-terminated array of elements
{
private:
	using _Data						= detail::_Basic_String_Data<Type, Alloc>;
	using _Alloc_Traits				= typename _Data::_Alloc_Traits;

public:
	static_assert(is_same_v<Type, typename Alloc::value_type>,
					"Object type and Allocator type must be the same!");
	static_assert(!is_array_v<Type> && is_trivial_v<Type> && is_standard_layout_v<Type>,
					"The character type of basic_string must be a non-array trivial standard-layout type.");

	using traits_type				= Traits;
	using value_type				= typename _Data::value_type;
	using difference_type			= typename _Data::difference_type;
	using reference					= typename _Data::reference;
	using const_reference			= typename _Data::const_reference;
	using pointer					= typename _Data::pointer;
	using const_pointer				= typename _Data::const_pointer;
	using allocator_type			= Alloc;

	using iterator					= detail::_Basic_String_Iterator<_Data>;
	using const_iterator			= detail::_Basic_String_Const_Iterator<_Data>;
	using reverse_iterator			= custom::reverse_iterator<iterator>;
	using const_reverse_iterator	= custom::reverse_iterator<const_iterator>;

	template<class StringViewType>
	using _Enable_Is_String_View =	enable_if_t<conjunction_v<
													is_convertible<const StringViewType&, basic_string_view<Type, Traits>>,
													negation<is_convertible<const StringViewType&, const Type*>>>,
									bool>;

	static constexpr size_t npos = static_cast<size_t>(-1);

private:
	static constexpr size_t _DEFAULT_CAPACITY = 15;

	_Data _data;
	allocator_type _alloc;

public:
	// Constructors

	constexpr basic_string()
	{
		_initialize_from_cstring(nullptr);
	}

	constexpr basic_string(const_pointer cstring)
	{
		_initialize_from_cstring(cstring);
	}

	template<class StringViewType, _Enable_Is_String_View<StringViewType> = true>
	constexpr basic_string(const StringViewType& sv)
	{
		_initialize_from_cstring(sv.data());
	}

	constexpr basic_string(const size_t newCapacity)
	{
		_alloc_empty(newCapacity);
	}

	constexpr basic_string(std::initializer_list<value_type> list)
	{
		_alloc_empty(list.size());
		for (const auto& val : list)
			push_back(val);
	}

	constexpr basic_string(const basic_string& other)
	{
		_copy(other);
	}

	constexpr basic_string(basic_string&& other) noexcept
	{
		_move(custom::move(other));
	}

	constexpr ~basic_string() noexcept
	{
		_clean_up_string();
	}

public:
	// Operators

    constexpr operator basic_string_view<value_type, traits_type>() const noexcept
	{
        // return a string_view around *this's character-type sequence
        return basic_string_view<value_type, traits_type> {_data._First, size()};
    }

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

	constexpr basic_string& operator=(const basic_string& other)
	{
		if (_data._First != other._data._First)
		{
			_clean_up_string();
			_copy(other);
		}

		return *this;
	}

	constexpr basic_string& operator=(basic_string&& other) noexcept
	{
		if (_data._First != other._data._First)
		{
			_clean_up_string();
			_move(custom::move(other));
		}

		return *this;
	}

	constexpr basic_string& operator+=(const basic_string& other)
	{
		append(other);
		return *this;
	}

	constexpr basic_string& operator+=(const_pointer cstring)
	{
		append(cstring);
		return *this;
	}

	constexpr basic_string& operator+=(value_type chr)
	{
		push_back(chr);
		return *this;
	}

public:
	// Main functions

	constexpr void reserve(const size_t newCapacity)
	{
		if (newCapacity < size())		// Can also shrink
			_data._Last = _data._First + newCapacity;

		size_t newSize		= size();
		pointer newString 	= _alloc.allocate(newCapacity + 1);
		(void)traits_type::copy(newString, _data._First, size());
		_clean_up_string();

		_data._First 		= newString;
		_data._Last			= _data._First + newSize;
		_data._Final 		= _data._First + newCapacity;
		_data._Last[0] 		= traits_type::NULLCHR;
	}

	constexpr void shrink_to_fit()
	{
		reserve(size());
	}

	constexpr void push_back(value_type chr)
	{
		_extend_if_full();
		*(_data._Last++)	= chr;
		_data._Last[0] 		= traits_type::NULLCHR;
	}

	constexpr void pop_back()
	{
		*(--_data._Last) = traits_type::NULLCHR;
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

	constexpr size_t capacity() const noexcept
	{
		return static_cast<size_t>(_data._Final - _data._First);
	}

	constexpr bool empty() const noexcept
	{
		return (_data._First == _data._Last);
	}

	constexpr void clear()
	{
		_data._Last 	= _data._First;
		_data._First[0] = traits_type::NULLCHR;
	}

	constexpr const_pointer data() const noexcept
	{
		return _data._First;
	}

	constexpr pointer data() noexcept
	{
		return _data._First;
	}

	constexpr const_pointer c_str() const noexcept
	{
		return _data._First;
	}

	constexpr const_reference at(const size_t index) const
	{
		if (index >= size())
			throw std::out_of_range("Index out of bounds.");

		return _data._First[index];
	}

	constexpr reference at(const size_t index)
	{
		if (index >= size())
			throw std::out_of_range("Index out of bounds.");

		return _data._First[index];
	}

	constexpr const_reference front() const
	{
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._First[0];
	}

	constexpr reference front()
	{
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._First[0];
	}

	constexpr const_reference back() const
	{
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Last[-1];
	}

	constexpr reference back()
	{
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Last[-1];
	}

    constexpr void swap(basic_string& other) noexcept
	{
		if (_data._First != other._data._First)
		{
			custom::swap(_data._First, other._data._First);
			custom::swap(_data._Last, other._data._Last);
			custom::swap(_data._Final, other._data._Final);
		}
    }

	constexpr basic_string substr(size_t pos, size_t len) const
	{
		if (pos + len > size())
			throw std::out_of_range("Invalid length or starting position.");

		basic_string sub(len);	// empty string with capacity = len
		(void)traits_type::copy(sub._data._First, _data._First + pos, len);
		sub._data._Last		= sub._data._Final;
		sub._data._Last[0]	= traits_type::NULLCHR;

		return sub;
	}

// Append function overload
	constexpr basic_string& append(const basic_string& string)
	{
		_insert_from_cstring(size(), string._data._First, 0, string.size());
		return *this;
	}

	constexpr basic_string& append(const basic_string& string, size_t subpos, size_t sublen)	// Appends a copy of a substring of string.
	{
		_insert_from_cstring(size(), string._data._First, subpos, sublen);
		return *this;
	}

	constexpr basic_string& append(const_pointer cstring)	// Appends a copy of a c-string.
	{
		_insert_from_cstring(size(), cstring, 0, traits_type::length(cstring));
		return *this;
	}

	constexpr basic_string& append(const_pointer cstring, size_t nchar)	// Appends a copy of the first n characters in the c-string
	{
		_insert_from_cstring(size(), cstring, 0, nchar);
		return *this;
	}

	constexpr basic_string& append(size_t nchar, value_type chr)	// Appends n consecutive copies of character c
	{
		_insert_char(size(), nchar, chr);
		return *this;
	}
// end Append

// Insert function overload
	constexpr basic_string& insert(size_t pos, const basic_string& string)
	{
		_insert_from_cstring(pos, string._data._First, 0, string.size());
		return *this;
	}

	constexpr basic_string& insert(size_t pos, const basic_string& string, size_t subpos, size_t sublen)
	{
		_insert_from_cstring(pos, string._data._First, subpos, sublen);
		return *this;	
	}

	constexpr basic_string& insert(size_t pos, const_pointer cstring)
	{
		_insert_from_cstring(pos, cstring, 0, traits_type::length(cstring));
		return *this;
	}

	constexpr basic_string& insert(size_t pos, const_pointer cstring, size_t len)
	{
		_insert_from_cstring(pos, cstring, 0, len);
		return *this;
	}

	constexpr basic_string& insert(size_t pos, size_t nchar, value_type chr)
	{
		_insert_char(pos, nchar, chr);
		return *this;
	}

	constexpr iterator insert(const_iterator where, value_type chr)
	{
		size_t pos = where.get_index();
		_insert_char(pos, 1, chr);

		return iterator(_data._First + pos, &_data);
	}

	constexpr iterator insert(const_iterator where, size_t nchar, value_type chr)
	{
		size_t pos = where.get_index();
		_insert_char(pos, nchar, chr);

		return iterator(_data._First + pos, &_data);
	}

	constexpr iterator insert(const_iterator where, const_iterator first, const_iterator last)
	{
		if (where._RefData->_First == first._RefData->_First ||
			first._RefData->_First != last._RefData->_First)	// Check if pos string != first/last string
			throw std::domain_error("basic_string provided by first and last must be the same, but different from the one provided by where");

		size_t pos 				= where.get_index();
		size_t posFrom 			= first.get_index();
		size_t posTo 			= last.get_index();
		const_pointer cstring 	= first._RefData->_First;
		_insert_from_cstring(pos, cstring, posFrom, posTo - posFrom);

		return iterator(_data._First + pos, &_data);
	}
// end Insert

// Erase function overload
	constexpr basic_string& erase(size_t pos, size_t len)
	{
		_remove_from_cstring(pos, len);
		return *this;
	}

	constexpr iterator erase(const_iterator where)
	{
		if (where.is_end())
			throw std::out_of_range("basic_string erase iterator outside range.");

		size_t pos = where.get_index();
		_remove_from_cstring(pos, 1);

		return iterator(_data._First + pos, &_data);
	}

	constexpr iterator erase(const_iterator first, const_iterator last)
	{
		if (first.is_end())
			throw std::out_of_range("basic_string erase iterator outside range.");

		size_t posFrom	= first.get_index();
		size_t posTo	= last.get_index();
		_remove_from_cstring(posFrom, posTo - posFrom);

		return iterator(_data._First + posFrom, &_data);
	}
// end Erase

// Compare function overload
	constexpr int compare(const basic_string& string) const
	{
		return _compare_with_cstring(0, size(), string._data._First, 0, string.size());
	}

	constexpr int compare(size_t pos, size_t len, const basic_string& string, size_t subpos, size_t sublen) const
	{
		return _compare_with_cstring(pos, len, string._data._First, subpos, sublen);
	}

	constexpr int compare(const_pointer cstring) const
	{
		return _compare_with_cstring(0, size(), cstring, 0, traits_type::length(cstring));
	}

	constexpr int compare(size_t pos, size_t len, const_pointer cstring, size_t subpos, size_t sublen) const
	{
		return _compare_with_cstring(pos, len, cstring, subpos, sublen);
	}
// end Compare

// Find function overload
	constexpr size_t find(const basic_string& string, size_t pos = 0) const
	{
		return _find_cstring(string._data._First, pos, string.size());
	}

	constexpr size_t find(const_pointer cstring, size_t pos = 0) const
	{
		return _find_cstring(cstring, pos, traits_type::length(cstring));
	}

	constexpr size_t find(const_pointer cstring, size_t pos, size_t len) const
	{
		return _find_cstring(cstring, pos, len);
	}

	constexpr size_t find(value_type chr, size_t pos = 0) const
	{
		return _find_cstring(&chr, pos, 1);
	}
// end Find

// Rfind function overload
	constexpr size_t rfind(const basic_string& string, size_t pos = npos) const
	{
		return _rfind_cstring(string._data._First, pos, string.size());
	}

	constexpr size_t rfind(const_pointer cstring, size_t pos = npos) const
	{
		return _rfind_cstring(cstring, pos, traits_type::length(cstring));
	}

	constexpr size_t rfind(const_pointer cstring, size_t pos, size_t len) const
	{
		return _rfind_cstring(cstring, pos, len);
	}

	constexpr size_t rfind(value_type chr, size_t pos = npos) const
	{
		return _rfind_cstring(&chr, pos, 1);
	}
// end Rfind

// Contains
    constexpr bool contains(const basic_string_view<value_type, traits_type>& sv) const noexcept
	{
        return find(sv.data()) != npos;
    }

    constexpr bool contains(const_pointer cstring) const noexcept
	{
        return find(cstring) != npos;
    }

    constexpr bool contains(const value_type chr) const noexcept
	{
        return find(chr) != npos;
    }
// END Contains

// Starts With overload
	constexpr bool starts_with(const basic_string_view<value_type, traits_type>& sv) const noexcept
	{
        return basic_string_view<value_type, traits_type> {_data._First, size()}.starts_with(sv);
    }

    constexpr bool starts_with(const value_type chr) const noexcept
	{
        return basic_string_view<value_type, traits_type> {_data._First, size()}.starts_with(chr);
    }

    constexpr bool starts_with(const_pointer cstring) const noexcept
	{
        return basic_string_view<value_type, traits_type> {_data._First, size()}.starts_with(cstring);
    }
// END Starts With

// Ends With overload
	constexpr bool ends_with(const basic_string_view<value_type, traits_type>& sv) const noexcept
	{
        return basic_string_view<value_type, traits_type> {_data._First, size()}.ends_with(sv);
    }

    constexpr bool ends_with(const value_type chr) const noexcept
	{
        return basic_string_view<value_type, traits_type> {_data._First, size()}.ends_with(chr);
    }

    constexpr bool ends_with(const_pointer cstring) const noexcept
	{
        return basic_string_view<value_type, traits_type> {_data._First, size()}.ends_with(cstring);
    }
// END Ends With

	constexpr void print_details() const
	{
		size_t currentSize 		= size();
		size_t currentCapacity 	= capacity();

		std::cout << currentSize << ' ' << currentCapacity << '\n';
		std::cout << _data._First << '\n';
	}

public:
	// iterator specific

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

	constexpr void _alloc_empty(const size_t capacity)
	{
		_data._First	= _alloc.allocate(capacity + 1);
		_data._Last		= _data._First;
		_data._Final	= _data._First + capacity;
		_data._Last[0]	= traits_type::NULLCHR;
	}

	constexpr void _initialize_from_cstring(const_pointer cstring)
	{
		if (cstring == nullptr)
			_alloc_empty(_DEFAULT_CAPACITY);
		else
		{
			size_t len		= traits_type::length(cstring);
			_data._First	= _alloc.allocate(len + 1);
			_data._Last		= _data._First + len;
			_data._Final	= _data._First + len;
			(void)traits_type::copy(_data._First, cstring, len);
			_data._Last[0] = traits_type::NULLCHR;
		}
	}

	constexpr void _clean_up_string()
	{
		if (_data._First != nullptr)
		{
			_alloc.deallocate(_data._First, capacity() + 1);
			_data._First	= nullptr;
			_data._Last		= nullptr;
			_data._Final	= nullptr;
		}
	}

	constexpr void _extend_if_full()	// Reserve 50% more capacity when full
	{
		if (_data._Last == _data._Final)
			reserve(capacity() + capacity() / 2 + 1);
	}

	constexpr void _copy(const basic_string& other)
	{
		_alloc_empty(other.capacity());
		(void)traits_type::copy(_data._First, other._data._First, other.size());

		_data._Last		= _data._First + other.size();
		_data._Final	= _data._First + other.capacity();
		_data._Last[0]	= traits_type::NULLCHR;
	}

	constexpr void _move(basic_string&& other)
	{
		_data._First	= other._data._First;
		_data._Last		= other._data._Last;
		_data._Final	= other._data._Final;

		other._initialize_from_cstring(nullptr);
	}

	constexpr void _insert_from_cstring(size_t pos, const_pointer cstring, size_t subpos, size_t sublen)
	{
		if (pos > size() || subpos + sublen > traits_type::length(cstring))
			throw std::out_of_range("Invalid length or starting position.");

		size_t newSize = size() + sublen;
		if (newSize > capacity())
			reserve(newSize);

		(void)traits_type::move(_data._First + pos + sublen, _data._First + pos, size() - pos);	// copy last sublen positions to right
		(void)traits_type::move(_data._First + pos, cstring + subpos, sublen);					// add substr from cstring between
		_data._Last 	= _data._First + newSize;
		_data._Last[0] 	= traits_type::NULLCHR;
	}

	constexpr void _insert_char(size_t pos, size_t nchar, value_type chr)
	{
		if (pos > size())
			throw std::out_of_range("Invalid starting position.");

		size_t newSize = size() + nchar;
		if (newSize > capacity())
			reserve(newSize);

		(void)traits_type::move(_data._First + pos + nchar, _data._First + pos, size() - pos);	// copy last nchar positions to right
		(void)traits_type::assign(_data._First + pos, nchar, chr);								// add nchar * chr in between
		_data._Last 	= _data._First + newSize;
		_data._Last[0] 	= traits_type::NULLCHR;
	}

	constexpr void _remove_from_cstring(size_t pos, size_t len)
	{
		if (pos + len > size())
			throw std::out_of_range("Invalid length or starting position.");

		(void)traits_type::move(_data._First + pos, _data._First + pos + len, size() - pos - len);
		_data._Last 	= _data._First + size() - len;
		_data._Last[0] 	= traits_type::NULLCHR;
	}

	constexpr int _compare_with_cstring(size_t pos, size_t len, const_pointer cstring, size_t subpos, size_t sublen) const
	{
		if (pos + len > size() || subpos + sublen > Traits::length(cstring))
			throw std::out_of_range("Invalid length or starting position.");

		return detail::_traits_cstring_compare<traits_type>(_data._First, pos, len, cstring, subpos, sublen);
	}

	constexpr size_t _find_cstring(const_pointer cstring, size_t pos, size_t len) const
	{
		if (pos > size() || len > traits_type::length(cstring))
			throw std::out_of_range("Invalid length or starting position.");

		return detail::_traits_cstring_find<traits_type>(_data._First, cstring, pos, len);
	}

	constexpr size_t _rfind_cstring(const_pointer cstring, size_t pos, size_t len) const
	{
		if (len > traits_type::length(cstring))
			throw std::out_of_range("Invalid length.");

		return detail::_traits_cstring_rfind<traits_type>(_data._First, cstring, pos, len);
	}
};	// END basic_string


// basic_string binary operators
template<class Type, class Alloc, class Traits>
constexpr bool operator==(	const basic_string<Type, Alloc, Traits>& left,
							const basic_string<Type, Alloc, Traits>& right)
{
	return left.compare(right) == 0;
}

template<class Type, class Alloc, class Traits>
constexpr bool operator!=(	const basic_string<Type, Alloc, Traits>& left,
							const basic_string<Type, Alloc, Traits>& right)
{
	return !(left == right);
}

template<class Type, class Alloc, class Traits>
constexpr basic_string<Type, Alloc, Traits> operator+(	const basic_string<Type, Alloc, Traits>& left,
														const basic_string<Type, Alloc, Traits>& right)
{
	// create empty string with capacity equal to sum of sizes and append left then right string
	return basic_string<Type, Alloc, Traits>(left.size() + right.size()).append(left).append(right);
}

template<class Type, class Alloc, class Traits>
constexpr basic_string<Type, Alloc, Traits> operator+(	const basic_string<Type, Alloc, Traits>& left,
														const Type* right)
{
	return basic_string<Type, Alloc, Traits>(left.size() + Traits::length(right)).append(left).append(right);
}

template<class Type, class Alloc, class Traits>
constexpr basic_string<Type, Alloc, Traits> operator+(	const Type* left,
														const basic_string<Type, Alloc, Traits>& right)
{
	return basic_string<Type, Alloc, Traits>(Traits::length(left) + right.size()).append(left).append(right);
}

template<class Type, class Alloc, class Traits>
constexpr basic_string<Type, Alloc, Traits> operator+(	const basic_string<Type, Alloc, Traits>& left,
														const Type right)
{
	return basic_string<Type, Alloc, Traits>(left.size() + 1).append(left).append(1, right);
}

template<class Type, class Alloc, class Traits>
constexpr basic_string<Type, Alloc, Traits> operator+(	const Type left,
														const basic_string<Type, Alloc, Traits>& right)
{
	return basic_string<Type, Alloc, Traits>(1 + right.size()).append(1, left).append(right);
}

CUSTOM_END