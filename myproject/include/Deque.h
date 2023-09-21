#pragma once
#include "xMemory.h"
#include "Utility.h"
#include "Algorithm.h"


CUSTOM_BEGIN

template<class Type>
struct DequeData
{
	using ValueType 		= Type;
	using Reference			= ValueType&;
	using ConstReference	= const Reference;
	using Pointer			= ValueType*;
	using ConstPointer		= const Pointer;

	using MapPtr 			= ValueType**;
	//using Alloc				= Allocator<ValueType>;

	MapPtr _Map				= nullptr;
	size_t _MapCapacity 	= 0;
	size_t _First 			= 0;
	size_t _Size 			= 0;

	static constexpr size_t BLOCK_SIZE = 4;

	size_t get_block(const size_t& offset) const noexcept {
		return (offset / BLOCK_SIZE) % _MapCapacity;
    }
};

template<class Deque>
class DequeConstIterator
{
public:
	using Data			= typename Deque::Data;
	using ValueType		= typename Deque::ValueType;
	using Reference		= typename Deque::ConstReference;
	using Pointer		= typename Deque::ConstPointer;

	size_t _Offset		= 0;
	const Data* _Data	= nullptr;

public:

    explicit DequeConstIterator(size_t offset, const Data* data) noexcept
		:_Offset(offset), _Data(data) { /*Empty*/ }

	DequeConstIterator& operator++() noexcept {
		CUSTOM_ASSERT(_Offset < _Data->_First + _Data->_Size, "Cannot increment end iterator...");
		++_Offset;
		return *this;
	}

	DequeConstIterator operator++(int) noexcept {
		DequeConstIterator temp = *this;
		++(*this);
		return temp;
	}

	DequeConstIterator& operator+=(const size_t& diff) noexcept {
		CUSTOM_ASSERT(	_Offset + diff >= _Data->_First &&
						_Offset + diff <= _Data->_First + _Data->_Size, 
						"Cannot increment end iterator...");
		_Offset += diff;
		return *this;
	}

	DequeConstIterator operator+(const size_t& diff) const noexcept {
		DequeConstIterator temp = *this;
		temp += diff;
		return temp;
	}

	DequeConstIterator& operator--() noexcept {
		CUSTOM_ASSERT(_Offset > _Data->_First, "Cannot decrement begin iterator...");
		--_Offset;
		return *this;
	}

	DequeConstIterator operator--(int) noexcept {
		DequeConstIterator temp = *this;
		--(*this);
		return temp;
	}

	DequeConstIterator& operator-=(const size_t& diff) noexcept {
		CUSTOM_ASSERT(	_Offset - diff >= _Data->_First &&					// TODO: use data overflow
						_Offset - diff <= _Data->_First + _Data->_Size,
						"Cannot decrement begin iterator...");
		_Offset -= diff;
		return *this;
	}

	DequeConstIterator operator-(const size_t& diff) const noexcept {
		DequeConstIterator temp = *this;
		temp -= diff;
		return temp;
	}

	Pointer operator->() const noexcept {
		CUSTOM_ASSERT(	_Offset >= _Data->_First &&
						_Offset < _Data->_First + _Data->_Size,
						"Cannot access end iterator...");

		size_t block = _Data->get_block(_Offset);
		size_t offset = _Offset % _Data->BLOCK_SIZE;
		return &_Data->_Map[block][offset];
	}

	Reference operator*() const noexcept {
		CUSTOM_ASSERT(	_Offset >= _Data->_First &&
						_Offset < _Data->_First + _Data->_Size,
						"Cannot dereference end iterator...");

		size_t block	= _Data->get_block(_Offset);
		size_t offset	= _Offset % _Data->BLOCK_SIZE;
		return _Data->_Map[block][offset];
	}

	bool operator==(const DequeConstIterator& other) const noexcept {
		return _Offset == other._Offset;
	}

	bool operator!=(const DequeConstIterator& other) const noexcept {
		return !(*this == other);
	}

public:

	size_t get_offset() const noexcept {
		return _Offset;
	}

	bool is_begin() const noexcept {
		return _Offset == _Data->_First;
	}

	bool is_end() const noexcept {
		return _Offset == _Data->_First + _Data->_Size;
	}
}; // END DequeConstIterator

