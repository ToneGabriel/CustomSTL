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
	using ConstReference 	= const ValueType&;
	using Pointer 			= ValueType*;
	using ConstPointer 		= const ValueType*;
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
	using _Base = _SearchTree<SetTraits<Key, Compare, Alloc>>;

public:
	using KeyType 				= typename _Base::KeyType;
	using MappedType 			= typename _Base::MappedType;
	using KeyCompare			= typename _Base::KeyCompare;
	using ValueType 			= typename _Base::ValueType;
	using Reference 			= typename _Base::Reference;
	using ConstReference 		= typename _Base::ConstReference;
	using Pointer 				= typename _Base::Pointer;
	using ConstPointer 			= typename _Base::ConstPointer;
	using AllocatorType 		= typename _Base::AllocatorType;

	using Iterator				= typename _Base::Iterator;
	using ConstIterator			= typename _Base::ConstIterator;
	using ReverseIterator		= typename _Base::ReverseIterator;
	using ConstReverseIterator	= typename _Base::ConstReverseIterator;

public:
    // Constructors

	Set()
		:_Base() { /*Empty*/ }

	Set(const Set& other)
		: _Base(other) { /*Empty*/ }

	Set(Set&& other) noexcept
		: _Base(custom::move(other)) { /*Empty*/ }

	~Set() { /*Empty*/ }

public:
    // Operators

	Set& operator=(const Set& other) {
		_Base::operator=(other);
		return *this;
	}

	Set& operator=(Set&& other) noexcept {
		_Base::operator=(custom::move(other));
		return *this;
	}
}; // END Set Template

CUSTOM_END