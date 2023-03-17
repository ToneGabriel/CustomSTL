#pragma once
#include "Allocator.h"
#include "Utility.h"


CUSTOM_BEGIN

template<class Type>
struct DequeIterationData {		// Data used for iterating Deque
	Type* _Begin		= nullptr;
	Type* _End			= nullptr;
	Type* _Base 		= nullptr;
	size_t _Size		= 0;
	size_t _Capacity 	= 0;

	DequeIterationData() = default;
	~DequeIterationData() {
		_Begin 	= nullptr;
		_End 	= nullptr;
		_Base	= nullptr;
	}
};


template<class Deque>
class DequeIterator			// Deque Iterator
{
public:
	using ValueType = typename Deque::ValueType;
	using IterType 	= typename Deque::IterType;
	using Data 		= DequeIterationData<IterType>;

	IterType* _Ptr	= nullptr;
	Data* _Data		= nullptr;

public:

	explicit DequeIterator(IterType* ptr, Data* data)
		:_Ptr(ptr), _Data(data) { /*Empty*/ }

	~DequeIterator() {
		_Ptr 	= nullptr;
		_Data 	= nullptr;
	}

	DequeIterator& operator++() {
		if (_Ptr == _Data->_End)
			throw std::out_of_range("Cannot increment end iterator...");

		_Ptr = _Data->_Base + Deque::circular_increment(get_index(), _Data->_Capacity);
		return *this;
	}

	DequeIterator operator++(int) {
		DequeIterator temp = *this;
		++(*this);
		return temp;
	}

	DequeIterator& operator+=(const size_t& diff) {
		if(get_pos() + diff > _Data->_Size)
			throw std::out_of_range("Cannot increment end iterator...");

		_Ptr = _Data->_Base + Deque::circular_increment(get_index(), _Data->_Capacity, diff);
		return *this;
	}

	DequeIterator operator+(const size_t& diff) const {
		DequeIterator temp = *this;
		temp += diff;
		return temp;
	}

	DequeIterator& operator--() {
		if (_Ptr == _Data->_Begin)
			throw std::out_of_range("Cannot decrement begin iterator...");

		_Ptr = _Data->_Base + Deque::circular_decrement(get_index(), _Data->_Capacity);
		return *this;
	}

	DequeIterator operator--(int) {
		DequeIterator temp = *this;
		--(*this);
		return temp;
	}

	DequeIterator& operator-=(const size_t& diff) {
		if(get_pos() < diff)
			throw std::out_of_range("Cannot decrement begin iterator...");

		_Ptr = _Data->_Base + Deque::circular_decrement(get_index(), _Data->_Capacity, diff);
		return *this;
	}

	DequeIterator operator-(const size_t& diff) const {
		DequeIterator temp = *this;
		temp -= diff;
		return temp;
	}

	IterType* operator->() {
		if (_Ptr == _Data->_End)
			throw std::out_of_range("Cannot access end iterator...");

		return _Ptr;
	}

	ValueType& operator*() {
		if (_Ptr == _Data->_End)
			throw std::out_of_range("Cannot dereference end iterator...");

		return *_Ptr;
	}

	bool operator==(const DequeIterator& other) const {
		return _Ptr == other._Ptr;
	}

	bool operator!=(const DequeIterator& other) const {
		return !(*this == other);
	}

public:

	const size_t get_index() const {						// Get the index for the element in array from iterator (relative to array address, NOT begin)
		return static_cast<size_t>(_Ptr - _Data->_Base);
	}

	const size_t get_pos() const {							// Get the position for the element in array from iterator (relative to begin)
		size_t frontIndex = static_cast<size_t>(_Data->_Begin - _Data->_Base);
		return Deque::circular_decrement(get_index(), _Data->_Capacity, frontIndex);	// position relative to front
	}

	const bool is_begin() const {
		return _Ptr == _Data->_Begin;
	}

	const bool is_end() const {
		return _Ptr == _Data->_End;
	}
}; // END Deque Iterator


template<class Type>
class Deque					// Deque Template implemented as circular array
{
public:
	using ValueType = Type;									// Type for stored values
	using IterType 	= ValueType;							// Type for iteration (same as value)
	using Alloc 	= Allocator<ValueType>;					// Allocator type
	using Iterator 	= DequeIterator<Deque<ValueType>>;		// Iterator type
	using Data		= typename Iterator::Data;				// Iteration data

private:
	Alloc _alloc;											// Allocator
	size_t _size 		= 0;								// Number of components held by this
	size_t _capacity 	= 0;								// Allocated momory of type ValueType
	size_t _front 		= 0;								// Indicator of first elem (begin())
	size_t _back 		= 0;								// Indicator of last elem (end() - 1)
	ValueType* _array 	= nullptr;							// Actual container array

	mutable Data _data;
	
	static constexpr size_t default_capacity = 8;

public:
	// Constructors

	Deque() {																	// Default Constructor
		reserve(default_capacity);
	}

	Deque(const size_t& newCapacity, const ValueType& copyValue) {				// Add multiple copies Constructor
		realloc(newCapacity, copyValue);
	}

