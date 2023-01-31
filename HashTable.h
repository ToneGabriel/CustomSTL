#pragma once
#include "Common.h"
#include "List.h"
#include "Vector.h"
#include "Utility.h"

#include <iostream>
#include <cmath>		// std::ceil
#include <functional>	// std::hash

CUSTOM_BEGIN

// Headings =================================================================================

template<class Traits>
class HashTable				// HashTable Template
{
protected:
    using KeyType           = typename Traits::KeyType;				// Type of Key
    using MappedType        = typename Traits::MappedType;			// Type of Mapped Value
    using Hasher            = typename Traits::HasherType;			// Hash struct

	using IterList			= List<typename Traits::ValueType>;		// List of ValueType used for iterating
	using Node				= typename IterList::Node;				// Node component from List
	using Bucket			= List<Node*>;							// List of Node* (as Value) from Iteration list
	using HashArray			= Vector<Bucket>;						// Array of lists of Node*
	using BucketIterator	= typename Bucket::Iterator;			// Iterator for Buckets

public:
	using ValueType			= typename Traits::ValueType;			// Type of values stored in container
	using Iterator			= typename List<ValueType>::Iterator;	// Iterator for this container (identical to List iterator)

protected:
	static constexpr float _maxLoadFactor	= 0.75;		// The maximum load factor admitted before rehashing
	static constexpr size_t _defaultBuckets = 8;		// Default number of buckets

	Hasher _hash;										// Used for initial(non-compressed) hash value
	IterList _elems;									// Used to iterate through container
	HashArray _buckets;									// Used to map IterList values

protected:
    // Constructors

    HashTable();
	HashTable(const size_t& buckets);
	HashTable(const HashTable& other);
	HashTable(HashTable&& other) noexcept;
	virtual ~HashTable();

protected:
	// Operators

	HashTable& operator=(const HashTable& other);
	HashTable& operator=(HashTable&& other) noexcept;

	bool operator==(const HashTable& other) const;					// Contains the same elems, but not the same hashtable

public:
    // Main functions

    template<class... Args>
	Iterator emplace(Args&&... args);								// Constructs Node first with any given arguments
	Iterator erase(const KeyType& key);
	Iterator erase(const Iterator& iterator);
	Iterator find(const KeyType& key) const;

	void rehash(const size_t& buckets);								// rebuild table with at least buckets
	void reserve(const size_t& size);								// rehash for at least "size" elements
	void clear();

	size_t bucket_count() const;									// Get HashArray size
	size_t bucket_size(const size_t& index) const;					// Get the size of the bucket at index
	size_t bucket(const KeyType& key) const;						// Get bucket index from key
	size_t size() const;
	bool empty() const;

	float load_factor() const;
	float max_load_factor() const;
	void print_details() const;										// For Debugging

public:
	// Iterator functions

	Iterator begin();
	const Iterator begin() const;

	Iterator end();
	const Iterator end() const;

protected:
	// Others

	template<class _KeyType, class... Args>
	Iterator _try_emplace(_KeyType&& key, Args&&... args);			// Force construction with known key and given arguments for object

	const MappedType& _at(const KeyType& key) const;				// Access Value at key with check
	MappedType& _at(const KeyType& key);

private:
	// Helpers

	BucketIterator _find_in_array(const KeyType& key) const;		// Get iterator from bucket with key
	void _force_rehash(const size_t& buckets);
	void _rehash_if_overload();										// Check load factor and rehash if needed
	size_t _min_load_factor_buckets(const size_t& size) const;		// returns the minimum number of buckets necessary for the elements in List
}; // END HashTable Template


// Definitions =================================================================================

// HashTable Template
template<class Traits>
HashTable<Traits>::HashTable() {
	rehash(_defaultBuckets);
}

template<class Traits>
HashTable<Traits>::HashTable(const size_t& buckets) {
	rehash((buckets < _defaultBuckets) ? _defaultBuckets : buckets);
}

template<class Traits>
HashTable<Traits>::HashTable(const HashTable& other) {
	_elems = other._elems;
	_force_rehash(other.bucket_count());
}

