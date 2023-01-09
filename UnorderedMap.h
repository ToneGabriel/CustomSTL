#pragma once
#include "HashTable.h"
#include "Utility.h"

#include <iostream>
#include <cmath>
#include <functional>

CUSTOM_BEGIN

// UnorderedMap Traits ==============================================
template<class Key, class Type, class Hasher = std::hash<Key>>
class UmapTraits
{
public:
	using KeyType 		= Key;
	using MappedType 	= Type;
	using HasherType 	= Hasher;
	using ValueType 	= Pair<Key, Type>;

public:

	UmapTraits() = default;
};
// UnorderedMap Traits ==============================================
// END

// UnorderedMap ========================================================
template<class Key, class Type, class Hasher = std::hash<Key>>
class UnorderedMap : public HashTable<UmapTraits<Key, Type, Hasher>>
{
private:
	using Base = HashTable<UmapTraits<Key, Type, Hasher>>;

public:
	using KeyType 		= typename Base::KeyType;
	using MappedType 	= typename Base::MappedType;
	using ValueType 	= typename Base::ValueType;
	using Iterator		= typename Base::Iterator;

private:
	using BucketIterator = typename Base::BucketIterator;

public:
	// Constructors

	UnorderedMap() 
		:Base() { }

	UnorderedMap(const size_t& buckets)
		:Base(buckets) { }

	UnorderedMap(const UnorderedMap& other)
		:Base(other) { }

	UnorderedMap(UnorderedMap&& other) noexcept
		:Base(other) { }

	~UnorderedMap() = default;

public:
	// Main functions

	template<class _KeyType, class... Args>
	Iterator try_emplace(_KeyType&& key, Args&&... args) {				// Force construction with known key and given arguments for object
		return this->_try_emplace(std::move(key), std::forward<Args>(args)...);
	}

	const Type& at(const Key& key) const {								// Access Value at key with check
		BucketIterator it = this->find_in_array(key);
		if (it == this->_buckets[this->bucket(key)].end())
			throw std::out_of_range("Invalid key...");

		return (*it)->Value.Second;
	}

	Type& at(const Key& key) {
		BucketIterator it = this->find_in_array(key);
		if (it == this->_buckets[bucket(key)].end())
			throw std::out_of_range("Invalid key...");

		return (*it)->Value.Second;
	}

	void print_details() const {										// For Debugging
		std::cout << "Capacity= " << this->_buckets.size() << ' ' << "Size= " << this->_elems.size() << '\n';
		for (size_t i = 0; i < this->_buckets.size(); i++)
		{
			std::cout << i << " : ";
			for (const auto& val : this->_buckets[i])
				std::cout << val->Value.First << ' ' << val->Value.Second << '\\';
			std::cout << '\n';
		}
	}

	
public:
	// Operators

	Type& operator[](const Key& key) {									// Access value or create new one with key and assignment (no const)
		return try_emplace(key)->Value.Second;
	}

	Type& operator[](Key&& key) {
		return try_emplace(std::move(key))->Value.Second;
	}

	UnorderedMap& operator=(const UnorderedMap& other) {
		Base::operator=(other);

		return *this;
	}

	UnorderedMap& operator=(UnorderedMap&& other) noexcept {
		Base::operator=(std::move(other));

		return *this;
	}

	bool operator==(const UnorderedMap& other) const {		// Contains the same elems, but not the same hashtable
		if (this->size() != other.size())
			return false;

		for (const ValueType& val : other)
		{
			Iterator it = this->find(val.First);		// Search for key
			if (it == this->end() || (*it).Second != val.Second)
				return false;
		}

		return true;
	}

	bool operator!=(const UnorderedMap& other) const {
		return !operator==(other);
	}

	friend std::ostream& operator<<(std::ostream& os, const UnorderedMap& map) {
		os << "S=" << map.size() << " B=" << map.bucket_count();
		return os;
	}

protected:
	KeyType& _get_key(ValueType& value) override {
		return value.First;
	}

	virtual const KeyType& _get_key(const ValueType& value) const {
		return value.First;
	}

};
// UnorderedMap ========================================================
// END

CUSTOM_END