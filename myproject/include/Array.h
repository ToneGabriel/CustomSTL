#pragma once
#include "Utility.h"


CUSTOM_BEGIN

template<class Type, size_t Size>
class ArrayConstIterator
{
public:
	using ValueType = Type;
	using Reference	= const ValueType&;
	using Pointer	= const ValueType*;

	ValueType* _Ptr	= nullptr;
	size_t _Index	= 0;

public:

	constexpr ArrayConstIterator() noexcept = default;

	constexpr explicit ArrayConstIterator(ValueType* ptr, size_t index) noexcept
		:_Ptr(ptr), _Index(index) { /*Empty*/ }

	constexpr ArrayConstIterator& operator++() noexcept {
		CUSTOM_ASSERT(_Index < Size, "Cannot increment end iterator...");
		++_Index;
		return *this;
	}

	constexpr ArrayConstIterator operator++(int) noexcept {
		ArrayConstIterator temp = *this;
		++(*this);
		return temp;
	}

	constexpr ArrayConstIterator& operator+=(const size_t& diff) noexcept {
		CUSTOM_ASSERT(_Index + diff <= Size, "Cannot increment end iterator...");
		_Index += diff;
		return *this;
	}

	constexpr ArrayConstIterator operator+(const size_t& diff) const noexcept {
		ArrayConstIterator temp = *this;
		temp += diff;
		return temp;
	}

	constexpr ArrayConstIterator& operator--() noexcept {
		CUSTOM_ASSERT(_Index > 0, "Cannot decrement begin iterator...");
		--_Index;
		return *this;
	}

	constexpr ArrayConstIterator operator--(int) noexcept {
		ArrayConstIterator temp = *this;
		--(*this);
		return temp;
	}

	constexpr ArrayConstIterator& operator-=(const size_t& diff) noexcept {
		CUSTOM_ASSERT(_Index >= diff, "Cannot decrement begin iterator...");
		_Index -= diff;
		return *this;
	}

	constexpr ArrayConstIterator operator-(const size_t& diff) const noexcept {
		ArrayConstIterator temp = *this;
		temp -= diff;
		return temp;
	}

	constexpr Pointer operator->() const noexcept {
		CUSTOM_ASSERT(_Index < Size, "Cannot access end iterator...");
		return _Ptr + _Index;
	}

	constexpr Reference operator*() const noexcept {
		CUSTOM_ASSERT(_Index < Size, "Cannot dereference end iterator...");
		return *(_Ptr + _Index);
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
}; // END ArrayConstIterator

template<class Type, size_t Size>
class ArrayIterator : public ArrayConstIterator<Type, Size>
{
private:
	using Base		= ArrayConstIterator<Type, Size>;
	
public:
	using ValueType = Type;
	using Reference	= ValueType&;
	using Pointer	= ValueType*;

public:

	constexpr ArrayIterator() noexcept = default;

	constexpr explicit ArrayIterator(ValueType* ptr, size_t index) noexcept
		:Base(ptr, index) { /*Empty*/ }

	constexpr ArrayIterator& operator++() noexcept {
		Base::operator++();
		return *this;
	}

	constexpr ArrayIterator operator++(int) noexcept {
		ArrayIterator temp = *this;
		Base::operator++();
		return temp;
	}

	constexpr ArrayIterator& operator+=(const size_t& diff) noexcept {
		Base::operator+=(diff);
		return *this;
	}

	constexpr ArrayIterator operator+(const size_t& diff) const noexcept {
		ArrayIterator temp = *this;
		temp += diff;
		return temp;
	}

	constexpr ArrayIterator& operator--() noexcept {
		Base::operator--();
		return *this;
	}

	constexpr ArrayIterator operator--(int) noexcept {
		ArrayIterator temp = *this;
		Base::operator--();
		return temp;
	}

	constexpr ArrayIterator& operator-=(const size_t& diff) noexcept {
		Base::operator-=(diff);
		return *this;
	}

	constexpr ArrayIterator operator-(const size_t& diff) const noexcept {
		ArrayIterator temp = *this;
		temp -= diff;
		return temp;
	}

	constexpr Pointer operator->() const noexcept {
		return const_cast<Pointer>(Base::operator->());
	}

