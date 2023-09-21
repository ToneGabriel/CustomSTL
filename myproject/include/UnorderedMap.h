#pragma once
#include "xHashTable.h"

CUSTOM_BEGIN

template<class Key, class Type, class Hash, class Compare, class Alloc>
class UmapTraits		// UnorderedMap Traits
{
public:
	using KeyType 			= Key;
	using MappedType 		= Type;
	using Hasher 			= Hash;
	using KeyEqual 			= Compare;
	using AllocatorType 	= Alloc;
	using ValueType 		= Pair<Key, Type>;
	using Reference 		= ValueType&;
	using ConstReference 	= const Reference;
	using Pointer 			= ValueType*;
	using ConstPointer 		= const Pointer;

public:

	UmapTraits() = default;

	static const KeyType& extract_key(const ValueType& value) noexcept {		// extract key from element value
		return value.First;
	}

	static const MappedType& extract_mapval(const ValueType& value) noexcept {	// extract mapped val from element value
		return value.Second;
	}
}; // END UnorderedMap Traits


template<class Key, class Type,
class Hash 		= std::hash<Key>,
class Compare 	= custom::EqualTo<Key>,
class Alloc 	= custom::Allocator<custom::Pair<Key, Type>>>
class UnorderedMap : public _HashTable<UmapTraits<Key, Type, Hash, Compare, Alloc>>	// UnorderedMap Template
{
private:
	using Base = _HashTable<UmapTraits<Key, Type, Hash, Compare, Alloc>>;

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

	MappedType& operator[](const KeyType& key) {				// Access value or create new one with key and assignment
		return this->_try_emplace(key).First->Second;
	}

	MappedType& operator[](KeyType&& key) {
		return this->_try_emplace(custom::move(key)).First->Second;
	}

	UnorderedMap& operator=(const UnorderedMap& other) {
		Base::operator=(other);
		return *this;
	}

	UnorderedMap& operator=(UnorderedMap&& other) noexcept {
		Base::operator=(custom::move(other));
		return *this;
	}

public:
	// Main functions

	template<class... Args>
	Pair<Iterator, bool> try_emplace(const KeyType& key, Args&&... args) {	// Force construction with known key and given arguments for object
		return this->_try_emplace(key, custom::forward<Args>(args)...);
	}

	template<class... Args>
	Pair<Iterator, bool> try_emplace(KeyType&& key, Args&&... args) {
		return this->_try_emplace(custom::move(key), custom::forward<Args>(args)...);
	}

	const MappedType& at(const KeyType& key) const {			// Access value at key with check
		return this->_at(key);
	}

	MappedType& at(const KeyType& key) {
		return this->_at(key);
	}
}; // END UnorderedMap Template

CUSTOM_END