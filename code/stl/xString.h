#pragma once
#include "xCharTraits.h"
#include "xMemory.h"
#include "c_iterator.h"
#include "c_algorithm.h"


CUSTOM_BEGIN


#pragma region BasicStringView
template<class Type>
struct _BasicStringViewData
{
	using ValueType			= Type;
	using DifferenceType	= ptrdiff_t;
	using reference			= ValueType&;
	using const_reference	= const ValueType&;
	using pointer			= ValueType*;
	using const_pointer		= const ValueType*;


	const_pointer _First		= nullptr;
	const_pointer _Last		= nullptr;
};  // END _BasicStringViewData


template<class BasicStrVData>
class BasicStringViewIterator	// This is only const
{
private:
	using _Data				= BasicStrVData;

public:
	using iterator_category	= random_access_iterator_tag;
	using ValueType			= typename _Data::ValueType;
	using DifferenceType	= typename _Data::DifferenceType;
	using reference			= typename _Data::const_reference;
	using pointer			= typename _Data::const_pointer;

	pointer _Ptr			= nullptr;
	const _Data* _RefData	= nullptr;

public:

	constexpr BasicStringViewIterator() noexcept = default;

	constexpr explicit BasicStringViewIterator(pointer ptr, const _Data* data) noexcept
		: _Ptr(ptr), _RefData(data) { /*Empty*/
	}

	constexpr BasicStringViewIterator& operator++() noexcept {
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot increment end iterator.");
		++_Ptr;
		return *this;
	}

	constexpr BasicStringViewIterator operator++(int) noexcept {
		BasicStringViewIterator temp = *this;
		++(*this);
		return temp;
	}

	constexpr BasicStringViewIterator& operator+=(const DifferenceType diff) noexcept {
		CUSTOM_ASSERT(_Ptr + diff <= _RefData->_Last, "Cannot increment end iterator.");
		_Ptr += diff;
		return *this;
	}

	constexpr BasicStringViewIterator operator+(const DifferenceType diff) const noexcept {
		BasicStringViewIterator temp = *this;
		temp += diff;
		return temp;
	}

	constexpr BasicStringViewIterator& operator--() noexcept {
		CUSTOM_ASSERT(_Ptr > _RefData->_First, "Cannot decrement begin iterator.");
		--_Ptr;
		return *this;
	}

	constexpr BasicStringViewIterator operator--(int) noexcept {
		BasicStringViewIterator temp = *this;
		--(*this);
		return temp;
	}

	constexpr BasicStringViewIterator& operator-=(const DifferenceType diff) noexcept {
		CUSTOM_ASSERT(_Ptr - diff >= _RefData->_First, "Cannot decrement begin iterator.");
		_Ptr -= diff;
		return *this;
	}

	constexpr BasicStringViewIterator operator-(const DifferenceType diff) const noexcept {
		BasicStringViewIterator temp = *this;
		temp -= diff;
		return temp;
	}

	constexpr pointer operator->() const noexcept {
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot access end iterator.");
		return _Ptr;
	}

	constexpr reference operator*() const noexcept {
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot access end iterator.");
		return *_Ptr;
	}

	constexpr reference operator[](const DifferenceType diff) const noexcept {
		return *(*this + diff);
	}

	constexpr bool operator==(const BasicStringViewIterator& other) const noexcept {
		return _Ptr == other._Ptr;
	}

	constexpr bool operator!=(const BasicStringViewIterator& other) const noexcept {
		return !(*this == other);
	}

public:

	friend constexpr void _verify_range(const BasicStringViewIterator& first,
		const BasicStringViewIterator& last) noexcept {

		CUSTOM_ASSERT(first._RefData == last._RefData, "StringView iterators in range are from different containers");
		CUSTOM_ASSERT(first._Ptr <= last._Ptr, "StringView iterator range transposed");
	}
};  // END BasicStringConstIterator


template<class Type, class Traits = custom::CharTraits<Type>>
class BasicStringView		// wrapper for any kind of contiguous character buffer
{
private:
	using _Data					= _BasicStringViewData<Type>;

public:
	static_assert(is_same_v<Type, typename Traits::CharType>,
		"The program is ill-formed if Traits::CharType is not the same type as Type.");

	static_assert(!is_array_v<Type>&& IsTrivial_v<Type>&& IsStandardLayout_v<Type>,
		"The character type of BasicStringView must be a non-array trivial standard-layout type.");

	using TraitsType			= Traits;
	using ValueType				= typename _Data::ValueType;
	using DifferenceType		= typename _Data::DifferenceType;
	using reference				= typename _Data::reference;
	using const_reference		= typename _Data::const_reference;
	using pointer				= typename _Data::pointer;
	using const_pointer			= typename _Data::const_pointer;

	using const_iterator			= BasicStringViewIterator<_Data>;
	using iterator				= const_iterator;		// only const
	using const_reverse_iterator	= custom::reverse_iterator<const_iterator>;
	using reverse_iterator		= const_reverse_iterator;	// only const

	static constexpr size_t npos = static_cast<size_t>(-1);

private:
	_Data _data;

public:
	// Constructors

	constexpr BasicStringView() noexcept						= default;
	constexpr BasicStringView(const BasicStringView&) noexcept	= default;
	constexpr BasicStringView(std::nullptr_t)					= delete;

