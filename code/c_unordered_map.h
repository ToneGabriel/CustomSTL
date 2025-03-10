#pragma once
#include <x_hash_table.h>

CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN

template<class Key, class Type, class Hash, class Compare, class Alloc>
class _Umap_Traits
{
public:
	using key_type			= Key;
	using mapped_type		= Type;
	using hasher			= Hash;
	using key_compare		= Compare;
	using value_type		= pair<Key, Type>;
	using allocator_type	= Alloc;

public:

	_Umap_Traits() = default;

	static const key_type& extract_key(const value_type& value) noexcept	// extract key from element value
	{
		return value.first;
	}

	static const mapped_type& extract_mapval(const value_type& value) noexcept	// extract mapped val from element value
	{
		return value.second;
	}
}; // END _Umap_Traits

CUSTOM_DETAIL_END

template<class Key, class Type,
class Hash 		= custom::hash<Key>,
class Compare 	= custom::equal_to<Key>,
class Alloc 	= custom::allocator<custom::pair<Key, Type>>>
class unordered_map : public detail::_Hash_Table<detail::_Umap_Traits<Key, Type, Hash, Compare, Alloc>>	// unordered_map Template
{
private:
	using _Base = detail::_Hash_Table<detail::_Umap_Traits<Key, Type, Hash, Compare, Alloc>>;

public:
	static_assert(is_same_v<pair<Key, Type>, typename Alloc::value_type>, "Object type and allocator type must be the same!");
	static_assert(is_object_v<Key>, "Containers require object type!");

	using key_type 			= typename _Base::key_type;
	using mapped_type 		= typename _Base::mapped_type;
	using hasher 			= typename _Base::hasher;
	using key_compare		= typename _Base::key_compare;
	using value_type 		= typename _Base::value_type;
	using reference 		= typename _Base::reference;
	using const_reference 	= typename _Base::const_reference;
	using pointer 			= typename _Base::pointer;
	using const_pointer		= typename _Base::const_pointer;
	using allocator_type 	= typename _Base::allocator_type;

	using iterator			= typename _Base::iterator;
	using const_iterator 	= typename _Base::const_iterator;

public:
	// Constructors

	unordered_map()
		:_Base() { /*Empty*/ }

	unordered_map(const size_t buckets)
		:_Base(buckets) { /*Empty*/ }

	unordered_map(std::initializer_list<value_type> list)
		:_Base(list) { /*Empty*/ }

	unordered_map(const unordered_map& other)
		:_Base(other) { /*Empty*/ }

	unordered_map(unordered_map&& other) noexcept
		:_Base(custom::move(other)) { /*Empty*/ }

	~unordered_map() = default;

public:
	// Operators

	mapped_type& operator[](const key_type& key)	// Access value or create new one with key and assignment
	{
		return this->_try_emplace(key).first->second;
	}

	mapped_type& operator[](key_type&& key)
	{
		return this->_try_emplace(custom::move(key)).first->second;
	}

	unordered_map& operator=(const unordered_map& other)
	{
		_Base::operator=(other);
		return *this;
	}

	unordered_map& operator=(unordered_map&& other) noexcept
	{
		_Base::operator=(custom::move(other));
		return *this;
	}

public:
	// Main functions

	template<class... Args>
	pair<iterator, bool> try_emplace(const key_type& key, Args&&... args)	// Force construction with known key and given arguments for object
	{
		return this->_try_emplace(key, custom::forward<Args>(args)...);
	}

	template<class... Args>
	pair<iterator, bool> try_emplace(key_type&& key, Args&&... args)
	{
		return this->_try_emplace(custom::move(key), custom::forward<Args>(args)...);
	}

	const mapped_type& at(const key_type& key) const	// Access value at key with check
	{
		return this->_at(key);
	}

	mapped_type& at(const key_type& key)
	{
		return this->_at(key);
	}
}; // END unordered_map

CUSTOM_END