	constexpr Reference operator*() const noexcept {
		return const_cast<Reference>(Base::operator*());
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
	using Reference 			= ValueType&;
	using ConstReference 		= const Reference;
	using Pointer 				= ValueType*;
	using ConstPointer 			= const Pointer;

	using Iterator				= ArrayIterator<ValueType, Size>;			// Iterator type
	using ConstIterator			= ArrayConstIterator<ValueType, Size>;		// Const Iterator type
	using ReverseIterator 		= custom::ReverseIterator<Iterator>;		// ReverseIterator type
	using ConstReverseIterator	= custom::ReverseIterator<ConstIterator>;	// Const Reverse Iterator type

	ValueType _array[Size];													// Actual container array

public:
	// Operators

	constexpr const ValueType& operator[](const size_t& index) const noexcept {	// Acces object at index (read only)
		CUSTOM_ASSERT(index < size(), "Index out of bounds...");
		return _array[index];
	}

	constexpr ValueType& operator[](const size_t& index) noexcept {				// Acces object at index
		CUSTOM_ASSERT(index < size(), "Index out of bounds...");
		return _array[index];
	}
	
public:
	// Main functions

    constexpr void fill(const ValueType& copyValue) {						// Fill the container with values
		// TODO: use fill_n(...)
		for (size_t i = 0; i < Size; ++i)
			_array[i] = copyValue;
	}

	// TODO: implement
	// constexpr void swap(Array& _Other) noexcept(_Is_nothrow_swappable<ValueType>::value) {
    //     _Swap_ranges_unchecked(_Elems, _Elems + _Size, _Other._Elems);
    // }

    constexpr ValueType& front() noexcept {									// Get the value of the first component
		CUSTOM_ASSERT(Size > 0, "Container is empty...");
		return _array[0];
	}

	constexpr const ValueType& front() const noexcept {
		CUSTOM_ASSERT(Size > 0, "Container is empty...");
		return _array[0];
	}

	constexpr ValueType& back() noexcept {									// Get the value of the last component
		CUSTOM_ASSERT(Size > 0, "Container is empty...");
		return _array[Size - 1];
	}

	constexpr const ValueType& back() const noexcept {
		CUSTOM_ASSERT(Size > 0, "Container is empty...");
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

	constexpr const ValueType& at(const size_t& index) const {			// Acces object at index with check (read only)
		if (index >= Size)
			throw std::out_of_range("Index out of bounds...");

		return _array[index];
	}

	constexpr ValueType& at(const size_t& index) {						// Acces object at index with check
		if (index >= Size)
			throw std::out_of_range("Index out of bounds...");

		return _array[index];
	}

	constexpr ValueType* data() noexcept {
		return _array;
	}

	constexpr const ValueType* data() const noexcept {
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
	return false;
	// TODO: implement
    //return _STD equal(left.begin(), left.end(), right.begin());
}

template<class Ty, size_t Size>
constexpr bool operator!=(const Array<Ty, Size>& left, const Array<Ty, Size>& right) {
	return !(left == right);
}


template<class First, class... Rest>
struct _AssertSame													// Assert all types to be the same
{
	static_assert(Conjunction_v<IsSame<First, Rest>...>, "All types must be the same. Otherwise the program is ill-formed.");

	using Type = First;												// Get type
};

template<class First, class... Rest>
Array(First, Rest...) -> Array<typename _AssertSame<First, Rest...>::Type, 1 + sizeof...(Rest)>;


// to array
template<class Ty, size_t Size, size_t... Idx>
constexpr Array<RemoveCV_t<Ty>, Size> _to_array_copy_impl(Ty(&builtInArray)[Size], IndexSequence<Idx...>) {
	return { builtInArray[Idx]... };
}

template<class Ty, size_t Size, size_t... Idx>
constexpr Array<RemoveCV_t<Ty>, Size> _to_array_move_impl(Ty(&&builtInArray)[Size], IndexSequence<Idx...>) {
	return { custom::move(builtInArray[Idx])... };
}

template<class Ty, size_t Size>
constexpr Array<RemoveCV_t<Ty>, Size> to_array(Ty(&builtInArray)[Size]) {
	static_assert(!IsArray_v<Ty>, "to_array does not accept multidimensional arrays...");
	static_assert(IsCopyConstructible_v<Ty>, "to_array requires copy constructible elements...");

	return _to_array_copy_impl(builtInArray, MakeIndexSequence<Size>{});
}

template<class Ty, size_t Size>
constexpr Array<RemoveCV_t<Ty>, Size> to_array(Ty(&&builtInArray)[Size]) {
	static_assert(!IsArray_v<Ty>, "to_array does not accept multidimensional arrays...");
	static_assert(IsMoveConstructible_v<Ty>, "to_array requires move constructible elements...");

	return _to_array_move_impl(custom::move(builtInArray), MakeIndexSequence<Size>{});
}

CUSTOM_END