	constexpr BasicStringView(const_pointer cstring) noexcept {
		_data._First = cstring;
		_data._Last = cstring + TraitsType::length(cstring);
	}

	constexpr BasicStringView(const_pointer cstring, const size_t len) noexcept {
		_data._First = cstring;
		_data._Last = cstring + len;
	}

public:
	// Operators

	constexpr BasicStringView& operator=(const BasicStringView&) noexcept = default;

	constexpr const_reference operator[](const size_t index) const noexcept {
		CUSTOM_ASSERT(index < size(), "Index out of bounds.");
		return _data._First[index];
	}

public:
	// Main functions

	constexpr size_t size() const noexcept {
		return static_cast<size_t>(_data._Last - _data._First);
	}

	constexpr size_t max_size() const noexcept {
		return (custom::min)(static_cast<size_t>((numeric_limits<DifferenceType>::max)()),
			static_cast<size_t>(-1) / sizeof(ValueType));
	}

	constexpr bool empty() const noexcept {
		return (_data._First == _data._Last);
	}

	constexpr const_pointer data() const noexcept {
		return _data._First;
	}

	constexpr const_reference at(const size_t index) const {						// Acces char at index with check (read only)
		if (index >= size())
			throw std::out_of_range("Index out of bounds.");

		return _data._First[index];
	}

	constexpr const_reference front() const noexcept {
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._First[0];
	}

	constexpr const_reference back() const noexcept {
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Last[-1];
	}

	constexpr void remove_prefix(size_t len) noexcept {
		CUSTOM_ASSERT(size() >= len, "Cannot remove prefix longer than total size");
		_data._First += len;
	}

	constexpr void remove_suffix(size_t len) noexcept {
		CUSTOM_ASSERT(size() >= len, "Cannot remove suffix longer than total size");
		_data._Last -= len;
	}

	constexpr void swap(BasicStringView& other) noexcept {
		if (_data._First != other._data._First)
		{
			custom::swap(_data._First, other._data._First);
			custom::swap(_data._Last, other._data._Last);
		}
	}

	constexpr size_t copy(ValueType* dest, size_t len, size_t pos = 0) const {
		CUSTOM_ASSERT(size() >= pos, "Offset longer than total size.");
		len = _clamp_suffix_size(pos, len);		// max number of elems that can be copied
		TraitsType::copy(dest, _data._First + pos, len);
		return len;
	}

	constexpr BasicStringView substr(const size_t pos = 0, size_t len = npos) const {
		// return a new BasicStringView moved forward by pos and trimmed to len elements
		CUSTOM_ASSERT(size() >= pos, "Offset longer than total size.");
		len = _clamp_suffix_size(pos, len);
		return BasicStringView(_data._First + pos, len);
	}

	// Compare
	constexpr int compare(const BasicStringView& other) const noexcept {
		return _compare_with_cstring(0, size(), other._data._First, 0, other.size());
	}

	constexpr int compare(const size_t pos, const size_t len, const BasicStringView& other) const {
		return _compare_with_cstring(pos, len, other._data._First, 0, other.size());
	}

	constexpr int compare(const size_t pos, const size_t len,
		const BasicStringView& other,
		const size_t subpos, const size_t sublen) const {

		return _compare_with_cstring(pos, len, other._data._First, subpos, sublen);
	}

	constexpr int compare(const_pointer cstring) const {
		return _compare_with_cstring(0, size(), cstring, 0, TraitsType::length(cstring));
	}

	constexpr int compare(const size_t pos, const size_t len, const_pointer cstring) const {
		return _compare_with_cstring(pos, len, cstring, 0, TraitsType::length(cstring));
	}

	constexpr int compare(const size_t pos, const size_t len,
		const_pointer cstring, const size_t cstringCount) const {

		return _compare_with_cstring(pos, len, cstring, 0, cstringCount);
	}
	// END Compare

	// Find
	constexpr size_t find(const BasicStringView& other, const size_t pos = 0) const noexcept {
		return _find_cstring(other._data._First, pos, other.size());
	}

	constexpr size_t find(const_pointer cstring, const size_t pos = 0) const noexcept {
		return _find_cstring(cstring, pos, TraitsType::length(cstring));
	}

	constexpr size_t find(const_pointer cstring, size_t pos, size_t len) const noexcept {
		return _find_cstring(cstring, pos, len);
	}

	constexpr size_t find(const ValueType chr, const size_t pos = 0) const noexcept {
		return _find_cstring(&chr, pos, 1);
	}
	// END Find

	// Rfind function
	constexpr size_t rfind(const BasicStringView& other, size_t pos = npos) const {
		return _rfind_cstring(other._data._First, pos, other.size());
	}

	constexpr size_t rfind(const_pointer cstring, size_t pos = npos) const {
		return _rfind_cstring(cstring, pos, TraitsType::length(cstring));
	}

	constexpr size_t rfind(const_pointer cstring, size_t pos, size_t len) const {
		return _rfind_cstring(cstring, pos, len);
	}

	constexpr size_t rfind(ValueType chr, size_t pos = npos) const {
		return _rfind_cstring(&chr, pos, 1);
	}
	// end Rfind

	// Contains
	constexpr bool contains(const BasicStringView& other) const noexcept {
		return find(other) != npos;
	}

