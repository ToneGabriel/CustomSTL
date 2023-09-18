#pragma once
#include "xHashTable.h"

CUSTOM_BEGIN

template<class Key, class Hash, class Compare, class Alloc>
class UsetTraits					// UnorderedSet Traits
{
public:
	using KeyType			= Key;
	using MappedType		= Key;
	using Hasher 			= Hash;
	using KeyEqual 			= Compare;
	using AllocatorType 	= Alloc;
	using ValueType			= MappedType;
	using Reference 		= ValueType&;
	using ConstReference 	= const Reference;
	using Pointer 			= ValueType*;
	using ConstPointer 		= const Pointer;

public:

	UsetTraits() = default;

	static const KeyType& extract_key(const ValueType& value) {			// extract key from element value
		return value;
	}

	static const MappedType& extract_mapval(const ValueType& value) {	// extract key from element value
		return value;
	}
}; // END UnorderedSet Traits


template<class Key,
class Hash 		= std::hash<Key>,
class Compare 	= custom::EqualTo<Key>,
class Alloc 	= custom::Allocator<Key>>
class UnorderedSet : public _HashTable<UsetTraits<Key, Hash, Compare, Alloc>>		// UnorderedSet Template
{
private:
	using Base = _HashTable<UsetTraits<Key, Hash, Compare, Alloc>>;

public:
	using KeyType 			= typename Base::KeyType;
	using MappedType 		= typename Base::MappedType;
	using Hasher 			= typename Base::Hasher;
	using KeyEqual 			= typename Base::KeyEqual;
	using ValueType 		= typename Base::ValueType;
	using Reference 		= typename Base::Reference;
	using ConstReference 	= typename Base::ConstReference;
	using Pointer 			= typename Base::Pointer;
	using ConstPointer 		= typename Base::ConstPointer;
	using AllocatorType 	= typename Base::AllocatorType;

	using Iterator			= typename Base::Iterator;
	using ConstIterator 	= typename Base::ConstIterator;

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