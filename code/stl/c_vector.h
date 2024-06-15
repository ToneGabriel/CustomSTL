#pragma once
#include "x_memory.h"
#include "c_utility.h"
#include "c_algorithm.h"
#include "c_iterator.h"


CUSTOM_BEGIN

template<class Type, class Alloc>
struct _Vector_Data
{
	using _AllocTraits		= allocator_traits<Alloc>;

	using value_type		= typename _AllocTraits::value_type;
	using difference_type	= typename _AllocTraits::difference_type;
	using reference			= typename _AllocTraits::reference;
	using const_reference	= typename _AllocTraits::const_reference;
	using pointer			= typename _AllocTraits::pointer;
	using const_pointer		= typename _AllocTraits::const_pointer;

	pointer _First			= nullptr;			// Actual container array
	pointer _Last			= nullptr;			// pointer to end
	pointer _Final			= nullptr;			// pointer to capacity end
};

template<class VecData>
class _Vector_Const_Iterator
{
private:
	using _Data				= VecData;

public:
    using iterator_category	= random_access_iterator_tag;
	using value_type		= typename _Data::value_type;
	using difference_type 	= typename _Data::difference_type;
	using reference 		= typename _Data::const_reference;
	using pointer			= typename _Data::const_pointer;

	value_type* _Ptr		= nullptr;
	const _Data* _RefData	= nullptr;

public:

	constexpr _Vector_Const_Iterator() noexcept : _Ptr() { /*Empty*/ }

	constexpr explicit _Vector_Const_Iterator(value_type* ptr, const _Data* data) noexcept
		:_Ptr(ptr), _RefData(data) { /*Empty*/ }

	constexpr _Vector_Const_Iterator& operator++() noexcept {
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot increment end iterator.");
		++_Ptr;
		return *this;
	}

	constexpr _Vector_Const_Iterator operator++(int) noexcept {
		_Vector_Const_Iterator temp = *this;
		++(*this);
		return temp;
	}

	constexpr _Vector_Const_Iterator& operator+=(const difference_type diff) noexcept {
		CUSTOM_ASSERT(_Ptr + diff <= _RefData->_Last, "Cannot increment end iterator.");
		_Ptr += diff;
		return *this;
	}

	constexpr _Vector_Const_Iterator operator+(const difference_type diff) const noexcept {
		_Vector_Const_Iterator temp = *this;
		temp += diff;
		return temp;
	}

	constexpr _Vector_Const_Iterator& operator--() noexcept {
		CUSTOM_ASSERT(_Ptr > _RefData->_First, "Cannot decrement begin iterator.");
		--_Ptr;
		return *this;
	}

	constexpr _Vector_Const_Iterator operator--(int) noexcept {
		_Vector_Const_Iterator temp = *this;
		--(*this);
		return temp;
	}

	constexpr _Vector_Const_Iterator& operator-=(const difference_type diff) noexcept {
		CUSTOM_ASSERT(_Ptr - diff >= _RefData->_First, "Cannot decrement begin iterator.");
		_Ptr -= diff;
		return *this;
	}

	constexpr _Vector_Const_Iterator operator-(const difference_type diff) const noexcept {
		_Vector_Const_Iterator temp = *this;
		temp -= diff;
		return temp;
	}

	constexpr pointer operator->() const noexcept {
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot access end iterator.");
		return _Ptr;
	}

	constexpr reference operator*() const noexcept {
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot dereference end iterator.");
		return *_Ptr;
	}

    constexpr reference operator[](const difference_type diff) const noexcept {
        return *(*this + diff);
    }

	constexpr bool operator==(const _Vector_Const_Iterator& other) const noexcept {
		return _Ptr == other._Ptr;
	}

	constexpr bool operator!=(const _Vector_Const_Iterator& other) const noexcept {
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

	friend constexpr void _verify_range(const _Vector_Const_Iterator& first, const _Vector_Const_Iterator& last) noexcept {
		CUSTOM_ASSERT(first._RefData == last._RefData, "vector iterators in range are from different containers");
		CUSTOM_ASSERT(first._Ptr <= last._Ptr, "vector iterator range transposed");
	}
}; // END _Vector_Const_Iterator

template<class VecData>
class _Vector_Iterator : public _Vector_Const_Iterator<VecData>			// vector iterator
{
private:
	using _Base				= _Vector_Const_Iterator<VecData>;
	using _Data				= VecData;
	
public:
    using iterator_category	= random_access_iterator_tag;
	using value_type 		= typename _Data::value_type;
	using difference_type 	= typename _Data::difference_type;
	using reference			= typename _Data::reference;
	using pointer			= typename _Data::pointer;

public:

	constexpr _Vector_Iterator() noexcept = default;

	constexpr explicit _Vector_Iterator(value_type* ptr, const _Data* data) noexcept
		:_Base(ptr, data) { /*Empty*/ }

	constexpr _Vector_Iterator& operator++() noexcept {
		_Base::operator++();
		return *this;
	}

	constexpr _Vector_Iterator operator++(int) noexcept {
		_Vector_Iterator temp = *this;
		_Base::operator++();
		return temp;
	}

	constexpr _Vector_Iterator& operator+=(const difference_type diff) noexcept {
		_Base::operator+=(diff);
		return *this;
	}

	constexpr _Vector_Iterator operator+(const difference_type diff) const noexcept {
		_Vector_Iterator temp = *this;
		temp += diff;
		return temp;
	}

	constexpr _Vector_Iterator& operator--() noexcept {
		_Base::operator--();
		return *this;
	}

	constexpr _Vector_Iterator operator--(int) noexcept {
		_Vector_Iterator temp = *this;
		_Base::operator--();
		return temp;
	}

	constexpr _Vector_Iterator& operator-=(const difference_type diff) noexcept {
		_Base::operator-=(diff);
		return *this;
	}

	constexpr _Vector_Iterator operator-(const difference_type diff) const noexcept {
		_Vector_Iterator temp = *this;
		temp -= diff;
		return temp;
	}

	constexpr pointer operator->() const noexcept {
		return const_cast<pointer>(_Base::operator->());
	}

	constexpr reference operator*() const noexcept {
		return const_cast<reference>(_Base::operator*());
	}

    constexpr reference operator[](const difference_type diff) const noexcept {
        return const_cast<reference>(_Base::operator[](diff));
    }
}; // END _Vector_Iterator


template<class Type, class Alloc = custom::allocator<Type>>
class vector			// vector Template
{
private:
	using _Data						= _Vector_Data<Type, Alloc>;					// Members that are modified
	using _AllocTraits				= typename _Data::_AllocTraits;

public:
	static_assert(is_same_v<Type, typename Alloc::value_type>, "Object type and allocator type must be the same!");
	static_assert(is_object_v<Type>, "Containers require object type!");

	using value_type				= typename _Data::value_type;				// Type for stored values
	using difference_type			= typename _Data::difference_type;
	using reference					= typename _Data::reference;
	using const_reference			= typename _Data::const_reference;
	using pointer					= typename _Data::pointer;
	using const_pointer				= typename _Data::const_pointer;
	using allocator_type			= Alloc;
	
	using iterator					= _Vector_Iterator<_Data>;
	using const_iterator			= _Vector_Const_Iterator<_Data>;
	using reverse_iterator			= custom::reverse_iterator<iterator>;
	using const_reverse_iterator	= custom::reverse_iterator<const_iterator>;

private:
	_Data _data;															// Actual container data
	allocator_type _alloc;													// allocator
	
	static constexpr size_t _DEFAULT_CAPACITY = 8;

public:
	// Constructors

	constexpr vector() {													// Default Constructor
		reserve(_DEFAULT_CAPACITY);
	}

	constexpr vector(const size_t newCapacity) {							// Add multiple default copies Constructor
		realloc(newCapacity);
	}

	constexpr vector(	const size_t newCapacity,
						const value_type& copyValue) {						// Add multiple copies Constructor
		realloc(newCapacity, copyValue);
	}

	constexpr vector(const vector& other) {									// Copy Constructor
		_copy(other);
	}

	constexpr vector(vector&& other) noexcept {								// Move Constructor
		_move(custom::move(other));
	}

	constexpr ~vector() noexcept {											// Destructor
		_clean_up_array();
	}

public:
	// Operators

	constexpr const_reference operator[](const size_t index) const noexcept {	// Acces object at index (read only)
		CUSTOM_ASSERT(index < size(), "Index out of bounds.");
		return _data._First[index];
	}

	constexpr reference operator[](const size_t index) noexcept {				// Acces object at index
		CUSTOM_ASSERT(index < size(), "Index out of bounds.");
		return _data._First[index];
	}

	constexpr vector& operator=(const vector& other) {							// Assign operator using reference
		if (_data._First != other._data._First)
		{
			_clean_up_array();
			_copy(other);
		}

		return *this;
	}