	constexpr bool contains(const_pointer cstring) const noexcept {
		return find(cstring) != npos;
	}

	constexpr bool contains(const ValueType chr) const noexcept {
		return find(chr) != npos;
	}
	// END Contains

	// Starts With
	constexpr bool starts_with(const BasicStringView& other) const noexcept {
		const size_t otherSize = other.size();

		if (size() < otherSize)
			return false;

		return TraitsType::compare(_data._First, other._data._First, otherSize) == 0;
	}

	constexpr bool starts_with(const ValueType chr) const noexcept {
		return !empty() && TraitsType::eq(front(), chr);
	}

	constexpr bool starts_with(const_pointer cstring) const noexcept {
		return starts_with(BasicStringView(cstring));
	}
	// END Starts With

	// Ends With
	constexpr bool ends_with(const BasicStringView& other) const noexcept {
		const size_t otherSize = other.size();

		if (size() < otherSize)
			return false;

		return TraitsType::compare(_data._Last - otherSize, other._data._First, otherSize) == 0;
	}

	constexpr bool ends_with(const ValueType chr) const noexcept {
		return !empty() && TraitsType::eq(back(), chr);
	}

	constexpr bool ends_with(const_pointer cstring) const noexcept {
		return ends_with(BasicStringView(cstring));
	}
	// END Ends With

public:
	// iterator specific

	constexpr iterator begin() noexcept {
		return iterator(_data._First, &_data);
	}

	constexpr const_iterator begin() const noexcept {
		return const_iterator(_data._First, &_data);
	}

	constexpr reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}

	constexpr const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}

	constexpr iterator end() noexcept {
		return iterator(_data._Last, &_data);
	}

	constexpr const_iterator end() const noexcept {
		return const_iterator(_data._Last, &_data);
	}

	constexpr reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}

	constexpr const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}

private:
	// Helpers

	constexpr size_t _clamp_suffix_size(const size_t pos, const size_t len) const noexcept {
		// trims len to the longest it can be assuming a string at/after pos
		return (custom::min)(len, size() - pos);
	}

	constexpr int _compare_with_cstring(size_t pos, size_t len, const_pointer cstring, size_t subpos, size_t sublen) const {
		if (pos + len > size() || subpos + sublen > TraitsType::length(cstring))
			throw std::out_of_range("Invalid length or starting position.");

		return detail::_traits_cstring_compare<TraitsType>(_data._First, pos, len,
			cstring, subpos, sublen);
	}

	constexpr size_t _find_cstring(const_pointer cstring, size_t pos, size_t len) const {
		if (pos > size() || len > TraitsType::length(cstring))
			throw std::out_of_range("Invalid length or starting position.");

		return detail::_traits_cstring_find<TraitsType>(_data._First, cstring, pos, len);
	}

	constexpr size_t _rfind_cstring(const_pointer cstring, size_t pos, size_t len) const {
		if (len > TraitsType::length(cstring))
			throw std::out_of_range("Invalid length.");

		return detail::_traits_cstring_rfind<TraitsType>(_data._First, cstring, pos, len);
	}
};  // END BasicStringView


// BasicStringView binary operators
template<class Type, class Traits>
constexpr bool operator==(const BasicStringView<Type, Traits>& left,
	const BasicStringView<Type, Traits>& right) {

	return left.compare(right) == 0;
}

template<class Type, class Alloc, class Traits>
constexpr bool operator!=(const BasicStringView<Type, Traits>& left,
	const BasicStringView<Type, Traits>& right) {

	return !(left == right);
}
#pragma endregion BasicStringView



#pragma region BasicString
template<class Type, class Alloc>
struct _BasicStringData
{
	using _AllocTraits		= allocator_traits<Alloc>;

	using ValueType			= typename _AllocTraits::ValueType;
	using DifferenceType	= typename _AllocTraits::DifferenceType;
	using reference			= typename _AllocTraits::reference;
	using const_reference	= typename _AllocTraits::const_reference;
	using pointer			= typename _AllocTraits::pointer;
	using const_pointer		= typename _AllocTraits::const_pointer;


	pointer _First			= nullptr;			// Actual container array
	pointer _Last			= nullptr;			// pointer to end
	pointer _Final			= nullptr;			// pointer to capacity end
};	// END _BasicStringData


template<class BasicStrData>
class BasicStringConstIterator
{
private:
	using _Data				= BasicStrData;

public:
    using iterator_category 	= random_access_iterator_tag;
	using ValueType 		= typename _Data::ValueType;
	using DifferenceType	= typename _Data::DifferenceType;
	using reference 		= typename _Data::const_reference;
	using pointer			= typename _Data::const_pointer;

	ValueType* _Ptr			= nullptr;
	const _Data* _RefData	= nullptr;

public:

	constexpr BasicStringConstIterator() noexcept : _Ptr() { /*Empty*/ }

	constexpr explicit BasicStringConstIterator(ValueType* ptr, const _Data* data) noexcept
		:_Ptr(ptr), _RefData(data) { /*Empty*/ }

	constexpr BasicStringConstIterator& operator++() noexcept {
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot increment end iterator.");
		++_Ptr;
		return *this;
	}

	constexpr BasicStringConstIterator operator++(int) noexcept {
		BasicStringConstIterator temp = *this;
		++(*this);
		return temp;
	}

