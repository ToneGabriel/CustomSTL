#pragma once
#include "xMemory.h"
#include "Utility.h"
#include "Algorithm.h"
#include "Iterator.h"


CUSTOM_BEGIN

template<class Type, class Alloc>
struct _VectorData
{
	using _AllocTraits		= AllocatorTraits<Alloc>;

	using ValueType			= typename _AllocTraits::ValueType;
	using Reference			= typename _AllocTraits::Reference;
	using ConstReference	= typename _AllocTraits::ConstReference;
	using Pointer			= typename _AllocTraits::Pointer;
	using ConstPointer		= typename _AllocTraits::ConstPointer;

	Pointer _First			= nullptr;			// Actual container array
	Pointer _Last			= nullptr;			// Pointer to end
	Pointer _Final			= nullptr;			// Pointer to capacity end
};

template<class VecData>
class VectorConstIterator
{
private:
	using _Data		= VecData;

public:
	using ValueType = typename _Data::ValueType;
	using Reference = typename _Data::ConstReference;
	using Pointer	= typename _Data::ConstPointer;

	ValueType* _Ptr			= nullptr;
	const _Data* _RefData	= nullptr;

public:

	constexpr VectorConstIterator() noexcept : _Ptr() { /*Empty*/ }

	constexpr explicit VectorConstIterator(ValueType* ptr, const _Data* data) noexcept
		:_Ptr(ptr), _RefData(data) { /*Empty*/ }

	constexpr VectorConstIterator& operator++() noexcept {
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot increment end iterator...");
		++_Ptr;
		return *this;
	}

	constexpr VectorConstIterator operator++(int) noexcept {
		VectorConstIterator temp = *this;
		++(*this);
		return temp;
	}

	constexpr VectorConstIterator& operator+=(const size_t& diff) noexcept {
		CUSTOM_ASSERT(_Ptr + diff < _RefData->_Last, "Cannot increment end iterator...");
		_Ptr += diff;
		return *this;
	}

	constexpr VectorConstIterator operator+(const size_t& diff) const noexcept {
		VectorConstIterator temp = *this;
		temp += diff;
		return temp;
	}

	constexpr VectorConstIterator& operator--() noexcept {
		CUSTOM_ASSERT(_Ptr > _RefData->_First, "Cannot decrement begin iterator...");
		--_Ptr;
		return *this;
	}

	constexpr VectorConstIterator operator--(int) noexcept {
		VectorConstIterator temp = *this;
		--(*this);
		return temp;
	}

	constexpr VectorConstIterator& operator-=(const size_t& diff) noexcept {
		CUSTOM_ASSERT(_Ptr - diff > _RefData->_First, "Cannot decrement begin iterator...");
		_Ptr -= diff;
		return *this;
	}

	constexpr VectorConstIterator operator-(const size_t& diff) const noexcept {
		VectorConstIterator temp = *this;
		temp -= diff;
		return temp;
	}

	constexpr Pointer operator->() const noexcept {
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot access end iterator...");
		return _Ptr;
	}

	constexpr Reference operator*() const noexcept {
		CUSTOM_ASSERT(_Ptr < _RefData->_Last, "Cannot dereference end iterator...");
		return *_Ptr;
	}

	constexpr bool operator==(const VectorConstIterator& other) const noexcept {
		return _Ptr == other._Ptr;
	}

	constexpr bool operator!=(const VectorConstIterator& other) const noexcept {
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

	friend constexpr void _verify_range(const VectorConstIterator& first, const VectorConstIterator& last) noexcept {
		CUSTOM_ASSERT(first._RefData == last._RefData, "Vector iterators in range are from different containers");
		CUSTOM_ASSERT(first._Ptr <= last._Ptr, "Vector iterator range transposed");
	}
}; // END VectorConstIterator

template<class VecData>
class VectorIterator : public VectorConstIterator<VecData>			// Vector Iterator
{
private:
	using _Base		= VectorConstIterator<VecData>;
	using _Data		= VecData;
	
public:
	using ValueType = typename _Data::ValueType;
	using Reference	= typename _Data::Reference;
	using Pointer	= typename _Data::Pointer;

public:

	constexpr VectorIterator() noexcept = default;

	constexpr explicit VectorIterator(ValueType* ptr, const _Data* data) noexcept
		:_Base(ptr, data) { /*Empty*/ }

	constexpr VectorIterator& operator++() noexcept {
		_Base::operator++();
		return *this;
	}

	constexpr VectorIterator operator++(int) noexcept {
		VectorIterator temp = *this;
		_Base::operator++();
		return temp;
	}

	constexpr VectorIterator& operator+=(const size_t& diff) noexcept {
		_Base::operator+=(diff);
		return *this;
	}

	constexpr VectorIterator operator+(const size_t& diff) const noexcept {
		VectorIterator temp = *this;
		temp += diff;
		return temp;
	}

	constexpr VectorIterator& operator--() noexcept {
		_Base::operator--();
		return *this;
	}

	constexpr VectorIterator operator--(int) noexcept {
		VectorIterator temp = *this;
		_Base::operator--();
		return temp;
	}