	Deque(const Deque& other) {													// Copy Constructor
		_copy(other);
	}

	Deque(Deque&& other) noexcept {												// Move Constructor
		_move(custom::move(other));
	}

	~Deque() {
		_clean_up_array();
	}

public:
	// Main functions

	static size_t circular_increment(const size_t& index, const size_t& capacity, const size_t& count = 1) {	// increment and return index
		return (index + count) % (capacity + 1);
	}

	static size_t circular_decrement(const size_t& index, const size_t& capacity, const size_t& count = 1) {	// decrement and return index
		return ((capacity + 1) + index - count) % (capacity + 1);
	}

	void reserve(const size_t& newCapacity) {									// Allocate memory and move values if needed
		if (newCapacity < _size)
			_size = newCapacity;

		ValueType* newArray = _alloc_array(newCapacity);
		size_t aux = _front;
		for (size_t i = 0; i < _size; ++i)
		{
			_alloc.construct(&newArray[i], custom::move(_array[aux]));
			aux = circular_increment(aux, _capacity);
		}

		_clean_up_array();
		_array 		= newArray;
		_capacity 	= newCapacity;
		_front 		= 0;
		_back 		= (_size == 0) ? 0 : _size - 1;	// TODO: check modulo variant
	}

	void shrink_to_fit() {														// Allocate memory with capacity equal to size and move values there
		reserve(_size);
	}

	void realloc(const size_t& newCapacity) {									// Allocate memory and populate it with default values (delete old)
		_clean_up_array();

		_capacity 	= newCapacity;
		_size 		= _capacity;
		_front 		= 0;
		_back 		= (_size == 0) ? 0 : _size - 1;

		_array 		= _alloc_array(_capacity);
		_alloc.construct_range(_array, _capacity);
	}

	void realloc(const size_t& newCapacity, const ValueType& copyValue) {		// Allocate memory and populate it with given reference (delete old)
		_clean_up_array();

		_capacity 	= newCapacity;
		_size 		= _capacity;
		_front 		= 0;
		_back 		= (_size == 0) ? 0 : _size - 1;

		_array 		= _alloc.alloc(_capacity);
		_alloc.construct_range(_array, _capacity, copyValue);
	}

	void resize(const size_t& newSize) {										// Change size and Construct/Destruct objects with default value if needed
		// TODO: implement
	}

	void resize(const size_t& newSize, const ValueType& copyValue) {			// Change size and Construct/Destruct objects with given reference if needed
		// TODO: implement
	}

	template<class... Args>
	void emplace_back(Args&&... args) {											// Construct object using arguments (Args) and add it to the tail
		_extend_if_full();

		if(!empty())
			_back = circular_increment(_back, _capacity);

		_alloc.construct(&_array[_back], custom::forward<Args>(args)...);
		++_size;
	}

	void push_back(const ValueType& copyValue) {								// Construct object using reference and add it to the tail
		emplace_back(copyValue);
	}

	void push_back(ValueType&& moveValue) {										// Construct object using temporary and add it to the tail
		emplace_back(custom::move(moveValue));
	}

	void pop_back() {															// Remove last component
		if (_size > 0)
		{
			_alloc.destroy(&_array[_back]);
			_back = circular_decrement(_back, _capacity);
			--_size;
		}
	}

	template<class... Args>
	void emplace_front(Args&&... args) {										// Construct object using arguments (Args) and add it to the front
		_extend_if_full();

		if(!empty())
			_front = circular_decrement(_front, _capacity);

		_alloc.construct(&_array[_front], custom::forward<Args>(args)...);
		++_size;
	}

	void push_front(const ValueType& copyValue) {								// Construct object using reference and add it to the front
		emplace_front(copyValue);
	}

	void push_front(ValueType&& moveValue) {									// Construct object using temporary and add it to the front
		emplace_front(custom::move(moveValue));
	}

	void pop_front() {															// Remove first component
		if (_size > 0)
		{
			_alloc.destroy(&_array[_front]);
			_front = circular_increment(_front, _capacity);
			--_size;
		}
	}

	template<class... Args>
	Iterator emplace(const Iterator& iterator, Args&&... args) {				// Emplace object at iterator position with given arguments
		size_t pos = iterator.get_pos();							// position relative to front
		emplace_back();
		size_t index = circular_increment(_front, _capacity, pos);	// real index after (possible) realocation

		for(size_t i = _back; i != index; i = circular_decrement(i, _capacity))
			_array[i] = custom::move(_array[circular_decrement(i, _capacity)]);

		_alloc.destroy(&_array[index]);
		_alloc.construct(&_array[index], custom::forward<Args>(args)...);

		return Iterator(&_array[index], _update_iteration_data());
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
		for (size_t i = index; i != circular_increment(_back, _capacity) && _size > 0; i = circular_increment(i, _capacity))
			_array[i] = custom::move(_array[circular_increment(i, _capacity)]);
		pop_back();

		return Iterator(&_array[index], _update_iteration_data());
	}

