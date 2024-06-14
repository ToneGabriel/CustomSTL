#pragma once
#include "xMemory.h"
#include "c_utility.h"
#include "c_iterator.h"
#include "c_algorithm.h"


CUSTOM_BEGIN

template<class Type, class Alloc>
struct _DequeData
{
	using _AllocTraits		= allocator_traits<Alloc>;
	using _AllocPtr 		= typename _AllocTraits::template RebindAlloc<typename _AllocTraits::pointer>;
    using _AllocPtrTraits 	= allocator_traits<_AllocPtr>;
	using _MapPtr 			= typename _AllocPtrTraits::pointer;

	using ValueType			= typename _AllocTraits::ValueType;
	using DifferenceType 	= typename _AllocTraits::DifferenceType;
	using reference			= typename _AllocTraits::reference;
	using const_reference	= typename _AllocTraits::const_reference;
	using pointer			= typename _AllocTraits::pointer;
	using const_pointer		= typename _AllocTraits::const_pointer;

	_MapPtr _Map 			= nullptr;
	size_t _MapCapacity 	= 0;
	size_t _First 			= 0;
	size_t _Size 			= 0;

	static constexpr size_t BLOCK_SIZE = 4;

	size_t get_block(const size_t offset) const noexcept {
		return (offset / BLOCK_SIZE) % _MapCapacity;
    }
};

template<class DequeData>
class DequeConstIterator
{
private:
	using _Data				= DequeData;

public:
    using iterator_category 	= random_access_iterator_tag;
	using ValueType			= typename _Data::ValueType;
	using DifferenceType 	= typename _Data::DifferenceType;
	using reference			= typename _Data::const_reference;
	using pointer			= typename _Data::const_pointer;

	size_t _Offset			= 0;
	const _Data* _RefData	= nullptr;

public:

	DequeConstIterator() noexcept = default;

    explicit DequeConstIterator(size_t offset, const _Data* data) noexcept
		:_Offset(offset), _RefData(data) { /*Empty*/ }

	DequeConstIterator& operator++() noexcept {
		CUSTOM_ASSERT(_Offset < _RefData->_First + _RefData->_Size, "Cannot increment end iterator.");
		++_Offset;
		return *this;
	}

	DequeConstIterator operator++(int) noexcept {
		DequeConstIterator temp = *this;
		++(*this);
		return temp;
	}

	DequeConstIterator& operator+=(const DifferenceType diff) noexcept {
		CUSTOM_ASSERT(	_Offset + static_cast<size_t>(diff) >= _RefData->_First &&
						_Offset + static_cast<size_t>(diff) <= _RefData->_First + _RefData->_Size, 
						"Cannot increment end iterator.");
		_Offset += static_cast<size_t>(diff);
		return *this;
	}

	DequeConstIterator operator+(const DifferenceType diff) const noexcept {
		DequeConstIterator temp = *this;
		temp += diff;
		return temp;
	}

	DequeConstIterator& operator--() noexcept {
		CUSTOM_ASSERT(_Offset > _RefData->_First, "Cannot decrement begin iterator.");
		--_Offset;
		return *this;
	}

	DequeConstIterator operator--(int) noexcept {
		DequeConstIterator temp = *this;
		--(*this);
		return temp;
	}

	DequeConstIterator& operator-=(const DifferenceType diff) noexcept {
		CUSTOM_ASSERT(	_Offset - static_cast<size_t>(diff) >= _RefData->_First &&
						_Offset - static_cast<size_t>(diff) <= _RefData->_First + _RefData->_Size,
						"Cannot decrement begin iterator.");
		_Offset -= static_cast<size_t>(diff);
		return *this;
	}

	DequeConstIterator operator-(const DifferenceType diff) const noexcept {
		DequeConstIterator temp = *this;
		temp -= diff;
		return temp;
	}

	pointer operator->() const noexcept {
        return pointer_traits<pointer>::pointer_to(**this);	//return &(**this); calls operator*
	}

	reference operator*() const noexcept {
		CUSTOM_ASSERT(	_Offset >= _RefData->_First &&
						_Offset < _RefData->_First + _RefData->_Size,
						"Cannot dereference end iterator.");

		size_t block	= _RefData->get_block(_Offset);
		size_t offset	= _Offset % _RefData->BLOCK_SIZE;
		return _RefData->_Map[block][offset];
	}

