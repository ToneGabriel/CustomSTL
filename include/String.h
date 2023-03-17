#pragma once
#include "Allocator.h"
#include "Utility.h"

#include <iostream>
#include <cstring>


CUSTOM_BEGIN

// Headings =================================================================================

template<class Type>
struct StringIterationData {	// Data used for iterating String
	Type* _Begin	= nullptr;
	Type* _End		= nullptr;

	StringIterationData() = default;
	~StringIterationData() {
		_Begin	= nullptr;
		_End	= nullptr;
	}
};

template<class String>
class StringIterator		// String Iterator
{
public:
	using ValueType = typename String::ValueType;
	using IterType	= typename String::IterType;
	using Data		= StringIterationData<IterType>;

	IterType* _Ptr	= nullptr;
	Data* _Data		= nullptr;

public:

	explicit StringIterator(IterType* ptr, Data* data)
		:_Ptr(ptr), _Data(data) { /*Empty*/ }

	~StringIterator() {
		_Ptr	= nullptr;
		_Data	= nullptr;
	}

public:

	StringIterator& operator++() {
		if (_Ptr >= _Data->_End)
			throw std::out_of_range("Cannot increment end iterator...");

		++_Ptr;
		return *this;
	}

	StringIterator operator++(int) {
		StringIterator temp = *this;
		++(*this);
		return temp;
	}

	StringIterator& operator--() {
		if (_Ptr <= _Data->_Begin)
			throw std::out_of_range("Cannot decrement begin iterator...");

		--_Ptr;
		return *this;
	}

	StringIterator operator--(int) {
		StringIterator temp = *this;
		--(*this);
		return temp;
	}

	IterType* operator->() {
		if (_Ptr >= _Data->_End)
			throw std::out_of_range("Cannot access end iterator...");

		return _Ptr;
	}

	ValueType& operator*() {
		if (_Ptr >= _Data->_End)
			throw std::out_of_range("Cannot dereference end iterator...");

		return *_Ptr;
	}

	bool operator==(const StringIterator& other) const {
		return _Ptr == other._Ptr;
	}

	bool operator!=(const StringIterator& other) const {
		return !(*this == other);
	}

public:

	const size_t get_index() const {							// Get the position for the element in array from iterator
		return static_cast<size_t>(_Ptr - _Data->_Begin);
	}

	const bool is_begin() const {
		return _Ptr == _Data->_Begin;
	}

	const bool is_end() const {
		return _Ptr == _Data->_End;
	}
}; // END String Iterator


class String	// String
{
public:
	using ValueType 		= char;
	using IterType			= ValueType;
	using Alloc				= Allocator<char>;					// Allocator type
	using Iterator			= StringIterator<String>;			// Iterator type			
	using Data				= typename Iterator::Data;			// Iteration data
	using ReverseIterator 	= ReverseIterator<Iterator>;		// ReverseIterator type

	static constexpr size_t npos = static_cast<size_t>(-1);

private:
	Alloc _alloc;
	size_t _size		= 0;
	size_t _capacity	= 0;
	char* _string		= nullptr;

	mutable Data _data;

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
		_dealloc_string(_string, _capacity);
	}

