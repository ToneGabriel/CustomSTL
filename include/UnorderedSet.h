#pragma once
#include "HashTable.h"

CUSTOM_BEGIN

template<class Key, class Hasher>
class UsetTraits					// UnorderedSet Traits
{
public:
	using KeyType		= Key;
	using MappedType	= Key;
	using HasherType	= Hasher;
	using ValueType		= MappedType;

public:

	UsetTraits() = default;

	static const KeyType& extract_key(const ValueType& value) {			// extract key from element value
		return value;
	}

	static const MappedType& extract_mapval(const ValueType& value) {	// extract key from element value
		return value;
	}
}; // END UnorderedSet Traits


template<class Key, class Hasher = std::hash<Key>>
class UnorderedSet : public HashTable<UsetTraits<Key, Hasher>>		// UnorderedSet Template
{
private:
	using Base = HashTable<UsetTraits<Key, Hasher>>;

public:
	using KeyType		= typename Base::KeyType;
	using MappedType	= typename Base::MappedType;
	using ValueType		= typename Base::ValueType;
	using Iterator		= typename Base::Iterator;
	using ConstIterator = typename Base::ConstIterator;

public:
	// Constructors

	UnorderedSet()
		:Base() { /*Empty*/ }

	UnorderedSet(const size_t& buckets)
		:Base(buckets) { /*Empty*/ }

	UnorderedSet(const UnorderedSet& other)
		:Base(other) { /*Empty*/ }

	UnorderedSet(UnorderedSet&& other) noexcept
		:Base(custom::move(other)) { /*Empty*/ }

	~UnorderedSet() = default;
	
public:
	// Operators

	UnorderedSet& operator=(const UnorderedSet& other) {
		Base::operator=(other);
		return *this;
	}

	UnorderedSet& operator=(UnorderedSet&& other) noexcept {
		Base::operator=(custom::move(other));
		return *this;
	}

	bool operator==(const UnorderedSet& other) const {				// Contains the same elems, but not the same hashtable
		return Base::operator==(other);
	}

	bool operator!=(const UnorderedSet& other) const {
		return Base::operator!=(other);
	}
}; // END UnorderedSet Template

CUSTOM_END