	constexpr vector& operator=(vector&& other) noexcept {						// Assign operator using temporary
		if (_data._First != other._data._First)
		{
			_clean_up_array();
			_move(custom::move(other));
		}

		return *this;
	}

public:
	// Main functions

	constexpr void reserve(const size_t newCapacity) {							// Allocate memory and move values if needed
		if (newCapacity < size())
			_data._Last = _data._First + newCapacity;

		value_type* newArray 	= _alloc.allocate(newCapacity);
		size_t newSize			= size();

		for (size_t i = 0; i < newSize; ++i)
			_AllocTraits::construct(_alloc, &newArray[i], custom::move(_data._First[i]));

		_clean_up_array();
		_data._First	= newArray;
		_data._Last		= _data._First + newSize;
		_data._Final	= _data._First + newCapacity;
	}

	constexpr void shrink_to_fit() {											// Allocate memory with capacity equal to size and move values there
		reserve(size());
	}

	constexpr void realloc(const size_t newCapacity) {							// Allocate memory and populate it with default values (delete old)
		_clean_up_array();

		_data._First	= _alloc.allocate(newCapacity);
		_data._Last		= _data._First + newCapacity;
		_data._Final	= _data._First + newCapacity;
		_construct_range(_data._First, newCapacity);
	}

	constexpr void realloc(	const size_t newCapacity,
							const value_type& copyValue) {						// Allocate memory and populate it with given reference (delete old)
		_clean_up_array();

		_data._First	= _alloc.allocate(newCapacity);
		_data._Last		= _data._First + newCapacity;
		_data._Final	= _data._First + newCapacity;
		_construct_range(_data._First, newCapacity, copyValue);
	}
	
	constexpr void resize(const size_t newSize) {								// Change size and Construct/Destruct objects with default value if needed
		if (newSize < size())
			_destroy_range(_data._First + newSize, size() - newSize);
		else
		{
			if (newSize > capacity())
				reserve(newSize);

			_construct_range(_data._Last, newSize - size());
		}

		_data._Last = _data._First + newSize;
	}

	constexpr void resize(const size_t newSize, const value_type& copyValue) {	// Change size and Construct/Destruct objects with given reference if needed
		if (newSize < size())
			_destroy_range(_data._First + newSize, size() - newSize);
		else
		{
			if (newSize > capacity())
				reserve(newSize);

			_construct_range(_data._Last, newSize - size(), copyValue);
		}

		_data._Last = _data._First + newSize;
	}

	template<class... Args>
	constexpr void emplace_back(Args&&... args) {								// Construct object using arguments (Args) and add it to the tail
		_extend_if_full();
		_AllocTraits::construct(_alloc, _data._Last++, custom::forward<Args>(args)...);
	}

	constexpr void push_back(const value_type& copyValue) {						// Construct object using reference and add it to the tail
		emplace_back(copyValue);
	}

	constexpr void push_back(value_type&& moveValue) {							// Construct object using temporary and add it to the tail
		emplace_back(custom::move(moveValue));
	}

	constexpr void pop_back() {													// Remove last component
		if (!empty())
			_AllocTraits::destroy(_alloc, --_data._Last);
	}

	template<class... Args>
	constexpr iterator emplace(const_iterator iterator, Args&&... args) {		// Emplace object at iterator position with given arguments
		size_t index = iterator.get_index();	// Don't check end()
		emplace_back();

		for (size_t i = size() - 1; i > index; --i)
			_data._First[i] = custom::move(_data._First[i - 1]);

		_AllocTraits::destroy(_alloc, _data._First + index);
		_AllocTraits::construct(_alloc, _data._First + index, custom::forward<Args>(args)...);

		return iterator(_data._First + index, &_data);
	}

	constexpr iterator push(const_iterator iterator, const value_type& copyValue) {		// Push copy object at iterator position
		return emplace(iterator, copyValue);
	}

	constexpr iterator push(const_iterator iterator, value_type&& moveValue) {			// Push temporary object at iterator position
		return emplace(iterator, custom::move(moveValue));
	}

	constexpr iterator pop(const_iterator iterator) {									// Remove component at iterator position
		if (iterator.is_end())
			throw std::out_of_range("vector pop iterator outside range.");

		size_t index = iterator.get_index();
		size_t beforeLast = size() - 1;

		for (size_t i = index; i < beforeLast; ++i)
			_data._First[i] = custom::move(_data._First[i + 1]);
		pop_back();

		return iterator(_data._First + index, &_data);
	}

