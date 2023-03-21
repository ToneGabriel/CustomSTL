#pragma once
#include "Allocator.h"
#include "Utility.h"

#include <iostream>
#include <cstring>


CUSTOM_BEGIN

struct StringData
{
	using ValueType		= char;									// Type for stored values
	using IterType		= ValueType;							// Type for iteration (same as value)
	using Alloc			= Allocator<ValueType>;					// Allocator type

	ValueType* _First	= nullptr;								// Actual container array
	ValueType* _Last	= nullptr;								// Pointer to end
	ValueType* _Final	= nullptr;								// Pointer to capacity end
};

template<class String>
class StringConstIterator
{
public:
	using Data			= typename String::Data;
	using ValueType		= typename String::ValueType;
	using IterType		= typename String::IterType;

	IterType* _Ptr		= nullptr;
	const Data* _Data	= nullptr;

public:

	explicit StringConstIterator(IterType* ptr, const Data* data) noexcept
		:_Ptr(ptr), _Data(data) { /*Empty*/ }

	StringConstIterator& operator++() {
		if (_Ptr >= _Data->_Last)
			throw std::out_of_range("Cannot increment end iterator...");

		++_Ptr;
		return *this;
	}

	StringConstIterator operator++(int) {
		StringConstIterator temp = *this;
		++(*this);
		return temp;
	}

	StringConstIterator& operator+=(const size_t& diff) {
		if (_Ptr + diff >= _Data->_Last)
			throw std::out_of_range("Cannot increment end iterator...");

		_Ptr += diff;
		return *this;
	}

	StringConstIterator operator+(const size_t& diff) const {
		StringConstIterator temp = *this;
		temp += diff;
		return temp;
	}

	StringConstIterator& operator--() {
		if (_Ptr <= _Data->_First)
			throw std::out_of_range("Cannot decrement begin iterator...");

		--_Ptr;
		return *this;
	}

	StringConstIterator operator--(int) {
		StringConstIterator temp = *this;
		--(*this);
		return temp;
	}

	StringConstIterator& operator-=(const size_t& diff) {
		if (_Ptr - diff <= _Data->_First)
			throw std::out_of_range("Cannot decrement begin iterator...");

		_Ptr -= diff;
		return *this;
	}

	StringConstIterator operator-(const size_t& diff) const {
		StringConstIterator temp = *this;
		temp -= diff;
		return temp;
	}

	const IterType* operator->() const {
		if (_Ptr >= _Data->_Last)
			throw std::out_of_range("Cannot access end iterator...");

		return _Ptr;
	}

	const ValueType& operator*() const {
		if (_Ptr >= _Data->_Last)
			throw std::out_of_range("Cannot dereference end iterator...");

		return *_Ptr;
	}

	bool operator==(const StringConstIterator& other) const {
		return _Ptr == other._Ptr;
	}

	bool operator!=(const StringConstIterator& other) const {
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
};

template<class String>
class StringIterator : public StringConstIterator<String>		// String Iterator
{
private:
	using Base		= StringConstIterator<String>;

public:
	using Data		= typename String::Data;
	using ValueType = typename String::ValueType;
	using IterType	= typename String::IterType;

public:

	explicit StringIterator(IterType* ptr, const Data* data)
		:Base(ptr, data) { /*Empty*/ }

	StringIterator& operator++() {
		Base::operator++();
		return *this;
	}

	StringIterator operator++(int) {
		StringIterator temp = *this;
		Base::operator++();
		return temp;
	}

	StringIterator& operator+=(const size_t& diff) {
		Base::operator+=(diff);
		return *this;
	}

	StringIterator operator+(const size_t& diff) const {
		StringIterator temp = *this;
		temp += diff;
		return temp;
	}

	StringIterator& operator--() {
		Base::operator--();
		return *this;
	}

	StringIterator operator--(int) {
		StringIterator temp = *this;
		Base::operator--();
		return temp;
	}