template<class Deque>
class DequeIterator : public DequeConstIterator<Deque>			// Deque Iterator
{
private:
	using Base		= DequeConstIterator<Deque>;

public:
	using Data		= typename Deque::Data;
	using ValueType	= typename Deque::ValueType;
	using Reference	= typename Deque::Reference;
	using Pointer	= typename Deque::Pointer;

public:

	explicit DequeIterator(size_t offset, const Data* data) noexcept
		:Base(offset, data) { /*Empty*/ }

	DequeIterator& operator++() noexcept {
		Base::operator++();
		return *this;
	}

	DequeIterator operator++(int) noexcept {
		DequeIterator temp = *this;
		Base::operator++();
		return temp;
	}

	DequeIterator& operator+=(const size_t& diff) noexcept {
		Base::operator+=(diff);
		return *this;
	}

	DequeIterator operator+(const size_t& diff) const noexcept {
		DequeIterator temp = *this;
		temp += diff;
		return temp;
	}

	DequeIterator& operator--() noexcept {
		Base::operator--();
		return *this;
	}

	DequeIterator operator--(int) noexcept {
		DequeIterator temp = *this;
		Base::operator--();
		return temp;
	}

	DequeIterator& operator-=(const size_t& diff) noexcept {
		Base::operator-=(diff);
		return *this;
	}

	DequeIterator operator-(const size_t& diff) const noexcept {
		DequeIterator temp = *this;
		temp -= diff;
		return temp;
	}

	Pointer operator->() const noexcept {
		return const_cast<Pointer>(Base::operator->());
	}

	Reference operator*() const noexcept {
		return const_cast<Reference>(Base::operator*());
	}
}; // END Deque Iterator


template<class Type, class Alloc = custom::Allocator<Type>>
class Deque					// Deque Template implemented as map of blocks
{
public:
	using Data					= DequeData<Type>;
	using ValueType 			= typename Data::ValueType;					// Type for stored values
	using Reference				= typename Data::Reference;
	using ConstReference		= typename Data::ConstReference;
	using Pointer				= typename Data::Pointer;
	using ConstPointer			= typename Data::ConstPointer;
	using AllocatorType 		= Alloc;									// Allocator for block

	using Iterator				= DequeIterator<Deque<ValueType>>;			// Iterator type
	using ConstIterator			= DequeConstIterator<Deque<ValueType>>;		// Const Iterator type
	using ReverseIterator 		= custom::ReverseIterator<Iterator>;		// ReverseIterator type
	using ConstReverseIterator	= custom::ReverseIterator<ConstIterator>;	// Const Reverse Iterator type

private:
	Data _data;
	Alloc _alloc;

	static constexpr size_t _DEFAULT_CAPACITY = 8;

public:
	// Constructors

	Deque() {
		_init_map(_DEFAULT_CAPACITY);
	}

	Deque(const size_t& newSize) {
		resize(newSize);
	}

