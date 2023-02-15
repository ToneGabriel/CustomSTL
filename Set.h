#pragma once
#include "Common.h"
#include "List.h"

#include <iostream>
#include <cmath>
#include <functional>

CUSTOM_BEGIN

// Set ========================================================
template<class Key, class Compare = std::less<Key>>
class Set 
{
    public:
	using ValueType = Key;                                      // Type of values stored in container
	using Iterator	= typename List<ValueType>::Iterator;		// Iterator for this container (identical to List iterator)

private:
	using IterList			= List<ValueType>;					// List of ValueType used for iterating
	using Node				= typename IterList::Node;			// Node component from List

private:
	Compare _comp;                                              // Used for comparison of objects
	IterList _elems;											// Used to iterate through container

public:
    // Constructors

    Set() {

    }

    Set(const Set& other) {
        
    }

    Set(Set&& other) {
        
    }

    ~Set() {
        clear();
    }

public:
	// Main functions

	template<class... Args>
	Iterator emplace(Args&&... args) {									// Constructs Node first with any given arguments

    }
    
	Iterator erase(const Key& key) {
    
    }

	Iterator erase(const Iterator& iterator) {
    
    }

	Iterator find(const Key& key) const {
    
    }

    void clear() {
		_elems.clear();													// Delete all Node* with values
        
    }

    bool empty() const {
		return _elems.empty();
	}

	size_t size() const {
		return _elems.size();
	}

public:
    // Operators

	Set<Key>& operator=(const Set& other) {
		_elems = other._elems;


		return *this;
    }

    Set<Key>& operator=(Set&& other) {
    	_elems = custom::move(other._elems);


		return *this;
    }

	bool operator==(const Set& other) const {
    	if (size() != other.size())
			return false;


		return true;
    }

    bool operator!=(const Set& other) const {
		return !operator==(other);
	}

public:
	// Iterator functions

	Iterator begin() {
		return _elems.begin();
	}

	const Iterator begin() const {
		return _elems.begin();
	}

	Iterator end() {
		return _elems.end();
	}

	const Iterator end() const {
		return _elems.end();
	}

private:
    // Others

};
// Set ========================================================
// END

CUSTOM_END