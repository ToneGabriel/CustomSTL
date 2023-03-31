#pragma once
#include "Allocator.h"
#include "Utility.h"
#include "Vector.h"


CUSTOM_BEGIN

template<class Type>
struct DequeData
{
	using ValueType 	= Type;
	using IterType		= ValueType;
	using Alloc			= Allocator<ValueType>;
	using AllocPtr		= Allocator<ValueType*>;

	static constexpr size_t BLOCK_SIZE = 4;

	ValueType** _Map	= nullptr;
	size_t _MapCapacity = 0;
	size_t _First 		= 0;
	size_t _Size 		= 0;

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
	using IterType		= typename Deque::IterType;
	using Reference		= const ValueType&;
	using Pointer		= const IterType*;

	size_t _Offset		= 0;
	const Data* _Data	= nullptr;

public:

    explicit DequeConstIterator(size_t offset, const Data* data)
		:_Offset(offset), _Data(data) { /*Empty*/ }

	DequeConstIterator& operator++() {
		if (_Offset >= _Data->_First + _Data->_Size)
			throw std::out_of_range("Cannot increment end iterator...");
		
		++_Offset;
		return *this;
	}

	DequeConstIterator operator++(int) {
		DequeConstIterator temp = *this;
		++(*this);
		return temp;
	}

	DequeConstIterator& operator+=(const size_t& diff) {
		if (_Offset + diff < _Data->_First || 
			_Offset + diff > _Data->_First + _Data->_Size)
			throw std::out_of_range("Cannot increment end iterator...");

		_Offset += diff;
		return *this;
	}

	DequeConstIterator operator+(const size_t& diff) const {
		DequeConstIterator temp = *this;
		temp += diff;
		return temp;
	}

	DequeConstIterator& operator--() {
		if (_Offset <= _Data->_First)
			throw std::out_of_range("Cannot decrement begin iterator...");

		--_Offset;
		return *this;
	}

	DequeConstIterator operator--(int) {
		DequeConstIterator temp = *this;
		--(*this);
		return temp;
	}

	DequeConstIterator& operator-=(const size_t& diff) {
		return *this;
	}

	DequeConstIterator operator-(const size_t& diff) const {
		DequeConstIterator temp = *this;
		temp -= diff;
		return temp;
	}

	Pointer operator->() const {
		if (_Offset < _Data->_First || _Offset >= _Data->_First + _Data->_Size)
			throw std::out_of_range("Cannot access end iterator...");

		size_t block = _Data->get_block(_Offset);
		size_t offset = _Offset % _Data->BLOCK_SIZE;
		return &_Data->_Map[block][offset];
	}

	Reference operator*() const {
		if (_Offset < _Data->_First || _Offset >= _Data->_First + _Data->_Size)
			throw std::out_of_range("Cannot dereference end iterator...");

		size_t block	= _Data->get_block(_Offset);
		size_t offset	= _Offset % _Data->BLOCK_SIZE;
		return _Data->_Map[block][offset];
	}

	bool operator==(const DequeConstIterator& other) const {
		return _Offset == other._Offset;
	}

	bool operator!=(const DequeConstIterator& other) const {
		return !(*this == other);
	}

public:

	const size_t get_index() const {
		return 0;	// TODO: implement
	}

	const bool is_begin() const {
		return _Offset == _Data->_First;
	}

	const bool is_end() const {
		return _Offset == _Data->_First + _Data->_Size;
	}
};

template<class Deque>
class DequeIterator : public DequeConstIterator<Deque>			// Deque Iterator
{
private:
	using Base		= DequeConstIterator<Deque>;

public:
	using Data		= typename Deque::Data;
	using ValueType	= typename Deque::ValueType;
	using IterType	= typename Deque::IterType;
	using Reference	= ValueType&;
	using Pointer	= IterType*;

public:

	explicit DequeIterator(size_t offset, const Data* data)
		:Base(offset, data) { /*Empty*/ }

	DequeIterator& operator++() {
		Base::operator++();
		return *this;
	}

	DequeIterator operator++(int) {
		DequeIterator temp = *this;
		Base::operator++();
		return temp;
	}

	DequeIterator& operator+=(const size_t& diff) {
		Base::operator+=(diff);
		return *this;
	}

	DequeIterator operator+(const size_t& diff) const {
		DequeIterator temp = *this;
		temp += diff;
		return temp;
	}

	DequeIterator& operator--() {
		Base::operator--();
		return *this;
	}

	DequeIterator operator--(int) {
		DequeIterator temp = *this;
		Base::operator--();
		return temp;
	}

	DequeIterator& operator-=(const size_t& diff) {
		Base::operator-=(diff);
		return *this;
	}

	DequeIterator operator-(const size_t& diff) const {
		DequeIterator temp = *this;
		temp -= diff;
		return temp;
	}

	Pointer operator->() const {
		return const_cast<Pointer>(Base::operator->());
	}

	Reference operator*() const {
		return const_cast<Reference>(Base::operator*());
	}
}; // END Deque Iterator


template<class Type>
class Deque					// Deque Template implemented using 2 vectors (front and back)
{
public:
	using Data					= DequeData<Type>;
	using ValueType 			= typename Data::ValueType;					// Type for stored values
	using IterType				= typename Data::IterType;					// Type for iteration (same as value)
	using Alloc					= typename Data::Alloc;						// Allocator for block
	using AllocPtr				= typename Data::AllocPtr;					// Allocator for map

