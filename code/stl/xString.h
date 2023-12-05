#pragma once
#include "xCharTraits.h"
#include "xMemory.h"
#include "cIterator.h"
#include "cAlgorithm.h"


CUSTOM_BEGIN


template<class Type, class Alloc>
struct _BasicStringData
{
	using _AllocTraits		= AllocatorTraits<Alloc>;

	using ValueType			= typename _AllocTraits::ValueType;
	using DifferenceType	= typename _AllocTraits::DifferenceType;
	using Reference			= typename _AllocTraits::Reference;
	using ConstReference	= typename _AllocTraits::ConstReference;
	using Pointer			= typename _AllocTraits::Pointer;
	using ConstPointer		= typename _AllocTraits::ConstPointer;


	Pointer _First			= nullptr;			// Actual container array
	Pointer _Last			= nullptr;			// Pointer to end
	Pointer _Final			= nullptr;			// Pointer to capacity end
};	// END _BasicStringData


template<class BasicStrData>
class BasicStringConstIterator
{
private:
	using _Data				= BasicStrData;

public:
    using IteratorCategory 	= RandomAccessIteratorTag;
	using ValueType 		= typename _Data::ValueType;
	using DifferenceType	= typename _Data::DifferenceType;
	using Reference 		= typename _Data::ConstReference;
	using Pointer			= typename _Data::ConstPointer;

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

	constexpr Pointer operator->() const noexcept {
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot access end iterator.");
		return PointerTraits<Pointer>::pointer_to(**this);
	}

	constexpr Reference operator*() const noexcept {
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot dereference end iterator.");
		return *_Ptr;
	}

