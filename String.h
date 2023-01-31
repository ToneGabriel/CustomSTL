#pragma once
#include "Common.h"
#include "Allocator.h"
#include "BaseIterator.h"
#include <iostream>
#include <cstring>	// TODO: check if needed


CUSTOM_BEGIN

// Headings =================================================================================

template<class String>
class StringIterator : public BaseIterator<String>	// String Iterator
{
private:
	using Base = BaseIterator<String>;

public:

	explicit StringIterator(typename Base::IterType* ptr, typename Base::Data* data);

	StringIterator& operator++();
	StringIterator& operator++(int);
	StringIterator& operator--();
	StringIterator& operator--(int);

	typename Base::IterType* operator->();
	typename Base::ValueType& operator*();

	bool operator==(const StringIterator& other) const;
	bool operator!=(const StringIterator& other) const;
}; // END String Iterator


class String	// String
{
public:
	using ValueType = char;
	using IterType	= ValueType;
	using Alloc		= Allocator<char>;					// Allocator type
	using Iterator	= StringIterator<String>;			// Iterator type			
	using Data		= typename Iterator::Data;			// Iteration data

	static constexpr size_t npos				= static_cast<size_t>(-1);
	static constexpr size_t istream_capacity	= 255;
	static constexpr char NULLCHR 				= '\0';

private:
	size_t _size		= 0;
	size_t _capacity	= 0;
	char* _string		= nullptr;

	mutable Data _data;
	mutable Alloc _alloc;

public:

	String();
	String(const char* cstring);
	String(const size_t& newCapacity);
	String(const String& other);
	String(String&& other) noexcept;
	~String();

public:
	// Main functions

	void reserve(const size_t& newCapacity);			// Allocate memory and copy values if needed
	void shrink_to_fit();								// Allocate memory with capacity equal to size and move values there
	void push_back(const char& chr);
	void pop_back();

	// Append function overload
	String& append(const String& string);												// Appends a copy of string
	String& append(const String& string, const size_t& subpos, const size_t& sublen);	// Appends a copy of a substring of string.
	String& append(const char* cstring);												// Appends a copy of a c-string.
	String& append(const char* cstring, const size_t nchar);							// Appends a copy of the first n characters in the c-string
	String& append(const size_t& nchar, const char& chr);								// Appends n consecutive copies of character c
	// end Append

	// Insert function overload
	String& insert(const size_t& pos, const String& string);
	String& insert(const size_t& pos, const String& string, const size_t& subpos, const size_t& sublen);
	String& insert(const size_t& pos, const char* cstring);
	String& insert(const size_t& pos, const char* cstring, const size_t& len);
	String& insert(const size_t& pos, const size_t& nchar, const char& chr);
	Iterator insert(const Iterator& where, const char& chr);
	Iterator insert(const Iterator& where, const Iterator& first, const Iterator& last);
	// end Insert

	// Erase function overload
	String& erase(const size_t& pos, const size_t& len);
	Iterator erase(const Iterator& where);
	Iterator erase(const Iterator& first, const Iterator& last);
	// end Erase

	// Find function overload
	size_t find(const String& string, const size_t& pos) const;
	size_t find(const char* cstring, const size_t& pos) const;
	size_t find(const char* cstring, const size_t& pos, const size_t& len) const;
	size_t find(const char& chr, const size_t& pos) const;
	// end Find

	// Compare function overload
	int compare(const String& string) const;
	int compare(const size_t& pos, const size_t& len, const String& string, const size_t& subpos, const size_t& sublen) const;
	int compare(const char* cstring) const;
	int compare(const size_t& pos, const size_t& len, const char* cstring, const size_t& subpos, const size_t& sublen) const;
	// end Compare

	const char* c_str() const;
	const size_t capacity() const;									// Get capacity
	const size_t size() const;										// Get size
	const bool empty() const;
	void clear();
	
	const char& at(const size_t& index) const;						// Acces char at index with check (read only)
	char& at(const size_t& index);									// Acces char at index with check
	
	const char& front() const;
	char& front();													// Get the value of the first component
	const char& back() const;
	char& back();													// Get the value of the last component

	String substr(const size_t& pos, const size_t& len) const;		// Create substring from current string
	void print_details() const;										// For Debugging

public:
	// Operators

	const char& operator[](const size_t& index) const;
	char& operator[](const size_t& index);							// Acces object at index

	String& operator=(const String& other);
	String& operator=(String&& other) noexcept;

	String& operator+=(const String& string);
	String& operator+=(const char* cstring);
	String& operator+=(const char& chr);

	bool operator==(const String& other) const;
	bool operator!=(const String& other) const;

	friend std::ostream& operator<<(std::ostream& os, const String& string);
	friend std::istream& operator>>(std::istream& is, String& string);

public:
	// Iterator specific

