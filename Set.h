#pragma once
#include "SearchTree.h"

CUSTOM_BEGIN

// Headings =================================================================================

template<class Key, class Type, class Compare>
class SetTraits										// Set Traits
{
public:
	using KeyType		= Key;
	using MappedType	= Type;
	using KeyCompare	= Compare;
	using ValueType		= Pair<Key, Type>;

public:

	SetTraits() = default;

	static const KeyType& extract_key(const ValueType& value) noexcept;			// extract key from element value
	static const MappedType& extract_mapval(const ValueType& value) noexcept;	// extract mapped val from element value
}; // END Set Traits


template<class Key, class Compare = std::less<Key>>
class Set : public SearchTree<SetTraits<Key, Key, Compare>>		// Set Template
{
private:
	using Base = SearchTree<SetTraits<Key, Key, Compare>>;

public:
	using KeyType		= typename Base::KeyType;
	using MappedType	= typename Base::MappedType;
	using ValueType		= typename Base::ValueType;
	using Iterator		= typename Base::Iterator;

public:
    // Constructors

	Set();
	Set(const Set& other);
	Set(Set&& other) noexcept;
	~Set();

public:
    // Operators

	Set& operator=(const Set& other);
	Set& operator=(Set&& other) noexcept;

	bool operator==(const Set& other) const;
	bool operator!=(const Set& other) const;
}; // END Set Template



// Definitions =================================================================================

// Set Traits
template<class Key, class Type, class Compare>
const typename SetTraits<Key, Type, Compare>::KeyType& SetTraits<Key, Type, Compare>::extract_key(const ValueType& value) noexcept {
	return value;
}

template<class Key, class Type, class Compare>
const typename SetTraits<Key, Type, Compare>::MappedType& SetTraits<Key, Type, Compare>::extract_mapval(const ValueType& value) noexcept {
	return value;
}
// END Set Traits

// Set Template
template<class Key, class Compare>
Set<Key, Compare>::Set()
	:Base() { /*Empty*/ }

template<class Key, class Compare>
Set<Key, Compare>::Set(const Set& other)
	: Base(other) { /*Empty*/ }

template<class Key, class Compare>
Set<Key, Compare>::Set(Set&& other) noexcept
	: Base(custom::move(other)) { /*Empty*/ }

template<class Key, class Compare>
Set<Key, Compare>::~Set() { /*Empty*/ }

template<class Key, class Compare>
Set<Key, Compare>& Set<Key, Compare>::operator=(const Set& other) {
	Base::operator=(other);
	return *this;
}

template<class Key, class Compare>
Set<Key, Compare>& Set<Key, Compare>::operator=(Set&& other) noexcept {
	Base::operator=(custom::move(other));
	return *this;
}

template<class Key, class Compare>
bool Set<Key, Compare>::operator==(const Set& other) const {
	return Base::operator==(other);
}

template<class Key, class Compare>
bool Set<Key, Compare>::operator!=(const Set& other) const {
	return Base::operator!=(other);
}
// END Set Template
CUSTOM_END