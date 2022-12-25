#pragma once
#include "List.h"
#include "Vector.h"

template<class Key, class Type>
class UnorderedMap
{
public:
	using ValueType = std::pair<Key, Type>;							// Type of values stored in container
	using Iterator = typename List<ValueType>::Iterator;		// Iterator for this container (identical to List iterator)

private:
	using Hash = std::hash<Key>;						// Strong Hash function
	using List = List<ValueType>;					// List of ValueType (encapsulated in Node*) used for iterating
	using Node = typename List::Node;					// Node component from List
	using Bucket = List<Node*>;					// List of Node* (as Value) from Iteration list
	using HashArray = DynamicArray<Bucket>;				// Array of lists of Node*
	using BucketIterator = typename Bucket::Iterator;	// Iterator for Buckets

private:
	const float _maxLoadFactor = 0.75;					// The maximum load factor admitted before rehashing
	const size_t _defaultBuckets = 8;					// Power of 2

	Hash _hash;											// Used for initial(non-compressed) hash value
	List _elems;										// Used to iterate through container
	HashArray _buckets;									// Used to map List values

public:
	// Constructors

	UnorderedMap() {
		rehash(_defaultBuckets);
	}

	UnorderedMap(const size_t& buckets) {
		rehash((buckets < _defaultBuckets) ? _defaultBuckets : buckets);
	}

	UnorderedMap(const UnorderedMap<Key, Type>& other) {
		_elems = other._elems;
		force_rehash(other.bucket_count());
	}

	UnorderedMap(UnorderedMap<Key, Type>&& other) noexcept {
		_elems = std::move(other._elems);
		force_rehash(other.bucket_count());
	}

	~UnorderedMap() {
		clear();
	}

public:
	// Main functions

	template<class... PairArgs>
	Iterator emplace(PairArgs&&... args) {								// Constructs Node first with any given std::pair arguments
		Node* newNode = new Node(std::forward<PairArgs>(args)...);
		Key& newKey = newNode->Value.first;
		Iterator it = find(newKey);

		if (it != end()) {												// Destroy newly-created Node if key exists
			delete newNode;
			return it;
		}
		else {
			rehash_if_overload();
			_elems.insert_node_before(_elems.end()._NodePtr, newNode);
			_buckets[bucket(newKey)].emplace_back(newNode);
			return Iterator(newNode, _elems.update_iteration_data());
		}
	}

	template<class... Args>
	Iterator try_emplace(Key&& key, Args&&... args) {					// Force construction with known key and given arguments for object
		Iterator it = find(key);										// Check key and decide to construct or not

		if (it != end())
			return it;
		else {
			Node* newNode = new Node(std::piecewise_construct,
				std::forward_as_tuple(std::forward<Key>(key)),
				std::forward_as_tuple(std::forward<Args>(args)...));
			Key& newKey = newNode->Value.first;

			rehash_if_overload();
			_elems.insert_node_before(_elems.end()._NodePtr, newNode);
			_buckets[bucket(newKey)].emplace_back(newNode);
			return Iterator(newNode, _elems.update_iteration_data());
		}
	}

	Iterator erase(const Key& key) {
		size_t index = bucket(key);
		BucketIterator it = find_in_array(key);

		if (it == _buckets[index].end())
			return end();

		Node* nodeToErase = (*it);
		_buckets[index].pop(it);										// Remove reference from array of lists
		return _elems.pop(Iterator(nodeToErase, _elems.update_iteration_data()));						// Remove value from iteration list and return next Node iterator
	}

	Iterator erase(const Iterator& iterator) {
		if (iterator == end())
			throw std::out_of_range("Map erase iterator outside range...");

		return erase(iterator._NodePtr->Value.first);
	}

	Iterator find(const Key& key) {
		BucketIterator it = find_in_array(key);
		if (it == _buckets[bucket(key)].end())
			return end();

		return Iterator(*it, _elems.update_iteration_data());
	}

	void rehash(const size_t& buckets) {											// rebuild table with at least buckets
		size_t newBucketCount = std::max(min_load_factor_buckets(size()), buckets);	// don't violate bucket_count() >= size() / max_load_factor()
		if (newBucketCount <= bucket_count())
			return;

		//TODO:
		//newBucketCount = static_cast<size_t>(1) << std::_Ceiling_of_log_2(newBucketCount);	// 1 * 2 ^ ceil( log2 __ )
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

	size_t bucket(const Key& key) const {								// Get bucket index from key
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

	const Type& at(const Key& key) const {								// Access Value at key with check
		BucketIterator it = find_in_array(key);
		if (it == _buckets[bucket(key)].end())
			throw std::out_of_range("Invalid key...");

		return (*it)->Value.second;
	}

	Type& at(const Key& key) {
		BucketIterator it = find_in_array(key);
		if (it == _buckets[bucket(key)].end())
			throw std::out_of_range("Invalid key...");

		return (*it)->Value.second;
	}

public:
	// Operators

	Type& operator[](const Key& key) {									// Access value or create new one with key and assignment (no const)
		return try_emplace(key)->Value.second;
	}

	Type& operator[](Key&& key) {
		return try_emplace(std::move(key))->Value.second;
	}

	UnorderedMap<Key, Type>& operator=(const UnorderedMap<Key, Type>& other) {
		_elems = other._elems;
		force_rehash(other.bucket_count());

		return *this;
	}

	UnorderedMap<Key, Type>& operator=(UnorderedMap<Key, Type>&& other) noexcept {
		_elems = std::move(other._elems);
		force_rehash(other.bucket_count());

		return *this;
	}

public:
	// Iterator functions

	Iterator begin() {
		return _elems.begin();
	}

	Iterator end() {
		return _elems.end();
	}

private:
	// Others

	BucketIterator find_in_array(const Key& key) {						// Get iterator from bucket with key
		Bucket& currentBucket = _buckets[bucket(key)];
		BucketIterator it = currentBucket.begin();
		while (it != currentBucket.end() && (*it)->Value.first != key)
			it++;

		return it;
	}

	void force_rehash(const size_t& buckets) {
		_buckets.clear();
		_buckets.resize(buckets);

		for (auto it = _elems.begin(); it != _elems.end(); ++it)
			_buckets[bucket(it->Value.first)].push_back(it._NodePtr);
	}

	void rehash_if_overload() {											// Check load factor and rehash if needed
		if (static_cast<float>(size() + 1) / static_cast<float>(bucket_count()) > _maxLoadFactor)
			force_rehash(2 * bucket_count());
	}

	size_t min_load_factor_buckets(const size_t& size) {				// returns the minimum number of buckets necessary for the elements in List
		return static_cast<size_t>(static_cast<float>(size) / _maxLoadFactor);
	}
};