	reference operator[](const DifferenceType diff) const noexcept {
        return *(*this + diff);
    }

	bool operator==(const DequeConstIterator& other) const noexcept {
		return _Offset == other._Offset;
	}

	bool operator!=(const DequeConstIterator& other) const noexcept {
		return !(*this == other);
	}

public:

	bool is_begin() const noexcept {
		return _Offset == _RefData->_First;
	}

	bool is_end() const noexcept {
		return _Offset == _RefData->_First + _RefData->_Size;
	}

	friend void _verify_range(const DequeConstIterator& first, const DequeConstIterator& last) noexcept {
		CUSTOM_ASSERT(first._RefData == last._RefData, "Deque iterators in range are from different containers");
		CUSTOM_ASSERT(first._Offset <= last._Offset, "Deque iterator range transposed");
	}
}; // END DequeConstIterator

template<class DequeData>
class DequeIterator : public DequeConstIterator<DequeData>			// Deque iterator
{
private:
	using _Base				= DequeConstIterator<DequeData>;
	using _Data				= DequeData;

public:
    using iterator_category 	= random_access_iterator_tag;
	using ValueType			= typename _Data::ValueType;
	using DifferenceType 	= typename _Data::DifferenceType;
	using reference			= typename _Data::reference;
	using pointer			= typename _Data::pointer;

public:

	DequeIterator() noexcept = default;

	explicit DequeIterator(size_t offset, const _Data* data) noexcept
		:_Base(offset, data) { /*Empty*/ }

	DequeIterator& operator++() noexcept {
		_Base::operator++();
		return *this;
	}

	DequeIterator operator++(int) noexcept {
		DequeIterator temp = *this;
		_Base::operator++();
		return temp;
	}

	DequeIterator& operator+=(const DifferenceType diff) noexcept {
		_Base::operator+=(diff);
		return *this;
	}

	DequeIterator operator+(const DifferenceType diff) const noexcept {
		DequeIterator temp = *this;
		temp += diff;
		return temp;
	}

	DequeIterator& operator--() noexcept {
		_Base::operator--();
		return *this;
	}

	DequeIterator operator--(int) noexcept {
		DequeIterator temp = *this;
		_Base::operator--();
		return temp;
	}

	DequeIterator& operator-=(const DifferenceType diff) noexcept {
		_Base::operator-=(diff);
		return *this;
	}

	DequeIterator operator-(const DifferenceType diff) const noexcept {
		DequeIterator temp = *this;
		temp -= diff;
		return temp;
	}

	pointer operator->() const noexcept {
		return const_cast<pointer>(_Base::operator->());
	}

	reference operator*() const noexcept {
		return const_cast<reference>(_Base::operator*());
	}

	reference operator[](const DifferenceType diff) const noexcept {
        return const_cast<reference>(_Base::operator[](diff));
    }
}; // END Deque iterator


template<class Type, class Alloc = custom::allocator<Type>>
class Deque					// Deque Template implemented as map of blocks
{
private:
	using _Data 				= _DequeData<Type, Alloc>;
	using _AllocTraits			= typename _Data::_AllocTraits;
	using _AllocPtr 			= typename _Data::_AllocPtr;
	using _AllocPtrTraits		= typename _Data::_AllocPtrTraits;
	using _MapPtr				= typename _Data::_MapPtr;

public:
	static_assert(is_same_v<Type, typename Alloc::ValueType>, "Object type and allocator type must be the same!");
	static_assert(is_object_v<Type>, "Containers require object type!");

	using ValueType 			= typename _Data::ValueType;				// Type for stored values
	using DifferenceType 		= typename _Data::DifferenceType;
	using reference				= typename _Data::reference;
	using const_reference		= typename _Data::const_reference;
	using pointer				= typename _Data::pointer;
	using const_pointer			= typename _Data::const_pointer;
	using allocator_type 		= Alloc;									// allocator for block