public:
	// Main functions

	void reserve(const size_t& newCapacity) {			// Allocate memory and copy values if needed
		if (newCapacity < _size)						// Can also shrink
			_size = newCapacity;

		char* newString = _alloc_string(newCapacity);
		memcpy(newString, _string, _size);
		_dealloc_string(_string, _capacity);
		newString[_size] = NULLCHR;

		_capacity = newCapacity;
		_string = newString;
	}

	void shrink_to_fit() {								// Allocate memory with capacity equal to size and move values there
		reserve(_size);
	}

	void push_back(const char& chr) {
		_extend_if_full();
		_string[_size++] = chr;
		_string[_size] = NULLCHR;
	}

	void pop_back() {
		_string[--_size] = NULLCHR;
	}

	// Append function overload
	String& append(const String& string) {												// Appends a copy of string
		_insert_from_cstring(_size, string._string, 0, string._size);
		return *this;
	}

	String& append(const String& string, const size_t& subpos, const size_t& sublen) {	// Appends a copy of a substring of string.
		_insert_from_cstring(_size, string._string, subpos, sublen);
		return *this;
	}

	String& append(const char* cstring) {												// Appends a copy of a c-string.
		_insert_from_cstring(_size, cstring, 0, strlen(cstring));
		return *this;
	}

	String& append(const char* cstring, const size_t nchar) {							// Appends a copy of the first n characters in the c-string
		_insert_from_cstring(_size, cstring, 0, nchar);
		return *this;
	}

	String& append(const size_t& nchar, const char& chr) {								// Appends n consecutive copies of character c
		char* seq = _alloc_string(nchar);
		for (size_t i = 0; i < nchar; ++i)
			seq[i] = chr;

		_insert_from_cstring(_size, seq, 0, nchar);
		_dealloc_string(seq, nchar);
		return *this;
	}
	// end Append

	// Insert function overload
	String& insert(const size_t& pos, const String& string) {
		_insert_from_cstring(pos, string._string, 0, string._size);
		return *this;
	}

	String& insert(const size_t& pos, const String& string, const size_t& subpos, const size_t& sublen) {
		_insert_from_cstring(pos, string._string, subpos, sublen);
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

		return Iterator(_string + pos, _update_iteration_data());
	}

	Iterator insert(const Iterator& where, const Iterator& first, const Iterator& last) {
		if (where._Data->_Begin == first._Data->_Begin ||
			first._Data->_Begin != last._Data->_Begin)	// Check if pos string != first/last string
			throw std::domain_error("String provided by first and last must be the same, but different from the one provided by where");

		size_t pos 			= where.get_index();
		size_t posFrom 		= first.get_index();
		size_t posTo 		= last.get_index();
		const char* cstring = first._Data->_Begin;
		_insert_from_cstring(pos, cstring, posFrom, posTo - posFrom);

		return Iterator(_string + pos, _update_iteration_data());
	}
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

		return Iterator(_string + pos, _update_iteration_data());
	}

	Iterator erase(const Iterator& first, const Iterator& last) {
		if (first.is_end())
			throw std::out_of_range("String erase iterator outside range...");

		size_t posFrom = first.get_index();
		size_t posTo = last.get_index();
		_remove_from_cstring(posFrom, posTo - posFrom);

		return Iterator(_string + posFrom, _update_iteration_data());
	}
	// end Erase

	// Find function overload
	size_t find(const String& string, const size_t& pos) const {
		return _search_for_cstring(string._string, pos, string._size);
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
		return _compare_with_cstring(0, _size, string._string, 0, string._size);
	}

	int compare(const size_t& pos, const size_t& len, const String& string, const size_t& subpos, const size_t& sublen) const {
		return _compare_with_cstring(pos, len, string._string, subpos, sublen);
	}

	int compare(const char* cstring) const {
		return _compare_with_cstring(0, _size, cstring, 0, strlen(cstring));
	}

	int compare(const size_t& pos, const size_t& len, const char* cstring, const size_t& subpos, const size_t& sublen) const {
		return _compare_with_cstring(pos, len, cstring, subpos, sublen);
	}
	// end Compare

	const char* c_str() const {
		return _string;
	}

	const size_t capacity() const {
		return _capacity;
	}

	const size_t size() const {
		return _size;
	}

	const bool empty() const {
		return _size == 0;
	}

	void clear() {
		_size 		= 0;
		_string[0] 	= NULLCHR;
	}

	const char& at(const size_t& index) const {						// Acces char at index with check (read only)
		if (index >= _size)
			throw std::out_of_range("Invalid Index...");

		return _string[index];
	}

	char& at(const size_t& index) {									// Acces char at index with check
		if (index >= _size)
			throw std::out_of_range("Invalid Index...");

		return _string[index];
	}

	const char& front() const {
		assert(_size > 0);
		return _string[0];
	}

	char& front() {													// Get the value of the first component
		assert(_size > 0);
		return _string[0];
	}

	const char& back() const {
		assert(_size > 0);
		return _string[_size - 1];
	}

	char& back() {													// Get the value of the last component
		assert(_size > 0);
		return _string[_size - 1];
	}

	String substr(const size_t& pos, const size_t& len) const {		// Create substring from current string
		if (pos < 0 || pos > _size)
			throw std::out_of_range("Invalid starting position...");
		if (len < 0 || len + pos > _size)
			throw std::out_of_range("Invalid length...");

		String sub(len);	// empty string with _capacity = len
		memcpy(sub._string, _string + pos, len);
		sub._string[len] = NULLCHR;
		sub._size = sub._capacity;

		return sub;
	}

	void print_details() const {									// For Debugging
		for (size_t i = 0; i <= _capacity; ++i)
		{
			if (_string[i] == NULLCHR)
				std::cout << 'N' << ' ';
			else
				std::cout << _string[i] << ' ';
		}
		std::cout << '\n';
		std::cout << _size << ' ' << _capacity << '\n';
	}

