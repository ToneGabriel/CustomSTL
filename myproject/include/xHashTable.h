#pragma once
#include "List.h"
#include "Vector.h"
#include "Pair.h"		// _try_emplace
#include "Utility.h"
#include "Functional.h"	// EqualTo, Hash

#include <cmath>		// std::ceil


CUSTOM_BEGIN

template<class Traits>
class _HashTable				// _HashTable Template implemented as vector of lists
{
protected:
    using KeyType           	= typename Traits::KeyType;				// Type of Key
    using MappedType        	= typename Traits::MappedType;			// Type of Mapped _Value
    using Hasher            	= typename Traits::Hasher;				// Hash struct
	using KeyEqual 				= typename Traits::KeyEqual;
	using ValueType				= typename Traits::ValueType;			// Type of values stored in container
	using Reference 			= typename Traits::Reference;
	using ConstReference 		= typename Traits::ConstReference;
	using Pointer 				= typename Traits::Pointer;
	using ConstPointer 			= typename Traits::ConstPointer;
	using AllocatorType 		= typename Traits::AllocatorType;

	using _IterList				= List<ValueType, AllocatorType>;		// List of ValueType used for iteration
	using _AllocNode			= typename _IterList::_AllocNode;
	using _AllocNodeTraits		= typename _IterList::_AllocNodeTraits;
	using _NodePtr 				= typename _IterList::_NodePtr;
	using _Bucket				= List<_NodePtr>;						// List of Node* (as _Value) from Iteration list
	using _HashArray			= Vector<_Bucket>;						// Vector of lists of Node*
	using _BucketIterator		= typename _Bucket::Iterator;			// Iterator for Buckets
	using _ConstBucketIterator 	= typename _Bucket::ConstIterator;

	using Iterator				= typename _IterList::Iterator;			// Iterator for this container (identical to List iterator)
	using ConstIterator			= typename _IterList::ConstIterator;

protected:
	Hasher _hash;														// Used for initial(non-compressed) hash value
	_IterList _elems;													// Used to iterate through container
	_HashArray _buckets;												// Used to map elems from _IterList
	_AllocNode _alloc;

	static constexpr float _TABLE_LOAD_FACTOR	= 0.75;					// The maximum load factor admitted before rehashing
	static constexpr size_t _DEFAULT_BUCKETS	= 8;					// Default number of buckets

protected:
    // Constructors

    _HashTable() {
		rehash(_DEFAULT_BUCKETS);
	}

	_HashTable(const size_t buckets) {
		rehash((buckets < _DEFAULT_BUCKETS) ? _DEFAULT_BUCKETS : buckets);
	}

	_HashTable(const _HashTable& other)
		: _elems(other._elems), _buckets(other._buckets) { /*Empty*/ }

	_HashTable(_HashTable&& other) noexcept
		: _elems(custom::move(other._elems)), _buckets(custom::move(other._buckets)) { /*Empty*/ }

	virtual ~_HashTable() = default;

protected:
	// Operators

	_HashTable& operator=(const _HashTable& other) {
		if (_elems._data._Head != other._elems._data._Head)
		{
			_elems 		= other._elems;
			_buckets 	= other._buckets;
		}

		return *this;
	}

	_HashTable& operator=(_HashTable&& other) noexcept {
		if (_elems._data._Head != other._elems._data._Head)
		{
			_elems 		= custom::move(other._elems);
			_buckets 	= custom::move(other._buckets);
		}

		return *this;
	}

public:
    // Main functions

    template<class... Args>
	Iterator emplace(Args&&... args) {
		_NodePtr newNode 		= _alloc.allocate(1);
		_AllocNodeTraits::construct(_alloc, &(newNode->_Value), custom::forward<Args>(args)...);
		const KeyType& newKey 	= Traits::extract_key(newNode->_Value);
		Iterator it 			= find(newKey);

		if (it != end())	// Destroy newly-created Node if key exists
		{
			_AllocNodeTraits::destroy(_alloc, &(newNode->_Value));
			_alloc.deallocate(newNode, 1);
			return it;
		}
		else
		{
			_rehash_if_overload();
			_elems._insert_node_before(_elems._data._Head, newNode);
			_buckets[bucket(newKey)].emplace_back(newNode);
			return Iterator(newNode, &_elems._data);
		}
	}

	Iterator erase(const KeyType& key) {
		size_t index 		= bucket(key);
		_BucketIterator it 	= _find_in_array(key);

		if (it == _buckets[index].end())
			return end();

		_NodePtr nodeToErase = (*it);
		_buckets[index].pop(it);									// Remove reference hashtable
		return _elems.pop(Iterator(nodeToErase, &_elems._data));	// Remove value from iteration list and return next Node iterator
	}

	Iterator erase(ConstIterator iterator) {
		if (iterator == end())
			throw std::out_of_range("Map erase iterator outside range...");

		return erase(iterator._Ptr->_Value);
	}

	Iterator find(const KeyType& key) {
		_BucketIterator it = _find_in_array(key);
		if (it == _buckets[bucket(key)].end())
			return end();

		return Iterator(*it, &_elems._data);
	}

	ConstIterator find(const KeyType& key) const {
		_ConstBucketIterator it = _find_in_array(key);
		if (it == _buckets[bucket(key)].end())
			return end();

		return ConstIterator(*it, &_elems._data);
	}

	bool contains(const KeyType& key) const {
		return find(key) != end();
	}

