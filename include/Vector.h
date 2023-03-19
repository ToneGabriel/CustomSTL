#pragma once
#include "Allocator.h"
#include "Utility.h"


CUSTOM_BEGIN

template<class Type>
struct VectorData {
	using ValueType		= Type;									// Type for stored values
	using IterType		= ValueType;							// Type for iteration (same as value)
	using Alloc			= Allocator<ValueType>;					// Allocator type

	ValueType* _First	= nullptr;								// Actual container array
	ValueType* _Last	= nullptr;								// Pointer to end
	ValueType* _Final	= nullptr;								// Pointer to capacity end
};

template<class Vector>
class VectorIterator			// Vector Iterator
{
public:
	using ValueType = typename Vector::ValueType;
	using IterType	= typename Vector::IterType;
	using Data		= typename Vector::Data;

private:
	IterType* _Ptr	= nullptr;
	Data* _Data		= nullptr;

public:

	explicit VectorIterator(IterType* ptr, Data* data)
		:_Ptr(ptr), _Data(data) { /*Empty*/ }

	~VectorIterator() {
		_Ptr	= nullptr;
		_Data	= nullptr;
	}

public:

	VectorIterator& operator++() {
		if (_Ptr >= _Data->_Last)
			throw std::out_of_range("Cannot increment end iterator...");

		++_Ptr;
		return *this;
	}

	VectorIterator operator++(int) {
		VectorIterator temp = *this;
		++(*this);
		return temp;
	}

	VectorIterator& operator+=(const size_t& diff) {
		if (_Ptr + diff >= _Data->_Last)
			throw std::out_of_range("Cannot increment end iterator...");

		_Ptr += diff;
		return *this;
	}

	VectorIterator operator+(const size_t& diff) const {
		VectorIterator temp = *this;
		temp += diff;
		return temp;
	}

	VectorIterator& operator--() {
		if (_Ptr <= _Data->_First)
			throw std::out_of_range("Cannot decrement begin iterator...");

		--_Ptr;
		return *this;
	}

	VectorIterator operator--(int) {
		VectorIterator temp = *this;
		--(*this);
		return temp;
	}

	VectorIterator& operator-=(const size_t& diff) {
		if (_Ptr - diff <= _Data->_First)
			throw std::out_of_range("Cannot decrement begin iterator...");

		_Ptr -= diff;
		return *this;
	}

	VectorIterator operator-(const size_t& diff) const {
		VectorIterator temp = *this;
		temp -= diff;
		return temp;
	}

	IterType* operator->() {
		if (_Ptr >= _Data->_Last)
			throw std::out_of_range("Cannot access end iterator...");

		return _Ptr;
	}

	ValueType& operator*() {
		if (_Ptr >= _Data->_Last)
			throw std::out_of_range("Cannot dereference end iterator...");

		return *_Ptr;
	}

	bool operator==(const VectorIterator& other) const {
		return _Ptr == other._Ptr;
	}

	bool operator!=(const VectorIterator& other) const {
		return !(*this == other);
	}

public:

	const size_t get_index() const {						// Get the position for the element in array from iterator
		return static_cast<size_t>(_Ptr - _Data->_First);
	}

	const bool is_begin() const {
		return _Ptr == _Data->_First;
	}

	const bool is_end() const {
		return _Ptr == _Data->_Last;
	}
}; // END Vector Iterator


template<class Type>
class Vector			// Vector Template
{
public:
	using Data				= VectorData<Type>;
	using ValueType 		= typename Data::ValueType;				// Type for stored values
	using IterType			= typename Data::IterType;				// Type for iteration (same as value)
	using Alloc				= typename Data::Alloc;					// Allocator type
	using Iterator			= VectorIterator<Vector<ValueType>>;	// Iterator type
	using ReverseIterator 	= ReverseIterator<Iterator>;			// ReverseIterator type

private:
	Alloc _alloc;													// Allocator
	Data _data;														// Actual container data
	
	static constexpr size_t default_capacity = 8;

public:
	// Constructors

	Vector() {														// Default Constructor
		reserve(default_capacity);
	}

	Vector(const size_t& newCapacity, const ValueType& copyValue) {	// Add multiple copies Constructor
		realloc(newCapacity, copyValue);
	}

	Vector(const Vector& other) {									// Copy Constructor
		_copy(other);
	}