template<class Traits>
HashTable<Traits>::HashTable(HashTable&& other) noexcept {
	_elems = std::move(other._elems);
	_force_rehash(other.bucket_count());
}

template<class Traits>
HashTable<Traits>::~HashTable() {
	clear();
}

template<class Traits>
HashTable<Traits>& HashTable<Traits>::operator=(const HashTable& other) {
	if (_elems._head != other._elems._head)
	{
		_elems = other._elems;
		_force_rehash(other.bucket_count());
	}

	return *this;
}

template<class Traits>
HashTable<Traits>& HashTable<Traits>::operator=(HashTable&& other) noexcept {
	if (_elems._head != other._elems._head)
	{
		_elems = std::move(other._elems);
		_force_rehash(other.bucket_count());
	}

	return *this;
}

template<class Traits>
bool HashTable<Traits>::operator==(const HashTable& other) const {
	if (size() != other.size())
		return false;

	for (const ValueType& val : other)
	{
		Iterator it = find(Traits::extract_key(val));				// Search for key
		if (it == end() || Traits::extract_mapval(*it) != Traits::extract_mapval(val))
			return false;
	}

	return true;
}

template<class Traits>
template<class... Args>
typename HashTable<Traits>::Iterator HashTable<Traits>::emplace(Args&&... args) {
	Node* newNode = new Node(std::forward<Args>(args)...);
	const KeyType& newKey = Traits::extract_key(newNode->Value);
	Iterator it = find(newKey);

	if (it != end()) {												// Destroy newly-created Node if key exists
		delete newNode;
		return it;
	}
	else {
		_rehash_if_overload();
		_elems._insert_node_before(_elems._head, newNode);
		_buckets[bucket(newKey)].emplace_back(newNode);
		return Iterator(newNode, _elems._update_iteration_data());
	}
}

template<class Traits>
typename HashTable<Traits>::Iterator HashTable<Traits>::erase(const KeyType& key) {
	size_t index = bucket(key);
	BucketIterator it = _find_in_array(key);

	if (it == _buckets[index].end())
		return end();

	Node* nodeToErase = (*it);
	_buckets[index].pop(it);													// Remove reference from array of lists
	return _elems.pop(Iterator(nodeToErase, _elems._update_iteration_data()));	// Remove value from iteration list and return next Node iterator
}

template<class Traits>
typename HashTable<Traits>::Iterator HashTable<Traits>::erase(const Iterator& iterator) {
	if (iterator == end())
		throw std::out_of_range("Map erase iterator outside range...");

	return erase(iterator._Ptr->Value);
}

template<class Traits>
typename HashTable<Traits>::Iterator HashTable<Traits>::find(const KeyType& key) const {
	BucketIterator it = _find_in_array(key);
	if (it == _buckets[bucket(key)].end())
		return end();

	return Iterator(*it, _elems._update_iteration_data());
}

template<class Traits>
void HashTable<Traits>::rehash(const size_t& buckets) {
	size_t newBucketCount = std::max(_min_load_factor_buckets(size()), buckets);	// don't violate bucket_count() >= size() / max_load_factor()
	if (newBucketCount <= bucket_count())
		return;

	_force_rehash(newBucketCount);
}

template<class Traits>
void HashTable<Traits>::reserve(const size_t& size) {
	rehash(_min_load_factor_buckets(size));
}

template<class Traits>
void HashTable<Traits>::clear() {
	_elems.clear();						// Delete all Node* with values
	_buckets.clear();					// Remove lists with Node* references
}

template<class Traits>
size_t HashTable<Traits>::bucket_count() const {
	return _buckets.size();
}

template<class Traits>
size_t HashTable<Traits>::bucket_size(const size_t& index) const {
	return _buckets[index].size();
}

template<class Traits>
size_t HashTable<Traits>::bucket(const KeyType& key) const {
	return _hash(key) % bucket_count();
}

template<class Traits>
bool HashTable<Traits>::empty() const {
	return _elems.empty();
}

