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

	ArrayConstIterator& operator++() {
		if (_Index >= Size)
			throw std::out_of_range("Cannot increment end iterator...");

		++_Index;
		return *this;
	}

	ArrayConstIterator operator++(int) {
		ArrayConstIterator temp = *this;
		++(*this);
		return temp;
	}

	ArrayConstIterator& operator+=(const size_t& diff) {
		if (_Index + diff > Size)
			throw std::out_of_range("Cannot increment end iterator...");

		_Index += diff;
		return *this;
	}

	ArrayConstIterator operator+(const size_t& diff) const {
		ArrayConstIterator temp = *this;
		temp += diff;
		return temp;
	}

	ArrayConstIterator& operator--() {
		if (_Index == 0)
			throw std::out_of_range("Cannot decrement begin iterator...");

		--_Index;
		return *this;
	}

	ArrayConstIterator operator--(int) {
		ArrayConstIterator temp = *this;
		--(*this);
		return temp;
	}

	ArrayConstIterator& operator-=(const size_t& diff) {
		if (_Index < diff)
			throw std::out_of_range("Cannot decrement begin iterator...");

		_Index -= diff;
		return *this;
	}

	ArrayConstIterator operator-(const size_t& diff) const {
		ArrayConstIterator temp = *this;
		temp -= diff;
		return temp;
	}

	Pointer operator->() const {
		if (_Index >= Size)
			throw std::out_of_range("Cannot access end iterator...");

		return _Ptr + _Index;
	}

	Reference operator*() const {
		if (_Index >= Size)
			throw std::out_of_range("Cannot dereference end iterator...");

		return *(_Ptr + _Index);
	}

	bool operator==(const ArrayConstIterator& other) const {
		return (_Ptr == other._Ptr && _Index == other._Index);
	}

	bool operator!=(const ArrayConstIterator& other) const {
		return !(*this == other);
	}

public:

	const size_t get_index() const {						// Get the position for the element in array from iterator
		return _Index;
	}

	const bool is_begin() const {
		return _Index == 0;
	}

	const bool is_end() const {
		return _Index == Size;
	}
};

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

	ArrayIterator& operator++() {
		Base::operator++();
		return *this;
	}

	ArrayIterator operator++(int) {
		ArrayIterator temp = *this;
		Base::operator++();
		return temp;
	}

	ArrayIterator& operator+=(const size_t& diff) {
		Base::operator+=(diff);
		return *this;
	}

	ArrayIterator operator+(const size_t& diff) const {
		ArrayIterator temp = *this;
		temp += diff;
		return temp;
	}

	ArrayIterator& operator--() {
		Base::operator--();
		return *this;
	}

	ArrayIterator operator--(int) {
		ArrayIterator temp = *this;
		Base::operator--();
		return temp;
	}

	ArrayIterator& operator-=(const size_t& diff) {
		Base::operator-=(diff);
		return *this;
	}

	ArrayIterator operator-(const size_t& diff) const {
		ArrayIterator temp = *this;
		temp -= diff;
		return temp;
	}

	Pointer operator->() const {
		return const_cast<Pointer>(Base::operator->());
	}

	Reference operator*() const {
		return const_cast<Reference>(Base::operator*());
	}
}; // END Array Iterator


template<class Type, size_t Size>
class Array							// Array Template
{
public:
	using ValueType 			= Type;										// Type for stored values
	
	using Iterator				= ArrayIterator<ValueType, Size>;			// Iterator type
	using ConstIterator			= ArrayConstIterator<ValueType, Size>;		// Const Iterator type
	using ReverseIterator 		= custom::ReverseIterator<Iterator>;		// ReverseIterator type
	using ConstReverseIterator	= custom::ReverseIterator<ConstIterator>;	// Const Reverse Iterator type

private:
	ValueType _array[Size];											// Actual container array

public:
	// Operators

	const ValueType& operator[](const size_t& index) const {		// Acces object at index (read only)
		assert((void("Index out of bounds..."), !(index >= Size)));
		return _array[index];
	}

	ValueType& operator[](const size_t& index) {					// Acces object at index
		assert((void("Index out of bounds..."), !(index >= Size)));
		return _array[index];
	}
	
	Array& operator=(const Array& other) {							// Assign operator using reference
		if (_array != other._array)
			_copy(other);

		return *this;
	}

	Array& operator=(Array&& other) noexcept {						// Assign operator using temporary
		if (_array != other._array)
			_move(custom::move(other));

		return *this;
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

    void fill(const ValueType& copyValue) {
		for (size_t i = 0; i < Size; ++i)
			_array[i] = copyValue;
	}

    ValueType& front() {											// Get the value of the first component
		assert((void("Container is empy..."), Size > 0));
		return _array[0];
	}

	const ValueType& front() const {
		assert((void("Container is empy..."), Size > 0));
		return _array[0];
	}

	ValueType& back() {												// Get the value of the last component
		assert((void("Container is empy..."), Size > 0));
		return _array[Size - 1];
	}

	const ValueType& back() const {
		assert((void("Container is empy..."), Size > 0));
		return _array[Size - 1];
	}

	const size_t size() const {										// Get size
		return Size;
	}

	bool empty() const {											// Check if array is empty
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

public:
	// Iterator specific functions

	Iterator begin() {
		return Iterator(_array, 0);
	}

	ConstIterator begin() const {
		return ConstIterator(_array, 0);
	}

	ReverseIterator rbegin() {
		return ReverseIterator(end());
	}

	ConstReverseIterator rbegin() const {
		return ConstReverseIterator(end());
	}

	Iterator end() {
		return Iterator(_array, Size);
	}

	ConstIterator end() const {
		return ConstIterator(_array, Size);
	}

	ReverseIterator rend() {
		return ReverseIterator(begin());
	}

	ConstReverseIterator rend() const {
		return ConstReverseIterator(begin());
	}

private:
	// Others

	void _copy(const Array& other) {								// Generic copy function for vector
		for(size_t i = 0; i < Size; ++i)
			_array[i] = other._array[i];
	}

	void _move(Array&& other) {										// Generic move function for vector
		for(size_t i = 0; i < Size; ++i)
			_array[i] = custom::move(other._array[i]);
	}
}; // END Array Template


// template<class First, class... Rest>
// Array(First, Rest...) -> Array<EnableIf_t<(IsSame_v<First, Rest> && ...), First>, 1 + sizeof...(Rest)>;

CUSTOM_END