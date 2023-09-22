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
	using ValueType			= MappedType;
	using Reference 		= ValueType&;
	using ConstReference 	= const Reference;
	using Pointer 			= ValueType*;
	using ConstPointer 		= const Pointer;
	using AllocatorType 	= Alloc;

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
	using _Base = _HashTable<UsetTraits<Key, Hash, Compare, Alloc>>;

public:
	static_assert(IsSame_v<Key, typename Alloc::ValueType>, "Object type and Allocator type must be the same!");
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

	UnorderedSet()
		:_Base() { /*Empty*/ }

	UnorderedSet(const size_t& buckets)
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