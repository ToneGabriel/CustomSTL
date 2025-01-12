#pragma once
#include "x_search_tree.h"

CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN

template<class Key, class Compare, class Alloc>
class _Set_Traits										// set Traits
{
public:
	using key_type 			= Key;
	using mapped_type 		= Key;
	using key_compare 		= Compare;
	using value_type 		= mapped_type;
	using allocator_type 	= Alloc;

public:

	_Set_Traits() = default;

	static const key_type& extract_key(const value_type& value) noexcept	// extract key from element value
	{
		return value;
	}

	static const mapped_type& extract_mapval(const value_type& value) noexcept	// extract mapped val from element value
	{
		return value;
	}
}; // END _Set_Traits

CUSTOM_DETAIL_END

template<class Key,
class Compare 	= custom::less<Key>,
class Alloc		= custom::allocator<Key>>
class set : public detail::_Search_Tree<detail::_Set_Traits<Key, Compare, Alloc>>		// set Template
{
private:
	using _Base = detail::_Search_Tree<detail::_Set_Traits<Key, Compare, Alloc>>;

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

	set()
		:_Base() { /*Empty*/ }

	set(std::initializer_list<value_type> list)
		:_Base(list) { /*Empty*/ }

	set(const set& other)
		: _Base(other) { /*Empty*/ }

	set(set&& other) noexcept
		: _Base(custom::move(other)) { /*Empty*/ }

	~set() { /*Empty*/ }

public:
    // Operators

	set& operator=(const set& other)
	{
		_Base::operator=(other);
		return *this;
	}

	set& operator=(set&& other) noexcept
	{
		_Base::operator=(custom::move(other));
		return *this;
	}
}; // END set Template

CUSTOM_END