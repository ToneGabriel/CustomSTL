#pragma once
#include "HashTable.h"

CUSTOM_BEGIN

// Headings =================================================================================

template<class Key, class Type, class Hasher>
class UmapTraits								// UnorderedMap Traits
{
public:
	using KeyType 		= Key;
	using MappedType 	= Type;
	using HasherType 	= Hasher;
	using ValueType 	= Pair<Key, Type>;

public:

	UmapTraits() = default;

	static const KeyType& extract_key(const ValueType& value) noexcept;			// extract key from element value
	static const MappedType& extract_mapval(const ValueType& value) noexcept;	// extract mapped val from element value
}; // END UnorderedMap Traits


template<class Key, class Type, class Hasher = std::hash<Key>>
class UnorderedMap : public HashTable<UmapTraits<Key, Type, Hasher>>	// UnorderedMap Template
{
private:
	using Base = HashTable<UmapTraits<Key, Type, Hasher>>;

public:
	using KeyType 		= typename Base::KeyType;
	using MappedType 	= typename Base::MappedType;
	using ValueType 	= typename Base::ValueType;
	using Iterator		= typename Base::Iterator;

public:
	// Constructors

	UnorderedMap();
	UnorderedMap(const size_t& buckets);
	UnorderedMap(const UnorderedMap& other);
	UnorderedMap(UnorderedMap&& other) noexcept;
	~UnorderedMap();

public:
	// Operators

	MappedType& operator[](const Key& key);					// Access value or create new one with key and assignment (no const)
	MappedType& operator[](Key&& key);

	UnorderedMap& operator=(const UnorderedMap& other);
	UnorderedMap& operator=(UnorderedMap&& other) noexcept;

	bool operator==(const UnorderedMap& other) const;		// Contains the same elems, but not the same hashtable
	bool operator!=(const UnorderedMap& other) const;

public:
	// Main functions

	template<class _KeyType, class... Args>
	Iterator try_emplace(_KeyType&& key, Args&&... args);	// Force construction with known key and given arguments for object

	const MappedType& at(const Key& key) const;				// Access value at key with check
	MappedType& at(const Key& key);
}; // END UnorderedMap Template



// Definitions =================================================================================

// UnorderedMap Traits
template<class Key, class Type, class Hasher>
const typename UmapTraits<Key, Type, Hasher>::KeyType& UmapTraits<Key, Type, Hasher>::extract_key(const ValueType& value) noexcept {
	return value.First;
}

template<class Key, class Type, class Hasher>
const typename UmapTraits<Key, Type, Hasher>::MappedType& UmapTraits<Key, Type, Hasher>::extract_mapval(const ValueType& value) noexcept {
	return value.Second;
}
// END UnorderedMap Traits


// UnorderedMap Template
template<class Key, class Type, class Hasher>
UnorderedMap<Key, Type, Hasher>::UnorderedMap()
	:Base() { /*Empty*/ }

template<class Key, class Type, class Hasher>
UnorderedMap<Key, Type, Hasher>::UnorderedMap(const size_t& buckets)
	:Base(buckets) { /*Empty*/ }

template<class Key, class Type, class Hasher>
UnorderedMap<Key, Type, Hasher>::UnorderedMap(const UnorderedMap& other)
	:Base(other) { /*Empty*/ }

template<class Key, class Type, class Hasher>
UnorderedMap<Key, Type, Hasher>::UnorderedMap(UnorderedMap&& other) noexcept
	:Base(std::move(other)) { /*Empty*/ }

template<class Key, class Type, class Hasher>
UnorderedMap<Key, Type, Hasher>::~UnorderedMap() { /*Empty*/ }

template<class Key, class Type, class Hasher>
typename UnorderedMap<Key, Type, Hasher>::MappedType& UnorderedMap<Key, Type, Hasher>::operator[](const Key& key) {
	return try_emplace(key)->_Value.Second;
}

template<class Key, class Type, class Hasher>
typename UnorderedMap<Key, Type, Hasher>::MappedType& UnorderedMap<Key, Type, Hasher>::operator[](Key&& key) {
	return try_emplace(std::move(key))->_Value.Second;
}

template<class Key, class Type, class Hasher>
UnorderedMap<Key, Type, Hasher>& UnorderedMap<Key, Type, Hasher>::operator=(const UnorderedMap& other) {
	Base::operator=(other);

	return *this;
}

template<class Key, class Type, class Hasher>
UnorderedMap<Key, Type, Hasher>& UnorderedMap<Key, Type, Hasher>::operator=(UnorderedMap&& other) noexcept {
	Base::operator=(std::move(other));

	return *this;
}

template<class Key, class Type, class Hasher>
bool UnorderedMap<Key, Type, Hasher>::operator==(const UnorderedMap& other) const {
	return Base::operator==(other);
}

template<class Key, class Type, class Hasher>
bool UnorderedMap<Key, Type, Hasher>::operator!=(const UnorderedMap& other) const {
	return Base::operator!=(other);
}

template<class Key, class Type, class Hasher>
template<class _KeyType, class... Args>
typename UnorderedMap<Key, Type, Hasher>::Iterator UnorderedMap<Key, Type, Hasher>::try_emplace(_KeyType&& key, Args&&... args) {
	return this->_try_emplace(std::move(key), std::forward<Args>(args)...);
}

template<class Key, class Type, class Hasher>
const typename UnorderedMap<Key, Type, Hasher>::MappedType& UnorderedMap<Key, Type, Hasher>::at(const Key& key) const {
	return this->_at(key);
}

template<class Key, class Type, class Hasher>
typename UnorderedMap<Key, Type, Hasher>::MappedType& UnorderedMap<Key, Type, Hasher>::at(const Key& key) {
	return this->_at(key);
}
// END UnorderedMap Template

CUSTOM_END