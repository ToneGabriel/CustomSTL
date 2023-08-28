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

	explicit ArrayConstIterator(ValueType* ptr, size_t index) noexcept
		:_Ptr(ptr), _Index(index) { /*Empty*/ }

	ArrayConstIterator& operator++() noexcept {
		CUSTOM_ASSERT(_Index < Size, "Cannot increment end iterator...");
		++_Index;
		return *this;
	}

	ArrayConstIterator operator++(int) noexcept {
		ArrayConstIterator temp = *this;
		++(*this);
		return temp;
	}

	ArrayConstIterator& operator+=(const size_t& diff) noexcept {
		CUSTOM_ASSERT(_Index + diff <= Size, "Cannot increment end iterator...");
		_Index += diff;
		return *this;
	}

	ArrayConstIterator operator+(const size_t& diff) const noexcept {
		ArrayConstIterator temp = *this;
		temp += diff;
		return temp;
	}

	ArrayConstIterator& operator--() noexcept {
		CUSTOM_ASSERT(_Index > 0, "Cannot decrement begin iterator...");
		--_Index;
		return *this;
	}

	ArrayConstIterator operator--(int) noexcept {
		ArrayConstIterator temp = *this;
		--(*this);
		return temp;
	}

	ArrayConstIterator& operator-=(const size_t& diff) noexcept {
		CUSTOM_ASSERT(_Index >= diff, "Cannot decrement begin iterator...");
		_Index -= diff;
		return *this;
	}

	ArrayConstIterator operator-(const size_t& diff) const noexcept {
		ArrayConstIterator temp = *this;
		temp -= diff;
		return temp;
	}

	Pointer operator->() const noexcept {
		CUSTOM_ASSERT(_Index < Size, "Cannot access end iterator...");
		return _Ptr + _Index;
	}

	Reference operator*() const noexcept {
		CUSTOM_ASSERT(_Index < Size, "Cannot dereference end iterator...");
		return *(_Ptr + _Index);
	}

	bool operator==(const ArrayConstIterator& other) const noexcept {
		return (_Ptr == other._Ptr && _Index == other._Index);
	}

	bool operator!=(const ArrayConstIterator& other) const noexcept {
		return !(*this == other);
	}

public:

	const size_t get_index() const noexcept {				// Get the position for the element in array from iterator
		return _Index;
	}

	const bool is_begin() const noexcept {
		return _Index == 0;
	}

	const bool is_end() const noexcept {
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

	explicit ArrayIterator(ValueType* ptr, size_t index) noexcept
		:Base(ptr, index) { /*Empty*/ }

	ArrayIterator& operator++() noexcept {
		Base::operator++();
		return *this;
	}

	ArrayIterator operator++(int) noexcept {
		ArrayIterator temp = *this;
		Base::operator++();
		return temp;
	}

	ArrayIterator& operator+=(const size_t& diff) noexcept {
		Base::operator+=(diff);
		return *this;
	}

	ArrayIterator operator+(const size_t& diff) const noexcept {
		ArrayIterator temp = *this;
		temp += diff;
		return temp;
	}

	ArrayIterator& operator--() noexcept {
		Base::operator--();
		return *this;
	}

	ArrayIterator operator--(int) noexcept {
		ArrayIterator temp = *this;
		Base::operator--();
		return temp;
	}

	ArrayIterator& operator-=(const size_t& diff) noexcept {
		Base::operator-=(diff);
		return *this;
	}

	ArrayIterator operator-(const size_t& diff) const noexcept {
		ArrayIterator temp = *this;
		temp -= diff;
		return temp;
	}

	Pointer operator->() const noexcept {
		return const_cast<Pointer>(Base::operator->());
	}

	Reference operator*() const noexcept {
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
	using Iterator				= ArrayIterator<ValueType, Size>;			// Iterator type
	using ConstIterator			= ArrayConstIterator<ValueType, Size>;		// Const Iterator type
	using ReverseIterator 		= custom::ReverseIterator<Iterator>;		// ReverseIterator type
	using ConstReverseIterator	= custom::ReverseIterator<ConstIterator>;	// Const Reverse Iterator type

	ValueType _array[Size];													// Actual container array

public:
	// Operators

	const ValueType& operator[](const size_t& index) const noexcept {// Acces object at index (read only)
		CUSTOM_ASSERT(index < size(), "Index out of bounds...");
		return _array[index];
	}

	ValueType& operator[](const size_t& index) noexcept {			// Acces object at index
		CUSTOM_ASSERT(index < size(), "Index out of bounds...");
		return _array[index];
	}

	bool operator==(const Array& other) const {
		for (size_t i = 0; i < Size; ++i)
			if (_array[i] != other._array[i])
				return false;

		return true;
	}

	bool operator!=(const Array& other) const {
		return !(*this == other);
	}
	
public:
	// Main functions

    void fill(const ValueType& copyValue) {							// Fill the container with values
		// TODO: use fill_n(...)
		for (size_t i = 0; i < Size; ++i)
			_array[i] = copyValue;
	}

    ValueType& front() noexcept {									// Get the value of the first component
		CUSTOM_ASSERT(Size > 0, "Container is empty...");
		return _array[0];
	}

	const ValueType& front() const noexcept {
		CUSTOM_ASSERT(Size > 0, "Container is empty...");
		return _array[0];
	}

	ValueType& back() noexcept {									// Get the value of the last component
		CUSTOM_ASSERT(Size > 0, "Container is empty...");
		return _array[Size - 1];
	}

	const ValueType& back() const noexcept {
		CUSTOM_ASSERT(Size > 0, "Container is empty...");
		return _array[Size - 1];
	}

	const size_t size() const noexcept {							// Get size
		return Size;
	}

	bool empty() const noexcept {									// Check if array is empty
		return Size == 0;
	}

	const ValueType& at(const size_t& index) const {				// Acces object at index with check (read only)
		if (index >= Size)
			throw std::out_of_range("Index out of bounds...");

		return _array[index];
	}

	ValueType& at(const size_t& index) {							// Acces object at index with check
		if (index >= Size)
			throw std::out_of_range("Index out of bounds...");

		return _array[index];
	}

	ValueType* data() noexcept {
		return _array;
	}

	const ValueType* data() const noexcept {
		return _array;
	}

public:
	// Iterator specific functions

	Iterator begin() noexcept {
		return Iterator(_array, 0);
	}

	ConstIterator begin() const noexcept {
		return ConstIterator(_array, 0);
	}

	ReverseIterator rbegin() noexcept {
		return ReverseIterator(end());
	}

	ConstReverseIterator rbegin() const noexcept {
		return ConstReverseIterator(end());
	}

	Iterator end() noexcept {
		return Iterator(_array, Size);
	}

	ConstIterator end() const noexcept {
		return ConstIterator(_array, Size);
	}

	ReverseIterator rend() noexcept {
		return ReverseIterator(begin());
	}

	ConstReverseIterator rend() const noexcept {
		return ConstReverseIterator(begin());
	}
}; // END Array Template


template<class First, class... Rest>
struct _AssertSame													// Assert all types to be the same
{
	static_assert(Conjunction_v<IsSame<First, Rest>...>,
					"All types must be the same. Otherwise the program is ill-formed.");

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