#pragma once
#include "xCharTraits.h"
#include "xMemory.h"
#include "cIterator.h"
#include "cAlgorithm.h"


CUSTOM_BEGIN

// TODO: implement all

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
class BasicStringViewConstIterator
{
private:
	using _Data				= BasicStrVData;

public:
    using IteratorCategory 	= RandomAccessIteratorTag;
	using ValueType 		= typename _Data::ValueType;
	using DifferenceType	= typename _Data::DifferenceType;
	using Reference 		= typename _Data::ConstReference;
	using Pointer			= typename _Data::ConstPointer;
};  // END BasicStringConstIterator


template<class BasicStrVData>
class BasicStringViewIterator : public BasicStringViewConstIterator<BasicStrVData>
{
private:
	using _Base				= BasicStringViewConstIterator<BasicStrVData>;
	using _Data				= BasicStrVData;

public:
    using IteratorCategory 	= RandomAccessIteratorTag;
	using ValueType 		= typename _Data::ValueType;
	using DifferenceType	= typename _Data::DifferenceType;
	using Reference 		= typename _Data::ConstReference;
	using Pointer			= typename _Data::ConstPointer;
};  // END BasicStringIterator


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
    
    using Iterator 				= BasicStringViewIterator<_Data>;
	using ConstIterator 		= BasicStringViewConstIterator<_Data>;
    using ReverseIterator 		= custom::ReverseIterator<Iterator>;
    using ConstReverseIterator 	= custom::ReverseIterator<ConstIterator>;

    static constexpr size_t npos = static_cast<size_t>(-1);

private:
    _Data _data;

public:
    // Constructors

    constexpr BasicStringView() noexcept                        = default;
    constexpr BasicStringView(const BasicStringView&) noexcept  = default;

    constexpr BasicStringView(ConstPointer cstring) noexcept {
        _data._First    = cstring;
        _data._Last     = cstring + TraitsType::length(cstring);
    }

    constexpr BasicStringView(ConstPointer cstring, const size_t count) noexcept {
        _data._First    = cstring;
        _data._Last     = cstring + count;
    }

public:
	// Operators

    constexpr BasicStringView& operator=(const BasicStringView&) noexcept = default;
	
    constexpr ConstReference operator[](const size_t index) const noexcept {
		CUSTOM_ASSERT(index < size(), "Index out of bounds...");
		return _data._First[index];
	}

public:
	// Main functions

	constexpr size_t size() const noexcept {
		return static_cast<size_t>(_data._Last - _data._First);
	}
    
    constexpr size_t max_size() const noexcept {
        // bound to PTRDIFF_MAX to make end() - begin() well defined (also makes room for npos)
        // bound to static_cast<size_t>(-1) / sizeof(ValueType) by address space limits
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
			throw std::out_of_range("Index out of bounds...");

		return _data._First[index];
	}

	constexpr ConstReference front() const {
		CUSTOM_ASSERT(!empty(), "Container is empty...");
		return _data._First[0];
	}

	constexpr ConstReference back() const {
		CUSTOM_ASSERT(!empty(), "Container is empty...");
		return _data._Last[-1];
	}

public:
	// Iterator specific

	// constexpr Iterator begin() noexcept {
	// 	return Iterator(_data._First, &_data);
	// }

	// constexpr ConstIterator begin() const noexcept {
	// 	return ConstIterator(_data._First, &_data);
	// }

	// constexpr ReverseIterator rbegin() noexcept {
	// 	return ReverseIterator(end());
	// }

	// constexpr ConstReverseIterator rbegin() const noexcept {
	// 	return ConstReverseIterator(end());
	// }

	// constexpr Iterator end() noexcept {
	// 	return Iterator(_data._Last, &_data);
	// }

	// constexpr ConstIterator end() const noexcept {
	// 	return ConstIterator(_data._Last, &_data);
	// }

	// constexpr ReverseIterator rend() noexcept {
	// 	return ReverseIterator(begin());
	// }

	// constexpr ConstReverseIterator rend() const noexcept {
	// 	return ConstReverseIterator(begin());
	// }
};  // END BasicStringView


CUSTOM_END