	using iterator				= DequeIterator<_Data>;						// iterator type
	using const_iterator			= DequeConstIterator<_Data>;				// Const iterator type
	using reverse_iterator 		= custom::reverse_iterator<iterator>;		// reverse_iterator type
	using const_reverse_iterator	= custom::reverse_iterator<const_iterator>;	// Const Reverse iterator type

private:
	_Data _data;
	allocator_type _alloc;

	static constexpr size_t _DEFAULT_CAPACITY = 8;

public:
	// Constructors

	Deque() {
		_init_map(_DEFAULT_CAPACITY);
	}

	Deque(const size_t newSize) {
		resize(newSize);
	}

	Deque(const size_t newSize, const ValueType& copyValue) {
		resize(newSize, copyValue);
	}

	Deque(const Deque& other) {
		_copy(other);
	}

	Deque(Deque&& other) noexcept {
		_move(custom::move(other));
	}

	~Deque() noexcept {
		_clean_up_map();
	}

public:
	// Operators

	const ValueType& operator[](const size_t index) const {
		CUSTOM_ASSERT(index < size(), "Index out of bounds.");
		return *(begin() + static_cast<DifferenceType>(index));
	}

	ValueType& operator[](const size_t index) {
		CUSTOM_ASSERT(index < size(), "Index out of bounds.");
		return *(begin() + static_cast<DifferenceType>(index));
	}

	Deque& operator=(const Deque& other) {
		if (_data._Map != other._data._Map)
		{
			_clean_up_map();
			_copy(other);
		}

		return *this;
	}

	Deque& operator=(Deque&& other) noexcept {
		if (_data._Map != other._data._Map)
		{
			_clean_up_map();
			_move(custom::move(other));
		}

		return *this;
	}

public:
	// Main functions

	void resize(const size_t newSize) {
		while (_data._Size < newSize)
			emplace_back();

        while (_data._Size > newSize)
            pop_back();
	}

	void resize(const size_t newSize, const ValueType& copyValue) {
		while (_data._Size < newSize)
			emplace_back(copyValue);

        while (_data._Size > newSize)
            pop_back();
	}

	template<class... Args>
	void emplace_back(Args&&... args) {
		_extend_if_full();
		
		size_t backOffset	= _data._First + _data._Size;
		size_t block		= _data.get_block(backOffset);

		if (_data._Map[block] == nullptr)
			_data._Map[block] = _alloc.allocate(_data.BLOCK_SIZE);

		_AllocTraits::construct(_alloc, _data._Map[block] + backOffset % _data.BLOCK_SIZE, custom::forward<Args>(args)...);
		++_data._Size;
	}

	void push_back(const ValueType& copyValue) {
		emplace_back(copyValue);
	}

	void push_back(ValueType&& moveValue) {
		emplace_back(custom::move(moveValue));
	}

	void pop_back() {
		if (!empty())
		{
			size_t backOffset	= _data._First + _data._Size - 1;
			size_t block		= _data.get_block(backOffset);

			_AllocTraits::destroy(_alloc, _data._Map[block] + backOffset % _data.BLOCK_SIZE);
			if (--_data._Size == 0)
				_data._First = 0;
		}
	}

	template<class... Args>
	void emplace_front(Args&&... args) {
		_extend_if_full();

		if (_data._First == 0)
			_data._First = _data._MapCapacity * _data.BLOCK_SIZE;

		size_t block = _data.get_block(--_data._First);
		if (_data._Map[block] == nullptr)
			_data._Map[block] = _alloc.allocate(_data.BLOCK_SIZE);

		_AllocTraits::construct(_alloc, _data._Map[block] + _data._First % _data.BLOCK_SIZE, custom::forward<Args>(args)...);
		++_data._Size;
	}

	void push_front(const ValueType& copyValue) {
		emplace_front(copyValue);
	}

	void push_front(ValueType&& moveValue) {
		emplace_front(custom::move(moveValue));
	}

	void pop_front() {
		if (!empty())
		{
			size_t block = _data.get_block(_data._First);

			_AllocTraits::destroy(_alloc, _data._Map[block] + _data._First % _data.BLOCK_SIZE);
			if (--_data._Size == 0)
				_data._First = 0;
			else
				++_data._First;
		}
	}