	Iterator begin();
	const Iterator begin() const;

	Iterator end();
	const Iterator end() const;

private:
	// Others

	char* _alloc_string(const size_t& capacity);						// Allocate memory +1 null term
	void _dealloc_string(char* address, const size_t& capacity);		// Deallocate memory +1 null term
	void _alloc_empty(const size_t& capacity);

	void _initialize_from_cstring(const char* cstring);
	void _insert_from_cstring(const size_t& pos, const char* cstring, const size_t& subpos, const size_t& sublen);
	void _remove_from_cstring(const size_t& pos, const size_t& len);	// TODO: check
	int _compare_with_cstring(const size_t& pos, const size_t& len, const char* cstring, const size_t& subpos, const size_t& sublen) const;
	size_t _search_for_cstring(const char* cstring, const size_t& pos, const size_t& len) const;
	
	void _extend_if_full();												// Reserve 50% more capacity when full
	void _copy(const String& other);									// Generic copy function for string
	void _move(String&& other);											// Generic move function for vector
	const size_t _get_index(const Iterator& iterator) const;			// Get the position for the element in array from iterator
	const bool _is_end(const Iterator& iterator) const;
	Data* _update_iteration_data() const;
}; // END String



// Definitions =================================================================================

// String Iterator
template<class String>
StringIterator<String>::StringIterator(typename Base::IterType* ptr, typename Base::Data* data)
	:Base(ptr, data) {}

template<class String>
StringIterator<String>& StringIterator<String>::operator++() {
	if (this->_Ptr >= this->_IterationData->_End)
		throw std::out_of_range("Cannot increment end iterator...");

	this->_Ptr++;
	return *this;
}

template<class String>
StringIterator<String>& StringIterator<String>::operator++(int) {
	StringIterator temp = *this;
	++(*this);
	return temp;
}

template<class String>
StringIterator<String>& StringIterator<String>::operator--() {
	if (this->_Ptr <= this->_IterationData->_Begin)
		throw std::out_of_range("Cannot decrement begin iterator...");

	this->_Ptr--;
	return *this;
}

template<class String>
StringIterator<String>& StringIterator<String>::operator--(int) {
	StringIterator temp = *this;
	--(*this);
	return temp;
}

template<class String>
typename StringIterator<String>::Base::IterType* StringIterator<String>::operator->() {
	if (this->_Ptr >= this->_IterationData->_End)
		throw std::out_of_range("Cannot access end iterator...");

	return this->_Ptr;
}

template<class String>
typename StringIterator<String>::Base::ValueType& StringIterator<String>::operator*() {
	if (this->_Ptr >= this->_IterationData->_End)
		throw std::out_of_range("Cannot dereference end iterator...");

	return *this->_Ptr;
}

template<class String>
bool StringIterator<String>::operator==(const StringIterator& other) const {
	return this->_Ptr == other._Ptr;
}

template<class String>
bool StringIterator<String>::operator!=(const StringIterator& other) const {
	return !(*this == other);
}
// END String Iterator


// String
String::String() {
	_initialize_from_cstring(nullptr);
}

String::String(const char* cstring) {
	_initialize_from_cstring(cstring);
}

String::String(const size_t& newCapacity) {
	_alloc_empty(newCapacity);
}

String::String(const String& other) {
	_copy(other);
}

String::String(String&& other) noexcept {
	_move(std::move(other));
}

String::~String() {
	_dealloc_string(_string, _capacity);
}

void String::reserve(const size_t& newCapacity) {
	if (newCapacity < _size)						// Can also shrink
		_size = newCapacity;

	char* newString = _alloc_string(newCapacity);
	memcpy(newString, _string, _size);
	_dealloc_string(_string, _capacity);
	newString[_size] = NULLCHR;

	_capacity = newCapacity;
	_string = newString;
}

void String::shrink_to_fit() {
	reserve(_size);
}

void String::push_back(const char& chr) {
	_extend_if_full();
	_string[_size++] = chr;
	_string[_size] = NULLCHR;
}

void String::pop_back() {
	_string[--_size] = NULLCHR;
}

// Append function overload
String& String::append(const String& string) {
	_insert_from_cstring(_size, string._string, 0, string._size);

	return *this;
}

String& String::append(const String& string, const size_t& subpos, const size_t& sublen) {
	_insert_from_cstring(_size, string._string, subpos, sublen);

	return *this;
}

String& String::append(const char* cstring) {
	_insert_from_cstring(_size, cstring, 0, strlen(cstring));

	return *this;
}

String& String::append(const char* cstring, const size_t nchar) {
	_insert_from_cstring(_size, cstring, 0, nchar);

	return *this;
}

