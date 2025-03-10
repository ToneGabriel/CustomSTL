#pragma once
#include "c_list.h"
#include "c_vector.h"
#include "c_pair.h"
#include "c_utility.h"
#include "c_functional.h"	// EqualTo, Hash
#include <cmath>			// std::ceil


CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN

// _Hash_Table Template implemented as vector of nodes stored in a list
// The vector holds pairs for bucket count and the first node in bucket
template<class Traits>
class _Hash_Table
{
protected:
	using _Iter_List			= list<typename Traits::value_type, typename Traits::allocator_type>;		// list of value_type used for iteration
	using _Alloc_Node			= typename _Iter_List::_Alloc_Node;
	using _Alloc_Node_Traits	= typename _Iter_List::_Alloc_Node_Traits;
	using _NodePtr 				= typename _Iter_List::_NodePtr;
	using _Bucket 				= pair<size_t, _NodePtr>;
	using _Hash_Vector			= vector<_Bucket>;						// vector of pairs

	using key_type           	= typename Traits::key_type;
    using mapped_type        	= typename Traits::mapped_type;
    using hasher            	= typename Traits::hasher;				// hash struct
	using key_compare			= typename Traits::key_compare;
	
	using value_type			= typename _Iter_List::value_type;		// Type of values stored in container
	using difference_type		= typename _Iter_List::difference_type;
	using reference 			= typename _Iter_List::reference;
	using const_reference 		= typename _Iter_List::const_reference;
	using pointer 				= typename _Iter_List::pointer;
	using const_pointer			= typename _Iter_List::const_pointer;
	using allocator_type 		= typename _Iter_List::allocator_type;

	using iterator				= typename _Iter_List::iterator;			// iterator for this container (identical to list iterator)
	using const_iterator		= typename _Iter_List::const_iterator;

protected:
	hasher _hash;														// Used for initial(non-compressed) hash value
	key_compare _compare;												// Used for comparison between keys
	_Iter_List _elems;													// Used to iterate through container
	_Hash_Vector _buckets;												// Used to map elems from _Iter_List
	_Alloc_Node _alloc;													// Used to allocate nodes

	static constexpr float _TABLE_LOAD_FACTOR	= 0.75;					// The maximum load factor admitted before rehashing
	static constexpr size_t _DEFAULT_BUCKETS	= 8;					// Default number of buckets

protected:
    // Constructors

    _Hash_Table()
	{
		rehash(_DEFAULT_BUCKETS);
	}

	_Hash_Table(const size_t noBuckets)
	{
		rehash((noBuckets < _DEFAULT_BUCKETS) ? _DEFAULT_BUCKETS : noBuckets);
	}

	_Hash_Table(const _Hash_Table& other)
		: _elems(other._elems), _buckets(other._buckets) { /*Empty*/ }

	_Hash_Table(_Hash_Table&& other) noexcept
		: _elems(custom::move(other._elems)), _buckets(custom::move(other._buckets)) { /*Empty*/ }

	virtual ~_Hash_Table() = default;

protected:
	// Operators

	_Hash_Table& operator=(const _Hash_Table& other)
	{
		if (_elems._data._Head != other._elems._data._Head)
		{
			_elems 		= other._elems;
			_buckets 	= other._buckets;
		}

		return *this;
	}

	_Hash_Table& operator=(_Hash_Table&& other) noexcept
	{
		if (_elems._data._Head != other._elems._data._Head)
		{
			_elems 		= custom::move(other._elems);
			_buckets 	= custom::move(other._buckets);
		}

		return *this;
	}

public:
	// Main Functions