	Deque(const size_t& newSize, const ValueType& copyValue) {
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

	const ValueType& operator[](const size_t& index) const {
		CUSTOM_ASSERT(index < size(), "Index out of bounds...");
		return *(begin() + index);
	}

	ValueType& operator[](const size_t& index) {
		CUSTOM_ASSERT(index < size(), "Index out of bounds...");
		return *(begin() + index);
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

	void resize(const size_t& newSize) {
		while (_data._Size < newSize)
			emplace_back();

        while (_data._Size > newSize)
            pop_back();
	}

	void resize(const size_t& newSize, const ValueType& copyValue) {
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

		_alloc.construct(_data._Map[block] + backOffset % _data.BLOCK_SIZE, custom::forward<Args>(args)...);
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

			_alloc.destroy(_data._Map[block] + backOffset % _data.BLOCK_SIZE);
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

		_alloc.construct(_data._Map[block] + _data._First % _data.BLOCK_SIZE, custom::forward<Args>(args)...);
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

			_alloc.destroy(_data._Map[block] + _data._First % _data.BLOCK_SIZE);
			if (--_data._Size == 0)
				_data._First = 0;
			else
				++_data._First;
		}
	}

	template<class... Args>
	Iterator emplace(ConstIterator iterator, Args&&... args) {
		size_t off = iterator.get_offset() - _data._First;

		if (off <= _data._Size / 2)		// closer to front
		{
			emplace_front(custom::forward<Args>(args)...);
			
			Iterator current 	= begin() + off;
			ValueType val 		= custom::move(*begin());

			for (Iterator it = begin(); it != current; ++it)
				*it = custom::move(*(it + 1));

			*current = val;
		}
		else							// closer to back
		{
			emplace_back(custom::forward<Args>(args)...);

			Iterator current 	= begin() + off;
			ValueType val 		= custom::move(*--end());

			for (Iterator it = --end(); it != current; --it)
				*it = custom::move(*(it - 1));

			*current = val;
		}
		
		return begin() + off;
	}

	Iterator push(ConstIterator iterator, const ValueType& copyValue) {
		return emplace(iterator, copyValue);
	}

	Iterator push(ConstIterator iterator, ValueType&& moveValue) {
		return emplace(iterator, custom::move(moveValue));
	}

	Iterator pop(ConstIterator iterator) {
		if (iterator.is_end())
			throw std::out_of_range("Array pop iterator outside range...");
			
		size_t off = iterator.get_offset() - _data._First;

		if (off <= _data._Size / 2)		// closer to front
		{
			Iterator current 	= begin() + off;
			Iterator first 		= begin();

			for (Iterator it = current; it != first; --it)
				*it = custom::move(*(it - 1));

			pop_front();
		}
		else							// closer to back
		{
			Iterator current 	= begin() + off;
			Iterator last 		= --end();

			for (Iterator it = current; it != last; ++it)
				*it = custom::move(*(it + 1));
			
			pop_back();
		}

		return begin() + off;
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

	bool empty() const noexcept {
		return (_data._Size == 0);
	}

	ConstReference at(const size_t& index) const {
		if (index >= size())
			throw std::out_of_range("Index out of bounds...");

		return *(begin() + index);
	}

	Reference at(const size_t& index) {
		if (index >= size())
			throw std::out_of_range("Index out of bounds...");

		return *(begin() + index);
	}

	ConstReference front() const noexcept {
		CUSTOM_ASSERT(!empty(), "Container is empty...");
		return *begin();
	}

	Reference front() noexcept {										// Get the value of the first component
		CUSTOM_ASSERT(!empty(), "Container is empty...");
		return *begin();
	}

	ConstReference back() const noexcept {
		CUSTOM_ASSERT(!empty(), "Container is empty...");
		return *(--end());
	}

	Reference back() noexcept {                                                      	// Get the value of the last component
		CUSTOM_ASSERT(!empty(), "Container is empty...");
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
	// Iterator specific functions

	Iterator begin() noexcept {
		return Iterator(_data._First, &_data);
	}

	ConstIterator begin() const noexcept {
		return ConstIterator(_data._First, &_data);
	}

	ReverseIterator rbegin() noexcept {
		return ReverseIterator(end());
	}

	ConstReverseIterator rbegin() const noexcept {
		return ConstReverseIterator(end());
	}

	Iterator end() noexcept {
		return Iterator(_data._First + _data._Size, &_data);
	}

	ConstIterator end() const noexcept {
		return ConstIterator(_data._First + _data._Size, &_data);
	}

	ReverseIterator rend() noexcept {
		return ReverseIterator(begin());
	}

	ConstReverseIterator rend() const noexcept {
		return ConstReverseIterator(begin());
	}

private:
	// Helpers

	void _reserve(const size_t& newMapCapacity) {
		size_t newSize			= _data._Size;
		size_t newFirst			= _data._First % _data.BLOCK_SIZE;
		ValueType** newMap		= new ValueType* [newMapCapacity] {nullptr};

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

	void _init_map(const size_t& newMapCapacity) {
		size_t newCapacity 	= (newMapCapacity < _DEFAULT_CAPACITY) ? _DEFAULT_CAPACITY : newMapCapacity;

		_data._Map			= new ValueType* [newCapacity] {nullptr};
		_data._MapCapacity	= newCapacity;
		_data._Size			= 0;
		_data._First		= 0;
	}

	void _clean_up_map() {
		if (_data._Map != nullptr)
		{
			clear();
			delete[] _data._Map;
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