template<class Traits>
size_t HashTable<Traits>::size() const {
	return _elems.size();
}

template<class Traits>
float HashTable<Traits>::load_factor() const {
	return static_cast<float>(size()) / static_cast<float>(bucket_count());
}

template<class Traits>
float HashTable<Traits>::max_load_factor() const {
	return _maxLoadFactor;
}

template<class Traits>
void HashTable<Traits>::print_details() const {
	std::cout << "Capacity= " << _buckets.size() << ' ' << "Size= " << _elems.size() << '\n';
	for (size_t i = 0; i < _buckets.size(); i++)
	{
		std::cout << i << " : ";
		for (const auto& val : _buckets[i])
			std::cout << Traits::extract_key(val->Value) << ' ' << Traits::extract_mapval(val->Value) << '\\';
		std::cout << '\n';
	}
}

template<class Traits>
typename HashTable<Traits>::Iterator HashTable<Traits>::begin() {
	return _elems.begin();
}

template<class Traits>
const typename HashTable<Traits>::Iterator HashTable<Traits>::begin() const {
	return _elems.begin();
}

template<class Traits>
typename HashTable<Traits>::Iterator HashTable<Traits>::end() {
	return _elems.end();
}

template<class Traits>
const typename HashTable<Traits>::Iterator HashTable<Traits>::end() const {
	return _elems.end();
}

template<class Traits>
template<class _KeyType, class... Args>
typename HashTable<Traits>::Iterator HashTable<Traits>::_try_emplace(_KeyType&& key, Args&&... args) {
	Iterator it = find(key);		// Check key and decide to construct or not

	if (it != end())
		return it;
	else {
		Node* newNode = new Node(
						piecewise_construct,
						forward_as_tuple(std::forward<_KeyType>(key)),
						forward_as_tuple(std::forward<Args>(args)...)
						);
		const KeyType& newKey = Traits::extract_key(newNode->Value);

		_rehash_if_overload();
		_elems._insert_node_before(_elems._head, newNode);
		_buckets[bucket(newKey)].emplace_back(newNode);
		return Iterator(newNode, _elems._update_iteration_data());
	}
}

template<class Traits>
const typename HashTable<Traits>::MappedType& HashTable<Traits>::_at(const KeyType& key) const {
	BucketIterator it = _find_in_array(key);
	if (it == _buckets[bucket(key)].end())
		throw std::out_of_range("Invalid key...");

	return Traits::extract_mapval((*it)->Value);
}

template<class Traits>
typename HashTable<Traits>::MappedType& HashTable<Traits>::_at(const KeyType& key) {
	BucketIterator it = _find_in_array(key);
	if (it == _buckets[bucket(key)].end())
		throw std::out_of_range("Invalid key...");

	return Traits::extract_mapval((*it)->Value);
}

template<class Traits>
typename HashTable<Traits>::BucketIterator HashTable<Traits>::_find_in_array(const KeyType& key) const {
	const Bucket& currentBucket = _buckets[bucket(key)];
	BucketIterator it = currentBucket.begin();
	while (it != currentBucket.end() && Traits::extract_key((*it)->Value) != key)
		it++;

	return it;
}

template<class Traits>
void HashTable<Traits>::_force_rehash(const size_t& buckets) {
	_buckets.clear();
	_buckets.resize(buckets);

	for (auto it = _elems.begin(); it != _elems.end(); ++it)
		_buckets[bucket(Traits::extract_key(it->Value))].push_back(it._Ptr);
}

template<class Traits>
void HashTable<Traits>::_rehash_if_overload() {
	if (static_cast<float>(size() + 1) / static_cast<float>(bucket_count()) > _maxLoadFactor)
		_force_rehash(2 * bucket_count());
}

template<class Traits>
size_t HashTable<Traits>::_min_load_factor_buckets(const size_t& size) const {
	return static_cast<size_t>(std::ceil(static_cast<float>(size) / _maxLoadFactor));
}
// END HashTable Template

CUSTOM_END