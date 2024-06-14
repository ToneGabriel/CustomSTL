#pragma once
#include "xHashTable.h"

CUSTOM_BEGIN

template<class Key, class hash, class Compare, class Alloc>
class UsetTraits					// UnorderedSet Traits
{
public:
	using KeyType			= Key;
	using MappedType		= Key;
	using Hasher 			= hash;
	using KeyEqual 			= Compare;
	using ValueType			= MappedType;
	using allocator_type 	= Alloc;

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
class hash 		= custom::hash<Key>,
class Compare 	= custom::EqualTo<Key>,
class Alloc 	= custom::allocator<Key>>
class UnorderedSet : public detail::_HashTable<UsetTraits<Key, hash, Compare, Alloc>>		// UnorderedSet Template
{
private:
	using _Base = detail::_HashTable<UsetTraits<Key, hash, Compare, Alloc>>;

public:
	static_assert(is_same_v<Key, typename Alloc::ValueType>, "Object type and Allocator type must be the same!");
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

	UnorderedSet()
		:_Base() { /*Empty*/ }

	UnorderedSet(const size_t buckets)
		:_Base(buckets) { /*Empty*/ }

	UnorderedSet(const UnorderedSet& other)
		:_Base(other) { /*Empty*/ }

	UnorderedSet(UnorderedSet&& other) noexcept
		:_Base(custom::move(other)) { /*Empty*/ }

	~UnorderedSet() = default;
	
public:
	// Operators

	UnorderedSet& operator=(const UnorderedSet& other) {
		_Base::operator=(other);
		return *this;
	}

	UnorderedSet& operator=(UnorderedSet&& other) noexcept {
		_Base::operator=(custom::move(other));
		return *this;
	}
}; // END UnorderedSet Template

CUSTOM_END