	constexpr BasicStringConstIterator& operator+=(const DifferenceType diff) noexcept {
		CUSTOM_ASSERT(_Ptr + diff <= _RefData->_Last, "Cannot increment end iterator.");
		_Ptr += diff;
		return *this;
	}

	constexpr BasicStringConstIterator operator+(const DifferenceType diff) const noexcept {
		BasicStringConstIterator temp = *this;
		temp += diff;
		return temp;
	}

	constexpr BasicStringConstIterator& operator--() noexcept {
		CUSTOM_ASSERT(_Ptr > _RefData->_First, "Cannot decrement begin iterator.");
		--_Ptr;
		return *this;
	}

	constexpr BasicStringConstIterator operator--(int) noexcept {
		BasicStringConstIterator temp = *this;
		--(*this);
		return temp;
	}

	constexpr BasicStringConstIterator& operator-=(const DifferenceType diff) noexcept {
		CUSTOM_ASSERT(_Ptr - diff >= _RefData->_First, "Cannot decrement begin iterator.");
		_Ptr -= diff;
		return *this;
	}

	constexpr BasicStringConstIterator operator-(const DifferenceType diff) const noexcept {
		BasicStringConstIterator temp = *this;
		temp -= diff;
		return temp;
	}

	constexpr pointer operator->() const noexcept {
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot access end iterator.");
		return pointer_traits<pointer>::pointer_to(**this);
	}

	constexpr reference operator*() const noexcept {
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot dereference end iterator.");
		return *_Ptr;
	}

	constexpr reference operator[](const DifferenceType diff) const noexcept {
        return *(*this + diff);
    }

	constexpr bool operator==(const BasicStringConstIterator& other) const noexcept {
		return _Ptr == other._Ptr;
	}

	constexpr bool operator!=(const BasicStringConstIterator& other) const noexcept {
		return !(*this == other);
	}

public:

	constexpr size_t get_index() const noexcept {					// Get the position for the element in array from iterator
		return static_cast<size_t>(_Ptr - _RefData->_First);
	}

	constexpr bool is_begin() const noexcept {
		return _Ptr == _RefData->_First;
	}

	constexpr bool is_end() const noexcept {
		return _Ptr == _RefData->_Last;
	}

	friend constexpr void _verify_range(const BasicStringConstIterator& first, const BasicStringConstIterator& last) noexcept {
		CUSTOM_ASSERT(first._RefData == last._RefData, "BasicString iterators in range are from different containers");
		CUSTOM_ASSERT(first._Ptr <= last._Ptr, "BasicString iterator range transposed");
	}
}; // END BasicStringConstIterator


template<class BasicStrData>
class BasicStringIterator : public BasicStringConstIterator<BasicStrData>		// BasicString iterator
{
private:
	using _Base				= BasicStringConstIterator<BasicStrData>;
	using _Data				= BasicStrData;
	
public:
    using iterator_category 	= random_access_iterator_tag;
	using ValueType 		= typename _Data::ValueType;
	using DifferenceType	= typename _Data::DifferenceType;
	using reference			= typename _Data::reference;
	using pointer			= typename _Data::pointer;

public:

	constexpr BasicStringIterator() noexcept  = default;

	constexpr explicit BasicStringIterator(ValueType* ptr, const _Data* data) noexcept
		: _Base(ptr, data) { /*Empty*/ }

	constexpr BasicStringIterator& operator++() noexcept {
		_Base::operator++();
		return *this;
	}

	constexpr BasicStringIterator operator++(int) noexcept {
		BasicStringIterator temp = *this;
		_Base::operator++();
		return temp;
	}

	constexpr BasicStringIterator& operator+=(const DifferenceType diff) noexcept {
		_Base::operator+=(diff);
		return *this;
	}

	constexpr BasicStringIterator operator+(const DifferenceType diff) const noexcept {
		BasicStringIterator temp = *this;
		temp += diff;
		return temp;
	}

	constexpr BasicStringIterator& operator--() noexcept {
		_Base::operator--();
		return *this;
	}

	constexpr BasicStringIterator operator--(int) noexcept {
		BasicStringIterator temp = *this;
		_Base::operator--();
		return temp;
	}

	constexpr BasicStringIterator& operator-=(const DifferenceType diff) noexcept {
		_Base::operator-=(diff);
		return *this;
	}

	constexpr BasicStringIterator operator-(const DifferenceType diff) const noexcept {
		BasicStringIterator temp = *this;
		temp -= diff;
		return temp;
	}

	constexpr pointer operator->() const noexcept {
		return const_cast<pointer>(_Base::operator->());
	}

	constexpr reference operator*() const noexcept {
		return const_cast<reference>(_Base::operator*());
	}

	constexpr reference operator[](const DifferenceType diff) const noexcept {
        return const_cast<reference>(_Base::operator[](diff));
    }
}; // END BasicStringIterator


template<class Type, class Alloc = custom::allocator<Type>, class Traits = custom::CharTraits<Type>>
class BasicString		// null-terminated array of elements
{
private:
	using _Data					= _BasicStringData<Type, Alloc>;
	using _AllocTraits			= typename _Data::_AllocTraits;

public:
	static_assert(is_same_v<Type, typename Alloc::ValueType>, "Object type and Allocator type must be the same!");
	static_assert(!is_array_v<Type> && IsTrivial_v<Type> && IsStandardLayout_v<Type>,
					"The character type of BasicString must be a non-array trivial standard-layout type.");