	void rehash(const size_t buckets) {							// rebuild table with at least buckets
		size_t newBucketCount = std::max(_min_load_factor_buckets(size()), buckets);	// don't violate bucket_count() >= size() / max_load_factor()
		if (newBucketCount > bucket_count())
			_force_rehash(newBucketCount);
	}

	void reserve(const size_t size) {								// rehash for at least "size" elements
		rehash(_min_load_factor_buckets(size));
	}

	void clear() {
		_elems.clear();						// Delete all Node* with values
		_buckets.clear();					// Remove lists with Node* references
	}

	size_t bucket_count() const {									// Get _HashArray size
		return _buckets.size();
	}

	size_t bucket_size(const size_t index) const {					// Get the size of the bucket at index
		return _buckets[index].size();
	}

	size_t bucket(const KeyType& key) const {						// Get bucket index from key
		return _hash(key) % bucket_count();
	}

	size_t size() const {
		return _elems.size();
	}

	bool empty() const {
		return _elems.empty();
	}

	float load_factor() const {
		return static_cast<float>(size()) / static_cast<float>(bucket_count());
	}

	float max_load_factor() const {
		return _TABLE_LOAD_FACTOR;
	}

	void print_details()  {									// For Debugging
		std::cout << "Capacity= " << _buckets.size() << ' ' << "Size= " << _elems.size() << '\n';
		for (size_t i = 0; i < _buckets.size(); ++i)
		{
			std::cout << i << " : ";
			for ( auto& val : _buckets[i])
				std::cout << Traits::extract_key(val->_Value) << ' ' << Traits::extract_mapval(val->_Value) << '\\';
			std::cout << '\n';
		}
	}

public:
	// Iterator functions

	Iterator begin() {
		return _elems.begin();
	}

	ConstIterator begin() const {
		return _elems.begin();
	}

	Iterator end() {
		return _elems.end();
	}

	ConstIterator end() const {
		return _elems.end();
	}

protected:
	// Others

	template<class _KeyType, class... Args>
	Pair<Iterator, bool> _try_emplace(_KeyType&& key, Args&&... args) {			// Force construction with known key and given arguments for object
		Iterator it = find(key);		// Check key and decide to construct or not

		if (it != end())
			return {it, false};
		else
		{
			_NodePtr newNode = _alloc.allocate(1);
			_AllocNodeTraits::construct(
										_alloc,
										&(newNode->_Value),
										custom::PiecewiseConstruct,
										custom::forward_as_tuple(custom::forward<_KeyType>(key)),
										custom::forward_as_tuple(custom::forward<Args>(args)...)
										);
			const KeyType& newKey = Traits::extract_key(newNode->_Value);

			_rehash_if_overload();
			_elems._insert_node_before(_elems._data._Head, newNode);
			_buckets[bucket(newKey)].emplace_back(newNode);
			return {Iterator(newNode, &_elems._data), true};
		}
	}

	const MappedType& _at(const KeyType& key) const {				// Access _Value at key with check
		_ConstBucketIterator it = _find_in_array(key);
		
		if (it == _buckets[bucket(key)].end())
			throw std::out_of_range("Invalid key...");

		return Traits::extract_mapval((*it)->_Value);
	}

	MappedType& _at(const KeyType& key) {
		_BucketIterator it = _find_in_array(key);

		if (it == _buckets[bucket(key)].end())
			throw std::out_of_range("Invalid key...");

		return const_cast<MappedType&>(Traits::extract_mapval((*it)->_Value));
	}

private:
	// Helpers

	_BucketIterator _find_in_array(const KeyType& key) {
		_Bucket& currentBucket 	= _buckets[bucket(key)];
		_BucketIterator it 		= currentBucket.begin();

		while (it != currentBucket.end() && Traits::extract_key((*it)->_Value) != key)
			++it;

		return it;
	}

	_ConstBucketIterator _find_in_array(const KeyType& key) const {		// Get iterator from bucket with key
		const _Bucket& currentBucket = _buckets[bucket(key)];
		_ConstBucketIterator it 		= currentBucket.begin();

		while (it != currentBucket.end() && Traits::extract_key((*it)->_Value) != key)
			++it;

		return it;
	}

	void _force_rehash(const size_t buckets) {
		_buckets.clear();
		_buckets.resize(buckets);

		for (auto it = _elems.begin(); it != _elems.end(); ++it)
			_buckets[bucket(Traits::extract_key(*it))].push_back(it._Ptr);
	}

	void _rehash_if_overload() {									// Check load factor and rehash if needed
		if (static_cast<float>(size() + 1) / static_cast<float>(bucket_count()) > max_load_factor())
			_force_rehash(2 * bucket_count());
	}

	size_t _min_load_factor_buckets(const size_t size) const {		// returns the minimum number of buckets necessary for the elements in List
		return static_cast<size_t>(std::ceil(static_cast<float>(size) / max_load_factor()));
	}
}; // END _HashTable Template


// _HashTable binary operators
template<class Traits>
bool operator==(const _HashTable<Traits>& left, const _HashTable<Traits>& right) {	// Contains the same elems, but not the same hashtable
	if (left.size() != right.size())
		return false;

	for (const auto& val : right)
	{
		auto it = left.find(Traits::extract_key(val));	// Search for key
		if (it == left.end() || Traits::extract_mapval(*it) != Traits::extract_mapval(val))
			return false;
	}

	return true;
}

template<class Traits>
bool operator!=(const _HashTable<Traits>& left, const _HashTable<Traits>& right) {
	return !(left == right);
}

CUSTOM_END