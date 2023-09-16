#pragma once
#include "xHashTable.h"

CUSTOM_BEGIN

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

	static const KeyType& extract_key(const ValueType& value) noexcept {		// extract key from element value
		return value.First;
	}

	static const MappedType& extract_mapval(const ValueType& value) noexcept {	// extract mapped val from element value
		return value.Second;
	}
}; // END UnorderedMap Traits


template<class Key, class Type, class Hasher = std::hash<Key>>
class UnorderedMap : public _HashTable<UmapTraits<Key, Type, Hasher>>	// UnorderedMap Template
{
private:
	using Base = _HashTable<UmapTraits<Key, Type, Hasher>>;

public:
	using KeyType 		= typename Base::KeyType;
	using MappedType 	= typename Base::MappedType;
	using ValueType 	= typename Base::ValueType;
	using Iterator		= typename Base::Iterator;
	using ConstIterator = typename Base::ConstIterator;

public:
	// Constructors

	UnorderedMap()
		:Base() { /*Empty*/ }

	UnorderedMap(const size_t& buckets)
		:Base(buckets) { /*Empty*/ }

	UnorderedMap(const UnorderedMap& other)
		:Base(other) { /*Empty*/ }

	UnorderedMap(UnorderedMap&& other) noexcept
		:Base(custom::move(other)) { /*Empty*/ }

	~UnorderedMap() = default;

public:
	// Operators

	MappedType& operator[](const Key& key) {				// Access value or create new one with key and assignment (no const)
		return try_emplace(key)->Second;
	}

	MappedType& operator[](Key&& key) {
		return try_emplace(custom::move(key))->Second;
	}

	UnorderedMap& operator=(const UnorderedMap& other) {
		Base::operator=(other);
		return *this;
	}

	UnorderedMap& operator=(UnorderedMap&& other) noexcept {
		Base::operator=(custom::move(other));
		return *this;
	}

	bool operator==(const UnorderedMap& other) const {		// Contains the same elems, but not the same hashtable
		return Base::operator==(other);
	}

	bool operator!=(const UnorderedMap& other) const {
		return Base::operator!=(other);
	}

public:
	// Main functions

	template<class _KeyType, class... Args>
	Iterator try_emplace(_KeyType&& key, Args&&... args) {	// Force construction with known key and given arguments for object
		return this->_try_emplace(custom::move(key), custom::forward<Args>(args)...);
	}

	const MappedType& at(const Key& key) const {			// Access value at key with check
		return this->_at(key);
	}

	MappedType& at(const Key& key) {
		return this->_at(key);
	}
}; // END UnorderedMap Template

CUSTOM_END