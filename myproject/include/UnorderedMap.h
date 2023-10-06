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
	using ValueType 		= Pair<Key, Type>;
	using Reference 		= ValueType&;
	using ConstReference 	= const ValueType&;
	using Pointer 			= ValueType*;
	using ConstPointer 		= const ValueType*;
	using AllocatorType 	= Alloc;

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
class Hash 		= custom::Hash<Key>,
class Compare 	= custom::EqualTo<Key>,
class Alloc 	= custom::Allocator<custom::Pair<Key, Type>>>
class UnorderedMap : public _HashTable<UmapTraits<Key, Type, Hash, Compare, Alloc>>	// UnorderedMap Template
{
private:
	using _Base = _HashTable<UmapTraits<Key, Type, Hash, Compare, Alloc>>;

public:
	static_assert(IsSame_v<Pair<Key, Type>, typename Alloc::ValueType>, "Object type and Allocator type must be the same!");
	static_assert(IsObject_v<Key>, "Containers require object type!");

	using KeyType 			= typename _Base::KeyType;
	using MappedType 		= typename _Base::MappedType;
	using Hasher 			= typename _Base::Hasher;
	using KeyEqual 			= typename _Base::KeyEqual;
	using ValueType 		= typename _Base::ValueType;
	using Reference 		= typename _Base::Reference;
	using ConstReference 	= typename _Base::ConstReference;
	using Pointer 			= typename _Base::Pointer;
	using ConstPointer 		= typename _Base::ConstPointer;
	using AllocatorType 	= typename _Base::AllocatorType;

	using Iterator			= typename _Base::Iterator;
	using ConstIterator 	= typename _Base::ConstIterator;

public:
	// Constructors

	UnorderedMap()
		:_Base() { /*Empty*/ }

	UnorderedMap(const size_t buckets)
		:_Base(buckets) { /*Empty*/ }

	UnorderedMap(const UnorderedMap& other)
		:_Base(other) { /*Empty*/ }

	UnorderedMap(UnorderedMap&& other) noexcept
		:_Base(custom::move(other)) { /*Empty*/ }

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
		_Base::operator=(other);
		return *this;
	}

	UnorderedMap& operator=(UnorderedMap&& other) noexcept {
		_Base::operator=(custom::move(other));
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