	Vector(Vector&& other) noexcept {								// Move Constructor
		_move(custom::move(other));
	}

	~Vector() {														// Destructor
		_clean_up_array();
	}

public:
	// Main functions

	void reserve(const size_t& newCapacity) {									// Allocate memory and move values if needed
		if (newCapacity < size())
			_data._Last = _data._First + newCapacity;

		ValueType* newArray = _alloc.alloc(newCapacity);
		size_t newSize		= size();

		for (size_t i = 0; i < newSize; ++i)
			_alloc.construct(&newArray[i], custom::move(_data._First[i]));

		_clean_up_array();
		_data._First	= newArray;
		_data._Last		= _data._First + newSize;
		_data._Final	= _data._First + newCapacity;
	}

	void shrink_to_fit() {														// Allocate memory with capacity equal to size and move values there
		reserve(size());
	}

	void realloc(const size_t& newCapacity) {									// Allocate memory and populate it with default values (delete old)
		_clean_up_array();

		_data._First	= _alloc.alloc(newCapacity);
		_data._Last		= _data._First + newCapacity;
		_data._Final	= _data._First + newCapacity;
		_alloc.construct_range(_data._First, newCapacity);
	}

	void realloc(const size_t& newCapacity, const ValueType& copyValue) {		// Allocate memory and populate it with given reference (delete old)
		_clean_up_array();

		_data._First	= _alloc.alloc(newCapacity);
		_data._Last		= _data._First + newCapacity;
		_data._Final	= _data._First + newCapacity;
		_alloc.construct_range(_data._First, newCapacity, copyValue);
	}
	
	void resize(const size_t& newSize) {										// Change size and Construct/Destruct objects with default value if needed
		if (newSize < size())
			_alloc.destroy_range(_data._First + newSize, size() - newSize);
		else {
			if (newSize > capacity())
				reserve(newSize);

			_alloc.construct_range(_data._Last, newSize - size());
		}

		_data._Last = _data._First + newSize;
	}

	void resize(const size_t& newSize, const ValueType& copyValue) {			// Change size and Construct/Destruct objects with given reference if needed
		if (newSize < size())
			_alloc.destroy_range(_data._First + newSize, size() - newSize);
		else {
			if (newSize > capacity())
				reserve(newSize);

			_alloc.construct_range(_data._Last, newSize - size(), copyValue);
		}

		_data._Last = _data._First + newSize;
	}

	template<class... Args>
	void emplace_back(Args&&... args) {											// Construct object using arguments (Args) and add it to the tail
		_extend_if_full();
		_alloc.construct(_data._Last++, custom::forward<Args>(args)...);
	}

	void push_back(const ValueType& copyValue) {								// Construct object using reference and add it to the tail
		emplace_back(copyValue);
	}

	void push_back(ValueType&& moveValue) {										// Construct object using temporary and add it to the tail
		emplace_back(custom::move(moveValue));
	}

	void pop_back() {															// Remove last component
		if (!empty())
			_alloc.destroy(--_data._Last);
	}

	template<class... Args>
	Iterator emplace(const Iterator& iterator, Args&&... args) {				// Emplace object at iterator position with given arguments
		size_t index = iterator.get_index();				// Don't check end()
		emplace_back();

		for (size_t i = size() - 1; i > index; --i)
			_data._First[i] = custom::move(_data._First[i - 1]);

		_alloc.destroy(_data._First + index);
		_alloc.construct(_data._First + index, custom::forward<Args>(args)...);

		return Iterator(_data._First + index, &_data);
	}

	Iterator push(const Iterator& iterator, const ValueType& copyValue) {		// Push copy object at iterator position
		return emplace(iterator, copyValue);
	}

	Iterator push(const Iterator& iterator, ValueType&& moveValue) {			// Push temporary object at iterator position
		return emplace(iterator, custom::move(moveValue));
	}

	Iterator pop(const Iterator& iterator) {									// Remove component at iterator position
		if (iterator.is_end())
			throw std::out_of_range("Array pop iterator outside range...");

		size_t index = iterator.get_index();
		size_t beforeLast = size() - 1;

		for (size_t i = index; i < beforeLast; ++i)
			_data._First[i] = custom::move(_data._First[i + 1]);
		pop_back();

		return Iterator(_data._First + index, &_data);
	}

