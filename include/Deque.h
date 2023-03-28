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

	static constexpr size_t block_size = 4;

	ValueType** _Map;
	size_t _MapSize 	= 0;
	size_t _InitOffset 	= 0;
	size_t _Size 		= 0;

	size_t get_block(const size_t& offset) const noexcept {
		return (offset / block_size) % _MapSize;
    }
};

template<class Deque>
class DequeConstIterator
{
public:
	using Data			= typename Deque::Data;
	using ValueType		= typename Deque::ValueType;
	using IterType		= typename Deque::IterType;

	size_t _Offset		= 0;
	const Data* _Data	= nullptr;

public:

    explicit DequeConstIterator(size_t offset, const Data* data)
		:_Offset(offset), _Data(data) { /*Empty*/ }

	DequeConstIterator& operator++() {


		return *this;
	}

	DequeConstIterator operator++(int) {
		DequeConstIterator temp = *this;
		++(*this);
		return temp;
	}

	DequeConstIterator& operator+=(const size_t& diff) {
		return *this;
	}

	DequeConstIterator operator+(const size_t& diff) const {
		DequeConstIterator temp = *this;
		temp += diff;
		return temp;
	}

	DequeConstIterator& operator--() {
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

	const IterType* operator->() const {

	}

	const ValueType& operator*() const {

	}

	bool operator==(const DequeConstIterator& other) const {
		return _Offset == other._Offset;
	}

	bool operator!=(const DequeConstIterator& other) const {
		return !(*this == other);
	}

public:

	const size_t get_index() const {						// Get the index for the element in array from iterator (relative to array address, NOT begin)
		//return static_cast<size_t>(_Ptr - _Data->_Base);
	}

	const bool is_begin() const {
		//return _Ptr == _Data->_Front;
	}

	const bool is_end() const {
		//return _Ptr == _Data->_End;
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

	IterType* operator->() const {
		return const_cast<IterType*>(Base::operator->());
	}

	ValueType& operator*() const {
		return const_cast<ValueType&>(Base::operator*());
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

	static constexpr size_t default_capacity = 8;

public:
	// Constructors

	Deque() {
		AllocPtr allMap;

		//_data._Map			= new ValueType* [default_capacity] {nullptr};

		_data._Map			= allMap.alloc(default_capacity);
		allMap.construct_range(_data._Map, default_capacity, nullptr);

		_data._MapSize		= default_capacity;
		_data._Size			= 0;
		_data._InitOffset	= default_capacity / 2 * _data.block_size - 1;
	}

	Deque(const ValueType& copyValue) {}

	Deque(const Deque& other) {}

	Deque(Deque&& other) {}

	~Deque() {
		//delete[] _data._Map;

		AllocPtr allMap;
		allMap.dealloc(_data._Map, default_capacity);
	}

public:
	// Main functions

	void reserve(const size_t& newMapSize) {
		// TODO: implement
		// update _InitOffset
	}

	void resize(const size_t& newSize, const ValueType& copyValue = ValueType()) {
		// TODO: implement
	}

	template<class... Args>
	void emplace_back(Args&&... args) {
		extend_if_full();
		
		size_t backOffset	= _data._InitOffset + _data._Size;
		size_t block		= _data.get_block(backOffset);

		if (_data._Map[block] == nullptr)
			_data._Map[block] = _alloc.alloc(_data.block_size);

		_alloc.construct(_data._Map[block] + backOffset % _data.block_size, custom::forward<Args>(args)...);
		++_data._Size;
	}

	void push_back(const ValueType& copyValue) {
		emplace_back(copyValue);
	}

	void push_back(ValueType&& moveValue) {
		emplace_back(custom::move(moveValue));
	}

	void pop_back() {
		
	}

	template<class... Args>
	void emplace_front(Args&&... args) {
		extend_if_full();

		// TODO: implement circularr offset
		size_t block = _data.get_block(--_data._InitOffset);
		if (_data._Map[block] == nullptr)
			_data._Map[block] = _alloc.alloc(_data.block_size);

		_alloc.construct(_data._Map[block] + _data._InitOffset % _data.block_size, custom::forward<Args>(args)...);
		++_data._Size;
	}

	void push_front(const ValueType& copyValue) {
		emplace_front(copyValue);
	}

	void push_front(ValueType&& moveValue) {
		emplace_front(custom::move(moveValue));
	}

	void pop_front() {

	}

	template<class... Args>
	Iterator emplace(const Iterator& iterator, Args&&... args) {

	}

	Iterator push(const Iterator& iterator, const ValueType& copyValue) {
		return emplace(iterator, copyValue);
	}

	Iterator push(const Iterator& iterator, ValueType&& moveValue) {
		return emplace(iterator, custom::move(moveValue));
	}

	Iterator pop(const Iterator& iterator) {
	
	}

	const size_t size() const {
		return _data._Size;
	}

	bool empty() const {
		return (_data._Size == 0);
	}

	void clear() {

	}

	const ValueType& at(const size_t& index) const {

	}

	ValueType& at(const size_t& index) {

	}

	// const ValueType& front() const {
	// 	assert(!empty());
	// 	return;
	// }

	// ValueType& front() {                                                     	// Get the value of the first component
	// 	assert(!empty());
	// 	return;
	// }

	// const ValueType& back() const {
	// 	assert(!empty());
	// 	return;
	// }

	// ValueType& back() {                                                      	// Get the value of the last component
	// 	assert(!empty());
	// 	return;
	// }

	void print_details() {
		std::cout << "Map Size= " << _data._MapSize << '\n';
		std::cout << "Size= " << _data._Size << '\n';
		std::cout << "Init Offset= " << _data._InitOffset << '\n';

		for (size_t i = 0; i < _data._MapSize; ++i)
		{
			std::cout << "Block " << i << " = ";
			if (_data._Map[i] == nullptr)
				std::cout << "NULL\n";
			else
			{
				for (size_t j = 0; j < _data.block_size; ++j)
					std::cout << _data._Map[i][j] << ' ';
				std::cout << '\n';
			}
		}
	}

public:
	// Operators

	const ValueType& operator[](const size_t& index) const {
	
	}

	ValueType& operator[](const size_t& index) {

	}

	Deque& operator=(const Deque& other) {
		return *this;
	}

	Deque& operator=(Deque&& other) noexcept {
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
		
	}

	ConstIterator begin() const {

	}

	ReverseIterator rbegin() {
		return ReverseIterator(end());
	}

	ConstReverseIterator rbegin() const {
		return ConstReverseIterator(end());
	}

	Iterator end() {

	}

	ConstIterator end() const {

	}

	ReverseIterator rend() {
		return ReverseIterator(begin());
	}

	ConstReverseIterator rend() const {
		return ConstReverseIterator(begin());
	}

private:
	// Helpers

	void extend_if_full() {
		if (_data._InitOffset == 0 || _data._InitOffset == _data._MapSize * _data.block_size - 1)
			reserve(2 * _data._MapSize);
	}

	void _copy(const Deque& other) {

	}

	void _move(Deque&& other) {

	}

	void _clear_map() {

	}

}; // END Deque Template

CUSTOM_END