	StringIterator& operator-=(const size_t& diff) {
		Base::operator-=(diff);
		return *this;
	}

	StringIterator operator-(const size_t& diff) const {
		StringIterator temp = *this;
		temp -= diff;
		return temp;
	}

	IterType* operator->() {
		return const_cast<IterType*>(Base::operator->());
	}

	ValueType& operator*() {
		return const_cast<ValueType&>(Base::operator*());
	}
}; // END String Iterator


class String	// String container
{
public:
	using Data					= StringData;								// Members that are modified
	using ValueType 			= typename Data::ValueType;					// Type for stored values
	using IterType				= typename Data::IterType;					// Type for iteration (same as value)
	using Alloc					= typename Data::Alloc;						// Allocator type
	
	using Iterator				= StringIterator<String>;					// Iterator type
	using ConstIterator			= StringConstIterator<String>;				// Const Iterator type
	using ReverseIterator 		= custom::ReverseIterator<Iterator>;		// ReverseIterator type
	using ConstReverseIterator	= custom::ReverseIterator<ConstIterator>;	// Const Reverse Iterator type

	static constexpr size_t npos = static_cast<size_t>(-1);

private:
	Data _data;
	Alloc _alloc;

	static constexpr size_t istream_capacity	= 255;
	static constexpr char NULLCHR 				= '\0';

public:

	String() {
		_initialize_from_cstring(nullptr);
	}

	String(const char* cstring) {
		_initialize_from_cstring(cstring);
	}

	String(const size_t& newCapacity) {
		_alloc_empty(newCapacity);
	}

	String(const String& other) {
		_copy(other);
	}

	String(String&& other) noexcept {
		_move(custom::move(other));
	}

	~String() {
		_dealloc_string(_data._First, capacity());
	}

public:
	// Main functions

	void reserve(const size_t& newCapacity) {			// Allocate memory and copy values if needed
		if (newCapacity < size())						// Can also shrink
			_data._Last = _data._First + newCapacity;

		size_t newSize	= size();
		char* newString = _alloc_string(newCapacity);
		memcpy(newString, _data._First, size());
		_dealloc_string(_data._First, capacity());

		_data._First 	= newString;
		_data._Last		= _data._First + newSize;
		_data._Final 	= _data._First + newCapacity;
		_data._Last[0] 	= NULLCHR;
	}

	void shrink_to_fit() {								// Allocate memory with capacity equal to size and move values there
		reserve(size());
	}

	void push_back(const char& chr) {
		_extend_if_full();
		*(_data._Last++)	= chr;
		_data._Last[0] 		= NULLCHR;
	}

	void pop_back() {
		*(--_data._Last) = NULLCHR;
	}

	// Append function overload
	String& append(const String& string) {												// Appends a copy of string
		_insert_from_cstring(size(), string._data._First, 0, string.size());
		return *this;
	}

	String& append(const String& string, const size_t& subpos, const size_t& sublen) {	// Appends a copy of a substring of string.
		_insert_from_cstring(size(), string._data._First, subpos, sublen);
		return *this;
	}

	String& append(const char* cstring) {												// Appends a copy of a c-string.
		_insert_from_cstring(size(), cstring, 0, strlen(cstring));
		return *this;
	}

	String& append(const char* cstring, const size_t nchar) {							// Appends a copy of the first n characters in the c-string
		_insert_from_cstring(size(), cstring, 0, nchar);
		return *this;
	}

	String& append(const size_t& nchar, const char& chr) {								// Appends n consecutive copies of character c
		char* seq = _alloc_string(nchar);
		for (size_t i = 0; i < nchar; ++i)
			seq[i] = chr;

		_insert_from_cstring(size(), seq, 0, nchar);
		_dealloc_string(seq, nchar);
		return *this;
	}
	// end Append

	// Insert function overload
	String& insert(const size_t& pos, const String& string) {
		_insert_from_cstring(pos, string._data._First, 0, string.size());
		return *this;
	}

