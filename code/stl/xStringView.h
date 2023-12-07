#pragma once
#include "xCharTraits.h"
#include "xMemory.h"
#include "cIterator.h"
#include "cAlgorithm.h"


CUSTOM_BEGIN


template<class Type>
struct _BasicStringViewData
{
    using ValueType 			= Type;
    using DifferenceType 		= ptrdiff_t;
    using Reference 			= ValueType&;
    using ConstReference 		= const ValueType&;
    using Pointer 				= ValueType*;
    using ConstPointer 			= const ValueType*;


	ConstPointer _First			= nullptr;
	ConstPointer _Last			= nullptr;
};  // END _BasicStringViewData


template<class BasicStrVData>
class BasicStringViewIterator	// This is only const
{
private:
	using _Data				= BasicStrVData;

public:
    using IteratorCategory 	= RandomAccessIteratorTag;
	using ValueType 		= typename _Data::ValueType;
	using DifferenceType	= typename _Data::DifferenceType;
	using Reference 		= typename _Data::ConstReference;
	using Pointer			= typename _Data::ConstPointer;

	Pointer _Ptr 			= nullptr;
	const _Data* _RefData	= nullptr;

public:

	constexpr BasicStringViewIterator() noexcept = default;

	constexpr explicit BasicStringViewIterator(Pointer ptr, const _Data* data) noexcept
		: _Ptr(ptr), _RefData(data) { /*Empty*/ }

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

	constexpr Pointer operator->() const noexcept {
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot access end iterator.");
		return _Ptr;
	}

	constexpr Reference operator*() const noexcept {
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot access end iterator.");
		return *_Ptr;
	}

