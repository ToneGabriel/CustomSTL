#pragma once
#include <iostream>
#include "Allocator.h"
#include "BaseIterator.h"

// String Iterator ============================================
template<class String>
class StringIterator : public BaseIterator<String>
{
private:
	using Base = BaseIterator<String>;

public:

	explicit StringIterator(Base::IterType* ptr, Base::Data* data)
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

	Base::IterType* operator->() {
		if (this->_Ptr >= this->_IterationData->_IterEnd)
			throw std::out_of_range("Cannot access end iterator...");

		return this->_Ptr;;
	}

	Base::IterType& operator*() {
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
	using Alloc		= typename Allocator<char>;			// Allocator type
	using Iterator	= typename StringIterator<String>;	// Iterator type			
	using Data		= typename Iterator::Data;			// Iteration data

private:
	size_t _size		= 0;
	size_t _capacity	= 0;
	char* _string		= nullptr;

	mutable Data _data;
	mutable Alloc _alloc;

	static constexpr char s_null_term_char		= '\0';
	static constexpr size_t s_istream_capacity	= 256;

public:

	String() {
		initialize_from_cstring(nullptr);
	}

	String(const char* cstring) {
		initialize_from_cstring(cstring);
	}

	String(const size_t& newCapacity) {
		realloc_empty(newCapacity);
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

	void reserve(const size_t& newCapacity) {       // Allocate memory and copy values if needed
		if (newCapacity < _size)					// Can also shrink
			_size = newCapacity;

		char* newString = alloc_string(newCapacity);
		strncpy(newString, _string, _size);
		dealloc_string(_string, _capacity);
		newString[_size] = s_null_term_char;

		_capacity = newCapacity;
		_string = newString;
	}

	void shrink_to_fit() {                          // Allocate memory with capacity equal to size and move values there
		reserve(_size);
	}

	void push_back(const char& chr) {
		extend_if_full();
		_string[_size++] = chr;
		_string[_size] = s_null_term_char;
	}

	void pop_back() {
		_string[--_size] = s_null_term_char;
	}

	// Append function overload
	String& append(const String& string) {		// Appends a copy of string
		insert_from_cstring(_size, string._string, 0, string._size);

		return *this;
	}

	String& append(const String& string, const size_t& subpos, const size_t& sublen) {	// Appends a copy of a substring of string.
		insert_from_cstring(_size, string._string, subpos, sublen);

		return *this;
	}

	String& append(const char* cstring) {		// Appends a copy of a c-string.
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

	//Iterator insert(const Iterator& posIter, const Iterator& right, const Iterator& left) {
	//	insert_from_cstring(get_iterator_index(posIter), right._Ptr - get_iterator_index(right), get_iterator_index(right), get_iterator_index(left));
	//	
	//	return Iterator(right._Ptr, update_iteration_data());
	//}
	// end Insert

	void print_details() {
		for (size_t i = 0; i <= _capacity; i++)
		{
			if (_string[i] == s_null_term_char)
				std::cout << 'N' << ' ';
			else
				std::cout << _string[i] << ' ';
		}
		std::cout << '\n';
		std::cout << _size << ' ' << _capacity << '\n';
	}

	String substr(const size_t& pos, const size_t& len) const {		// Create substring from current string
		if (pos < 0 || pos > _size)
			throw std::out_of_range("Invalid starting position...");
		if (len < 0 || len + pos > _size)
			throw std::out_of_range("Invalid length...");

		String sub(len);	// empty string with _capacity = len
		strncpy(sub._string, _string + pos, len);
		sub._string[len] = s_null_term_char;
		sub._size = sub._capacity;

		return sub;
	}

	const char* c_str() const {
		return _string;
	}

	const size_t capacity() const {                                       // Get capacity
		return _capacity;
	}

	const size_t size() const {                                           // Get size
		return _size;
	}

	const bool empty() const {
		return _size == 0;
	}

	void clear() {
		_size = 0;
		_string[0] = s_null_term_char;
	}

	const char& at(const size_t& index) const {                      // Acces char at index with check (read only)
		if (index < 0 || index >= _size)
			throw std::out_of_range("Invalid Index...");

		return _string[index];
	}

	char& at(const size_t& index) {                                  // Acces char at index with check
		if (index < 0 || index >= _size)
			throw std::out_of_range("Invalid Index...");

		return _string[index];
	}

public:
	// Operators

	const char& operator[](const size_t& index) const {
		return _string[index];
	}

	char& operator[](const size_t& index) {                          // Acces object at index
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

	friend std::ostream& operator<<(std::ostream& os, const String& string) {
		os << string._string;
		return os;
	}

	friend std::istream& operator>>(std::istream& is, String& string) {
		string.realloc_empty(s_istream_capacity);
		is.getline(string._string, s_istream_capacity);
		string._size = strlen(string._string);
		return is;
	}

public:
	// Iterator specific

	Iterator begin() const {
		return Iterator(_string, update_iteration_data());
	}

	Iterator end() const {
		return Iterator(_string + _size, update_iteration_data());
	}

private:
	// Others

	char* alloc_string(const size_t& capacity) {			// Allocate memory +1 null term
		return _alloc.alloc(capacity + 1);
	}

	void dealloc_string(char* address, const size_t& capacity) {	// Deallocate memory +1 null term
		_alloc.dealloc(address, capacity + 1);
	}

	void initialize_from_cstring(const char* cstring) {
		if (cstring == nullptr) {
			_capacity = 0;
			_size = 0;
			_string = alloc_string(_capacity);
			_string[0] = s_null_term_char;
		}
		else {
			_capacity = strlen(cstring);
			_size = _capacity;
			_string = alloc_string(_capacity);
			strcpy(_string, cstring);
		}
	}

	String& insert_from_cstring(size_t pos, const char* cstring, const size_t& subpos, const size_t& sublen) {
		if (pos < 0 || pos > _size)
			throw std::out_of_range("Invalid starting position...");

		size_t newSize = _size + sublen;

		if (newSize > _capacity)
			reserve(newSize);

		memcpy(_string + pos + sublen, _string + pos, _size - pos);	// copy last chars "sublen" positions to right
		memcpy(_string + pos, cstring + subpos, sublen);			// add substr from cstring between
		_size = newSize;
		_string[_size] = s_null_term_char;

		return *this;
	}

	void realloc_empty(const size_t& newCapacity) {
		dealloc_string(_string, _capacity);
		_string = alloc_string(newCapacity);
		_size = 0;
		_capacity = newCapacity;
		_string[_size] = s_null_term_char;
	}

	const size_t get_iterator_index(const Iterator& iterator) const {    // Get the position for the element in array from iterator
		return iterator._Ptr - begin()._Ptr;
	}

	void extend_if_full() {				// Reserve 50% more capacity when full
		if (_size >= _capacity)
			reserve(_capacity + _capacity / 2 + 1);
	}

	void copy(const String& other) {
		realloc_empty(other._capacity);
		strcpy(_string, other._string);

		_size = other._size;
		_capacity = other._capacity;
	}

	void move(String&& other) {
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