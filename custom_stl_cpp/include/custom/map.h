#pragma once
#include "custom/_search_tree.h"

CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN

template<class Key, class Type, class Compare, class Alloc>
class _Map_Traits
{
public:
	using key_type 			= Key;
	using mapped_type 		= Type;
	using key_compare 		= Compare;
	using value_type 		= pair<Key, Type>;
	using allocator_type 	= Alloc;

public:

	_Map_Traits() = default;

	static const key_type& extract_key(const value_type& value) noexcept	// extract key from element value
	{
		return value.first;
	}

	static const mapped_type& extract_mapval(const value_type& value) noexcept	// extract mapped val from element value
	{
		return value.second;
	}
}; // END _Map_Traits

CUSTOM_DETAIL_END

template<class Key, class Type,
class Compare 	= custom::less<Key>,
class Alloc		= custom::allocator<custom::pair<Key, Type>>>
class map : public detail::_Search_Tree<detail::_Map_Traits<Key, Type, Compare, Alloc>>		// map Template
{
private:
	using _Base = detail::_Search_Tree<detail::_Map_Traits<Key, Type, Compare, Alloc>>;

public:
	using key_type					= typename _Base::key_type;
	using mapped_type				= typename _Base::mapped_type;
	using key_compare				= typename _Base::key_compare;
	using value_type				= typename _Base::value_type;
	using reference					= typename _Base::reference;
	using const_reference			= typename _Base::const_reference;
	using pointer					= typename _Base::pointer;
	using const_pointer				= typename _Base::const_pointer;
	using allocator_type			= typename _Base::allocator_type;

	using iterator					= typename _Base::iterator;
	using const_iterator			= typename _Base::const_iterator;
	using reverse_iterator			= typename _Base::reverse_iterator;
	using const_reverse_iterator	= typename _Base::const_reverse_iterator;

public:
	// Constructors

	map()
		:_Base() { /*Empty*/ }

	map(std::initializer_list<value_type> list)
		:_Base(list) { /*Empty*/ }

	map(const map& other)
		:_Base(other) { /*Empty*/ }

	map(map&& other) noexcept
		:_Base(custom::move(other)) { /*Empty*/ }

	~map() { /*Empty*/ }

public:
	// Operators

	mapped_type& operator[](const key_type& key)	// Access value or create new one with key and assignment (no const)
	{
		return this->_try_emplace(key).first->second;
	}

	mapped_type& operator[](key_type&& key)
	{
		return this->_try_emplace(custom::move(key)).first->second;
	}

	map& operator=(const map& other)
	{
		_Base::operator=(other);
		return *this;
	}

	map& operator=(map&& other) noexcept
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
};  // END map Template

CUSTOM_END