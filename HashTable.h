#pragma once
#include "Common.h"
#include "List.h"
#include "Vector.h"
#include "Utility.h"

#include <iostream>
#include <cmath>
#include <functional>

CUSTOM_BEGIN

template<class Traits>
class HashTable
{
protected:
    using KeyType           = typename Traits::KeyType;
    using MappedType        = typename Traits::MappedType;
    using Hasher            = typename Traits::HasherType;

	using IterList			= List<typename Traits::ValueType>;	// List of ValueType used for iterating
	using Node				= typename IterList::Node;			// Node component from List
	using Bucket			= List<Node*>;						// List of Node* (as Value) from Iteration list
	using HashArray			= Vector<Bucket>;					// Array of lists of Node*
	using BucketIterator	= typename Bucket::Iterator;		// Iterator for Buckets

public:
	using ValueType = typename Traits::ValueType;				// Type of values stored in container
	using Iterator	= typename List<ValueType>::Iterator;		// Iterator for this container (identical to List iterator)

protected:
	static constexpr float _maxLoadFactor	= 0.75;				// The maximum load factor admitted before rehashing
	static constexpr size_t _defaultBuckets = 8;				// Default number of buckets

	Hasher _hash;												// Used for initial(non-compressed) hash value
	IterList _elems;											// Used to iterate through container
	HashArray _buckets;											// Used to map IterList values

protected:
    // Constructors

    HashTable() {
		rehash(_defaultBuckets);
	}

	HashTable(const size_t& buckets) {
		rehash((buckets < _defaultBuckets) ? _defaultBuckets : buckets);
	}

	HashTable(const HashTable& other) {
		_elems = other._elems;
		force_rehash(other.bucket_count());
	}

	HashTable(HashTable&& other) noexcept {
		_elems = std::move(other._elems);
		force_rehash(other.bucket_count());
	}

	~HashTable() {
		clear();
	}

public:
    // Main functions

    template<class... Args>
	Iterator emplace(Args&&... args) {									// Constructs Node first with any given arguments
		Node* newNode = new Node(std::forward<Args>(args)...);
		KeyType& newKey = _get_key(newNode->Value);
		Iterator it = find(newKey);

		if (it != end()) {												// Destroy newly-created Node if key exists
			delete newNode;
			return it;
		}
		else {
			rehash_if_overload();
			_elems.insert_node_before(_elems.end()._Ptr, newNode);
			_buckets[bucket(newKey)].emplace_back(newNode);
			return Iterator(newNode, _elems.update_iteration_data());
		}
	}

	Iterator erase(const KeyType& key) {
		size_t index = bucket(key);
		BucketIterator it = find_in_array(key);

		if (it == _buckets[index].end())
			return end();

		Node* nodeToErase = (*it);
		_buckets[index].pop(it);													// Remove reference from array of lists
		return _elems.pop(Iterator(nodeToErase, _elems.update_iteration_data()));	// Remove value from iteration list and return next Node iterator
	}

	Iterator erase(const Iterator& iterator) {
		if (iterator == end())
			throw std::out_of_range("Map erase iterator outside range...");

		return erase(iterator._Ptr->Value);
	}

	Iterator find(const KeyType& key) const {
		BucketIterator it = find_in_array(key);
		if (it == _buckets[bucket(key)].end())
			return end();

		return Iterator(*it, _elems.update_iteration_data());
	}

	void rehash(const size_t& buckets) {											// rebuild table with at least buckets
		size_t newBucketCount = std::max(min_load_factor_buckets(size()), buckets);	// don't violate bucket_count() >= size() / max_load_factor()
		if (newBucketCount <= bucket_count())
			return;

		force_rehash(newBucketCount);
	}

	void reserve(const size_t& size) {									// rehash for at least "size" elements
		rehash(min_load_factor_buckets(size));
	}

	void clear() {
		_elems.clear();													// Delete all Node* with values
		_buckets.clear();												// Remove lists with Node* references
	}

	size_t bucket_count() const {										// Get HashArray size
		return _buckets.size();
	}

	size_t bucket_size(const size_t& index) const {						// Get the size of the bucket at index
		return _buckets[index].size();
	}

	size_t bucket(const KeyType& key) const {								// Get bucket index from key
		return _hash(key) % bucket_count();
	}

	bool empty() const {
		return _elems.empty();
	}

	size_t size() const {
		return _elems.size();
	}

	float load_factor() const {
		return static_cast<float>(size()) / static_cast<float>(bucket_count());
	}

	float max_load_factor() const {
		return _maxLoadFactor;
	}

public:
	// Iterator functions

	Iterator begin() {
		return _elems.begin();
	}

	const Iterator begin() const {
		return _elems.begin();
	}

	Iterator end() {
		return _elems.end();
	}

	const Iterator end() const {
		return _elems.end();
	}

protected:

	HashTable& operator=(const HashTable& other) {
		_elems = other._elems;
		force_rehash(other.bucket_count());

		return *this;
	}

	HashTable& operator=(HashTable&& other) noexcept {
		_elems = std::move(other._elems);
		force_rehash(other.bucket_count());

		return *this;
	}

protected:
	// Others

	virtual KeyType& _get_key(ValueType& value) = 0;

	virtual const KeyType& _get_key(const ValueType& value) const = 0;

	template<class _KeyType, class... Args>
	Iterator _try_emplace(_KeyType&& key, Args&&... args) {				// Force construction with known key and given arguments for object
		Iterator it = this->find(key);									// Check key and decide to construct or not

		if (it != this->end())
			return it;
		else {
			Node* newNode = new Node(
							piecewise_construct,
							forward_as_tuple(std::forward<_KeyType>(key)),
							forward_as_tuple(std::forward<Args>(args)...)
							);
			_KeyType& newKey = newNode->Value.First;

			this->rehash_if_overload();
			this->_elems.insert_node_before(this->_elems.end()._Ptr, newNode);
			this->_buckets[this->bucket(newKey)].emplace_back(newNode);
			return Iterator(newNode, this->_elems.update_iteration_data());
		}
	}

	BucketIterator find_in_array(const KeyType& key) const {				// Get iterator from bucket with key
		const Bucket& currentBucket = _buckets[bucket(key)];
		BucketIterator it = currentBucket.begin();
		while (it != currentBucket.end() && _get_key((*it)->Value) != key)
			it++;

		return it;
	}

	void force_rehash(const size_t& buckets) {
		_buckets.clear();
		_buckets.resize(buckets);

		for (auto it = _elems.begin(); it != _elems.end(); ++it)
			_buckets[bucket(_get_key(it->Value))].push_back(it._Ptr);
	}

	void rehash_if_overload() {											// Check load factor and rehash if needed
		if (static_cast<float>(size() + 1) / static_cast<float>(bucket_count()) > _maxLoadFactor)
			force_rehash(2 * bucket_count());
	}

	size_t min_load_factor_buckets(const size_t& size) const {			// returns the minimum number of buckets necessary for the elements in List
		return static_cast<size_t>(std::ceil(static_cast<float>(size) / _maxLoadFactor));
	}

};

CUSTOM_END