	constexpr size_t capacity() const noexcept {								// Get capacity
		return static_cast<size_t>(_data._Final - _data._First);
	}

	constexpr size_t size() const noexcept {									// Get size
		return static_cast<size_t>(_data._Last - _data._First);
	}

	constexpr size_t max_size() const noexcept {
		return (custom::min)(	static_cast<size_t>((numeric_limits<difference_type>::max)()),
								_AllocTraits::max_size(_alloc));
	}

	constexpr bool empty() const noexcept {										// Check if array is empty
		return (_data._First == _data._Last);
	}
	
	constexpr void clear() {													// Remove ALL components but keep memory
		_destroy_range(_data._First, size());
		_data._Last = _data._First;
	}

	constexpr const_reference at(const size_t index) const {						// Acces object at index with check (read only)
		if (index >= size())
			throw std::out_of_range("Index out of bounds.");

		return _data._First[index];
	}

	constexpr reference at(const size_t index) {								// Acces object at index with check
		if (index >= size())
			throw std::out_of_range("Index out of bounds.");

		return _data._First[index];
	}

	constexpr const_reference front() const noexcept {
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._First[0];
	}

	constexpr reference front() noexcept {										// Get the value of the first component
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._First[0];
	}

	constexpr const_reference back() const noexcept {
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Last[-1];
	}

	constexpr reference back() noexcept {										// Get the value of the last component
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return _data._Last[-1];
	}

	constexpr const_reference data() const noexcept  {
		return _data._First;
	}

	constexpr reference data() noexcept {
		return _data._First;
	}

public:
	// iterator specific functions

	constexpr iterator begin() noexcept {
		return iterator(_data._First, &_data);
	}

	constexpr const_iterator begin() const noexcept {
		return const_iterator(_data._First, &_data);
	}

	constexpr reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}

	constexpr const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}

	constexpr iterator end() noexcept {
		return iterator(_data._Last, &_data);
	}

	constexpr const_iterator end() const noexcept {
		return const_iterator(_data._Last, &_data);
	}

	constexpr reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}

	constexpr const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}

private:
	// Helpers

	constexpr void _copy(const vector& other) {									// Generic copy function for vector
		_data._First	= _alloc.allocate(other.capacity());
		size_t newSize	= other.size();

		for (size_t i = 0; i < newSize; ++i)
			_AllocTraits::construct(_alloc, &_data._First[i], other._data._First[i]);

		_data._Last		= _data._First + other.size();
		_data._Final	= _data._First + other.capacity();
	}

	constexpr void _move(vector&& other) noexcept {								// Generic move function for vector
		_data._First 	= custom::exchange(other._data._First, nullptr);
		_data._Last 	= custom::exchange(other._data._Last, nullptr);
		_data._Final 	= custom::exchange(other._data._Final, nullptr);
	}

	constexpr void _construct_range(value_type* const address, const size_t length) {
		for (size_t i = 0; i < length; ++i)
			_AllocTraits::construct(_alloc, address + i);
	}

	constexpr void _construct_range(value_type* const address, const size_t length, const value_type& value) {
		for (size_t i = 0; i < length; ++i)
			_AllocTraits::construct(_alloc, address + i, value);
	}

	constexpr void _destroy_range(value_type* address, const size_t length) {
		for (size_t i = 0; i < length; ++i)
			_AllocTraits::destroy(_alloc, address + i);
	}

	constexpr void _extend_if_full() {											// Reserve 50% more capacity when full
		if (_data._Last == _data._Final)
			reserve(capacity() + capacity() / 2 + 1);
	}

	constexpr void _clean_up_array() {											// Clear and Deallocate array
		if (_data._First != nullptr)
		{
			_destroy_range(_data._First, size());
			_alloc.deallocate(_data._First, capacity());
			_data._First	= nullptr;
			_data._Last		= nullptr;
			_data._Final	= nullptr;
		}
	}
}; // END vector Template


// vector binary operators
template<class _Type, class _Alloc>
constexpr bool operator==(const vector<_Type, _Alloc>& left, const vector<_Type, _Alloc>& right) {
	if (left.size() != right.size())
		return false;

	return custom::equal(left.begin(), left.end(), right.begin());
}

template<class _Type, class _Alloc>
constexpr bool operator!=(const vector<_Type, _Alloc>& left, const vector<_Type, _Alloc>& right) {
	return !(left == right);
}

CUSTOM_END