	constexpr Reference operator[](const DifferenceType diff) const noexcept {
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
class BasicStringIterator : public BasicStringConstIterator<BasicStrData>		// BasicString Iterator
{
private:
	using _Base				= BasicStringConstIterator<BasicStrData>;
	using _Data				= BasicStrData;
	
public:
    using IteratorCategory 	= RandomAccessIteratorTag;
	using ValueType 		= typename _Data::ValueType;
	using DifferenceType	= typename _Data::DifferenceType;
	using Reference			= typename _Data::Reference;
	using Pointer			= typename _Data::Pointer;

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

	constexpr Pointer operator->() const noexcept {
		return const_cast<Pointer>(_Base::operator->());
	}

	constexpr Reference operator*() const noexcept {
		return const_cast<Reference>(_Base::operator*());
	}

	constexpr Reference operator[](const DifferenceType diff) const noexcept {
        return const_cast<Reference>(_Base::operator[](diff));
    }
}; // END BasicStringIterator


template<class Type, class Alloc = custom::Allocator<Type>, class Traits = custom::CharTraits<Type>>
class BasicString		// null-terminated array of elements
{
private:
	using _Data					= _BasicStringData<Type, Alloc>;
	using _AllocTraits			= typename _Data::_AllocTraits;

public:
	static_assert(IsSame_v<Type, typename Alloc::ValueType>, "Object type and Allocator type must be the same!");
	static_assert(!IsArray_v<Type> && IsTrivial_v<Type> && IsStandardLayout_v<Type>,
					"The character type of BasicString must be a non-array trivial standard-layout type.");

	using TraitsType			= Traits;
	using ValueType				= typename _Data::ValueType;
	using DifferenceType		= typename _Data::DifferenceType;
	using Reference				= typename _Data::Reference;
	using ConstReference		= typename _Data::ConstReference;
	using Pointer				= typename _Data::Pointer;
	using ConstPointer			= typename _Data::ConstPointer;
	using AllocatorType			= Alloc;

	using Iterator				= BasicStringIterator<_Data>;
	using ConstIterator			= BasicStringConstIterator<_Data>;
	using ReverseIterator		= custom::ReverseIterator<Iterator>;
	using ConstReverseIterator	= custom::ReverseIterator<ConstIterator>;

	static constexpr size_t npos = static_cast<size_t>(-1);

private:
	static constexpr size_t _DEFAULT_CAPACITY = 15;

	_Data _data;
	AllocatorType _alloc;

public:
	// Constructors

	constexpr BasicString() {
		_initialize_from_cstring(nullptr);
	}

	constexpr BasicString(ConstPointer cstring) {
		_initialize_from_cstring(cstring);
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

	constexpr ConstReference operator[](const size_t index) const noexcept{
		CUSTOM_ASSERT(index < size(), "Index out of bounds.");
		return _data._First[index];
	}

	constexpr Reference operator[](const size_t index) noexcept {
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

	constexpr BasicString& operator+=(ConstPointer cstring) {
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
		Pointer newString 	= _alloc.allocate(newCapacity + 1);
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
		return (custom::min)(	static_cast<size_t>((NumericLimits<DifferenceType>::max)()),
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

	constexpr ConstPointer data() const noexcept {
		return _data._First;
	}

	constexpr Pointer data() noexcept {
		return _data._First;
	}

	constexpr ConstPointer c_str() const noexcept {
		return _data._First;
	}

	constexpr ConstReference at(const size_t index) const {						// Acces char at index with check (read only)
		if (index >= size())
			throw std::out_of_range("Index out of bounds.");

		return _data._First[index];
	}

	constexpr Reference at(const size_t index) {									// Acces char at index with check
		if (index >= size())
			throw std::out_of_range("Index out of bounds.");

		return _data._First[index];
	}

	constexpr ConstReference front() const {
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._First[0];
	}

	constexpr Reference front() {													// Get the value of the first component
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._First[0];
	}

	constexpr ConstReference back() const {
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Last[-1];
	}

	constexpr Reference back() {													// Get the value of the last component
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Last[-1];
	}

	constexpr BasicString substr(size_t pos, size_t len) const {					// Create substring from current string
		if (pos > size())
			throw std::out_of_range("Invalid starting position.");
		if (len + pos > size())
			throw std::out_of_range("Invalid length.");

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

	constexpr BasicString& append(ConstPointer cstring) {									// Appends a copy of a c-string.
		_insert_from_cstring(size(), cstring, 0, TraitsType::length(cstring));
		return *this;
	}

	constexpr BasicString& append(ConstPointer cstring, size_t nchar) {						// Appends a copy of the first n characters in the c-string
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

	constexpr BasicString& insert(size_t pos, ConstPointer cstring) {
		_insert_from_cstring(pos, cstring, 0, TraitsType::length(cstring));
		return *this;
	}

	constexpr BasicString& insert(size_t pos, ConstPointer cstring, size_t len) {
		_insert_from_cstring(pos, cstring, 0, len);
		return *this;
	}

	constexpr BasicString& insert(size_t pos, size_t nchar, ValueType chr) {
		_insert_char(pos, nchar, chr);
		return *this;
	}

	constexpr Iterator insert(ConstIterator where, ValueType chr) {
		size_t pos = where.get_index();
		_insert_char(pos, 1, chr);

		return Iterator(_data._First + pos, &_data);
	}

	constexpr Iterator insert(ConstIterator where, size_t nchar, ValueType chr) {
		size_t pos = where.get_index();
		_insert_char(pos, nchar, chr);

		return Iterator(_data._First + pos, &_data);
	}

	constexpr Iterator insert(ConstIterator where, ConstIterator first, ConstIterator last) {
		if (where._RefData->_First == first._RefData->_First ||
			first._RefData->_First != last._RefData->_First)	// Check if pos string != first/last string
			throw std::domain_error("BasicString provided by first and last must be the same, but different from the one provided by where");

		size_t pos 				= where.get_index();
		size_t posFrom 			= first.get_index();
		size_t posTo 			= last.get_index();
		ConstPointer cstring 	= first._RefData->_First;
		_insert_from_cstring(pos, cstring, posFrom, posTo - posFrom);

		return Iterator(_data._First + pos, &_data);
	}
// end Insert

// Erase function overload
	constexpr BasicString& erase(size_t pos, size_t len) {
		_remove_from_cstring(pos, len);
		return *this;
	}

	constexpr Iterator erase(ConstIterator where) {
		if (where.is_end())
			throw std::out_of_range("BasicString erase iterator outside range.");

		size_t pos = where.get_index();
		_remove_from_cstring(pos, 1);

		return Iterator(_data._First + pos, &_data);
	}

	constexpr Iterator erase(ConstIterator first, ConstIterator last) {
		if (first.is_end())
			throw std::out_of_range("BasicString erase iterator outside range.");

		size_t posFrom	= first.get_index();
		size_t posTo	= last.get_index();
		_remove_from_cstring(posFrom, posTo - posFrom);

		return Iterator(_data._First + posFrom, &_data);
	}
// end Erase

// Compare function overload
	constexpr int compare(const BasicString& string) const {
		return _compare_with_cstring(0, size(), string._data._First, 0, string.size());
	}

	constexpr int compare(size_t pos, size_t len, const BasicString& string, size_t subpos, size_t sublen) const {
		return _compare_with_cstring(pos, len, string._data._First, subpos, sublen);
	}

	constexpr int compare(ConstPointer cstring) const {
		return _compare_with_cstring(0, size(), cstring, 0, TraitsType::length(cstring));
	}

	constexpr int compare(size_t pos, size_t len, ConstPointer cstring, size_t subpos, size_t sublen) const {
		return _compare_with_cstring(pos, len, cstring, subpos, sublen);
	}
// end Compare

// Find function overload
	constexpr size_t find(const BasicString& string, size_t pos = 0) const {
		return _search_for_cstring(string._data._First, pos, string.size());
	}

	constexpr size_t find(ConstPointer cstring, size_t pos = 0) const {
		return _search_for_cstring(cstring, pos, TraitsType::length(cstring));
	}

	constexpr size_t find(ConstPointer cstring, size_t pos, size_t len) const {
		return _search_for_cstring(cstring, pos, len);
	}

	constexpr size_t find(ValueType chr, size_t pos = 0) const {
		return _search_for_cstring(&chr, pos, 1);
	}
// end Find

	constexpr void print_details() const {									// For Debugging
		size_t currentSize 		= size();
		size_t currentCapacity 	= capacity();

		std::cout << currentSize << ' ' << currentCapacity << '\n';
		std::cout << _data._First << '\n';
	}

public:
	// Iterator specific

	constexpr Iterator begin() noexcept {
		return Iterator(_data._First, &_data);
	}

	constexpr ConstIterator begin() const noexcept {
		return ConstIterator(_data._First, &_data);
	}

	constexpr ReverseIterator rbegin() noexcept {
		return ReverseIterator(end());
	}

	constexpr ConstReverseIterator rbegin() const noexcept {
		return ConstReverseIterator(end());
	}

	constexpr Iterator end() noexcept {
		return Iterator(_data._Last, &_data);
	}

	constexpr ConstIterator end() const noexcept {
		return ConstIterator(_data._Last, &_data);
	}

	constexpr ReverseIterator rend() noexcept {
		return ReverseIterator(begin());
	}

	constexpr ConstReverseIterator rend() const noexcept {
		return ConstReverseIterator(begin());
	}

private:
	// Helpers

	constexpr void _alloc_empty(const size_t capacity) {
		_data._First	= _alloc.allocate(capacity + 1);
		_data._Last		= _data._First;
		_data._Final	= _data._First + capacity;
		_data._Last[0]	= TraitsType::NULLCHR;
	}

	constexpr void _initialize_from_cstring(ConstPointer cstring) {
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

	constexpr void _insert_from_cstring(size_t pos, ConstPointer cstring, size_t subpos, size_t sublen) {
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

	constexpr int _compare_with_cstring(size_t pos, size_t len, ConstPointer cstring, size_t subpos, size_t sublen) const {
		if (pos + len > size() || subpos + sublen > Traits::length(cstring))
			throw std::out_of_range("Invalid length or starting position.");

		return detail::_traits_cstring_compare<TraitsType>(	_data._First, pos, len,
															cstring, subpos, sublen);
	}

	constexpr size_t _search_for_cstring(ConstPointer cstring, size_t pos, size_t len) const {
		if (pos + len > size() || len > TraitsType::length(cstring))
			throw std::out_of_range("Invalid length or starting position.");

		size_t endLoop = size() - len;
		for (size_t i = pos; i <= endLoop; ++i)
			if (TraitsType::compare(_data._First + i, cstring, len) == 0)
				return i;

		return npos;
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


CUSTOM_END