String& String::append(const size_t& nchar, const char& chr) {
	char* seq = _alloc_string(nchar);
	for (size_t i = 0; i < nchar; i++)
		seq[i] = chr;

	_insert_from_cstring(_size, seq, 0, nchar);
	_dealloc_string(seq, nchar);

	return *this;
}
// end Append

// Insert function overload
String& String::insert(const size_t& pos, const String& string) {
	_insert_from_cstring(pos, string._string, 0, string._size);

	return *this;
}

String& String::insert(const size_t& pos, const String& string, const size_t& subpos, const size_t& sublen) {
	_insert_from_cstring(pos, string._string, subpos, sublen);

	return *this;
}

String& String::insert(const size_t& pos, const char* cstring) {
	_insert_from_cstring(pos, cstring, 0, strlen(cstring));

	return *this;
}

String& String::insert(const size_t& pos, const char* cstring, const size_t& len) {
	_insert_from_cstring(pos, cstring, 0, len);

	return *this;
}

String& String::insert(const size_t& pos, const size_t& nchar, const char& chr) {
	char* seq = _alloc_string(nchar);
	for (size_t i = 0; i < nchar; i++)
		seq[i] = chr;

	_insert_from_cstring(pos, seq, 0, nchar);
	_dealloc_string(seq, nchar);

	return *this;
}

typename String::Iterator String::insert(const Iterator& where, const char& chr) {
	size_t pos = _get_index(where);
	_insert_from_cstring(pos, &chr, 0, 1);

	return Iterator(_string + pos, _update_iteration_data());
}

typename String::Iterator String::insert(const Iterator& where, const Iterator& first, const Iterator& last) {
	if (where._IterationData->_Begin == first._IterationData->_Begin ||
		first._IterationData->_Begin != last._IterationData->_Begin)	// Check if pos string != first/last string
		throw std::domain_error("String provided by first and last must be the same, but different from the one provided by where");

	size_t pos = _get_index(where);
	size_t posFrom = _get_index(first);
	size_t posTo = _get_index(last);
	const char* cstring = first._IterationData->_Begin;
	_insert_from_cstring(pos, cstring, posFrom, posTo - posFrom);

	return Iterator(_string + pos, _update_iteration_data());
}
// end Insert

// Erase function overload
String& String::erase(const size_t& pos, const size_t& len) {
	_remove_from_cstring(pos, len);

	return *this;
}

typename String::Iterator String::erase(const Iterator& where) {
	if (_is_end(where))
		throw std::out_of_range("String erase iterator outside range...");

	size_t pos = _get_index(where);
	_remove_from_cstring(pos, 1);

	return Iterator(_string + pos, _update_iteration_data());
}

typename String::Iterator String::erase(const Iterator& first, const Iterator& last) {
	if (_is_end(first))
		throw std::out_of_range("String erase iterator outside range...");

	size_t posFrom = _get_index(first);
	size_t posTo = _get_index(last);
	_remove_from_cstring(posFrom, posTo - posFrom);

	return Iterator(_string + posFrom, _update_iteration_data());
}
// end Erase

// Find function overload
size_t String::find(const String& string, const size_t& pos) const {
	return _search_for_cstring(string._string, pos, string._size);
}

size_t String::find(const char* cstring, const size_t& pos) const {
	return _search_for_cstring(cstring, pos, strlen(cstring));
}

size_t String::find(const char* cstring, const size_t& pos, const size_t& len) const {
	return _search_for_cstring(cstring, pos, len);
}

size_t String::find(const char& chr, const size_t& pos) const {
	return _search_for_cstring(&chr, pos, 1);
}
// end Find

// Compare function overload
int String::compare(const String& string) const {
	return _compare_with_cstring(0, _size, string._string, 0, string._size);
}

int String::compare(const size_t& pos, const size_t& len, const String& string, const size_t& subpos, const size_t& sublen) const {
	return _compare_with_cstring(pos, len, string._string, subpos, sublen);
}

int String::compare(const char* cstring) const {
	return _compare_with_cstring(0, _size, cstring, 0, strlen(cstring));
}

int String::compare(const size_t& pos, const size_t& len, const char* cstring, const size_t& subpos, const size_t& sublen) const {
	return _compare_with_cstring(pos, len, cstring, subpos, sublen);
}
// end Compare

