#pragma once
#include "SearchTree.h"

CUSTOM_BEGIN

template<class Key, class Compare>
class SetTraits										// Set Traits
{
public:
	using KeyType		= Key;
	using MappedType	= Key;
	using KeyCompare	= Compare;
	using ValueType		= MappedType;

public:

	SetTraits() = default;

	static const KeyType& extract_key(const ValueType& value) noexcept {		// extract key from element value
		return value;
	}

	static const MappedType& extract_mapval(const ValueType& value) noexcept {	// extract mapped val from element value
		return value;
	}
}; // END Set Traits


template<class Key, class Compare = std::less<Key>>
class Set : public SearchTree<SetTraits<Key, Compare>>		// Set Template
{
private:
	using Base = SearchTree<SetTraits<Key, Compare>>;

public:
	using KeyType		= typename Base::KeyType;
	using MappedType	= typename Base::MappedType;
	using ValueType		= typename Base::ValueType;
	using Iterator		= typename Base::Iterator;

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