#pragma once
#include "cUtility.h"
#include "cIterator.h"
#include "cAlgorithm.h"


CUSTOM_BEGIN

template<class Type, size_t Size>
class ArrayConstIterator
{
public:
    using IteratorCategory 	= RandomAccessIteratorTag;
	using ValueType 		= Type;
	using DifferenceType 	= ptrdiff_t;
	using Reference			= const ValueType&;
	using Pointer			= const ValueType*;

	ValueType* _Ptr			= nullptr;
	size_t _Index			= 0;

public:

	constexpr ArrayConstIterator() noexcept = default;

	constexpr explicit ArrayConstIterator(ValueType* ptr, size_t index) noexcept
		:_Ptr(ptr), _Index(index) { /*Empty*/ }

	constexpr ArrayConstIterator& operator++() noexcept {
		CUSTOM_ASSERT(_Index < Size, "Cannot increment end iterator.");
		++_Index;
		return *this;
	}

	constexpr ArrayConstIterator operator++(int) noexcept {
		ArrayConstIterator temp = *this;
		++(*this);
		return temp;
	}

	constexpr ArrayConstIterator& operator+=(const DifferenceType diff) noexcept {
		CUSTOM_ASSERT(_Index + static_cast<size_t>(diff) <= Size, "Cannot increment end iterator.");
		_Index += static_cast<size_t>(diff);
		return *this;
	}

	constexpr ArrayConstIterator operator+(const DifferenceType diff) const noexcept {
		ArrayConstIterator temp = *this;
		temp += diff;
		return temp;
	}

	constexpr ArrayConstIterator& operator--() noexcept {
		CUSTOM_ASSERT(_Index > 0, "Cannot decrement begin iterator.");
		--_Index;
		return *this;
	}

	constexpr ArrayConstIterator operator--(int) noexcept {
		ArrayConstIterator temp = *this;
		--(*this);
		return temp;
	}

	constexpr ArrayConstIterator& operator-=(const DifferenceType diff) noexcept {
		CUSTOM_ASSERT(_Index >= static_cast<size_t>(diff), "Cannot decrement begin iterator.");
		_Index -= static_cast<size_t>(diff);
		return *this;
	}

	constexpr ArrayConstIterator operator-(const DifferenceType diff) const noexcept {
		ArrayConstIterator temp = *this;
		temp -= diff;
		return temp;
	}

	constexpr Pointer operator->() const noexcept {
		CUSTOM_ASSERT(_Index < Size, "Cannot access end iterator.");
		return _Ptr + _Index;
	}

	constexpr Reference operator*() const noexcept {
		CUSTOM_ASSERT(_Index < Size, "Cannot dereference end iterator.");
		return *(_Ptr + _Index);
	}

    constexpr Reference operator[](const DifferenceType diff) const noexcept {
        return *(*this + diff);
    }

	constexpr bool operator==(const ArrayConstIterator& other) const noexcept {
		return (_Ptr == other._Ptr && _Index == other._Index);
	}

	constexpr bool operator!=(const ArrayConstIterator& other) const noexcept {
		return !(*this == other);
	}

public:

	constexpr size_t get_index() const noexcept {				// Get the position for the element in array from iterator
		return _Index;
	}

	constexpr bool is_begin() const noexcept {
		return _Index == 0;
	}

	constexpr bool is_end() const noexcept {
		return _Index == Size;
	}

	friend constexpr void _verify_range(const ArrayConstIterator& first, const ArrayConstIterator& last) noexcept {
		CUSTOM_ASSERT(first._Ptr == last._Ptr, "Array iterators in range are from different containers");
		CUSTOM_ASSERT(first._Index <= last._Index, "Array iterator range transposed");
	}
}; // END ArrayConstIterator