	String& insert(const size_t& pos, const String& string, const size_t& subpos, const size_t& sublen) {
		_insert_from_cstring(pos, string._data._First, subpos, sublen);
		return *this;	
	}

	String& insert(const size_t& pos, const char* cstring) {
		_insert_from_cstring(pos, cstring, 0, strlen(cstring));
		return *this;
	}

	String& insert(const size_t& pos, const char* cstring, const size_t& len) {
		_insert_from_cstring(pos, cstring, 0, len);
		return *this;
	}

	String& insert(const size_t& pos, const size_t& nchar, const char& chr) {
		char* seq = _alloc_string(nchar);
		for (size_t i = 0; i < nchar; ++i)
			seq[i] = chr;

		_insert_from_cstring(pos, seq, 0, nchar);
		_dealloc_string(seq, nchar);
		return *this;
	}

	Iterator insert(const Iterator& where, const char& chr) {
		size_t pos = where.get_index();
		_insert_from_cstring(pos, &chr, 0, 1);

		return Iterator(_data._First + pos, &_data);
	}

	// Iterator insert(const Iterator& where, const Iterator& first, const Iterator& last) {
	// 	if (where._Data->_Begin == first._Data->_Begin ||
	// 		first._Data->_Begin != last._Data->_Begin)	// Check if pos string != first/last string
	// 		throw std::domain_error("String provided by first and last must be the same, but different from the one provided by where");

	// 	size_t pos 			= where.get_index();
	// 	size_t posFrom 		= first.get_index();
	// 	size_t posTo 		= last.get_index();
	// 	const char* cstring = first._Data->_Begin;
	// 	_insert_from_cstring(pos, cstring, posFrom, posTo - posFrom);

	// 	return Iterator(_data._First + pos, &_data);
	// }
	// end Insert

	// Erase function overload
	String& erase(const size_t& pos, const size_t& len) {
		_remove_from_cstring(pos, len);
		return *this;
	}

	Iterator erase(const Iterator& where) {
		if (where.is_end())
			throw std::out_of_range("String erase iterator outside range...");

		size_t pos = where.get_index();
		_remove_from_cstring(pos, 1);

		return Iterator(_data._First + pos, &_data);
	}

	Iterator erase(const Iterator& first, const Iterator& last) {
		if (first.is_end())
			throw std::out_of_range("String erase iterator outside range...");

		size_t posFrom	= first.get_index();
		size_t posTo	= last.get_index();
		_remove_from_cstring(posFrom, posTo - posFrom);

		return Iterator(_data._First + posFrom, &_data);
	}
	// end Erase

	// Find function overload
	size_t find(const String& string, const size_t& pos) const {
		return _search_for_cstring(string._data._First, pos, string.size());
	}

	size_t find(const char* cstring, const size_t& pos) const {
		return _search_for_cstring(cstring, pos, strlen(cstring));
	}

	size_t find(const char* cstring, const size_t& pos, const size_t& len) const {
		return _search_for_cstring(cstring, pos, len);
	}

	size_t find(const char& chr, const size_t& pos) const {
		return _search_for_cstring(&chr, pos, 1);
	}
	// end Find

	// Compare function overload
	int compare(const String& string) const {
		return _compare_with_cstring(0, size(), string._data._First, 0, string.size());
	}

	int compare(const size_t& pos, const size_t& len, const String& string, const size_t& subpos, const size_t& sublen) const {
		return _compare_with_cstring(pos, len, string._data._First, subpos, sublen);
	}

	int compare(const char* cstring) const {
		return _compare_with_cstring(0, size(), cstring, 0, strlen(cstring));
	}

	int compare(const size_t& pos, const size_t& len, const char* cstring, const size_t& subpos, const size_t& sublen) const {
		return _compare_with_cstring(pos, len, cstring, subpos, sublen);
	}
	// end Compare

	const char* c_str() const {
		return _data._First;
	}

	const size_t capacity() const {
		return static_cast<size_t>(_data._Final - _data._First);
	}

