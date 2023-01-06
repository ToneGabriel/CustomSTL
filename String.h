#pragma once
#include "Common.h"
#include "Allocator.h"
#include "BaseIterator.h"
#include <iostream>
#include <cstring>

#define NULLCHR '\0'

CUSTOM_BEGIN
// String Iterator ============================================
template<class String>
class StringIterator : public BaseIterator<String>
{
private:
	using Base = BaseIterator<String>;

public:

	explicit StringIterator(typename Base::IterType* ptr, typename Base::Data* data)
		:Base(ptr, data) {}

	StringIterator& operator++() {
		if (this->_Ptr >= this->_IterationData->_IterEnd)
			throw std::out_of_range("Cannot increment end iterator...");

		this->_Ptr++;
		return *this;
	}

	StringIterator& operator++(int) {
		StringIterator temp = *this;
		++(*this);
		return temp;
	}

	StringIterator& operator--() {
		if (this->_Ptr <= this->_IterationData->_IterBegin)
			throw std::out_of_range("Cannot decrement begin iterator...");

		this->_Ptr--;
		return *this;
	}

	StringIterator& operator--(int) {
		StringIterator temp = *this;
		--(*this);
		return temp;
	}

	typename Base::IterType* operator->() {
		if (this->_Ptr >= this->_IterationData->_IterEnd)
			throw std::out_of_range("Cannot access end iterator...");

		return this->_Ptr;
	}

	typename Base::ValueType& operator*() {
		if (this->_Ptr >= this->_IterationData->_IterEnd)
			throw std::out_of_range("Cannot dereference end iterator...");

		return *this->_Ptr;
	}

	bool operator==(const StringIterator& other) const {
		return this->_Ptr == other._Ptr;
	}

	bool operator!=(const StringIterator& other) const {
		return !(*this == other);
	}
};
// String Iterator ============================================
// END


// String ============================================
class String
{
public:
	using ValueType = char;
	using IterType	= ValueType;
	using Alloc		= Allocator<char>;					// Allocator type
	using Iterator	= StringIterator<String>;			// Iterator type			
	using Data		= typename Iterator::Data;			// Iteration data

	static constexpr size_t npos				= static_cast<size_t>(-1);
	static constexpr size_t istream_capacity	= 255;

private:
	size_t _size		= 0;
	size_t _capacity	= 0;
	char* _string		= nullptr;

	mutable Data _data;
	mutable Alloc _alloc;

public:

	String() {
		initialize_from_cstring(nullptr);
	}

	String(const char* cstring) {
		initialize_from_cstring(cstring);
	}

	String(const size_t& newCapacity) {
		alloc_empty(newCapacity);
	}

	String(const String& other) {
		copy(other);
	}

	String(String&& other) noexcept {
		move(std::move(other));
	}

	~String() {
		dealloc_string(_string, _capacity);
	}

public:
	// Main functions

	void reserve(const size_t& newCapacity) {			// Allocate memory and copy values if needed
		if (newCapacity < _size)						// Can also shrink
			_size = newCapacity;

		char* newString = alloc_string(newCapacity);
		memcpy(newString, _string, _size);
		dealloc_string(_string, _capacity);
		newString[_size] = NULLCHR;

		_capacity = newCapacity;
		_string = newString;
	}

	void shrink_to_fit() {								// Allocate memory with capacity equal to size and move values there
		reserve(_size);
	}

	void push_back(const char& chr) {
		extend_if_full();
		_string[_size++] = chr;
		_string[_size] = NULLCHR;
	}

	void pop_back() {
		_string[--_size] = NULLCHR;
	}

	// Append function overload
	String& append(const String& string) {						// Appends a copy of string
		insert_from_cstring(_size, string._string, 0, string._size);

		return *this;
	}

	String& append(const String& string, const size_t& subpos, const size_t& sublen) {	// Appends a copy of a substring of string.
		insert_from_cstring(_size, string._string, subpos, sublen);

		return *this;
	}

	String& append(const char* cstring) {						// Appends a copy of a c-string.
		insert_from_cstring(_size, cstring, 0, strlen(cstring));

		return *this;
	}

	String& append(const char* cstring, const size_t nchar) {	// Appends a copy of the first n characters in the c-string
		insert_from_cstring(_size, cstring, 0, nchar);

		return *this;
	}

	String& append(const size_t& nchar, const char& chr) {		// Appends n consecutive copies of character c
		char* seq = alloc_string(nchar);
		for (size_t i = 0; i < nchar; i++)
			seq[i] = chr;

		insert_from_cstring(_size, seq, 0, nchar);
		dealloc_string(seq, nchar);

		return *this;
	}
	// end Append

	// Insert function overload
	String& insert(const size_t& pos, const String& string) {
		insert_from_cstring(pos, string._string, 0, string._size);

		return *this;
	}