template<class Type, size_t Size>
class ArrayIterator : public ArrayConstIterator<Type, Size>
{
private:
	using _Base				= ArrayConstIterator<Type, Size>;
	
public:
    using IteratorCategory 	= RandomAccessIteratorTag;
	using ValueType 		= Type;
	using DifferenceType 	= ptrdiff_t;
	using Reference			= ValueType&;
	using Pointer			= ValueType*;

public:

	constexpr ArrayIterator() noexcept = default;

	constexpr explicit ArrayIterator(ValueType* ptr, size_t index) noexcept
		:_Base(ptr, index) { /*Empty*/ }

	constexpr ArrayIterator& operator++() noexcept {
		_Base::operator++();
		return *this;
	}

	constexpr ArrayIterator operator++(int) noexcept {
		ArrayIterator temp = *this;
		_Base::operator++();
		return temp;
	}

	constexpr ArrayIterator& operator+=(const DifferenceType diff) noexcept {
		_Base::operator+=(diff);
		return *this;
	}

	constexpr ArrayIterator operator+(const DifferenceType diff) const noexcept {
		ArrayIterator temp = *this;
		temp += diff;
		return temp;
	}

	constexpr ArrayIterator& operator--() noexcept {
		_Base::operator--();
		return *this;
	}

	constexpr ArrayIterator operator--(int) noexcept {
		ArrayIterator temp = *this;
		_Base::operator--();
		return temp;
	}

	constexpr ArrayIterator& operator-=(const DifferenceType diff) noexcept {
		_Base::operator-=(diff);
		return *this;
	}

	constexpr ArrayIterator operator-(const DifferenceType diff) const noexcept {
		ArrayIterator temp = *this;
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
}; // END ArrayIterator


template<class Type, size_t Size>
class Array							// Array Template (follows aggregate rules)
{
//Aggregate Rules:
//Is a class type(a struct, class, or union), or an array type;
//Has no private or protected non-static data members;
//Has no user-declared or inherited constructors;
//Has no base classes;
//Has no virtual member functions;

public:
	using ValueType 			= Type;										// Type for stored values
	using DifferenceType 		= ptrdiff_t;
	using Reference 			= ValueType&;
	using ConstReference 		= const ValueType&;
	using Pointer 				= ValueType*;
	using ConstPointer 			= const ValueType*;

	using Iterator				= ArrayIterator<ValueType, Size>;			// Iterator type
	using ConstIterator			= ArrayConstIterator<ValueType, Size>;		// Const Iterator type
	using ReverseIterator 		= custom::ReverseIterator<Iterator>;		// ReverseIterator type
	using ConstReverseIterator	= custom::ReverseIterator<ConstIterator>;	// Const Reverse Iterator type

	ValueType _array[Size];													// Actual container array

public:
	// Operators

	constexpr ConstReference operator[](const size_t index) const noexcept {	// Acces object at index (read only)
		CUSTOM_ASSERT(index < size(), "Index out of bounds.");
		return _array[index];
	}

	constexpr Reference operator[](const size_t index) noexcept {				// Acces object at index
		CUSTOM_ASSERT(index < size(), "Index out of bounds.");
		return _array[index];
	}
	
public:
	// Main functions

    constexpr void fill(const ValueType& copyValue) {						// Fill the container with values
		(void)custom::fill_n(begin(), Size, copyValue);
	}

	constexpr void swap(Array& other) {
		custom::swap(_array, other._array);
		//(void)custom::swap_ranges(_array, _array + Size, other._array);
    }

    constexpr Reference front() noexcept {									// Get the value of the first component
		CUSTOM_ASSERT(Size > 0, "Container is empty.");
		return _array[0];
	}

	constexpr ConstReference front() const noexcept {
		CUSTOM_ASSERT(Size > 0, "Container is empty.");
		return _array[0];
	}

	constexpr Reference back() noexcept {									// Get the value of the last component
		CUSTOM_ASSERT(Size > 0, "Container is empty.");
		return _array[Size - 1];
	}

	constexpr ConstReference back() const noexcept {
		CUSTOM_ASSERT(Size > 0, "Container is empty.");
		return _array[Size - 1];
	}

