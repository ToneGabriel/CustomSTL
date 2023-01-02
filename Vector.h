#pragma once
#include "Allocator.h"
#include "BaseIterator.h"

namespace custom {

	// Vector Iterator ========================================================
	template<class Vector>
	class VectorIterator : public BaseIterator<Vector>
	{
	private:
		using Base = BaseIterator<Vector>;

	public:

		explicit VectorIterator(Base::IterType* ptr, Base::Data* data)
			:Base(ptr, data) { }

		VectorIterator& operator++() {
			if (this->_Ptr >= this->_IterationData->_IterEnd)
				throw std::out_of_range("Cannot increment end iterator...");

			this->_Ptr++;
			return *this;
		}

		VectorIterator& operator++(int) {
			VectorIterator temp = *this;
			++(*this);
			return temp;
		}

		VectorIterator& operator+=(const size_t& diff) {
			if (this->_Ptr + diff >= this->_IterationData->_IterEnd)
				throw std::out_of_range("Cannot increment end iterator...");

			this->_Ptr += diff;
			return *this;
		}

		VectorIterator operator+(const size_t& diff) const {
			VectorIterator temp = *this;
			temp += diff;
			return temp;
		}

		VectorIterator& operator--() {
			if (this->_Ptr <= this->_IterationData->_IterBegin)
				throw std::out_of_range("Cannot decrement begin iterator...");

			this->_Ptr--;
			return *this;
		}

		VectorIterator& operator--(int) {
			VectorIterator temp = *this;
			--(*this);
			return temp;
		}

		VectorIterator& operator-=(const size_t& diff) {
			if (this->_Ptr - diff <= this->_IterationData->_IterBegin)
				throw std::out_of_range("Cannot decrement begin iterator...");

			this->_Ptr -= diff;
			return *this;
		}

		VectorIterator operator-(const size_t& diff) const {
			VectorIterator temp = *this;
			temp -= diff;
			return temp;
		}

		Base::IterType* operator->() {
			if (this->_Ptr >= this->_IterationData->_IterEnd)
				throw std::out_of_range("Cannot access end iterator...");

			return this->_Ptr;
		}

		Base::ValueType& operator*() {
			if (this->_Ptr >= this->_IterationData->_IterEnd)
				throw std::out_of_range("Cannot dereference end iterator...");

			return *this->_Ptr;
		}

		bool operator==(const VectorIterator& other) const {
			return this->_Ptr == other._Ptr;
		}

		bool operator!=(const VectorIterator& other) const {
			return !(*this == other);
		}
	};
	// Vector Iterator ========================================================
	// END

	// Vector Template=======================================
	template<class Type>
	class Vector
	{
	public:
		using ValueType = Type;											// Type for stored values
		using IterType	= ValueType;
		using Alloc		= typename Allocator<ValueType>;				// Allocator type
		using Iterator	= typename VectorIterator<Vector<ValueType>>;	// Iterator type
		using Data		= typename Iterator::Data;						// Iteration data

	private:
		size_t _size		= 0;										// Number of components held by this
		size_t _capacity	= 0;										// Allocated momory of type ValueType
		ValueType* _array	= nullptr;									// Actual container array

		mutable Data _data;												// Stores the ends of the array
		mutable Alloc _alloc;											// Allocator

	public:
		// Constructors

		Vector() = default;												// Default Constructor

		Vector(const size_t& newCapacity, const ValueType& copyValue) {	// Reference type Constructor
			realloc(newCapacity, copyValue);
		}

		Vector(const Vector<ValueType>& other) {						// Copy Constructor
			copy(other);
		}

		Vector(Vector<ValueType>&& other) noexcept {					// Move Constructor
			move(std::move(other));
		}

		~Vector() {														// Destructor
			clean_up_array();
		}

	public:
		// Main functions

		void reserve(const size_t& newCapacity) {									// Allocate memory and move values if needed
			if (newCapacity < _size)
				_size = newCapacity;

			ValueType* newArray = _alloc.alloc(newCapacity);
			for (size_t i = 0; i < _size; i++)
				_alloc.construct(&newArray[i], std::move(_array[i]));

			clean_up_array();
			_array = newArray;
			_capacity = newCapacity;
		}

		void shrink_to_fit() {														// Allocate memory with capacity equal to size and move values there
			reserve(_size);
		}

		void realloc(const size_t& newCapacity) {									// Allocate memory and populate it with default values (delete old)
			clean_up_array();

			_capacity = newCapacity;
			_size = _capacity;

			_array = _alloc.alloc(_capacity);
			_alloc.construct_range(_array, _capacity);
		}

		void realloc(const size_t& newCapacity, const ValueType& copyValue) {		// Allocate memory and populate it with given reference (delete old)
			clean_up_array();

			_capacity = newCapacity;
			_size = _capacity;

			_array = _alloc.alloc(_capacity);
			_alloc.construct_range(_array, _capacity, copyValue);
		}

		void resize(const size_t& newSize) {										// Change size and Construct/Destruct objects with default value if needed
			if (newSize < _size)
				_alloc.destroy_range(&_array[newSize], _size - newSize);
			else {
				if (newSize > _capacity)
					reserve(newSize);

				_alloc.construct_range(&_array[_size], newSize - _size);
			}

			_size = newSize;
		}

		void resize(const size_t& newSize, const ValueType& copyValue) {			// Change size and Construct/Destruct objects with given reference if needed
			if (newSize < _size)
				_alloc.destroy_range(&_array[newSize], _size - newSize);
			else {
				if (newSize > _capacity)
					reserve(newSize);

				_alloc.construct_range(&_array[_size], newSize - _size, copyValue);
			}

			_size = newSize;
		}

