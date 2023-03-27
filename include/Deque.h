#pragma once
#include "Allocator.h"
#include "Utility.h"
#include "Vector.h"


CUSTOM_BEGIN

template<class Deque>
class DequeConstIterator
{
public:
	using ValueType		= typename Deque::ValueType;
	using IterType		= typename Deque::IterType;

	IterType* _Ptr					= nullptr;
	const Vector<ValueType>* _Front = nullptr;
	const Vector<ValueType>* _Back	= nullptr;

public:

    explicit DequeConstIterator(IterType* ptr, const Vector<ValueType>* front, const Vector<ValueType>* back)
		:_Ptr(ptr), _Front(front), _Back(back) { /*Empty*/ }

	DequeConstIterator& operator++() {
		if (_Ptr == _Back->end()._Ptr)
			throw std::out_of_range("Cannot increment end iterator...");

		if (_Ptr < _Front->end()._Ptr)
			--_Ptr;
		else if (_Ptr == _Front->end()._Ptr - 1)
			_Ptr = _Back->begin()._Ptr;
        else
            ++_Ptr;

		return *this;
	}

	DequeConstIterator operator++(int) {
		DequeConstIterator temp = *this;
		++(*this);
		return temp;
	}

	DequeConstIterator& operator+=(const size_t& diff) {
		// if(get_pos() + diff > _Data->_Size)
		// 	throw std::out_of_range("Cannot increment end iterator...");

		// _Data->increment(_Ptr, diff);
		// return *this;
	}

	DequeConstIterator operator+(const size_t& diff) const {
		DequeConstIterator temp = *this;
		temp += diff;
		return temp;
	}

	DequeConstIterator& operator--() {
		// if (_Ptr == _Data->_Begin)
		// 	throw std::out_of_range("Cannot decrement begin iterator...");

		// _Data->decrement(_Ptr);
		// return *this;
	}

	DequeConstIterator operator--(int) {
		DequeConstIterator temp = *this;
		--(*this);
		return temp;
	}

	DequeConstIterator& operator-=(const size_t& diff) {
		// if(get_pos() < diff)
		// 	throw std::out_of_range("Cannot decrement begin iterator...");

		// _Data->decrement(_Ptr, diff);
		// return *this;
	}

	DequeConstIterator operator-(const size_t& diff) const {
		DequeConstIterator temp = *this;
		temp -= diff;
		return temp;
	}

	const IterType* operator->() const {
		if (_Ptr == _Back->end()._Ptr)
			throw std::out_of_range("Cannot access end iterator...");

		return _Ptr;
	}

	const ValueType& operator*() const {
		if (_Ptr == _Back->end()._Ptr)
			throw std::out_of_range("Cannot dereference end iterator...");

		return *_Ptr;
}

	bool operator==(const DequeConstIterator& other) const {
		return _Ptr == other._Ptr;
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
	using ValueType	= typename Deque::ValueType;
	using IterType	= typename Deque::IterType;

public:

	explicit DequeIterator(IterType* ptr, const Vector<ValueType>* front, const Vector<ValueType>* back)
		:Base(ptr, front, back) { /*Empty*/ }

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
	using ValueType 			= Type;										// Type for stored values
	using IterType				= ValueType;								// Type for iteration (same as value)
	
	using Iterator				= DequeIterator<Deque<ValueType>>;			// Iterator type
	using ConstIterator			= DequeConstIterator<Deque<ValueType>>;		// Const Iterator type
	using ReverseIterator 		= custom::ReverseIterator<Iterator>;		// ReverseIterator type
	using ConstReverseIterator	= custom::ReverseIterator<ConstIterator>;	// Const Reverse Iterator type

private:
	Vector<ValueType> _FrontContainer;
	Vector<ValueType> _BackContainer;

public:
	// Constructors

	Deque() = default;

	Deque(const ValueType& copyValue)
		:	_FrontContainer(Vector<ValueType>::default_capacity / 2, copyValue),
			_BackContainer(Vector<ValueType>::default_capacity / 2, copyValue) { /*Empty*/ }

	Deque(const Deque& other)
		:	_FrontContainer(other._FrontContainer),
			_BackContainer(other._BackContainer) { /*Empty*/ }

	Deque(Deque&& other)
		:	_FrontContainer(custom::move(other._FrontContainer)),
			_BackContainer(custom::move(other._BackContainer)) { /*Empty*/ }

	~Deque() = default;

public:
	// Main functions

	void resize(const size_t& newSize, const ValueType& copyValue = ValueType()) {
		// TODO: implement
	}

	template<class... Args>
	void emplace_back(Args&&... args) {
		_BackContainer.emplace_back(custom::forward<Args>(args)...);
	}

	void push_back(const ValueType& copyValue) {
		_BackContainer.push_back(copyValue);
	}

	void push_back(ValueType&& moveValue) {
		_BackContainer.push_back(custom::move(moveValue));
	}

	void pop_back() {
		_BackContainer.pop_back();
	}

	template<class... Args>
	void emplace_front(Args&&... args) {
		_FrontContainer.emplace_back(custom::forward<Args>(args)...);
	}

	void push_front(const ValueType& copyValue) {
		_FrontContainer.push_back(copyValue);
	}

	void push_front(ValueType&& moveValue) {
		_FrontContainer.push_back(custom::move(moveValue));
	}

	void pop_front() {
		_FrontContainer.pop_back();
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
		return _FrontContainer.size() + _BackContainer.size();
	}

	bool empty() const {
		return (_FrontContainer.empty() && _BackContainer.empty());
	}

	void clear() {
		_FrontContainer.clear();
		_BackContainer.clear();
	}

	const ValueType& at(const size_t& index) const {

	}

	ValueType& at(const size_t& index) {

	}

	const ValueType& front() const {
		assert(!empty());
		return;
	}

	ValueType& front() {                                                     	// Get the value of the first component
		assert(!empty());
		return;
	}

	const ValueType& back() const {
		assert(!empty());
		return;
	}

	ValueType& back() {                                                      	// Get the value of the last component
		assert(!empty());
		return;
	}

	void print_details() {
		std::cout << "Size= " << size() << '\n';
		for (auto it = _FrontContainer.rbegin(); it != _FrontContainer.rend(); ++it)
			std::cout << *it << ' ';
		std::cout << '\n';

		for (auto it = _BackContainer.begin(); it != _BackContainer.end(); ++it)
			std::cout << *it << ' ';
		std::cout << '\n';
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
		return Iterator(_FrontContainer.data() + _FrontContainer.size() - 1, &_FrontContainer, &_BackContainer);
	}

	ConstIterator begin() const {
		return ConstIterator(_FrontContainer.data() + _FrontContainer.size() - 1, &_FrontContainer, &_BackContainer);
	}

	ReverseIterator rbegin() {
		return ReverseIterator(end());
	}

	ConstReverseIterator rbegin() const {
		return ConstReverseIterator(end());
	}

	Iterator end() {
		return Iterator(_BackContainer.data() + _BackContainer.size(), &_FrontContainer, &_BackContainer);
	}

	ConstIterator end() const {
		return ConstIterator(_BackContainer.data() + _BackContainer.size(), &_FrontContainer, &_BackContainer);
	}

	ReverseIterator rend() {
		return ReverseIterator(begin());
	}

	ConstReverseIterator rend() const {
		return ConstReverseIterator(begin());
	}
}; // END Deque Template

CUSTOM_END