    constexpr Reference operator[](const DifferenceType diff) const noexcept {
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
    static_assert(IsSame_v<Type, typename Traits::CharType>,
        			"The program is ill-formed if Traits::CharType is not the same type as Type.");

    static_assert(!IsArray_v<Type> && IsTrivial_v<Type> && IsStandardLayout_v<Type>,
					"The character type of BasicStringView must be a non-array trivial standard-layout type.");

	using TraitsType 			= Traits;
	using ValueType				= typename _Data::ValueType;
	using DifferenceType		= typename _Data::DifferenceType;
	using Reference				= typename _Data::Reference;
	using ConstReference		= typename _Data::ConstReference;
	using Pointer				= typename _Data::Pointer;
	using ConstPointer			= typename _Data::ConstPointer;
    
	using ConstIterator 		= BasicStringViewIterator<_Data>;
    using Iterator 				= ConstIterator;		// only const
    using ConstReverseIterator 	= custom::ReverseIterator<ConstIterator>;
    using ReverseIterator 		= ConstReverseIterator;	// only const

    static constexpr size_t npos = static_cast<size_t>(-1);

private:
    _Data _data;

public:
    // Constructors

    constexpr BasicStringView() noexcept                        = default;
    constexpr BasicStringView(const BasicStringView&) noexcept  = default;
    constexpr BasicStringView(std::nullptr_t) 					= delete;

    constexpr BasicStringView(ConstPointer cstring) noexcept {
        _data._First    = cstring;
        _data._Last     = cstring + TraitsType::length(cstring);
    }

    constexpr BasicStringView(ConstPointer cstring, const size_t len) noexcept {
        _data._First    = cstring;
        _data._Last     = cstring + len;
    }

public:
	// Operators

    constexpr BasicStringView& operator=(const BasicStringView&) noexcept = default;
	
    constexpr ConstReference operator[](const size_t index) const noexcept {
		CUSTOM_ASSERT(index < size(), "Index out of bounds.");
		return _data._First[index];
	}

public:
	// Main functions

	constexpr size_t size() const noexcept {
		return static_cast<size_t>(_data._Last - _data._First);
	}
    
    constexpr size_t max_size() const noexcept {
        return (custom::min)(   static_cast<size_t>((NumericLimits<DifferenceType>::max)()),
                                static_cast<size_t>(-1) / sizeof(ValueType));
    }

	constexpr bool empty() const noexcept {
		return (_data._First == _data._Last);
	}

	constexpr ConstPointer data() const noexcept {
		return _data._First;
	}

	constexpr ConstReference at(const size_t index) const {						// Acces char at index with check (read only)
		if (index >= size())
			throw std::out_of_range("Index out of bounds.");

		return _data._First[index];
	}

	constexpr ConstReference front() const noexcept {
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._First[0];
	}

	constexpr ConstReference back() const noexcept {
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

	constexpr size_t copy(ValueType* dest, size_t len, size_t pos = 0 ) const {
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

    constexpr int compare(	const size_t pos, const size_t len,
							const BasicStringView& other,
							const size_t subpos, const size_t sublen) const {

		return _compare_with_cstring(pos, len, other._data._First, subpos, sublen);
    }

    constexpr int compare(ConstPointer cstring) const {
		return _compare_with_cstring(0, size(), cstring, 0, TraitsType::length(cstring));
    }

    constexpr int compare(const size_t pos, const size_t len, ConstPointer cstring) const {
		return _compare_with_cstring(pos, len, cstring, 0, TraitsType::length(cstring));
    }

    constexpr int compare(	const size_t pos, const size_t len,
        					ConstPointer cstring, const size_t cstringCount) const {

		return _compare_with_cstring(pos, len, cstring, 0, cstringCount);
    }
// END Compare

// Find
    constexpr size_t find(const BasicStringView& other, const size_t pos = 0) const noexcept {
		return _find_cstring(other._data._First, pos, other.size());
    }

    constexpr size_t find(ConstPointer cstring, const size_t pos = 0) const noexcept {
		return _find_cstring(cstring, pos, TraitsType::length(cstring));
    }

    constexpr size_t find(ConstPointer cstring, size_t pos, size_t len) const noexcept {
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

	constexpr size_t rfind(ConstPointer cstring, size_t pos = npos) const {
		return _rfind_cstring(cstring, pos, TraitsType::length(cstring));
	}

	constexpr size_t rfind(ConstPointer cstring, size_t pos, size_t len) const {
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

    constexpr bool contains(ConstPointer cstring) const noexcept {
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

    constexpr bool starts_with(ConstPointer cstring) const noexcept {
        return starts_with(BasicStringView(cstring));
    }
// END Starts With

// Ends With
	constexpr bool ends_with(const BasicStringView& other) const noexcept {
        const size_t otherSize 	= other.size();

        if (size() < otherSize)
            return false;

        return TraitsType::compare(_data._Last - otherSize, other._data._First, otherSize) == 0;
    }

    constexpr bool ends_with(const ValueType chr) const noexcept {
        return !empty() && TraitsType::eq(back(), chr);
    }

    constexpr bool ends_with(ConstPointer cstring) const noexcept {
        return ends_with(BasicStringView(cstring));
    }
// END Ends With

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

    constexpr size_t _clamp_suffix_size(const size_t pos, const size_t len) const noexcept {
        // trims len to the longest it can be assuming a string at/after pos
        return (custom::min)(len, size() - pos);
    }

	constexpr int _compare_with_cstring(size_t pos, size_t len, ConstPointer cstring, size_t subpos, size_t sublen) const {
		if (pos + len > size() || subpos + sublen > TraitsType::length(cstring))
			throw std::out_of_range("Invalid length or starting position.");

		return detail::_traits_cstring_compare<TraitsType>(	_data._First, pos, len,
															cstring, subpos, sublen);
	}

	constexpr size_t _find_cstring(ConstPointer cstring, size_t pos, size_t len) const {
		if (pos > size() || len > TraitsType::length(cstring))
			throw std::out_of_range("Invalid length or starting position.");

		return detail::_traits_cstring_find<TraitsType>(_data._First, cstring, pos, len);
	}

	constexpr size_t _rfind_cstring(ConstPointer cstring, size_t pos, size_t len) const {
		if (len > TraitsType::length(cstring))
			throw std::out_of_range("Invalid length.");

		return detail::_traits_cstring_rfind<TraitsType>(_data._First, cstring, pos, len);
	}
};  // END BasicStringView


// BasicStringView binary operators
template<class Type, class Traits>
constexpr bool operator==(	const BasicStringView<Type, Traits>& left,
							const BasicStringView<Type, Traits>& right) {

	return left.compare(right) == 0;
}

template<class Type, class Alloc, class Traits>
constexpr bool operator!=(	const BasicStringView<Type, Traits>& left,
							const BasicStringView<Type, Traits>& right) {

	return !(left == right);
}


CUSTOM_END