	using TraitsType			= Traits;
	using ValueType				= typename _Data::ValueType;
	using DifferenceType		= typename _Data::DifferenceType;
	using reference				= typename _Data::reference;
	using const_reference		= typename _Data::const_reference;
	using pointer				= typename _Data::pointer;
	using const_pointer			= typename _Data::const_pointer;
	using allocator_type			= Alloc;

	using iterator				= BasicStringIterator<_Data>;
	using const_iterator			= BasicStringConstIterator<_Data>;
	using reverse_iterator		= custom::reverse_iterator<iterator>;
	using const_reverse_iterator	= custom::reverse_iterator<const_iterator>;

	template<class StringViewType>
	using IsStringView = 	enable_if_t<Conjunction_v<
										IsConvertible<const StringViewType&, BasicStringView<Type, Traits>>,
										negation<IsConvertible<const StringViewType&, const Type*>>>,
							bool>;

	static constexpr size_t npos = static_cast<size_t>(-1);

private:
	static constexpr size_t _DEFAULT_CAPACITY = 15;

	_Data _data;
	allocator_type _alloc;

public:
	// Constructors

	constexpr BasicString() {
		_initialize_from_cstring(nullptr);
	}

	constexpr BasicString(const_pointer cstring) {
		_initialize_from_cstring(cstring);
	}

	template<class StringViewType, IsStringView<StringViewType> = true>
	constexpr BasicString(const StringViewType& sv) {
		_initialize_from_cstring(sv.data());
	}

	constexpr BasicString(const size_t newCapacity) {
		_alloc_empty(newCapacity);
	}

	constexpr BasicString(const BasicString& other) {
		_copy(other);
	}

	constexpr BasicString(BasicString&& other) noexcept {
		_move(custom::move(other));
	}

	constexpr ~BasicString() noexcept {
		_clean_up_string();
	}

public:
	// Operators

    constexpr operator BasicStringView<ValueType, TraitsType>() const noexcept {
        // return a StringView around *this's character-type sequence
        return BasicStringView<ValueType, TraitsType> {_data._First, size()};
    }

	constexpr const_reference operator[](const size_t index) const noexcept{
		CUSTOM_ASSERT(index < size(), "Index out of bounds.");
		return _data._First[index];
	}

	constexpr reference operator[](const size_t index) noexcept {
		CUSTOM_ASSERT(index < size(), "Index out of bounds.");
		return _data._First[index];
	}

	constexpr BasicString& operator=(const BasicString& other) {
		if (_data._First != other._data._First)
		{
			_clean_up_string();
			_copy(other);
		}

		return *this;
	}

	constexpr BasicString& operator=(BasicString&& other) noexcept {
		if (_data._First != other._data._First)
		{
			_clean_up_string();
			_move(custom::move(other));
		}

		return *this;
	}

	constexpr BasicString& operator+=(const BasicString& other) {
		append(other);
		return *this;
	}

	constexpr BasicString& operator+=(const_pointer cstring) {
		append(cstring);
		return *this;
	}

	constexpr BasicString& operator+=(ValueType chr) {
		push_back(chr);
		return *this;
	}

public:
	// Main functions

	constexpr void reserve(const size_t newCapacity) {			// Allocate memory and copy values if needed
		if (newCapacity < size())		// Can also shrink
			_data._Last = _data._First + newCapacity;

		size_t newSize		= size();
		pointer newString 	= _alloc.allocate(newCapacity + 1);
		(void)TraitsType::copy(newString, _data._First, size());
		_clean_up_string();

		_data._First 		= newString;
		_data._Last			= _data._First + newSize;
		_data._Final 		= _data._First + newCapacity;
		_data._Last[0] 		= TraitsType::NULLCHR;
	}

	constexpr void shrink_to_fit() {							// Allocate memory with capacity equal to size and copy values there
		reserve(size());
	}

	constexpr void push_back(ValueType chr) {
		_extend_if_full();
		*(_data._Last++)	= chr;
		_data._Last[0] 		= TraitsType::NULLCHR;
	}

	constexpr void pop_back() {
		*(--_data._Last) = TraitsType::NULLCHR;
	}

	constexpr size_t size() const noexcept {
		return static_cast<size_t>(_data._Last - _data._First);
	}

	constexpr size_t max_size() const noexcept {
		return (custom::min)(	static_cast<size_t>((numeric_limits<DifferenceType>::max)()),
								_AllocTraits::max_size(_alloc));
	}

	constexpr size_t capacity() const noexcept {
		return static_cast<size_t>(_data._Final - _data._First);
	}

	constexpr bool empty() const noexcept {
		return (_data._First == _data._Last);
	}

	constexpr void clear() {
		_data._Last 	= _data._First;
		_data._First[0] = TraitsType::NULLCHR;
	}

	constexpr const_pointer data() const noexcept {
		return _data._First;
	}

	constexpr pointer data() noexcept {
		return _data._First;
	}

	constexpr const_pointer c_str() const noexcept {
		return _data._First;
	}

	constexpr const_reference at(const size_t index) const {						// Acces char at index with check (read only)
		if (index >= size())
			throw std::out_of_range("Index out of bounds.");

		return _data._First[index];
	}