		template<class... Args>
		void emplace_back(Args&&... args) {											// Construct object using arguments (Args) and add it to the tail
			extend_if_full();
			_alloc.construct(&_array[_size++], std::forward<Args>(args)...);
		}

		void push_back(const ValueType& copyValue) {								// Construct object using reference and add it to the tail
			emplace_back(copyValue);
		}

		void push_back(ValueType&& moveValue) {										// Construct object using temporary and add it to the tail
			emplace_back(std::move(moveValue));
		}

		void pop_back() {															// Remove last component
			if (_size > 0)
				_alloc.destroy(&_array[--_size]);
		}

		template<class... Args>
		Iterator emplace(const Iterator& iterator, Args&&... args) {				// Emplace object at iterator position with given arguments
			size_t index = get_index(iterator);										// Don't check end()
			emplace_back();
			for (size_t i = _size - 1; i > index; i--)
				_array[i] = std::move(_array[i - 1]);

			_alloc.destroy(&_array[index]);
			_alloc.construct(&_array[index], std::forward<Args>(args)...);

			return Iterator(&_array[index], update_iteration_data());
		}

		Iterator push(const Iterator& iterator, const ValueType& copyValue) {		// Push copy object at iterator position
			return emplace(iterator, copyValue);
		}

		Iterator push(const Iterator& iterator, ValueType&& moveValue) {			// Push temporary object at iterator position
			return emplace(iterator, std::move(moveValue));
		}

		Iterator pop(const Iterator& iterator) {									// Remove component at iterator position
			if (is_end(iterator))
				throw std::out_of_range("Array pop iterator outside range...");

			size_t index = get_index(iterator);
			for (size_t i = index; i < _size - 1; i++)
				_array[i] = std::move(_array[i + 1]);
			pop_back();

			return Iterator(&_array[index - 1], update_iteration_data());
		}

		const size_t capacity() const {											// Get capacity
			return _capacity;
		}

		const size_t size() const {												// Get size
			return _size;
		}

		void clear() {															// Remove ALL components but keep memory
			_alloc.destroy_range(_array, _size);
			_size = 0;
		}

		bool empty() const {													// Check if array is empty
			return _size == 0;
		}

		const ValueType& at(const size_t& index) const {						// Acces object at index with check (read only)
			if (index < 0 || index >= _size)
				throw std::out_of_range("Invalid Index...");

			return _array[index];
		}

		ValueType& at(const size_t& index) {									// Acces object at index with check
			if (index < 0 || index >= _size)
				throw std::out_of_range("Invalid Index...");

			return _array[index];
		}

	public:
		// Operators

		const ValueType& operator[](const size_t& index) const {				// Acces object at index (read only)
			return _array[index];
		}

		ValueType& operator[](const size_t& index) {							// Acces object at index
			return _array[index];
		}

		Vector<ValueType>& operator=(const Vector<ValueType>& other) {			// Assign operator using reference
			if (_array != other._array)
			{
				clean_up_array();
				copy(other);
			}

			return *this;
		}

		Vector<ValueType>& operator=(Vector<ValueType>&& other) noexcept {		// Assign operator using temporary
			if (_array != other._array)
			{
				clean_up_array();
				move(std::move(other));
			}

			return *this;
		}

		bool operator==(const Vector<ValueType>& other) {
			if (size() != other.size())
				return false;

			for (size_t i = 0; i < _size; i++)
				if (_array[i] != other._array[i])
					return false;

			return true;
		}

	public:
		// Iterator specific functions

		Iterator begin() {
			return Iterator(_array, update_iteration_data());
		}

		const Iterator begin() const {
			return Iterator(_array, update_iteration_data());
		}

		Iterator end() {
			return Iterator(_array + _size, update_iteration_data());
		}

		const Iterator end() const {
			return Iterator(_array + _size, update_iteration_data());
		}

	private:
		// Others

		void copy(const Vector<ValueType>& other) {								// Generic copy function for vector
			_array = _alloc.alloc(other._capacity);
			for (size_t i = 0; i < other._size; i++)
				_alloc.construct(&_array[i], other._array[i]);

			_size = other._size;
			_capacity = other._capacity;
			update_iteration_data();
		}

		void move(Vector<ValueType>&& other) {									// Generic move function for vector
			_array = other._array;
			_size = other._size;
			_capacity = other._capacity;
			update_iteration_data();

			other._size = 0;
			other._capacity = 0;
			other._array = nullptr;
			other.update_iteration_data();
		}

		Data* update_iteration_data() const {
			_data._IterBegin = _array;
			_data._IterEnd = _array + _size;

			return &_data;
		}

		void clean_up_array() {
			if (_array != nullptr)
			{
				_alloc.destroy_range(_array, _size);
				_alloc.dealloc(_array, _capacity);
				_array = nullptr;
			}
		}

		const size_t get_index(const Iterator& iterator) const {			// Get the position for the element in array from iterator
			return iterator._Ptr - _array;
		}

		const bool is_end(const Iterator& iterator) const {
			return iterator._Ptr == _array + _size;
		}

		void extend_if_full() {												// Reserve 50% more capacity when full
			if (_size >= _capacity)
				reserve(_capacity + _capacity / 2 + 1);
		}
	};
	// Vector Template=======================================
	// END

	// Vector Hash ========================================================
	struct VectorHash {
		template <class Type>
		size_t operator()(const Vector<Type>& vec) const {
			return 0;
		}
	};
	// Vector Hash ========================================================
	// END

} // END custom::