public:
	// Operators

	const char& operator[](const size_t& index) const {
		assert(!(index >= _size));
		return _string[index];
	}

	char& operator[](const size_t& index) {
		assert(!(index >= _size));
		return _string[index];
	}

	String& operator=(const String& other) {
		if (_string != other._string)
			_copy(other);

		return *this;
	}

	String& operator=(String&& other) noexcept {
		if (_string != other._string)
			_move(custom::move(other));

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
		append(1, chr);
		return *this;
	}

	bool operator==(const String& other) const {
		return compare(other) == 0;
	}

	bool operator!=(const String& other) const {
		return !(*this == other);
	}

	friend std::ostream& operator<<(std::ostream& os, const String& string) {
		os << string._string;
		return os;
	}

	friend std::istream& operator>>(std::istream& is, String& string) {
		if (string._string != nullptr)
			string._dealloc_string(string._string, string._capacity);

		string._alloc_empty(String::istream_capacity);
		is.getline(string._string, String::istream_capacity);
		string._size = strlen(string._string);
		return is;
	}

public:
	// Iterator specific

	Iterator begin() {
		return Iterator(_string, _update_iteration_data());
	}

	const Iterator begin() const {
		return Iterator(_string, _update_iteration_data());
	}

	ReverseIterator rbegin() {
		return ReverseIterator(end());
	}

	const ReverseIterator rbegin() const {
		return ReverseIterator(end());
	}

	Iterator end() {
		return Iterator(_string + _size, _update_iteration_data());
	}

	const Iterator end() const {
		return Iterator(_string + _size, _update_iteration_data());
	}

	ReverseIterator rend() {
		return ReverseIterator(begin());
	}

	const ReverseIterator rend() const {
		return ReverseIterator(begin());
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
		_string 		= _alloc_string(capacity);
		_size 			= 0;
		_capacity 		= capacity;
		_string[_size] 	= NULLCHR;
	}

	void _initialize_from_cstring(const char* cstring) {
		if (cstring == nullptr) {
			_capacity = _size = 0;
			_string = _alloc_string(_capacity);
		}
		else {
			_capacity = _size = strlen(cstring);
			_string = _alloc_string(_capacity);
			memcpy(_string, cstring, _size);
		}

		_string[_size] = NULLCHR;
	}

	void _insert_from_cstring(const size_t& pos, const char* cstring, const size_t& subpos, const size_t& sublen) {
		if (pos < 0 || pos > _size)		// pos = start pos
			throw std::out_of_range("Invalid starting position...");
		if (subpos + sublen > strlen(cstring))
			throw std::out_of_range("Invalid length...");

		size_t newSize = _size + sublen;

		if (newSize > _capacity)
			reserve(newSize);

		memcpy(_string + pos + sublen, _string + pos, _size - pos);	// copy last chars "sublen" positions to right
		memcpy(_string + pos, cstring + subpos, sublen);			// add substr from cstring between
		_size = newSize;
		_string[_size] = NULLCHR;
	}

	void _remove_from_cstring(const size_t& pos, const size_t& len) {	// TODO: check
		if (pos < 0 || pos > _size)		// pos = start pos
			throw std::out_of_range("Invalid starting position...");
		if (pos + len > _size)
			throw std::out_of_range("Invalid length...");

		memcpy(_string + pos, _string + pos + len, _size - pos - len);
		_size = _size - len;
		_string[_size] = NULLCHR;
	}

	int _compare_with_cstring(const size_t& pos, const size_t& len, const char* cstring, const size_t& subpos, const size_t& sublen) const {
		return strncmp(_string + pos, cstring + subpos, std::max(len, sublen));		// TODO: check why max	
	}

	size_t _search_for_cstring(const char* cstring, const size_t& pos, const size_t& len) const {
		if (pos < 0 || pos > _size)		// pos = start pos
			throw std::out_of_range("Invalid starting position...");
		if (pos + len > _size || len > strlen(cstring))
			throw std::out_of_range("Invalid length...");

		for (size_t i = pos; i <= _size - len; ++i)
			if (strncmp(_string + i, cstring, len) == 0)
				return i;

		return npos;
	}
	
	void _extend_if_full() {											// Reserve 50% more capacity when full
		if (_size >= _capacity)
			reserve(_capacity + _capacity / 2 + 1);
	}

	void _copy(const String& other) {									// Generic copy function for string
		if (_string != nullptr)
			_dealloc_string(_string, _capacity);

		_alloc_empty(other._capacity);
		memcpy(_string, other._string, other._size);
		_size 			= other._size;
		_capacity 		= other._capacity;
		_string[_size] 	= NULLCHR;
	}

	void _move(String&& other) {											// Generic move function for string
		if (_string != nullptr)
			_dealloc_string(_string, _capacity);

		_string 	= other._string;
		_size 		= other._size;
		_capacity 	= other._capacity;

		other._initialize_from_cstring(nullptr);
	}

	Data* _update_iteration_data() const {
		_data._Begin 	= _string;
		_data._End 		= _string + _size;

		return &_data;
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