	const size_t size() const {
		return static_cast<size_t>(_data._Last - _data._First);
	}

	const bool empty() const {
		return (_data._First == _data._Last);
	}

	void clear() {
		_data._Last 	= _data._First;
		_data._First[0] = NULLCHR;
	}

	const char& at(const size_t& index) const {						// Acces char at index with check (read only)
		if (index >= size())
			throw std::out_of_range("Invalid Index...");

		return _data._First[index];
	}

	char& at(const size_t& index) {									// Acces char at index with check
		if (index >= size())
			throw std::out_of_range("Invalid Index...");

		return _data._First[index];
	}

	const char& front() const {
		assert(!empty());
		return _data._First[0];
	}

	char& front() {													// Get the value of the first component
		assert(!empty());
		return _data._First[0];
	}

	const char& back() const {
		assert(!empty());
		return _data._Last[-1];
	}

	char& back() {													// Get the value of the last component
		assert(!empty());
		return _data._Last[-1];
	}

	String substr(const size_t& pos, const size_t& len) const {		// Create substring from current string
		if (pos > size())
			throw std::out_of_range("Invalid starting position...");
		if (len + pos > size())
			throw std::out_of_range("Invalid length...");

		String sub(len);	// empty string with _capacity = len
		memcpy(sub._data._First, _data._First + pos, len);
		sub._data._Last		= sub._data._Final;
		sub._data._Last[0]	= NULLCHR;

		return sub;
	}

	void print_details() const {									// For Debugging
		size_t currentSize 		= size();
		size_t currentCapacity 	= capacity();

		std::cout << '\n';
		for (size_t i = 0; i <= currentCapacity; ++i)
		{
			if (_data._First[i] == NULLCHR)
				std::cout << 'N' << ' ';
			else
				std::cout << _data._First[i] << ' ';
		}
		std::cout << '\n';
		std::cout << currentSize << ' ' << currentCapacity << '\n';
	}

public:
	// Operators

	const char& operator[](const size_t& index) const {
		assert(!(index >= size()));
		return _data._First[index];
	}

	char& operator[](const size_t& index) {
		assert(!(index >= size()));
		return _data._First[index];
	}

	String& operator=(const String& other) {
		if (_data._First != other._data._First)
		{
			_dealloc_string(_data._First, capacity());
			_copy(other);
		}

		return *this;
	}

	String& operator=(String&& other) noexcept {
		if (_data._First != other._data._First)
		{
			_dealloc_string(_data._First, capacity());
			_move(custom::move(other));
		}

		return *this;
	}

	String& operator+=(const String& string) {
		append(string);
		return *this;
	}

	String& operator+=(const char* cstring) {
		append(cstring);
		return *this;
	}

	String& operator+=(const char& chr) {
		push_back(chr);
		return *this;
	}

	bool operator==(const String& other) const {
		return compare(other) == 0;
	}

	bool operator!=(const String& other) const {
		return !(*this == other);
	}

	friend std::ostream& operator<<(std::ostream& os, const String& string) {
		os << string._data._First;
		return os;
	}

	friend std::istream& operator>>(std::istream& is, String& string) {
		if (string._data._First != nullptr)
			string._dealloc_string(string._data._First, string.capacity());

		string._alloc_empty(String::istream_capacity);
		is.getline(string._data._First, String::istream_capacity);
		string._data._Last = string._data._First + strlen(string._data._First);
		return is;
	}

public:
	// Iterator specific

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
		return Iterator(_data._Last, &_data);
	}

	ConstIterator end() const {
		return Iterator(_data._Last, &_data);
	}

	ReverseIterator rend() {
		return ReverseIterator(begin());
	}

	ConstReverseIterator rend() const {
		return ConstReverseIterator(begin());
	}

private:
	// Others

	char* _alloc_string(const size_t& capacity) {						// Allocate memory +1 null term
		return _alloc.alloc(capacity + 1);
	}

