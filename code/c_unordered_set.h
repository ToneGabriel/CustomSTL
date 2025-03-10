#pragma once
#include <x_hash_table.h>

CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN

template<class Key, class Hash, class Compare, class Alloc>
class _Uset_Traits
{
public:
	using key_type			= Key;
	using mapped_type		= Key;
	using hasher			= Hash;
	using key_compare		= Compare;
	using value_type		= mapped_type;
	using allocator_type	= Alloc;

public:

	_Uset_Traits() = default;

	static const key_type& extract_key(const value_type& value)	// extract key from element value
	{
		return value;
	}

	static const mapped_type& extract_mapval(const value_type& value)	// extract key from element value
	{
		return value;
	}
}; // END _Uset_Traits

CUSTOM_DETAIL_END

template<class Key,
class Hash 		= custom::hash<Key>,
class Compare 	= custom::equal_to<Key>,
class Alloc 	= custom::allocator<Key>>
class unordered_set : public detail::_Hash_Table<detail::_Uset_Traits<Key, Hash, Compare, Alloc>>		// unordered_set Template
{
private:
	using _Base = detail::_Hash_Table<detail::_Uset_Traits<Key, Hash, Compare, Alloc>>;

public:
	static_assert(is_same_v<Key, typename Alloc::value_type>, "Object type and Allocator type must be the same!");
	static_assert(is_object_v<Key>, "Containers require object type!");

	using key_type			= typename _Base::key_type;
	using mapped_type		= typename _Base::mapped_type;
	using hasher			= typename _Base::hasher;
	using key_compare		= typename _Base::key_compare;
	using value_type		= typename _Base::value_type;
	using reference			= typename _Base::reference;
	using const_reference	= typename _Base::const_reference;
	using pointer			= typename _Base::pointer;
	using const_pointer		= typename _Base::const_pointer;
	using allocator_type	= typename _Base::allocator_type;

	using iterator			= typename _Base::iterator;
	using const_iterator	= typename _Base::const_iterator;

public:
	// Constructors

	unordered_set()
		:_Base() { /*Empty*/ }

	unordered_set(const size_t buckets)
		:_Base(buckets) { /*Empty*/ }

	unordered_set(std::initializer_list<value_type> list)
		:_Base(list) { /*Empty*/ }

	unordered_set(const unordered_set& other)
		:_Base(other) { /*Empty*/ }

	unordered_set(unordered_set&& other) noexcept
		:_Base(custom::move(other)) { /*Empty*/ }

	~unordered_set() = default;
	
public:
	// Operators

	unordered_set& operator=(const unordered_set& other)
	{
		_Base::operator=(other);
		return *this;
	}

	unordered_set& operator=(unordered_set&& other) noexcept
	{
		_Base::operator=(custom::move(other));
		return *this;
	}
}; // END unordered_set

CUSTOM_END