	constexpr VectorIterator& operator-=(const size_t& diff) noexcept {
		_Base::operator-=(diff);
		return *this;
	}

	constexpr VectorIterator operator-(const size_t& diff) const noexcept {
		VectorIterator temp = *this;
		temp -= diff;
		return temp;
	}

	constexpr Pointer operator->() const noexcept {
		return const_cast<Pointer>(_Base::operator->());
	}

	constexpr Reference operator*() const noexcept {
		return const_cast<Reference>(_Base::operator*());
	}
}; // END VectorIterator


template<class Type, class Alloc = custom::Allocator<Type>>
class Vector			// Vector Template
{
private:
	using _Data					= _VectorData<Type, Alloc>;					// Members that are modified
	using _AllocTraits			= typename _Data::_AllocTraits;

public:
	static_assert(IsSame_v<Type, typename Alloc::ValueType>, "Object type and Allocator type must be the same!");
	static_assert(IsObject_v<Type>, "Containers require object type!");

	using ValueType 			= typename _Data::ValueType;				// Type for stored values
	using Reference				= typename _Data::Reference;
	using ConstReference		= typename _Data::ConstReference;
	using Pointer				= typename _Data::Pointer;
	using ConstPointer			= typename _Data::ConstPointer;
	using AllocatorType			= Alloc;
	
	using Iterator				= VectorIterator<_Data>;					// Iterator type
	using ConstIterator			= VectorConstIterator<_Data>;				// Const Iterator type
	using ReverseIterator 		= custom::ReverseIterator<Iterator>;		// ReverseIterator type
	using ConstReverseIterator	= custom::ReverseIterator<ConstIterator>;	// Const Reverse Iterator type

private:
	_Data _data;															// Actual container data
	AllocatorType _alloc;													// Allocator
	
	static constexpr size_t _DEFAULT_CAPACITY = 8;

public:
	// Constructors

	constexpr Vector() {													// Default Constructor
		reserve(_DEFAULT_CAPACITY);
	}

	constexpr Vector(	const size_t& newCapacity,
						const ValueType& copyValue) {						// Add multiple copies Constructor
		realloc(newCapacity, copyValue);
	}

	constexpr Vector(const Vector& other) {									// Copy Constructor
		_copy(other);
	}

	constexpr Vector(Vector&& other) noexcept {								// Move Constructor
		_move(custom::move(other));
	}

	constexpr ~Vector() noexcept {											// Destructor
		_clean_up_array();
	}

public:
	// Operators

	constexpr ConstReference operator[](const size_t& index) const noexcept {	// Acces object at index (read only)
		CUSTOM_ASSERT(index < size(), "Index out of bounds...");
		return _data._First[index];
	}

	constexpr Reference operator[](const size_t& index) noexcept {				// Acces object at index
		CUSTOM_ASSERT(index < size(), "Index out of bounds...");
		return _data._First[index];
	}

	constexpr Vector& operator=(const Vector& other) {							// Assign operator using reference
		if (_data._First != other._data._First)
		{
			_clean_up_array();
			_copy(other);
		}

		return *this;
	}

	constexpr Vector& operator=(Vector&& other) noexcept {						// Assign operator using temporary
		if (_data._First != other._data._First)
		{
			_clean_up_array();
			_move(custom::move(other));
		}

		return *this;
	}

public:
	// Main functions