    template<class... Args>
	iterator emplace(Args&&... args)
	{
		_NodePtr newNode 		= _alloc.allocate(1);
		_Alloc_Node_Traits::construct(_alloc, &(newNode->_Value), custom::forward<Args>(args)...);
		const key_type& newKey 	= Traits::extract_key(newNode->_Value);
		iterator it 			= find(newKey);

		if (it != end())	// Destroy newly-created Node if key exists
		{
			_Alloc_Node_Traits::destroy(_alloc, &(newNode->_Value));
			_alloc.deallocate(newNode, 1);
			return it;
		}
		else
		{
			_rehash_if_overload();
			_map_and_link_node(bucket(newKey), newNode);

			return iterator(newNode, &_elems._data);
		}
	}

	iterator erase(const key_type& key)
	{
		iterator it = find(key);

		if (it == end())
			return it;

		// Update bucket
		size_t index = bucket(key);
		if (it._Ptr == _buckets[index].second)	// is the starting node in bucket
		{
			if (_buckets[index].first == 1)	// is the only node in bucket
				_buckets[index].second = nullptr;
			else
				_buckets[index].second = it._Ptr->_Next;
		}
		--_buckets[index].first;
		
		// Remove value from iteration list and return next Node iterator
		return _elems.erase(it);
	}

	iterator erase(iterator where)
	{
		if (where == end())
			throw std::out_of_range("erase iterator outside range.");

		return erase(Traits::extract_key(*where));
	}
	
	iterator erase(const_iterator where)
	{
		if (where == end())
			throw std::out_of_range("erase iterator outside range.");

		return erase(Traits::extract_key(*where));
	}

	iterator find(const key_type& key)
	{
		return iterator(_find(key), &_elems._data);
	}

	const_iterator find(const key_type& key) const
	{
		return const_iterator(_find(key), &_elems._data);
	}

	bool contains(const key_type& key) const
	{
		return find(key) != end();
	}

	// rebuild table with at least noBuckets
	void rehash(const size_t noBuckets)
	{
		size_t newBucketCount = (custom::max)(_min_load_factor_buckets(size()), noBuckets);	// don't violate bucket_count() >= size() / max_load_factor()
		if (newBucketCount > bucket_count())
			_force_rehash(newBucketCount);
	}

	// rehash for at least "size" elements
	void reserve(const size_t size)
	{
		rehash(_min_load_factor_buckets(size));
	}

	void clear()
	{
		_elems.clear();						// Delete all Node* with values

		for (auto& val : _buckets)			// Update _buckets to empty
		{
			val.first 	= 0;
			val.second 	= nullptr;
		}
	}

	size_t bucket_count() const
	{
		return _buckets.size();
	}

	size_t bucket_size(const size_t index) const
	{
		return _buckets[index].first;
	}

	size_t bucket(const key_type& key) const
	{
		return _hash(key) % bucket_count();
	}

	size_t size() const
	{
		return _elems.size();
	}

	size_t max_size() const noexcept
	{
		return _elems.max_size();
	}

	bool empty() const
	{
		return _elems.empty();
	}

	float load_factor() const
	{
		return static_cast<float>(size()) / static_cast<float>(bucket_count());
	}

	float max_load_factor() const
	{
		return _TABLE_LOAD_FACTOR;
	}

	// For Debugging
	void print_details() 
	{
		std::cout << "Capacity= " << _buckets.size() << ' ' << "Size= " << _elems.size() << '\n';

		for (size_t i = 0; i < _buckets.size(); ++i)
		{
			std::cout << i << " : ";

			iterator it(_buckets[i].second, &_elems._data);
			for (size_t j = 0; j < _buckets[i].first; ++j)
			{
				std::cout << Traits::extract_key(*it) << ' ' << Traits::extract_mapval(*it) << '\\';
				++it;
			}
			std::cout << '\n';
		}
	}

public:
	// iterator functions

	iterator begin()
	{
		return _elems.begin();
	}

	const_iterator begin() const
	{
		return _elems.begin();
	}

	iterator end()
	{
		return _elems.end();
	}

	const_iterator end() const
	{
		return _elems.end();
	}

protected:
	// Others

