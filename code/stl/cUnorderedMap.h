#pragma once
#include "xHashTable.h"

CUSTOM_BEGIN

template<class Key, class Type, class hash, class Compare, class Alloc>
class UmapTraits		// UnorderedMap Traits
{
public:
	using KeyType 			= Key;
	using MappedType 		= Type;
	using Hasher 			= hash;
	using KeyEqual 			= Compare;
	using ValueType 		= pair<Key, Type>;
	using allocator_type 	= Alloc;

public:

	UmapTraits() = default;

	static const KeyType& extract_key(const ValueType& value) noexcept {		// extract key from element value
		return value.first;
	}

	static const MappedType& extract_mapval(const ValueType& value) noexcept {	// extract mapped val from element value
		return value.second;
	}
}; // END UnorderedMap Traits


template<class Key, class Type,
class hash 		= custom::hash<Key>,
class Compare 	= custom::EqualTo<Key>,
class Alloc 	= custom::allocator<custom::pair<Key, Type>>>
class UnorderedMap : public detail::_HashTable<UmapTraits<Key, Type, hash, Compare, Alloc>>	// UnorderedMap Template
{
private:
	using _Base = detail::_HashTable<UmapTraits<Key, Type, hash, Compare, Alloc>>;

public:
	static_assert(is_same_v<pair<Key, Type>, typename Alloc::ValueType>, "Object type and allocator type must be the same!");
	static_assert(is_object_v<Key>, "Containers require object type!");

	using KeyType 			= typename _Base::KeyType;
	using MappedType 		= typename _Base::MappedType;
	using Hasher 			= typename _Base::Hasher;
	using KeyEqual 			= typename _Base::KeyEqual;
	using ValueType 		= typename _Base::ValueType;
	using reference 		= typename _Base::reference;
	using const_reference 	= typename _Base::const_reference;
	using pointer 			= typename _Base::pointer;
	using const_pointer 		= typename _Base::const_pointer;
	using allocator_type 	= typename _Base::allocator_type;

	using iterator			= typename _Base::iterator;
	using const_iterator 	= typename _Base::const_iterator;

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
		return this->_try_emplace(key).first->second;
	}

	MappedType& operator[](KeyType&& key) {
		return this->_try_emplace(custom::move(key)).first->second;
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
	pair<iterator, bool> try_emplace(const KeyType& key, Args&&... args) {	// Force construction with known key and given arguments for object
		return this->_try_emplace(key, custom::forward<Args>(args)...);
	}

	template<class... Args>
	pair<iterator, bool> try_emplace(KeyType&& key, Args&&... args) {
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