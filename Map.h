#pragma once
#include "SearchTree.h"

CUSTOM_BEGIN

// Headings =================================================================================

template<class Key, class Type, class Compare>
class MapTraits										// Map Traits
{
public:
	using KeyType 		= Key;
	using MappedType 	= Type;
	using KeyCompare 	= Compare;
	using ValueType 	= Pair<Key, Type>;

public:

	MapTraits() = default;

	static const KeyType& extract_key(const ValueType& value) noexcept;			// extract key from element value
	static const MappedType& extract_mapval(const ValueType& value) noexcept;	// extract mapped val from element value
}; // END Map Traits


template<class Key, class Type, class Compare = std::less<Key>>
class Map : public SearchTree<MapTraits<Key, Type, Compare>>		// Map Template
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

	Map();
	Map(const Map& other);
	Map(Map&& other) noexcept;
	~Map();

public:
	// Operators

	MappedType& operator[](const Key& key);					// Access value or create new one with key and assignment (no const)
	MappedType& operator[](Key&& key);

	Map& operator=(const Map& other);
	Map& operator=(Map&& other) noexcept;

	bool operator==(const Map& other) const;				// Contains the same elems, but not the same hashtable
	bool operator!=(const Map& other) const;

public:
	// Main functions

	template<class _KeyType, class... Args>
	Iterator try_emplace(_KeyType&& key, Args&&... args);	// Force construction with known key and given arguments for object

	const MappedType& at(const Key& key) const;				// Access _Value at key with check
	MappedType& at(const Key& key);

};  // END Map Template



// Definitions =================================================================================

// Map Traits
template<class Key, class Type, class Compare>
const typename MapTraits<Key, Type, Compare>::KeyType& MapTraits<Key, Type, Compare>::extract_key(const ValueType& value) noexcept {
	return value.First;
}

template<class Key, class Type, class Compare>
const typename MapTraits<Key, Type, Compare>::MappedType& MapTraits<Key, Type, Compare>::extract_mapval(const ValueType& value) noexcept {
	return value.Second;
}
// END Map Traits


// Map Template
template<class Key, class Type, class Compare>
Map<Key, Type, Compare>::Map()
	:Base() { /*Empty*/ }

template<class Key, class Type, class Compare>
Map<Key, Type, Compare>::Map(const Map& other)
	:Base(other) { /*Empty*/ }

template<class Key, class Type, class Compare>
Map<Key, Type, Compare>::Map(Map&& other) noexcept
	:Base(custom::move(other)) { /*Empty*/ }

template<class Key, class Type, class Compare>
Map<Key, Type, Compare>::~Map() { /*Empty*/ }

template<class Key, class Type, class Compare>
typename Map<Key, Type, Compare>::MappedType& Map<Key, Type, Compare>::operator[](const Key& key) {
	return try_emplace(key)->_Value.Second;
}

template<class Key, class Type, class Compare>
typename Map<Key, Type, Compare>::MappedType& Map<Key, Type, Compare>::operator[](Key&& key) {
	return try_emplace(custom::move(key))->_Value.Second;
}

template<class Key, class Type, class Compare>
Map<Key, Type, Compare>& Map<Key, Type, Compare>::operator=(const Map& other) {
	Base::operator=(other);
	return *this;
}

template<class Key, class Type, class Compare>
Map<Key, Type, Compare>& Map<Key, Type, Compare>::operator=(Map&& other) noexcept {
	Base::operator=(custom::move(other));
	return *this;
}

template<class Key, class Type, class Compare>
bool Map<Key, Type, Compare>::operator==(const Map& other) const {
	return Base::operator==(other);
}

template<class Key, class Type, class Compare>
bool Map<Key, Type, Compare>::operator!=(const Map& other) const {
	return Base::operator!=(other);
}

template<class Key, class Type, class Compare>
template<class _KeyType, class... Args>
typename Map<Key, Type, Compare>::Iterator Map<Key, Type, Compare>::try_emplace(_KeyType&& key, Args&&... args) {
	return this->_try_emplace(custom::move(key), custom::forward<Args>(args)...);
}

template<class Key, class Type, class Compare>
const typename Map<Key, Type, Compare>::MappedType& Map<Key, Type, Compare>::at(const Key& key) const {
	return this->_at(key);
}

template<class Key, class Type, class Compare>
typename Map<Key, Type, Compare>::MappedType& Map<Key, Type, Compare>::at(const Key& key) {
	return this->_at(key);
}
// END Map Template
CUSTOM_END