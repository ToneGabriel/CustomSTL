#pragma once
#include "Common.h"
#include "List.h"
#include <iostream>
#include <cmath>
#include <functional>

CUSTOM_BEGIN

// Map ========================================================
template<class Key, class Type, class Compare = std::less<Key>>
class Map
{
public:
	using ValueType = std::pair<Key, Type>;						// Type of values stored in container
	using Iterator	= typename List<ValueType>::Iterator;		// Iterator for this container (identical to List iterator)

private:
	using IterList			= List<ValueType>;					// List of ValueType used for iterating
	using Node				= typename IterList::Node;			// Node component from List

private:
	Compare _comp;												// Used for initial(non-compressed) hash value
	IterList _elems;											// Used to iterate through container

public:
    // Constructors

    Map() {

    }

    Map(const Map& other) {

    }

    Map(Map&& other) {

    }

    ~Map() {
        clear();
    }

public:
	// Main functions

	template<class... PairArgs>
	Iterator emplace(PairArgs&&... args) {								// Constructs Node first with any given std::pair arguments
    }

    template<class KeyType, class... Args>
	Iterator try_emplace(KeyType&& key, Args&&... args) {				// Force construction with known key and given arguments for object
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

	const Type& at(const Key& key) const {								// Access Value at key with check
    }

	Type& at(const Key& key) {
    }

public:
	// Operators
	Type& operator[](const Key& key) {									// Access value or create new one with key and assignment (no const)
		return try_emplace(key)->Value->second;
	}

	Type& operator[](Key&& key) {
		return try_emplace(std::move(key))->Value->second;
	}

	Map& operator=(const Map& other) {
		_elems = other._elems;


		return *this;
	}

	Map& operator=(Map&& other) noexcept {
		_elems = std::move(other._elems);


		return *this;
	}

	bool operator==(const Map& other) const {		// Contains the same elems, but not the same hashtable
		if (size() != other.size())
			return false;


		return true;
	}

	bool operator!=(const Map& other) const {
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

	const Iterator end() const{
		return _elems.end();
	}

private:
	// Others

};
// Map ========================================================
// END

CUSTOM_END