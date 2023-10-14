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
	using ValueType 		= Pair<Key, Type>;
	using AllocatorType 	= Alloc;

public:

	MapTraits() = default;

	static const KeyType& extract_key(const ValueType& value) noexcept {		// extract key from element value
		return value.First;
	}

	static const MappedType& extract_mapval(const ValueType& value) noexcept {	// extract mapped val from element value
		return value.Second;
	}
}; // END Map Traits


template<class Key, class Type,
class Compare 	= custom::Less<Key>,
class Alloc		= custom::Allocator<custom::Pair<Key, Type>>>
class Map : public _SearchTree<MapTraits<Key, Type, Compare, Alloc>>		// Map Template
{
private:
	using _Base = _SearchTree<MapTraits<Key, Type, Compare, Alloc>>;

public:
	using KeyType 				= typename _Base::KeyType;
	using MappedType 			= typename _Base::MappedType;
	using KeyCompare			= typename _Base::KeyCompare;
	using ValueType 			= typename _Base::ValueType;
	using Reference 			= typename _Base::Reference;
	using ConstReference 		= typename _Base::ConstReference;
	using Pointer 				= typename _Base::Pointer;
	using ConstPointer 			= typename _Base::ConstPointer;
	using AllocatorType 		= typename _Base::AllocatorType;

	using Iterator				= typename _Base::Iterator;
	using ConstIterator			= typename _Base::ConstIterator;
	using ReverseIterator		= typename _Base::ReverseIterator;
	using ConstReverseIterator	= typename _Base::ConstReverseIterator;

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
		return this->_try_emplace(key).First->Second;
	}

	MappedType& operator[](KeyType&& key) {
		return this->_try_emplace(custom::move(key)).First->Second;
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
	Pair<Iterator, bool> try_emplace(const KeyType& key, Args&&... args) {	// Force construction with known key and given arguments for object
		return this->_try_emplace(key, custom::forward<Args>(args)...);
	}

	template<class... Args>
	Pair<Iterator, bool> try_emplace(KeyType&& key, Args&&... args) {
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