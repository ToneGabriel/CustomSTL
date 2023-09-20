#pragma once
#include "xSearchTree.h"

CUSTOM_BEGIN

template<class Key, class Compare, class Alloc>
class SetTraits										// Set Traits
{
public:
	using KeyType 			= Key;
	using MappedType 		= Key;
	using KeyCompare 		= Compare;
	using ValueType 		= MappedType;
	using Reference 		= ValueType&;
	using ConstReference 	= const Reference;
	using Pointer 			= ValueType*;
	using ConstPointer 		= const Pointer;
	using AllocatorType 	= Alloc;

public:

	SetTraits() = default;

	static const KeyType& extract_key(const ValueType& value) noexcept {		// extract key from element value
		return value;
	}

	static const MappedType& extract_mapval(const ValueType& value) noexcept {	// extract mapped val from element value
		return value;
	}
}; // END Set Traits


template<class Key,
class Compare 	= custom::Less<Key>,
class Alloc		= custom::Allocator<Key>>
class Set : public _SearchTree<SetTraits<Key, Compare, Alloc>>		// Set Template
{
private:
	using Base = _SearchTree<SetTraits<Key, Compare, Alloc>>;

public:
	using KeyType				= typename Base::KeyType;
	using MappedType			= typename Base::MappedType;
	using ValueType				= typename Base::ValueType;

	using Iterator				= typename Base::Iterator;
	using ConstIterator			= typename Base::ConstIterator;
	using ReverseIterator		= typename Base::ReverseIterator;
	using ConstReverseIterator	= typename Base::ConstReverseIterator;

public:
    // Constructors

	Set()
		:Base() { /*Empty*/ }

	Set(const Set& other)
		: Base(other) { /*Empty*/ }

	Set(Set&& other) noexcept
		: Base(custom::move(other)) { /*Empty*/ }

	~Set() { /*Empty*/ }

public:
    // Operators

	Set& operator=(const Set& other) {
		Base::operator=(other);
		return *this;
	}

	Set& operator=(Set&& other) noexcept {
		Base::operator=(custom::move(other));
		return *this;
	}

	bool operator==(const Set& other) const {
		return Base::operator==(other);
	}

	bool operator!=(const Set& other) const {
		return Base::operator!=(other);
	}
}; // END Set Template

CUSTOM_END