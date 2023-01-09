#pragma once
#include "HashTable.h"

CUSTOM_BEGIN

// UnorderedSet Traits ==============================================
template<class Key, class Hasher>
class UsetTraits
{
public:
	using KeyType		= Key;
	using MappedType	= Key;
	using HasherType	= Hasher;
	using ValueType		= MappedType;

public:

	UsetTraits() = default;
};
// Unordered Traits ==============================================
// END

// UnorderedSet ========================================================
template<class Key, class Hasher = std::hash<Key>>
class UnorderedSet : public HashTable<UsetTraits<Key, Hasher>>
{
private:
	using Base = HashTable<UsetTraits<Key, Hasher>>;

public:
	using KeyType		= typename Base::KeyType;
	using MappedType	= typename Base::MappedType;
	using ValueType		= typename Base::ValueType;
	using Iterator		= typename Base::Iterator;

public:
	// Constructors

	UnorderedSet()
		:Base() { }

	UnorderedSet(const size_t& buckets)
		:Base(buckets) { }

	UnorderedSet(const UnorderedSet& other)
		:Base(other) { }

	UnorderedSet(UnorderedSet&& other) noexcept
		:Base(std::move(other)) { }

	~UnorderedSet() { }
	
public:
	// Operators

	UnorderedSet& operator=(const UnorderedSet& other) {
		Base::operator=(other);

		return *this;
	}

	UnorderedSet& operator=(UnorderedSet&& other) noexcept {
		Base::operator=(std::move(other));

		return *this;
	}

	bool operator==(const UnorderedSet& other) const {				// Contains the same elems, but not the same hashtable
		return Base::operator==(other);
	}

	bool operator!=(const UnorderedSet& other) const {
		return !operator==(other);
	}

protected:
	// get key, value interface override

	KeyType& _get_key(ValueType& value) override {
		return value;
	}

	virtual const KeyType& _get_key(const ValueType& value) const {
		return value;
	}

	virtual MappedType& _get_mval(ValueType& value) {
		return value;
	}

	virtual const MappedType& _get_mval(const ValueType& value) const {
		return value;
	}
};
// UnorderedSet ========================================================
// END

CUSTOM_END