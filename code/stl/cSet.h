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
	using allocator_type 	= Alloc;

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
class Alloc		= custom::allocator<Key>>
class Set : public detail::_SearchTree<SetTraits<Key, Compare, Alloc>>		// Set Template
{
private:
	using _Base = detail::_SearchTree<SetTraits<Key, Compare, Alloc>>;

public:
	using KeyType 				= typename _Base::KeyType;
	using MappedType 			= typename _Base::MappedType;
	using KeyCompare			= typename _Base::KeyCompare;
	using ValueType 			= typename _Base::ValueType;
	using reference 			= typename _Base::reference;
	using const_reference 		= typename _Base::const_reference;
	using pointer 				= typename _Base::pointer;
	using const_pointer 			= typename _Base::const_pointer;
	using allocator_type 		= typename _Base::allocator_type;

	using iterator				= typename _Base::iterator;
	using const_iterator			= typename _Base::const_iterator;
	using reverse_iterator		= typename _Base::reverse_iterator;
	using const_reverse_iterator	= typename _Base::const_reverse_iterator;

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