	constexpr reference at(const size_t index) {									// Acces char at index with check
		if (index >= size())
			throw std::out_of_range("Index out of bounds.");

		return _data._First[index];
	}

	constexpr const_reference front() const {
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._First[0];
	}

	constexpr reference front() {													// Get the value of the first component
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._First[0];
	}

	constexpr const_reference back() const {
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Last[-1];
	}

	constexpr reference back() {													// Get the value of the last component
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Last[-1];
	}

    constexpr void swap(BasicString& other) noexcept {
		if (_data._First != other._data._First)
		{
			custom::swap(_data._First, other._data._First);
			custom::swap(_data._Last, other._data._Last);
			custom::swap(_data._Final, other._data._Final);
		}
    }

	constexpr BasicString substr(size_t pos, size_t len) const {					// Create substring from current string
		if (pos + len > size())
			throw std::out_of_range("Invalid length or starting position.");

		BasicString sub(len);	// empty string with capacity = len
		(void)TraitsType::copy(sub._data._First, _data._First + pos, len);
		sub._data._Last		= sub._data._Final;
		sub._data._Last[0]	= TraitsType::NULLCHR;

		return sub;
	}

// Append function overload
	constexpr BasicString& append(const BasicString& string) {								// Appends a copy of string
		_insert_from_cstring(size(), string._data._First, 0, string.size());
		return *this;
	}

	constexpr BasicString& append(const BasicString& string, size_t subpos, size_t sublen) {	// Appends a copy of a substring of string.
		_insert_from_cstring(size(), string._data._First, subpos, sublen);
		return *this;
	}

	constexpr BasicString& append(const_pointer cstring) {									// Appends a copy of a c-string.
		_insert_from_cstring(size(), cstring, 0, TraitsType::length(cstring));
		return *this;
	}

	constexpr BasicString& append(const_pointer cstring, size_t nchar) {						// Appends a copy of the first n characters in the c-string
		_insert_from_cstring(size(), cstring, 0, nchar);
		return *this;
	}

	constexpr BasicString& append(size_t nchar, ValueType chr) {							// Appends n consecutive copies of character c
		_insert_char(size(), nchar, chr);
		return *this;
	}
// end Append

// Insert function overload
	constexpr BasicString& insert(size_t pos, const BasicString& string) {
		_insert_from_cstring(pos, string._data._First, 0, string.size());
		return *this;
	}

	constexpr BasicString& insert(size_t pos, const BasicString& string, size_t subpos, size_t sublen) {
		_insert_from_cstring(pos, string._data._First, subpos, sublen);
		return *this;	
	}

	constexpr BasicString& insert(size_t pos, const_pointer cstring) {
		_insert_from_cstring(pos, cstring, 0, TraitsType::length(cstring));
		return *this;
	}

	constexpr BasicString& insert(size_t pos, const_pointer cstring, size_t len) {
		_insert_from_cstring(pos, cstring, 0, len);
		return *this;
	}

	constexpr BasicString& insert(size_t pos, size_t nchar, ValueType chr) {
		_insert_char(pos, nchar, chr);
		return *this;
	}

	constexpr iterator insert(const_iterator where, ValueType chr) {
		size_t pos = where.get_index();
		_insert_char(pos, 1, chr);

		return iterator(_data._First + pos, &_data);
	}

	constexpr iterator insert(const_iterator where, size_t nchar, ValueType chr) {
		size_t pos = where.get_index();
		_insert_char(pos, nchar, chr);

		return iterator(_data._First + pos, &_data);
	}

	constexpr iterator insert(const_iterator where, const_iterator first, const_iterator last) {
		if (where._RefData->_First == first._RefData->_First ||
			first._RefData->_First != last._RefData->_First)	// Check if pos string != first/last string
			throw std::domain_error("BasicString provided by first and last must be the same, but different from the one provided by where");

		size_t pos 				= where.get_index();
		size_t posFrom 			= first.get_index();
		size_t posTo 			= last.get_index();
		const_pointer cstring 	= first._RefData->_First;
		_insert_from_cstring(pos, cstring, posFrom, posTo - posFrom);

		return iterator(_data._First + pos, &_data);
	}
// end Insert

// Erase function overload
	constexpr BasicString& erase(size_t pos, size_t len) {
		_remove_from_cstring(pos, len);
		return *this;
	}

	constexpr iterator erase(const_iterator where) {
		if (where.is_end())
			throw std::out_of_range("BasicString erase iterator outside range.");

		size_t pos = where.get_index();
		_remove_from_cstring(pos, 1);

		return iterator(_data._First + pos, &_data);
	}

	constexpr iterator erase(const_iterator first, const_iterator last) {
		if (first.is_end())
			throw std::out_of_range("BasicString erase iterator outside range.");

		size_t posFrom	= first.get_index();
		size_t posTo	= last.get_index();
		_remove_from_cstring(posFrom, posTo - posFrom);

		return iterator(_data._First + posFrom, &_data);
	}
// end Erase

// Compare function overload
	constexpr int compare(const BasicString& string) const {
		return _compare_with_cstring(0, size(), string._data._First, 0, string.size());
	}

	constexpr int compare(size_t pos, size_t len, const BasicString& string, size_t subpos, size_t sublen) const {
		return _compare_with_cstring(pos, len, string._data._First, subpos, sublen);
	}