	using Iterator				= DequeIterator<Deque<ValueType>>;			// Iterator type
	using ConstIterator			= DequeConstIterator<Deque<ValueType>>;		// Const Iterator type
	using ReverseIterator 		= custom::ReverseIterator<Iterator>;		// ReverseIterator type
	using ConstReverseIterator	= custom::ReverseIterator<ConstIterator>;	// Const Reverse Iterator type

private:
	Data _data;
	Alloc _alloc;

	static constexpr size_t _DEFAULT_CAPACITY = 2;	// TODO: change back to 8

public:
	// Constructors

	Deque() {
		_init_map(_DEFAULT_CAPACITY);
	}

	Deque(const ValueType& copyValue) {}

	Deque(const Deque& other) {
		_copy(other);
	}

	Deque(Deque&& other) {
		_move(custom::move(other));
	}

	~Deque() {
		_clean_up_map();
	}

public:
	// Main functions

	void resize(const size_t& newSize, const ValueType& copyValue = ValueType()) {
		// TODO: implement
	}

	template<class... Args>
	void emplace_back(Args&&... args) {
		_extend_if_full();
		
		size_t backOffset	= _data._First + _data._Size;
		size_t block		= _data.get_block(backOffset);

		if (_data._Map[block] == nullptr)
			_data._Map[block] = _alloc.alloc(_data.BLOCK_SIZE);

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
			_data._Map[block] = _alloc.alloc(_data.BLOCK_SIZE);

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
	Iterator emplace(const Iterator& iterator, Args&&... args) {
		// TODO: implement
	}

	Iterator push(const Iterator& iterator, const ValueType& copyValue) {
		return emplace(iterator, copyValue);
	}

	Iterator push(const Iterator& iterator, ValueType&& moveValue) {
		return emplace(iterator, custom::move(moveValue));
	}

	Iterator pop(const Iterator& iterator) {
		// TODO: implement
	}

	const size_t size() const {
		return _data._Size;
	}

	bool empty() const {
		return (_data._Size == 0);
	}

	void clear() {
		while(!empty())
			pop_back();

		for (size_t i = 0; i < _data._MapCapacity; ++i)
			if (_data._Map[i] != nullptr)
			{
				_alloc.dealloc(_data._Map[i], _data.BLOCK_SIZE);
				_data._Map[i] = nullptr;
			}
	}

	const ValueType& at(const size_t& index) const {
		if (index >= size())
			throw std::out_of_range("Invalid Index...");

		return *(begin() + index);
	}

	ValueType& at(const size_t& index) {
		if (index >= size())
			throw std::out_of_range("Invalid Index...");

		return *(begin() + index);
	}

	const ValueType& front() const {
		assert(!empty());
		return *begin();
	}

	ValueType& front() {                                                     	// Get the value of the first component
		assert(!empty());
		return *begin();
	}

	const ValueType& back() const {
		assert(!empty());
		return *(--end());
	}

	ValueType& back() {                                                      	// Get the value of the last component
		assert(!empty());
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
	// Operators

	const ValueType& operator[](const size_t& index) const {
		assert(!(index >= size()));
		return *(begin() + index);
	}

	ValueType& operator[](const size_t& index) {
		assert(!(index >= size()));
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

	bool operator==(const Deque& other) const {
		if (size() != other.size())
			return false;

		auto it1 = begin();
		auto it2 = other.begin();
		while (it1 != end())
			if (*(it1++) != *(it2++))
				return false;

		return true;
	}

	bool operator!=(const Deque& other) const {
		return !(*this == other);
	}

public:
	// Iterator specific functions

	Iterator begin() {
		return Iterator(_data._First, &_data);
	}

	ConstIterator begin() const {
		return ConstIterator(_data._First, &_data);
	}

	ReverseIterator rbegin() {
		return ReverseIterator(end());
	}

	ConstReverseIterator rbegin() const {
		return ConstReverseIterator(end());
	}

	Iterator end() {
		return Iterator(_data._First + _data._Size, &_data);
	}

	ConstIterator end() const {
		return ConstIterator(_data._First + _data._Size, &_data);
	}

	ReverseIterator rend() {
		return ReverseIterator(begin());
	}

	ConstReverseIterator rend() const {
		return ConstReverseIterator(begin());
	}

private:
	// Helpers

	void _reserve(const size_t& newMapCapacity) {
		size_t capacity 	= (newMapCapacity < _DEFAULT_CAPACITY) ? _DEFAULT_CAPACITY : newMapCapacity;
		ValueType** newMap	= new ValueType* [newMapCapacity] {nullptr};

		for (size_t i = 0; i < _data._Size; ++i)
			
		// TODO: finish this
	}

	void _extend_if_full() {
		if (_data._Size == _data._MapCapacity * _data.BLOCK_SIZE)
			_reserve(2 * _data._MapCapacity);
	}

	void _copy(const Deque& other) {
		_init_map(other._data._MapCapacity);

		for (auto& val : other)
			push_back(val);
	}

	void _move(Deque&& other) {
		_data._Map 					= other._data._Map;
		_data._MapCapacity 			= other._data._MapCapacity;
		_data._Size 				= other._data._Size;
		_data._First 				= other._data._First;

		other._data._Map 			= nullptr;
		other._data._MapCapacity 	= 0;
		other._data._Size 			= 0;
		other._data._First 			= 0;
	}

	_init_map(const ValueType& newMapCapacity) {
		size_t capacity 	= (newMapCapacity < _DEFAULT_CAPACITY) ? _DEFAULT_CAPACITY : newMapCapacity;

		_data._Map			= new ValueType* [capacity] {nullptr};
		_data._MapCapacity	= capacity;
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

CUSTOM_END