String String::substr(const size_t& pos, const size_t& len) const {
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

const char* String::c_str() const {
	return _string;
}

const size_t String::capacity() const {
	return _capacity;
}

const size_t String::size() const {
	return _size;
}

const bool String::empty() const {
	return _size == 0;
}

void String::clear() {
	_size = 0;
	_string[0] = NULLCHR;
}

const char& String::at(const size_t& index) const {
	if (index < 0 || index >= _size)
		throw std::out_of_range("Invalid Index...");

	return _string[index];
}

char& String::at(const size_t& index) {
	if (index < 0 || index >= _size)
		throw std::out_of_range("Invalid Index...");

	return _string[index];
}

const char& String::front() const {
	return _string[0];
}

char& String::front() {
	return _string[0];
}

const char& String::back() const {
	return _string[_size - 1];
}

char& String::back() {
	return _string[_size - 1];
}

void String::print_details() const {
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

const char& String::operator[](const size_t& index) const {
	return _string[index];
}

char& String::operator[](const size_t& index) {
	return _string[index];
}

String& String::operator=(const String& other) {
	if (_string != other._string)
		_copy(other);

	return *this;
}

String& String::operator=(String&& other) noexcept {
	if (_string != other._string)
		_move(std::move(other));

	return *this;
}

String& String::operator+=(const String& string) {
	append(string);
	return *this;
}

String& String::operator+=(const char* cstring) {
	append(cstring);
	return *this;
}

String& String::operator+=(const char& chr) {
	append(1, chr);
	return *this;
}

bool String::operator==(const String& other) const {
	return compare(other) == 0;
}

bool String::operator!=(const String& other) const {
	return !operator==(other);
}

std::ostream& operator<<(std::ostream& os, const String& string) {
	os << string._string;
	return os;
}

std::istream& operator>>(std::istream& is, String& string) {
	if (string._string != nullptr)
		string._dealloc_string(string._string, string._capacity);

	string._alloc_empty(String::istream_capacity);
	is.getline(string._string, String::istream_capacity);
	string._size = strlen(string._string);
	return is;
}

typename String::Iterator String::begin() {
	return Iterator(_string, _update_iteration_data());
}

const typename String::Iterator String::begin() const {
	return Iterator(_string, _update_iteration_data());
}

typename String::Iterator String::end() {
	return Iterator(_string + _size, _update_iteration_data());
}

const typename String::Iterator String::end() const {
	return Iterator(_string + _size, _update_iteration_data());
}

char* String::_alloc_string(const size_t& capacity) {					// Allocate memory +1 null term
	return _alloc.alloc(capacity + 1);
}

void String::_dealloc_string(char* address, const size_t& capacity) {	// Deallocate memory +1 null term
	_alloc.dealloc(address, capacity + 1);
}

void String::_alloc_empty(const size_t& capacity) {
	_string = _alloc_string(capacity);
	_size = 0;
	_capacity = capacity;
	_string[_size] = NULLCHR;
}

void String::_initialize_from_cstring(const char* cstring) {
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

void String::_insert_from_cstring(const size_t& pos, const char* cstring, const size_t& subpos, const size_t& sublen) {
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

void String::_remove_from_cstring(const size_t& pos, const size_t& len) {	// TODO: check
	if (pos < 0 || pos > _size)		// pos = start pos
		throw std::out_of_range("Invalid starting position...");
	if (pos + len > _size)
		throw std::out_of_range("Invalid length...");

	memcpy(_string + pos, _string + pos + len, _size - pos - len);
	_size = _size - len;
	_string[_size] = NULLCHR;
}

size_t String::_search_for_cstring(const char* cstring, const size_t& pos, const size_t& len) const {
	if (pos < 0 || pos > _size)		// pos = start pos
		throw std::out_of_range("Invalid starting position...");
	if (pos + len > _size || len > strlen(cstring))
		throw std::out_of_range("Invalid length...");

	for (size_t i = pos; i <= _size - len; i++)
		if (strncmp(_string + i, cstring, len) == 0)
			return i;

	return npos;
}

int String::_compare_with_cstring(const size_t& pos, const size_t& len, const char* cstring, const size_t& subpos, const size_t& sublen) const {
	return strncmp(_string + pos, cstring + subpos, std::max(len, sublen));
}

const size_t String::_get_index(const Iterator& iterator) const {		// Get the position for the element in array from iterator
	return iterator._Ptr - iterator._IterationData->_Begin;
}

const bool String::_is_end(const Iterator& iterator) const {
	return iterator._Ptr == iterator._IterationData->_End;
}

void String::_extend_if_full() {											// Reserve 50% more capacity when full
	if (_size >= _capacity)
		reserve(_capacity + _capacity / 2 + 1);
}

void String::_copy(const String& other) {								// Generic copy function for string
	if (_string != nullptr)
		_dealloc_string(_string, _capacity);

	_alloc_empty(other._capacity);
	memcpy(_string, other._string, other._size);
	_size = other._size;
	_capacity = other._capacity;
	_string[_size] = NULLCHR;
}

void String::_move(String&& other) {										// Generic move function for vector
	if (_string != nullptr)
		_dealloc_string(_string, _capacity);

	_string = other._string;
	_size = other._size;
	_capacity = other._capacity;

	other._initialize_from_cstring(nullptr);
}

typename String::Data* String::_update_iteration_data() const {
	_data._Begin = _string;
	_data._End = _string + _size;

	return &_data;
}
// END String

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