	constexpr int compare(const_pointer cstring) const {
		return _compare_with_cstring(0, size(), cstring, 0, TraitsType::length(cstring));
	}

	constexpr int compare(size_t pos, size_t len, const_pointer cstring, size_t subpos, size_t sublen) const {
		return _compare_with_cstring(pos, len, cstring, subpos, sublen);
	}
// end Compare

// Find function overload
	constexpr size_t find(const BasicString& string, size_t pos = 0) const {
		return _find_cstring(string._data._First, pos, string.size());
	}

	constexpr size_t find(const_pointer cstring, size_t pos = 0) const {
		return _find_cstring(cstring, pos, TraitsType::length(cstring));
	}

	constexpr size_t find(const_pointer cstring, size_t pos, size_t len) const {
		return _find_cstring(cstring, pos, len);
	}

	constexpr size_t find(ValueType chr, size_t pos = 0) const {
		return _find_cstring(&chr, pos, 1);
	}
// end Find

// Rfind function overload
	constexpr size_t rfind(const BasicString& string, size_t pos = npos) const {
		return _rfind_cstring(string._data._First, pos, string.size());
	}

	constexpr size_t rfind(const_pointer cstring, size_t pos = npos) const {
		return _rfind_cstring(cstring, pos, TraitsType::length(cstring));
	}

	constexpr size_t rfind(const_pointer cstring, size_t pos, size_t len) const {
		return _rfind_cstring(cstring, pos, len);
	}

	constexpr size_t rfind(ValueType chr, size_t pos = npos) const {
		return _rfind_cstring(&chr, pos, 1);
	}
// end Rfind

// Contains
    constexpr bool contains(const BasicStringView<ValueType, TraitsType>& sv) const noexcept {
        return find(sv.data()) != npos;
    }

    constexpr bool contains(const_pointer cstring) const noexcept {
        return find(cstring) != npos;
    }

    constexpr bool contains(const ValueType chr) const noexcept {
        return find(chr) != npos;
    }
// END Contains

// Starts With overload
	constexpr bool starts_with(const BasicStringView<ValueType, TraitsType>& sv) const noexcept {
        return BasicStringView<ValueType, TraitsType> {_data._First, size()}.starts_with(sv);
    }

    constexpr bool starts_with(const ValueType chr) const noexcept {
        return BasicStringView<ValueType, TraitsType> {_data._First, size()}.starts_with(chr);
    }

    constexpr bool starts_with(const_pointer cstring) const noexcept {
        return BasicStringView<ValueType, TraitsType> {_data._First, size()}.starts_with(cstring);
    }
// END Starts With

// Ends With overload
	constexpr bool ends_with(const BasicStringView<ValueType, TraitsType>& sv) const noexcept {
        return BasicStringView<ValueType, TraitsType> {_data._First, size()}.ends_with(sv);
    }

    constexpr bool ends_with(const ValueType chr) const noexcept {
        return BasicStringView<ValueType, TraitsType> {_data._First, size()}.ends_with(chr);
    }

    constexpr bool ends_with(const_pointer cstring) const noexcept {
        return BasicStringView<ValueType, TraitsType> {_data._First, size()}.ends_with(cstring);
    }
// END Ends With

	constexpr void print_details() const {									// For Debugging
		size_t currentSize 		= size();
		size_t currentCapacity 	= capacity();

		std::cout << currentSize << ' ' << currentCapacity << '\n';
		std::cout << _data._First << '\n';
	}

public:
	// iterator specific

	constexpr iterator begin() noexcept {
		return iterator(_data._First, &_data);
	}

	constexpr const_iterator begin() const noexcept {
		return const_iterator(_data._First, &_data);
	}

	constexpr reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}

	constexpr const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}

	constexpr iterator end() noexcept {
		return iterator(_data._Last, &_data);
	}

	constexpr const_iterator end() const noexcept {
		return const_iterator(_data._Last, &_data);
	}

	constexpr reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}

	constexpr const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}

