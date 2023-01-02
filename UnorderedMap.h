#pragma once
#include "List.h"
#include "Vector.h"

namespace custom {

	// UnorderedMap ========================================================
	template<class Key, class Type, class Hash = std::hash<Key>>
	class UnorderedMap
	{
	public:
		using ValueType = std::pair<Key, Type>;						// Type of values stored in container
		using Iterator	= typename List<ValueType>::Iterator;		// Iterator for this container (identical to List iterator)

	private:
		using IterList			= List<ValueType>;					// List of ValueType used for iterating
		using Node				= typename IterList::Node;			// Node component from List
		using Bucket			= List<Node*>;						// List of Node* (as Value) from Iteration list
		using HashArray			= Vector<Bucket>;					// Array of lists of Node*
		using BucketIterator	= typename Bucket::Iterator;		// Iterator for Buckets

	private:
		static constexpr float _maxLoadFactor	= 0.75;				// The maximum load factor admitted before rehashing
		static constexpr size_t _defaultBuckets = 8;				// Default number of buckets

		Hash _hash;													// Used for initial(non-compressed) hash value
		IterList _elems;											// Used to iterate through container
		HashArray _buckets;											// Used to map IterList values

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
			Node* newNode = Node::create_non_head(std::forward<PairArgs>(args)...);
			Key& newKey = newNode->Value->first;
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

		template<class... Args>
		Iterator try_emplace(Key&& key, Args&&... args) {					// Force construction with known key and given arguments for object
			Iterator it = find(key);										// Check key and decide to construct or not

			if (it != end())
				return it;
			else {
				Node* newNode = Node::create_non_head(std::piecewise_construct,
								std::forward_as_tuple(std::forward<Key>(key)),
								std::forward_as_tuple(std::forward<Args>(args)...));
				Key& newKey = newNode->Value->first;

				rehash_if_overload();
				_elems.insert_node_before(_elems.end()._Ptr, newNode);
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
			_buckets[index].pop(it);													// Remove reference from array of lists
			return _elems.pop(Iterator(nodeToErase, _elems.update_iteration_data()));	// Remove value from iteration list and return next Node iterator
		}

		Iterator erase(const Iterator& iterator) {
			if (iterator == end())
				throw std::out_of_range("Map erase iterator outside range...");

			return erase(iterator._Ptr->Value->first);
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

			return (*it)->Value->second;
		}

		Type& at(const Key& key) {
			BucketIterator it = find_in_array(key);
			if (it == _buckets[bucket(key)].end())
				throw std::out_of_range("Invalid key...");

			return (*it)->Value->second;
		}

		void print_details() {												// For Debugging
			std::cout << "Capacity= " << _buckets.size() << ' ' << "Size= " << _elems.size() << '\n';
			for (size_t i = 0; i < _buckets.size(); i++)
			{
				std::cout << i << " : ";
				for (auto& val : _buckets[i])
					std::cout << val->Value->first << ' ' << val->Value->second << '\\';
				std::cout << '\n';
			}
		}

	public:
		// Operators

		Type& operator[](const Key& key) {									// Access value or create new one with key and assignment (no const)
			return try_emplace(key)->Value->second;
		}

		Type& operator[](Key&& key) {
			return try_emplace(std::move(key))->Value->second;
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

		bool operator==(const UnorderedMap<Key, Type>& other) {		// Contains the same elems, but not the same hashtable
			if (size() != other.size())
				return false;

			for (const ValueType& val : other)
			{
				Iterator it = find(val.first);		// Search for key
				if (it == end() || (*it).second != val.second)
					return false;
			}

			return true;
		}

		friend std::ostream& operator<<(std::ostream& os, const UnorderedMap<Key, Type>& map) {
			os << "S=" << map.size() << " B=" << map.bucket_count();
			return os;
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

		const Iterator end() const{
			return _elems.end();
		}

	private:
		// Others

		BucketIterator find_in_array(const Key& key) {						// Get iterator from bucket with key
			Bucket& currentBucket = _buckets[bucket(key)];
			BucketIterator it = currentBucket.begin();
			while (it != currentBucket.end() && (*it)->Value->first != key)
				it++;

			return it;
		}

		void force_rehash(const size_t& buckets) {
			_buckets.clear();
			_buckets.resize(buckets);

			for (auto it = _elems.begin(); it != _elems.end(); ++it)
				_buckets[bucket(it->Value->first)].push_back(it._Ptr);
		}

		void rehash_if_overload() {											// Check load factor and rehash if needed
			if (static_cast<float>(size() + 1) / static_cast<float>(bucket_count()) > _maxLoadFactor)
				force_rehash(2 * bucket_count());
		}

		size_t min_load_factor_buckets(const size_t& size) const {			// returns the minimum number of buckets necessary for the elements in List
			return static_cast<size_t>(std::ceilf(static_cast<float>(size) / _maxLoadFactor));
		}
	};
	// UnorderedMap ========================================================
	// END
	

	// UnorderedMap Hash ========================================================
	struct UnorderedMapHash {
		template <class Key, class Type>
		size_t operator()(const UnorderedMap<Key, Type>& map) const {
			return std::hash<Key>{}(map.size()) ^ std::hash<Type>{}(map.bucket_count());
		}
	};
	// UnorderedMap Hash ========================================================
	// END

} // END custom::