	constexpr void reserve(const size_t& newCapacity) {							// Allocate memory and move values if needed
		if (newCapacity < size())
			_data._Last = _data._First + newCapacity;

		ValueType* newArray = _alloc.allocate(newCapacity);
		size_t newSize		= size();

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

	constexpr void realloc(const size_t& newCapacity) {							// Allocate memory and populate it with default values (delete old)
		_clean_up_array();

		_data._First	= _alloc.allocate(newCapacity);
		_data._Last		= _data._First + newCapacity;
		_data._Final	= _data._First + newCapacity;
		_construct_range(_data._First, newCapacity);
	}

	constexpr void realloc(	const size_t& newCapacity,
							const ValueType& copyValue) {						// Allocate memory and populate it with given reference (delete old)
		_clean_up_array();

		_data._First	= _alloc.allocate(newCapacity);
		_data._Last		= _data._First + newCapacity;
		_data._Final	= _data._First + newCapacity;
		_construct_range(_data._First, newCapacity, copyValue);
	}
	
	constexpr void resize(const size_t& newSize) {								// Change size and Construct/Destruct objects with default value if needed
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

	constexpr void resize(const size_t& newSize, const ValueType& copyValue) {	// Change size and Construct/Destruct objects with given reference if needed
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

	constexpr void push_back(const ValueType& copyValue) {						// Construct object using reference and add it to the tail
		emplace_back(copyValue);
	}

	constexpr void push_back(ValueType&& moveValue) {							// Construct object using temporary and add it to the tail
		emplace_back(custom::move(moveValue));
	}

	constexpr void pop_back() {													// Remove last component
		if (!empty())
			_AllocTraits::destroy(_alloc, --_data._Last);
	}

	template<class... Args>
	constexpr Iterator emplace(ConstIterator iterator, Args&&... args) {		// Emplace object at iterator position with given arguments
		size_t index = iterator.get_index();	// Don't check end()
		emplace_back();

		for (size_t i = size() - 1; i > index; --i)
			_data._First[i] = custom::move(_data._First[i - 1]);

		_AllocTraits::destroy(_alloc, _data._First + index);
		_AllocTraits::construct(_alloc, _data._First + index, custom::forward<Args>(args)...);

		return Iterator(_data._First + index, &_data);
	}

	constexpr Iterator push(ConstIterator iterator, const ValueType& copyValue) {		// Push copy object at iterator position
		return emplace(iterator, copyValue);
	}

	constexpr Iterator push(ConstIterator iterator, ValueType&& moveValue) {			// Push temporary object at iterator position
		return emplace(iterator, custom::move(moveValue));
	}

	constexpr Iterator pop(ConstIterator iterator) {									// Remove component at iterator position
		if (iterator.is_end())
			throw std::out_of_range("Array pop iterator outside range...");

		size_t index = iterator.get_index();
		size_t beforeLast = size() - 1;

		for (size_t i = index; i < beforeLast; ++i)
			_data._First[i] = custom::move(_data._First[i + 1]);
		pop_back();

		return Iterator(_data._First + index, &_data);
	}

	constexpr size_t capacity() const noexcept {								// Get capacity
		return static_cast<size_t>(_data._Final - _data._First);
	}

	constexpr size_t size() const noexcept {									// Get size
		return static_cast<size_t>(_data._Last - _data._First);
	}

	//constexpr size_t max_size() const noexcept {
	//	return (std::min)(	static_cast<size_t>((custom::NumericLimits<ptrdiff_t>::max)()),
	//						_AllocTraits::max_size(_alloc));
	//}

	constexpr bool empty() const noexcept {										// Check if array is empty
		return (_data._First == _data._Last);
	}
	
	constexpr void clear() {													// Remove ALL components but keep memory
		_destroy_range(_data._First, size());
		_data._Last = _data._First;
	}

	constexpr ConstReference at(const size_t& index) const {					// Acces object at index with check (read only)
		if (index >= size())
			throw std::out_of_range("Index out of bounds...");

		return _data._First[index];
	}

	constexpr Reference at(const size_t& index) {								// Acces object at index with check
		if (index >= size())
			throw std::out_of_range("Index out of bounds...");

		return _data._First[index];
	}

	constexpr ConstReference front() const noexcept {
		CUSTOM_ASSERT(!empty(), "Container is empty...");
		return _data._First[0];
	}

	constexpr Reference front() noexcept {										// Get the value of the first component
		CUSTOM_ASSERT(!empty(), "Container is empty...");
		return _data._First[0];
	}

	constexpr ConstReference back() const noexcept {
		CUSTOM_ASSERT(!empty(), "Container is empty...");
		return _data._Last[-1];
	}

	constexpr Reference back() noexcept {										// Get the value of the last component
		CUSTOM_ASSERT(!empty(), "Container is empty...");
		return _data._Last[-1];
	}

	constexpr ConstReference data() const noexcept  {
		return _data._First;
	}

	constexpr Reference data() noexcept {
		return _data._First;
	}

public:
	// Iterator specific functions

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

	constexpr void _copy(const Vector& other) {									// Generic copy function for vector
		_data._First	= _alloc.allocate(other.capacity());
		size_t newSize	= other.size();

		for (size_t i = 0; i < newSize; ++i)
			_AllocTraits::construct(_alloc, &_data._First[i], other._data._First[i]);

		_data._Last		= _data._First + other.size();
		_data._Final	= _data._First + other.capacity();
	}

	constexpr void _move(Vector&& other) noexcept {								// Generic move function for vector
		_data._First 	= custom::exchange(other._data._First, nullptr);
		_data._Last 	= custom::exchange(other._data._Last, nullptr);
		_data._Final 	= custom::exchange(other._data._Final, nullptr);
	}

	constexpr void _construct_range(ValueType* const address, const size_t& length) {
		for (size_t i = 0; i < length; ++i)
			_AllocTraits::construct(_alloc, address + i);
	}

	constexpr void _construct_range(ValueType* const address, const size_t& length, const ValueType& value) {
		for (size_t i = 0; i < length; ++i)
			_AllocTraits::construct(_alloc, address + i, value);
	}

	constexpr void _destroy_range(ValueType* address, const size_t& length) {
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
}; // END Vector Template


// Vector binary operators
template<class _Type, class _Alloc>
constexpr bool operator==(const Vector<_Type, _Alloc>& left, const Vector<_Type, _Alloc>& right) {
	if (left.size() != right.size())
		return false;

	return custom::equal(left.begin(), left.end(), right.begin());
}

template<class _Type, class _Alloc>
constexpr bool operator!=(const Vector<_Type, _Alloc>& left, const Vector<_Type, _Alloc>& right) {
	return !(left == right);
}

CUSTOM_END