private:
	// Helpers

	constexpr void _alloc_empty(const size_t capacity) {
		_data._First	= _alloc.allocate(capacity + 1);
		_data._Last		= _data._First;
		_data._Final	= _data._First + capacity;
		_data._Last[0]	= TraitsType::NULLCHR;
	}

	constexpr void _initialize_from_cstring(const_pointer cstring) {
		if (cstring == nullptr)
			_alloc_empty(_DEFAULT_CAPACITY);
		else
		{
			size_t len		= TraitsType::length(cstring);
			_data._First	= _alloc.allocate(len + 1);
			_data._Last		= _data._First + len;
			_data._Final	= _data._First + len;
			(void)TraitsType::copy(_data._First, cstring, len);
			_data._Last[0] = TraitsType::NULLCHR;
		}
	}

	constexpr void _clean_up_string() {
		if (_data._First != nullptr)
		{
			_alloc.deallocate(_data._First, capacity() + 1);
			_data._First	= nullptr;
			_data._Last		= nullptr;
			_data._Final	= nullptr;
		}
	}

	constexpr void _extend_if_full() {											// Reserve 50% more capacity when full
		if (_data._Last == _data._Final)
			reserve(capacity() + capacity() / 2 + 1);
	}

	constexpr void _copy(const BasicString& other) {								// Generic copy function for string
		_alloc_empty(other.capacity());
		(void)TraitsType::copy(_data._First, other._data._First, other.size());

		_data._Last		= _data._First + other.size();
		_data._Final	= _data._First + other.capacity();
		_data._Last[0]	= TraitsType::NULLCHR;
	}

	constexpr void _move(BasicString&& other) {										// Generic move function for string
		_data._First	= other._data._First;
		_data._Last		= other._data._Last;
		_data._Final	= other._data._Final;

		other._initialize_from_cstring(nullptr);
	}

	constexpr void _insert_from_cstring(size_t pos, const_pointer cstring, size_t subpos, size_t sublen) {
		if (pos > size() || subpos + sublen > TraitsType::length(cstring))
			throw std::out_of_range("Invalid length or starting position.");

		size_t newSize = size() + sublen;
		if (newSize > capacity())
			reserve(newSize);

		(void)TraitsType::move(_data._First + pos + sublen, _data._First + pos, size() - pos);	// copy last sublen positions to right
		(void)TraitsType::move(_data._First + pos, cstring + subpos, sublen);					// add substr from cstring between
		_data._Last 	= _data._First + newSize;
		_data._Last[0] 	= TraitsType::NULLCHR;
	}

	constexpr void _insert_char(size_t pos, size_t nchar, ValueType chr) {
		if (pos > size())
			throw std::out_of_range("Invalid starting position.");

		size_t newSize = size() + nchar;
		if (newSize > capacity())
			reserve(newSize);

		(void)TraitsType::move(_data._First + pos + nchar, _data._First + pos, size() - pos);	// copy last nchar positions to right
		(void)TraitsType::assign(_data._First + pos, nchar, chr);								// add nchar * chr in between
		_data._Last 	= _data._First + newSize;
		_data._Last[0] 	= TraitsType::NULLCHR;
	}

	constexpr void _remove_from_cstring(size_t pos, size_t len) {
		if (pos + len > size())
			throw std::out_of_range("Invalid length or starting position.");

		(void)TraitsType::move(_data._First + pos, _data._First + pos + len, size() - pos - len);
		_data._Last 	= _data._First + size() - len;
		_data._Last[0] 	= TraitsType::NULLCHR;
	}

	constexpr int _compare_with_cstring(size_t pos, size_t len, const_pointer cstring, size_t subpos, size_t sublen) const {
		if (pos + len > size() || subpos + sublen > Traits::length(cstring))
			throw std::out_of_range("Invalid length or starting position.");

		return detail::_traits_cstring_compare<TraitsType>(	_data._First, pos, len,
															cstring, subpos, sublen);
	}

	constexpr size_t _find_cstring(const_pointer cstring, size_t pos, size_t len) const {
		if (pos > size() || len > TraitsType::length(cstring))
			throw std::out_of_range("Invalid length or starting position.");

		return detail::_traits_cstring_find<TraitsType>(_data._First, cstring, pos, len);
	}

	constexpr size_t _rfind_cstring(const_pointer cstring, size_t pos, size_t len) const {
		if (len > TraitsType::length(cstring))
			throw std::out_of_range("Invalid length.");

		return detail::_traits_cstring_rfind<TraitsType>(_data._First, cstring, pos, len);
	}
};	// END BasicString


// BasicString binary operators
template<class Type, class Alloc, class Traits>
constexpr bool operator==(	const BasicString<Type, Alloc, Traits>& left,
							const BasicString<Type, Alloc, Traits>& right) {

	return left.compare(right) == 0;
}

template<class Type, class Alloc, class Traits>
constexpr bool operator!=(	const BasicString<Type, Alloc, Traits>& left,
							const BasicString<Type, Alloc, Traits>& right) {

	return !(left == right);
}

template<class Type, class Alloc, class Traits>
constexpr BasicString<Type, Alloc, Traits> operator+(	const BasicString<Type, Alloc, Traits>& left,
														const BasicString<Type, Alloc, Traits>& right) {

	// create empty string with capacity equal to sum of sizes and append left then right string
	return BasicString<Type, Alloc, Traits>(left.size() + right.size()).append(left).append(right);
}

template<class Type, class Alloc, class Traits>
constexpr BasicString<Type, Alloc, Traits> operator+(	const BasicString<Type, Alloc, Traits>& left,
														const Type* right) {

	return BasicString<Type, Alloc, Traits>(left.size() + Traits::length(right)).append(left).append(right);
}

template<class Type, class Alloc, class Traits>
constexpr BasicString<Type, Alloc, Traits> operator+(	const Type* left,
														const BasicString<Type, Alloc, Traits>& right) {

	return BasicString<Type, Alloc, Traits>(Traits::length(left) + right.size()).append(left).append(right);
}

template<class Type, class Alloc, class Traits>
constexpr BasicString<Type, Alloc, Traits> operator+(	const BasicString<Type, Alloc, Traits>& left,
														const Type right) {

	return BasicString<Type, Alloc, Traits>(left.size() + 1).append(left).append(1, right);
}

template<class Type, class Alloc, class Traits>
constexpr BasicString<Type, Alloc, Traits> operator+(	const Type left,
														const BasicString<Type, Alloc, Traits>& right) {

	return BasicString<Type, Alloc, Traits>(1 + right.size()).append(1, left).append(right);
}
#pragma endregion BasicString


CUSTOM_END