	template<class... Args>
	iterator emplace(const_iterator iterator, Args&&... args) {
		size_t off = iterator._Offset - _data._First;

		if (off <= _data._Size / 2)		// closer to front
		{
			emplace_front(custom::forward<Args>(args)...);
			
			iterator current 	= begin() + static_cast<DifferenceType>(off);
			ValueType val 		= custom::move(*begin());

			for (iterator it = begin(); it != current; ++it)
				*it = custom::move(*(it + 1));

			*current = val;
		}
		else							// closer to back
		{
			emplace_back(custom::forward<Args>(args)...);

			iterator current 	= begin() + static_cast<DifferenceType>(off);
			ValueType val 		= custom::move(*--end());

			for (iterator it = --end(); it != current; --it)
				*it = custom::move(*(it - 1));

			*current = val;
		}
		
		return begin() + static_cast<DifferenceType>(off);
	}

	iterator push(const_iterator iterator, const ValueType& copyValue) {
		return emplace(iterator, copyValue);
	}

	iterator push(const_iterator iterator, ValueType&& moveValue) {
		return emplace(iterator, custom::move(moveValue));
	}

	iterator pop(const_iterator iterator) {
		if (iterator.is_end())
			throw std::out_of_range("array pop iterator outside range.");
			
		size_t off = iterator._Offset - _data._First;

		if (off <= _data._Size / 2)		// closer to front
		{
			iterator current 	= begin() + static_cast<DifferenceType>(off);
			iterator first 		= begin();

			for (iterator it = current; it != first; --it)
				*it = custom::move(*(it - 1));

			pop_front();
		}
		else							// closer to back
		{
			iterator current 	= begin() + static_cast<DifferenceType>(off);
			iterator last 		= --end();

			for (iterator it = current; it != last; ++it)
				*it = custom::move(*(it + 1));
			
			pop_back();
		}

		return begin() + static_cast<DifferenceType>(off);
	}

	void clear() {
		while (!empty())
			pop_back();

		for (size_t i = 0; i < _data._MapCapacity; ++i)
			if (_data._Map[i] != nullptr)
			{
				_alloc.deallocate(_data._Map[i], _data.BLOCK_SIZE);
				_data._Map[i] = nullptr;
			}
	}

	size_t size() const noexcept {
		return _data._Size;
	}

    size_t max_size() const noexcept {
        return (custom::min)(	static_cast<size_t>((numeric_limits<DifferenceType>::max)()),
								_AllocTraits::max_size(_alloc));
    }

	bool empty() const noexcept {
		return (_data._Size == 0);
	}

	const_reference at(const size_t index) const {
		if (index >= size())
			throw std::out_of_range("Index out of bounds.");

		return *(begin() + static_cast<DifferenceType>(index));
	}

	reference at(const size_t index) {
		if (index >= size())
			throw std::out_of_range("Index out of bounds.");

		return *(begin() + static_cast<DifferenceType>(index));
	}

	const_reference front() const noexcept {
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return *begin();
	}

	reference front() noexcept {										// Get the value of the first component
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return *begin();
	}

	const_reference back() const noexcept {
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return *(--end());
	}

	reference back() noexcept {                                                      	// Get the value of the last component
		CUSTOM_ASSERT(!empty(), "Container is empty.");
		return *(--end());
	}

	void print_details() {
		std::cout << "Map Capacity= " << _data._MapCapacity << '\n';
		std::cout << "Size= " << _data._Size << '\n';
		std::cout << "Init Offset= " << _data._First << '\n';

		for (size_t i = 0; i < _data._MapCapacity; ++i)
		{
			std::cout << "Block " << i << " = ";
			if (_data._Map[i] == nullptr)
				std::cout << "NULL\n";
			else
			{
				for (size_t j = 0; j < _data.BLOCK_SIZE; ++j)
					std::cout << _data._Map[i][j] << ' ';
				std::cout << '\n';
			}
		}
	}

public:
	// iterator specific functions

	iterator begin() noexcept {
		return iterator(_data._First, &_data);
	}

	const_iterator begin() const noexcept {
		return const_iterator(_data._First, &_data);
	}

