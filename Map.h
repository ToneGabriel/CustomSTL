#pragma once
#include "SearchTree.h"

CUSTOM_BEGIN

// Map Traits ==============================================
template<class Key, class Type, class Compare>
class MapTraits
{
public:
	using KeyType 		= Key;
	using MappedType 	= Type;
	using KeyCompare 	= Compare;
	using ValueType 	= Pair<Key, Type>;

public:

	MapTraits() = default;

	static const KeyType& extract_key(const ValueType& value) noexcept { // extract key from element value
		return value.First;
	}

	static const MappedType& extract_mapval(const ValueType& value) noexcept { // extract mapped val from element value
		return value.Second;
	}
};
// Map Traits ==============================================
// END

// Map ========================================================
template<class Key, class Type, class Compare = std::less<Key>>
class Map : public SearchTree<MapTraits<Key, Type, Compare>>
{
private:
	using Base = SearchTree<MapTraits<Key, Type, Compare>>;

public:
	using KeyType 		= typename Base::KeyType;
	using MappedType 	= typename Base::MappedType;
	using ValueType 	= typename Base::ValueType;
	using Iterator		= typename Base::Iterator;

public:
	// Constructors

	Map() 
		:Base() { }

	Map(const Map& other)
		:Base(other) { }

	Map(Map&& other) noexcept
		:Base(std::move(other)) { }

	~Map() { }

public:
	// Operators

	MappedType& operator[](const Key& key) {				// Access value or create new one with key and assignment (no const)
		return try_emplace(key)->Value.Second;
	}

	MappedType& operator[](Key&& key) {
		return try_emplace(std::move(key))->Value.Second;
	}

	Map& operator=(const Map& other) {
		Base::operator=(other);

		return *this;
	}

	Map& operator=(Map&& other) noexcept {
		Base::operator=(std::move(other));

		return *this;
	}

	bool operator==(const Map& other) const {		// Contains the same elems, but not the same hashtable
		return Base::operator==(other);
	}

	bool operator!=(const Map& other) const {
		return !operator==(other);
	}

public:
	// Main functions

	template<class _KeyType, class... Args>
	Iterator try_emplace(_KeyType&& key, Args&&... args) {				// Force construction with known key and given arguments for object
		return this->_try_emplace(std::move(key), std::forward<Args>(args)...);
	}

	const MappedType& at(const Key& key) const {						// Access Value at key with check
		return this->_at(key);
	}

	MappedType& at(const Key& key) {
		return this->_at(key);
	}

};
// Map ========================================================
// END

CUSTOM_END