	String& insert(const size_t& pos, const String& string, const size_t& subpos, const size_t& sublen) {
		insert_from_cstring(pos, string._string, subpos, sublen);

		return *this;
	}

	String& insert(const size_t& pos, const char* cstring) {
		insert_from_cstring(pos, cstring, 0, strlen(cstring));

		return *this;
	}

	String& insert(const size_t& pos, const char* cstring, const size_t& len) {
		insert_from_cstring(pos, cstring, 0, len);

		return *this;
	}

	String& insert(const size_t& pos, const size_t& nchar, const char& chr) {
		char* seq = alloc_string(nchar);
		for (size_t i = 0; i < nchar; i++)
			seq[i] = chr;

		insert_from_cstring(pos, seq, 0, nchar);
		dealloc_string(seq, nchar);

		return *this;
	}

	Iterator insert(const Iterator& where, const char& chr) {
		size_t pos = get_index(where);
		insert_from_cstring(pos, &chr, 0, 1);

		return Iterator(_string + pos, update_iteration_data());
	}

	Iterator insert(const Iterator& where, const Iterator& first, const Iterator& last) {
		if (where._IterationData->_IterBegin == first._IterationData->_IterBegin ||
			first._IterationData->_IterBegin != last._IterationData->_IterBegin)	// Check if pos string != first/last string
			throw std::domain_error("String provided by first and last must be the same, but different from the one provided by where");

		size_t pos 			= get_index(where);
		size_t posFrom 		= get_index(first);
		size_t posTo 		= get_index(last);
		const char* cstring = first._IterationData->_IterBegin;
		insert_from_cstring(pos, cstring, posFrom, posTo - posFrom);

		return Iterator(_string + pos, update_iteration_data());
	}
	// end Insert

	// Erase function overload
	String& erase(const size_t& pos, const size_t& len) {
		remove_from_cstring(pos, len);

		return *this;
	}

	Iterator erase(const Iterator& where) {
		if (is_end(where))
			throw std::out_of_range("String erase iterator outside range...");

		size_t pos = get_index(where);
		remove_from_cstring(pos, 1);

		return Iterator(_string + pos, update_iteration_data());
	}

	Iterator erase(const Iterator& first, const Iterator& last) {
		if (is_end(first))
			throw std::out_of_range("String erase iterator outside range...");

		size_t posFrom 		= get_index(first);
		size_t posTo 		= get_index(last);
		remove_from_cstring(posFrom, posTo - posFrom);

		return Iterator(_string + posFrom, update_iteration_data());
	}
	// end Erase

	// Find function overload
	size_t find(const String& string, const size_t& pos) const {
		return search_for_cstring(string._string, pos, string._size);
	}

	size_t find (const char* cstring, const size_t& pos) const {
		return search_for_cstring(cstring, pos, strlen(cstring));
	}

	size_t find (const char* cstring, const size_t& pos, const size_t& len) const {
		return search_for_cstring(cstring, pos, len);
	}

	size_t find (const char& chr, const size_t& pos) const {
		return search_for_cstring(&chr, pos, 1);
	}
	// end Find

	// Compare function overload
	int compare(const String& string) const {
		return compare_with_cstring(0, _size, string._string, 0, string._size);
	}

	int compare(const size_t& pos, const size_t& len, const String& string, const size_t& subpos, const size_t& sublen) const {
		return compare_with_cstring(pos, len, string._string, subpos, sublen);
	}

	int compare(const char* cstring) const {
		return compare_with_cstring(0, _size, cstring, 0, strlen(cstring));
	}

	int compare(const size_t& pos, const size_t& len, const char* cstring, const size_t& subpos, const size_t& sublen) const {
		return compare_with_cstring(pos, len, cstring, subpos, sublen);
	}
	// end Compare

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

	const char* c_str() const {
		return _string;
	}

	const size_t capacity() const {									// Get capacity
		return _capacity;
	}

	const size_t size() const {										// Get size
		return _size;
	}

	const bool empty() const {
		return _size == 0;
	}

	void clear() {
		_size = 0;
		_string[0] = NULLCHR;
	}

	const char& at(const size_t& index) const {						// Acces char at index with check (read only)
		if (index < 0 || index >= _size)
			throw std::out_of_range("Invalid Index...");

		return _string[index];
	}

	char& at(const size_t& index) {									// Acces char at index with check
		if (index < 0 || index >= _size)
			throw std::out_of_range("Invalid Index...");

		return _string[index];
	}

	const char& front() const {
		return _string[0];
	}

	char& front() {													// Get the value of the first component
		return _string[0];
	}

	const char& back() const {
		return _string[_size - 1];
	}

	char& back() {													// Get the value of the last component
		return _string[_size - 1];
	}

	void print_details() {											// For Debugging
		for (size_t i = 0; i <= _capacity; i++)
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
		return _string[index];
	}