	// Force construction with known key and given arguments for object
	template<class _KeyType, class... Args>
	pair<iterator, bool> _try_emplace(_KeyType&& key, Args&&... args)
	{
		iterator it = find(key);		// Check key and decide to construct or not

		if (it != end())
			return {it, false};
		else
		{
			_NodePtr newNode = _alloc.allocate(1);
			_Alloc_Node_Traits::construct(
										_alloc,
										&(newNode->_Value),
										custom::piecewise_construct,
										custom::forward_as_tuple(custom::forward<_KeyType>(key)),
										custom::forward_as_tuple(custom::forward<Args>(args)...)
										);
			const key_type& newKey = Traits::extract_key(newNode->_Value);

			_rehash_if_overload();
			_map_and_link_node(bucket(newKey), newNode);

			return {iterator(newNode, &_elems._data), true};
		}
	}

	const mapped_type& _at(const key_type& key) const
	{
		const_iterator it = find(key);

		if (it == end())
			throw std::out_of_range("Invalid key.");
		
		return Traits::extract_mapval(it._Ptr->_Value);
	}

	mapped_type& _at(const key_type& key)
	{
		const_iterator it = find(key);

		if (it == end())
			throw std::out_of_range("Invalid key.");

		return const_cast<mapped_type&>(Traits::extract_mapval(it._Ptr->_Value));
	}

private:
	// Helpers

	_NodePtr _find(const key_type& key) const
	{
		const _Bucket& currentBucket 	= _buckets[bucket(key)];
		size_t remainingNodes			= currentBucket.first;
		_NodePtr currentNode			= currentBucket.second;

		while (remainingNodes > 0 && !_compare(Traits::extract_key(currentNode->_Value), key))
		{
			currentNode = currentNode->_Next;
			--remainingNodes;
		}

		if (remainingNodes == 0)	// not found
			return _elems._data._Head;

		return currentNode;
	}

	void _map_and_link_node(size_t index, _NodePtr newNode)
	{
		if (_buckets[index].first == 0)	// bucket is empty
			_elems._link_node_before(_elems._data._Head, newNode);	// link last
		else
			_elems._link_node_before(_buckets[index].second, newNode);	// link before the first one in bucket

		// update bucket
		++_buckets[index].first;
		_buckets[index].second = newNode;
	}

	void _force_rehash(const size_t noBuckets)
	{
		_buckets.realloc(noBuckets);

		for (auto it = _elems.begin(); it != _elems.end(); /*no increment here*/)
		{
			// save next iterator
			auto nextIter = custom::next(it);

			// remap node
			size_t index = bucket(Traits::extract_key(*it));
			if (_buckets[index].first == 0)	// bucket is empty
			{
				// do nothing - no need to relink
			}
			else
			{
				// relink node before the first one in bucket
				_elems._unlink_node(it._Ptr);
				_elems._link_node_before(_buckets[index].second, it._Ptr);
			}

			// update bucket
			++_buckets[index].first;
			_buckets[index].second = it._Ptr;

			// increment here
			it = nextIter;
		}
	}

	// Check load factor and rehash if needed
	void _rehash_if_overload()
	{
		if (static_cast<float>(size() + 1) / static_cast<float>(bucket_count()) > max_load_factor())
			_force_rehash(2 * bucket_count());
	}

	// returns the minimum number of buckets necessary for the elements in list
	size_t _min_load_factor_buckets(const size_t size) const
	{
		return static_cast<size_t>(std::ceil(static_cast<float>(size) / max_load_factor()));
	}
};	// END _Hash_Table

// _Hash_Table binary operators
template<class Traits>
bool operator==(const _Hash_Table<Traits>& left, const _Hash_Table<Traits>& right)
{
	// Contains the same elems, but not the same hashtable
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
bool operator!=(const _Hash_Table<Traits>& left, const _Hash_Table<Traits>& right)
{
	return !(left == right);
}

CUSTOM_DETAIL_END

CUSTOM_END