	reverse_iterator rbegin() noexcept {
		return reverse_iterator(end());
	}

	const_reverse_iterator rbegin() const noexcept {
		return const_reverse_iterator(end());
	}

	iterator end() noexcept {
		return iterator(_data._First + _data._Size, &_data);
	}

	const_iterator end() const noexcept {
		return const_iterator(_data._First + _data._Size, &_data);
	}

	reverse_iterator rend() noexcept {
		return reverse_iterator(begin());
	}

	const_reverse_iterator rend() const noexcept {
		return const_reverse_iterator(begin());
	}

private:
	// Helpers

	void _reserve(const size_t newMapCapacity) {
		size_t newSize			= _data._Size;
		size_t newFirst			= _data._First % _data.BLOCK_SIZE;
		_MapPtr newMap 			= _create_empty_map(newMapCapacity);
		size_t firstBlock		= _data.get_block(_data._First);					// block to find first elem
		size_t lastBlock		= _data.get_block(_data._First + _data._Size - 1);	// block to find last elem (always != firstBlock)

		size_t newBlockIndex	= 0;
		size_t oldBlockIndex	= firstBlock;

		while (oldBlockIndex != lastBlock)								// copy pointers to blocks and nullify the old ones
		{
			newMap[newBlockIndex++]		= _data._Map[oldBlockIndex];
			_data._Map[oldBlockIndex]	= nullptr;

			if (oldBlockIndex == _data._MapCapacity - 1)
				oldBlockIndex = 0;
			else
				++oldBlockIndex;
		}
		newMap[newBlockIndex]		= _data._Map[oldBlockIndex];		// for lastBlock
		_data._Map[oldBlockIndex]	= nullptr;

		_data._Size = 0;												// clear the remaining empty blocks and map
		_clean_up_map();

		_data._Map				= newMap;
		_data._MapCapacity		= newMapCapacity;
		_data._Size				= newSize;
		_data._First			= newFirst;
	}

	void _extend_if_full() {												
		if (_data._Map == nullptr)												// after custom::move()
			_init_map(_DEFAULT_CAPACITY);
		else if (_data._Size >= (_data._MapCapacity - 1) * _data.BLOCK_SIZE)	// ensure first and last elem are not in the same block...
			_reserve(2 * _data._MapCapacity);									// ...with last < first
	}

	void _copy(const Deque& other) {
		_init_map(other._data._MapCapacity);

		for (auto& val : other)
			push_back(val);
	}

	void _move(Deque&& other) noexcept {
		_data._Map 			= custom::exchange(other._data._Map, nullptr);
		_data._MapCapacity 	= custom::exchange(other._data._MapCapacity, 0);
		_data._Size 		= custom::exchange(other._data._Size, 0);
		_data._First 		= custom::exchange(other._data._First, 0);
	}

	_MapPtr _create_empty_map(const size_t newMapCapacity) {
		_MapPtr newMap = _AllocPtr().allocate(newMapCapacity);
		for(size_t i = 0; i < newMapCapacity; ++i)
			newMap[i] = nullptr;

		return newMap;
	}

	void _init_map(const size_t newMapCapacity) {
		size_t newCapacity 	= (newMapCapacity < _DEFAULT_CAPACITY) ? _DEFAULT_CAPACITY : newMapCapacity;

		_data._Map 			= _create_empty_map(newCapacity);
		_data._MapCapacity	= newCapacity;
		_data._Size			= 0;
		_data._First		= 0;
	}

	void _clean_up_map() {
		if (_data._Map != nullptr)
		{
			clear();
			_AllocPtr().deallocate(_data._Map, _data._MapCapacity);
			_data._Map = nullptr;
		}
	}
}; // END Deque Template


// Deque binary operators
template<class _Type, class _Alloc>
bool operator==(const Deque<_Type, _Alloc>& left, const Deque<_Type, _Alloc>& right) {
	if (left.size() != right.size())
		return false;

	return custom::equal(left.begin(), left.end(), right.begin());
}

template<class _Type, class _Alloc>
bool operator!=(const Deque<_Type, _Alloc>& left, const Deque<_Type, _Alloc>& right) {
	return !(left == right);
}

CUSTOM_END