	char& operator[](const size_t& index) {							// Acces object at index
		return _string[index];
	}

	String& operator=(const String& other) {
		if (_string != other._string)
			copy(other);

		return *this;
	}

	String& operator=(String&& other) noexcept {
		if (_string != other._string)
			move(std::move(other));

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
		return !operator==(other);
	}

	friend std::ostream& operator<<(std::ostream& os, const String& string) {
		os << string._string;
		return os;
	}

	friend std::istream& operator>>(std::istream& is, String& string) {
		if (string._string != nullptr)
			string.dealloc_string(string._string, string._capacity);

		string.alloc_empty(istream_capacity);
		is.getline(string._string, istream_capacity);
		string._size = strlen(string._string);
		return is;
	}

public:
	// Iterator specific

	Iterator begin() {
		return Iterator(_string, update_iteration_data());
	}

	const Iterator begin() const {
		return Iterator(_string, update_iteration_data());
	}

	Iterator end() {
		return Iterator(_string + _size, update_iteration_data());
	}

	const Iterator end() const {
		return Iterator(_string + _size, update_iteration_data());
	}

private:
	// Others

	char* alloc_string(const size_t& capacity) {					// Allocate memory +1 null term
		return _alloc.alloc(capacity + 1);
	}

	void dealloc_string(char* address, const size_t& capacity) {	// Deallocate memory +1 null term
		_alloc.dealloc(address, capacity + 1);
	}

	void alloc_empty(const size_t& newCapacity) {
		_string = alloc_string(newCapacity);
		_size = 0;
		_capacity = newCapacity;
		_string[_size] = NULLCHR;
	}

	void initialize_from_cstring(const char* cstring) {
		if (cstring == nullptr) {
			_capacity = _size = 0;
			_string = alloc_string(_capacity);
		}
		else {
			_capacity = _size = strlen(cstring);
			_string = alloc_string(_capacity);
			memcpy(_string, cstring, _size);
		}

		_string[_size] = NULLCHR;
	}

	void insert_from_cstring(const size_t& pos, const char* cstring, const size_t& subpos, const size_t& sublen) {
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

	void remove_from_cstring(const size_t& pos, const size_t& len) {	// TODO: check
		if (pos < 0 || pos > _size)		// pos = start pos
			throw std::out_of_range("Invalid starting position...");
		if (pos + len > _size)
			throw std::out_of_range("Invalid length...");

		memcpy(_string + pos, _string + pos + len, _size - pos - len);
		_size = _size - len;
		_string[_size] = NULLCHR;
	}

	size_t search_for_cstring(const char* cstring, const size_t& pos, const size_t& len) const {
		if (pos < 0 || pos > _size)		// pos = start pos
			throw std::out_of_range("Invalid starting position...");
		if (pos + len > _size || len > strlen(cstring))
			throw std::out_of_range("Invalid length...");

		for(size_t i = pos; i <= _size - len; i++)
			if(strncmp(_string + i, cstring, len) == 0)
				return i;

		return npos;
	}

	int compare_with_cstring(const size_t& pos, const size_t& len, const char* cstring, const size_t& subpos, const size_t& sublen) const {
		if(len < sublen)
			return -1;
		if(len > sublen)
			return 1;
		
		return strncmp(_string + pos, cstring + subpos, len);
	}

	const size_t get_index(const Iterator& iterator) const {		// Get the position for the element in array from iterator
		return iterator._Ptr - iterator._IterationData->_IterBegin;
	}

	const bool is_end(const Iterator& iterator) const {
		return iterator._Ptr == iterator._IterationData->_IterEnd;
	}

	void extend_if_full() {											// Reserve 50% more capacity when full
		if (_size >= _capacity)
			reserve(_capacity + _capacity / 2 + 1);
	}

	void copy(const String& other) {								// Generic copy function for string
		if (_string != nullptr)
			dealloc_string(_string, _capacity);

		alloc_empty(other._capacity);
		memcpy(_string, other._string, other._size);
		_size = other._size;
		_capacity = other._capacity;
		_string[_size] = NULLCHR;
	}

	void move(String&& other) {										// Generic move function for vector
		if (_string != nullptr)
			dealloc_string(_string, _capacity);

		_string = other._string;
		_size = other._size;
		_capacity = other._capacity;

		other.initialize_from_cstring(nullptr);
	}

	Data* update_iteration_data() const {
		_data._IterBegin = _string;
		_data._IterEnd = _string + _size;

		return &_data;
	}
};
// String ============================================
// END
CUSTOM_END


STD_BEGIN
// String Hash ========================================================
template<>
struct hash<custom::String> {
	size_t operator()(const custom::String& string) const {
		return hash<char>()(string[0]) ^ string.size();
	}
};
// String Hash ========================================================
// END
STD_END