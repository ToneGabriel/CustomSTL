#pragma once
#include "xSearchTree.h"

CUSTOM_BEGIN

template<class Key, class Type, class Compare, class Alloc>
class MapTraits										// Map Traits
{
public:
	using KeyType 			= Key;
	using MappedType 		= Type;
	using KeyCompare 		= Compare;
	using ValueType 		= pair<Key, Type>;
	using allocator_type 	= Alloc;

public:

	MapTraits() = default;

	static const KeyType& extract_key(const ValueType& value) noexcept {		// extract key from element value
		return value.first;
	}

	static const MappedType& extract_mapval(const ValueType& value) noexcept {	// extract mapped val from element value
		return value.second;
	}
}; // END Map Traits


template<class Key, class Type,
class Compare 	= custom::Less<Key>,
class Alloc		= custom::allocator<custom::pair<Key, Type>>>
class Map : public detail::_SearchTree<MapTraits<Key, Type, Compare, Alloc>>		// Map Template
{
private:
	using _Base = detail::_SearchTree<MapTraits<Key, Type, Compare, Alloc>>;

public:
	using KeyType 				= typename _Base::KeyType;
	using MappedType 			= typename _Base::MappedType;
	using KeyCompare			= typename _Base::KeyCompare;
	using ValueType 			= typename _Base::ValueType;
	using reference 			= typename _Base::reference;
	using const_reference 		= typename _Base::const_reference;
	using pointer 				= typename _Base::pointer;
	using const_pointer 			= typename _Base::const_pointer;
	using allocator_type 		= typename _Base::allocator_type;

	using iterator				= typename _Base::iterator;
	using const_iterator			= typename _Base::const_iterator;
	using reverse_iterator		= typename _Base::reverse_iterator;
	using const_reverse_iterator	= typename _Base::const_reverse_iterator;

public:
	// Constructors

	Map()
		:_Base() { /*Empty*/ }

	Map(const Map& other)
		:_Base(other) { /*Empty*/ }

	Map(Map&& other) noexcept
		:_Base(custom::move(other)) { /*Empty*/ }

	~Map() { /*Empty*/ }

public:
	// Operators

	MappedType& operator[](const KeyType& key) {				// Access value or create new one with key and assignment (no const)
		return this->_try_emplace(key).first->second;
	}

	MappedType& operator[](KeyType&& key) {
		return this->_try_emplace(custom::move(key)).first->second;
	}

	Map& operator=(const Map& other) {
		_Base::operator=(other);
		return *this;
	}

	Map& operator=(Map&& other) noexcept {
		_Base::operator=(custom::move(other));
		return *this;
	}

public:
	// Main functions

	template<class... Args>
	pair<iterator, bool> try_emplace(const KeyType& key, Args&&... args) {	// Force construction with known key and given arguments for object
		return this->_try_emplace(key, custom::forward<Args>(args)...);
	}

	template<class... Args>
	pair<iterator, bool> try_emplace(KeyType&& key, Args&&... args) {
		return this->_try_emplace(custom::move(key), custom::forward<Args>(args)...);
	}

	const MappedType& at(const KeyType& key) const {			// Access _Value at key with check
		return this->_at(key);
	}

	MappedType& at(const KeyType& key) {
		return this->_at(key);
	}
};  // END Map Template

CUSTOM_END