	ValueType& front() {                                                     	// Get the value of the first component
		assert(_size > 0);
		return _array[_front];
	}

	const ValueType& front() const {
		assert(_size > 0);
		return _array[_front];
	}

	ValueType& back() {                                                      	// Get the value of the last component
		assert(_size > 0);
		return _array[_back];
	}

	const ValueType& back() const {
		assert(_size > 0);
		return _array[_back];
	}

	const size_t capacity() const {												// Get capacity
		return _capacity;
	}

	const size_t size() const {													// Get size
		return _size;
	}

	void clear() {																// Remove ALL components but keep memory
		_destroy_array();
		_size 	= 0;
		_front 	= 0;
		_back 	= 0;
	}

	bool empty() const {														// Check if array is empty
		return _size == 0;
	}

	const ValueType& at(const size_t& index) const {							// Acces object at index with check (read only)
		if (index >= _size)
			throw std::out_of_range("Invalid Index...");

		return _array[circular_increment(_front, _capacity, index)];
	}

	ValueType& at(const size_t& index) {										// Acces object at index with check
		if (index >= _size)
			throw std::out_of_range("Invalid Index...");

		return _array[circular_increment(_front, _capacity, index)];
	}

	void print_details() {
		std::cout << "Size= " << _size << '\n';
		std::cout << "Capacity= " << _capacity << '\n';
		std::cout << "Front= " << _front << '\n';
		std::cout << "Back= " << _back << '\n';

		for (size_t i = 0; i <= _capacity; ++i)
			std::cout << _array[i] << ' ';

		std::cout << "\n\n";
	}

public:
	// Operators

	const ValueType& operator[](const size_t& index) const {					// Acces object at index (read only)
		assert(!(index >= _size));
		return _array[circular_increment(_front, _capacity, index)];
	}

	ValueType& operator[](const size_t& index) {								// Acces object at index
		assert(!(index >= _size));
		return _array[circular_increment(_front, _capacity, index)];
	}

	Deque& operator=(const Deque& other) {										// Assign operator using reference
		if (_array != other._array)
		{
			_clean_up_array();
			_copy(other);
		}

		return *this;
	}

	Deque& operator=(Deque&& other) noexcept {									// Assign operator using temporary
		if (_array != other._array)
		{
			_clean_up_array();
			_move(custom::move(other));
		}

		return *this;
	}

	bool operator==(const Deque& other) const {
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

	bool operator!=(const Deque& other) const {
		return !(*this == other);
	}

public:
	// Iterator specific functions

	Iterator begin() {
	return Iterator(_array + _front, _update_iteration_data());
	}

	const Iterator begin() const {
	return Iterator(_array + _front, _update_iteration_data());
	}

	Iterator end() {
		return Iterator(_array + circular_increment(_front, _capacity, _size), _update_iteration_data());
	}

	const Iterator end() const {
		return Iterator(_array + circular_increment(_front, _capacity, _size), _update_iteration_data());
	}

private:
	// Helpers

	ValueType* _alloc_array(const size_t& capacity) {							// Allocate memory +1 null term
		return _alloc.alloc(capacity + 1);
	}

	void _dealloc_array() {														// Deallocate memory +1 null term
		_alloc.dealloc(_array, _capacity + 1);
	}

	void _destroy_array() {
		if (!empty())
		{
			size_t end = circular_increment(_back, _capacity);
			for (size_t i = _front; i != end; i = circular_increment(i, _capacity))
				_alloc.destroy(_array + i);
		}
	}

	void _copy(const Deque& other) {											// Generic copy function for deque
		_array = _alloc_array(other._capacity);

		if (!other.empty())
		{
			size_t otherEnd = circular_increment(other._back, other._capacity);
			for (size_t i = other._front; i != otherEnd; i = circular_increment(i, other._capacity))
				_alloc.construct(&_array[i], other._array[i]);
		}
		
		_size 		= other._size;
		_capacity 	= other._capacity;
		_front 		= other._front;
		_back 		= other._back;
	}

	void _move(Deque&& other) {													// Generic move function for deque
		_array 		= other._array;
		_size 		= other._size;
		_capacity 	= other._capacity;
		_front 		= other._front;
		_back 		= other._back;

		other._array 	= nullptr;
		other._size 	= 0;
		other._capacity = 0;
		other._front 	= 0;
		other._back 	= 0;
	}

	void _extend_if_full() {													// Reserve 50% more capacity when full
		if (_size >= _capacity)
			reserve(_capacity + _capacity / 2 + 1);
	}

	void _clean_up_array() {
		if (_array != nullptr)
		{
			_destroy_array();
			_dealloc_array();
			_array = nullptr;
		}
	}

	Data* _update_iteration_data() const {										// Update the data used in Iterator
		_data._Begin 	= _array + _front;
		_data._End 		= _array + circular_increment(_front, _capacity, _size);
		_data._Base 	= _array;
		_data._Size 	= _size;
		_data._Capacity = _capacity;

		return &_data;
	}
}; // END Deque Template

CUSTOM_END