	void _dealloc_string(char* address, const size_t& capacity) {		// Deallocate memory +1 null term
		_alloc.dealloc(address, capacity + 1);
	}

	void _alloc_empty(const size_t& capacity) {
		_data._First 	= _alloc_string(capacity);
		_data._Last 	= _data._First;
		_data._Final 	= _data._First + capacity;
		_data._Last[0] 	= NULLCHR;
	}

	void _initialize_from_cstring(const char* cstring) {
		if (cstring == nullptr) {
			_data._First 	= _alloc_string(0);
			_data._Last 	= _data._First;
			_data._Final 	= _data._First;
		}
		else {
			size_t len 		= strlen(cstring);
			_data._First 	= _alloc_string(len);
			_data._Last 	= _data._First + len;
			_data._Final 	= _data._First + len;
			memcpy(_data._First, cstring, len);
		}

		_data._Last[0] = NULLCHR;
	}

	void _insert_from_cstring(const size_t& pos, const char* cstring, const size_t& subpos, const size_t& sublen) {
		if (pos > size())		// pos = start pos
			throw std::out_of_range("Invalid starting position...");
		if (subpos + sublen > strlen(cstring))
			throw std::out_of_range("Invalid length...");

		size_t newSize = size() + sublen;

		if (newSize > capacity())
			reserve(newSize);

		memcpy(_data._First + pos + sublen, _data._First + pos, size() - pos);	// copy last chars "sublen" positions to right
		memcpy(_data._First + pos, cstring + subpos, sublen);			// add substr from cstring between
		_data._Last 	= _data._First + newSize;
		_data._Last[0] 	= NULLCHR;
	}

	void _remove_from_cstring(const size_t& pos, const size_t& len) {	// TODO: check
		if (pos > size())		// pos = start pos
			throw std::out_of_range("Invalid starting position...");
		if (pos + len > size())
			throw std::out_of_range("Invalid length...");

		memcpy(_data._First + pos, _data._First + pos + len, size() - pos - len);
		_data._Last 	= _data._First + size() - len;
		_data._Last[0] 	= NULLCHR;
	}

	int _compare_with_cstring(const size_t& pos, const size_t& len, const char* cstring, const size_t& subpos, const size_t& sublen) const {
		return strncmp(_data._First + pos, cstring + subpos, std::max(len, sublen));
	}

	size_t _search_for_cstring(const char* cstring, const size_t& pos, const size_t& len) const {
		if (pos > size())		// pos = start pos
			throw std::out_of_range("Invalid starting position...");
		if (pos + len > size() || len > strlen(cstring))
			throw std::out_of_range("Invalid length...");

		size_t endLoop = size() - len;
		for (size_t i = pos; i <= endLoop; ++i)
			if (strncmp(_data._First + i, cstring, len) == 0)
				return i;

		return npos;
	}
	
	void _extend_if_full() {											// Reserve 50% more capacity when full
		if (_data._Last == _data._Final)
			reserve(capacity() + capacity() / 2 + 1);
	}

	void _copy(const String& other) {									// Generic copy function for string
		_alloc_empty(other.capacity());
		memcpy(_data._First, other._data._First, other.size());
		_data._Last		= _data._First + other.size();
		_data._Final	= _data._First + other.capacity();
		_data._Last[0] 	= NULLCHR;
	}

	void _move(String&& other) {										// Generic move function for string
		_data._First	= other._data._First;
		_data._Last		= other._data._Last;
		_data._Final	= other._data._Final;

		other._initialize_from_cstring(nullptr);
	}
}; // END String

CUSTOM_END


STD_BEGIN

// String Helpers
template<>
struct hash<custom::String> {
	size_t operator()(const custom::String& string) const {
		return hash<char>()(string[0]) ^ string.size();
	}
};

template<>
struct less<custom::String> {
	bool operator()(const custom::String& left, const custom::String& right) const {
		return left.compare(right) < 0;
	}
};
// END String Helpers

STD_END