	constexpr size_t size() const noexcept {							// Get size
		return Size;
	}

    constexpr size_t max_size() const noexcept {
        return Size;
    }

	constexpr bool empty() const noexcept {								// Check if array is empty
		return Size == 0;
	}

	constexpr ConstReference at(const size_t index) const {			// Acces object at index with check (read only)
		if (index >= Size)
			throw std::out_of_range("Index out of bounds.");

		return _array[index];
	}

	constexpr Reference at(const size_t index) {						// Acces object at index with check
		if (index >= Size)
			throw std::out_of_range("Index out of bounds.");

		return _array[index];
	}

	constexpr Pointer data() noexcept {
		return _array;
	}

	constexpr ConstPointer data() const noexcept {
		return _array;
	}

public:
	// Iterator specific functions

	constexpr Iterator begin() noexcept {
		return Iterator(_array, 0);
	}

	constexpr ConstIterator begin() const noexcept {
		return ConstIterator(_array, 0);
	}

	constexpr ReverseIterator rbegin() noexcept {
		return ReverseIterator(end());
	}

	constexpr ConstReverseIterator rbegin() const noexcept {
		return ConstReverseIterator(end());
	}

	constexpr Iterator end() noexcept {
		return Iterator(_array, Size);
	}

	constexpr ConstIterator end() const noexcept {
		return ConstIterator(_array, Size);
	}

	constexpr ReverseIterator rend() noexcept {
		return ReverseIterator(begin());
	}

	constexpr ConstReverseIterator rend() const noexcept {
		return ConstReverseIterator(begin());
	}
}; // END Array Template

// Array binary operators
template<class Ty, size_t Size>
constexpr bool operator==(const Array<Ty, Size>& left, const Array<Ty, Size>& right) {
    return custom::equal(left.begin(), left.end(), right.begin());
}

template<class Ty, size_t Size>
constexpr bool operator!=(const Array<Ty, Size>& left, const Array<Ty, Size>& right) {
	return !(left == right);
}


CUSTOM_DETAIL_BEGIN

template<class Ty, size_t Size, size_t... Idx>
constexpr Array<RemoveCV_t<Ty>, Size> _to_array_copy_impl(Ty (&builtInArray)[Size], IndexSequence<Idx...>) {
	return { builtInArray[Idx]... };
}

template<class Ty, size_t Size, size_t... Idx>
constexpr Array<RemoveCV_t<Ty>, Size> _to_array_move_impl(Ty (&&builtInArray)[Size], IndexSequence<Idx...>) {
	return { custom::move(builtInArray[Idx])... };
}

CUSTOM_DETAIL_END


// Deduction guide (ex: custom::Array arr {1, 2, 3};)
// All types must be the same. Otherwise the program is ill-formed
template<class First, class... Rest>
Array(First, Rest...) -> Array<	EnableIf_t<(Conjunction_v<IsSame<First, Rest>...>), First>,
								1 + sizeof...(Rest)>;

// to array
template<class Ty, size_t Size>
constexpr Array<RemoveCV_t<Ty>, Size> to_array(Ty (&builtInArray)[Size]) {
	static_assert(!IsArray_v<Ty>, "to_array does not accept multidimensional arrays.");
	static_assert(IsCopyConstructible_v<Ty>, "to_array requires copy constructible elements.");

	return detail::_to_array_copy_impl(builtInArray, MakeIndexSequence<Size>{});
}

template<class Ty, size_t Size>
constexpr Array<RemoveCV_t<Ty>, Size> to_array(Ty (&&builtInArray)[Size]) {
	static_assert(!IsArray_v<Ty>, "to_array does not accept multidimensional arrays.");
	static_assert(IsMoveConstructible_v<Ty>, "to_array requires move constructible elements.");

	return detail::_to_array_move_impl(custom::move(builtInArray), MakeIndexSequence<Size>{});
}

CUSTOM_END