	const size_t capacity() const {												// Get capacity
		return static_cast<size_t>(_data._Final - _data._First);
	}

	const size_t size() const {													// Get size
		return static_cast<size_t>(_data._Last - _data._First);
	}

	bool empty() const {														// Check if array is empty
		return (_data._First == _data._Last);
	}
	
	void clear() {																// Remove ALL components but keep memory
		_alloc.destroy_range(_data._First, size());
		_data._Last = _data._First;
	}

	const ValueType& at(const size_t& index) const {							// Acces object at index with check (read only)
		if (index >= size())
			throw std::out_of_range("Invalid Index...");

		return _data._First[index];
	}

	ValueType& at(const size_t& index) {										// Acces object at index with check
		if (index >= size())
			throw std::out_of_range("Invalid Index...");

		return _data._First[index];
	}

	const ValueType& front() const {
		assert(!empty());
		return _data._First[0];
	}

	ValueType& front() {                                                     	// Get the value of the first component
		assert(!empty());
		return _data._First[0];
	}

	const ValueType& back() const {
		assert(!empty());
		return _data._Last[-1];
	}

	ValueType& back() {                                                      	// Get the value of the last component
		assert(!empty());
		return _data._Last[-1];
	}

public:
	// Operators

	const ValueType& operator[](const size_t& index) const {					// Acces object at index (read only)
		assert(!(index >= size()));
		return _data._First[index];
	}

	ValueType& operator[](const size_t& index) {								// Acces object at index
		assert(!(index >= size()));
		return _data._First[index];
	}

	Vector& operator=(const Vector& other) {									// Assign operator using reference
		if (_data._First != other._data._First)
		{
			_clean_up_array();
			_copy(other);
		}

		return *this;
	}

	Vector& operator=(Vector&& other) noexcept {								// Assign operator using temporary
		if (_data._First != other._data._First)
		{
			_clean_up_array();
			_move(custom::move(other));
		}

		return *this;
	}

	bool operator==(const Vector& other) const {
		if (size() != other.size())
			return false;

		Iterator it1 = begin();
		Iterator it2 = other.begin();
		while (it1 != end())
		{
			if (*it1 != *it2)
				return false;

			++it1;
			++it2;
		}

		return true;
	}

	bool operator!=(const Vector& other) const {
		return !(*this == other);
	}

public:
	// Iterator specific functions

	Iterator begin() {
		return Iterator(_data._First, &_data);
	}

	const Iterator begin() const {
		return Iterator(_data._First, &_data);
	}

	ReverseIterator rbegin() {
		return ReverseIterator(end());
	}

	const ReverseIterator rbegin() const {
		return ReverseIterator(end());
	}

	Iterator end() {
		return Iterator(_data._Last, &_data);
	}

	const Iterator end() const {
		return Iterator(_data._Last, &_data);
	}

	ReverseIterator rend() {
		return ReverseIterator(begin());
	}

	const ReverseIterator rend() const {
		return ReverseIterator(begin());
	}

private:
	// Helpers

	void _copy(const Vector& other) {											// Generic copy function for vector
		_data._First	= _alloc.alloc(other.capacity());
		size_t newSize	= other.size();

		for (size_t i = 0; i < newSize; ++i)
			_alloc.construct(&_data._First[i], other._data._First[i]));

		_data._Last		= _data._First + other.size();
		_data._Final	= _data._First + other.capacity();
	}

	void _move(Vector&& other) {												// Generic move function for vector
		_data._First	= other._data._First;
		_data._Last		= other._data._Last;
		_data._Final	= other._data._Final;

		other._data._First	= nullptr;
		other._data._Last	= nullptr;
		other._data._Final	= nullptr;
	}

	void _extend_if_full() {													// Reserve 50% more capacity when full
		if (_data._Last == _data._Final)
			reserve(capacity() + capacity() / 2 + 1);
	}

	void _clean_up_array() {													// Clear and Deallocate array
		if (_data._First != nullptr)
		{
			_alloc.destroy_range(_data._First, size());
			_alloc.dealloc(_data._First, capacity());
			_data._First	= nullptr;
			_data._Last		= nullptr;
			_data._Final	= nullptr;
		}
	}
}; // END Vector Template

CUSTOM_END