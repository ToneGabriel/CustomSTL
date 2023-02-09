#pragma once
#include "HashTable.h"

CUSTOM_BEGIN

// Headings =================================================================================

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

	static const KeyType& extract_key(const ValueType& value);			// extract key from element value
	static const MappedType& extract_mapval(const ValueType& value);	// extract key from element value
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

public:
	// Constructors

	UnorderedSet();
	UnorderedSet(const size_t& buckets);
	UnorderedSet(const UnorderedSet& other);
	UnorderedSet(UnorderedSet&& other) noexcept;
	~UnorderedSet();
	
public:
	// Operators

	UnorderedSet& operator=(const UnorderedSet& other);
	UnorderedSet& operator=(UnorderedSet&& other) noexcept;

	bool operator==(const UnorderedSet& other) const;				// Contains the same elems, but not the same hashtable
	bool operator!=(const UnorderedSet& other) const;
}; // END UnorderedSet Template



// Definitions =================================================================================

// UnorderedSet Traits
template<class Key, class Hasher>
const typename UsetTraits<Key, Hasher>::KeyType& UsetTraits<Key, Hasher>::extract_key(const ValueType& value) {
	return value;
}

template<class Key, class Hasher>
const typename UsetTraits<Key, Hasher>::MappedType& UsetTraits<Key, Hasher>::extract_mapval(const ValueType& value) {
	return value;
}
// END UnorderedSet Traits

// UnorderedSet Template
template<class Key, class Hasher>
UnorderedSet<Key, Hasher>::UnorderedSet()
	:Base() { /*Empty*/ }

template<class Key, class Hasher>
UnorderedSet<Key, Hasher>::UnorderedSet(const size_t& buckets)
	:Base(buckets) { /*Empty*/ }

template<class Key, class Hasher>
UnorderedSet<Key, Hasher>::UnorderedSet(const UnorderedSet& other)
	:Base(other) { /*Empty*/ }

template<class Key, class Hasher>
UnorderedSet<Key, Hasher>::UnorderedSet(UnorderedSet&& other) noexcept
	:Base(std::move(other)) { /*Empty*/ }

template<class Key, class Hasher>
UnorderedSet<Key, Hasher>::~UnorderedSet() { /*Empty*/ }

template<class Key, class Hasher>
UnorderedSet<Key, Hasher>& UnorderedSet<Key, Hasher>::operator=(const UnorderedSet& other) {
	Base::operator=(other);

	return *this;
}

template<class Key, class Hasher>
UnorderedSet<Key, Hasher>& UnorderedSet<Key, Hasher>::operator=(UnorderedSet&& other) noexcept {
	Base::operator=(std::move(other));

	return *this;
}

template<class Key, class Hasher>
bool UnorderedSet<Key, Hasher>::operator==(const UnorderedSet& other) const {				// Contains the same elems, but not the same hashtable
	return Base::operator==(other);
}

template<class Key, class Hasher>
bool UnorderedSet<Key, Hasher>::operator!=(const UnorderedSet& other) const {
	return Base::operator!=(other);
}
// END UnorderedSet Template

CUSTOM_END