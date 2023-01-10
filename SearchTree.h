#pragma once
#include "Common.h"
#include "List.h"
#include "Vector.h"
#include "Utility.h"

#include <iostream>
#include <cmath>		// std::ceil
#include <functional>	// std::less

CUSTOM_BEGIN

// SearchTree Template =====================================================
template<class Traits>
class SearchTree
{
protected:
    using KeyType       = typename Traits::KeyType;				// Type of Key
    using MappedType    = typename Traits::MappedType;			// Type of Mapped Value
    using KeyCompare    = typename Traits::KeyCompare;			// Comparison struct

    using IterList      = List<typename Traits::ValueType>;		// List of ValueType used for iterating
    using Node          = typename IterList::Node;				// Node component from List

public:
    using ValueType     = typename Traits::ValueType;			// Type of values stored in container
    using Iterator      = typename List<ValueType>::Iterator;	// Iterator for this container (identical to List iterator)

protected:
    KeyCompare _comp;								// Used for comparison
    IterList _elems;								// Used to iterate through container

protected:
	// Constructors and Operators

	SearchTree() {

	}

	SearchTree(const size_t& buckets) {

	}

	SearchTree(const SearchTree& other) {
		_elems = other._elems;
	}

	SearchTree(SearchTree&& other) noexcept {
		_elems = std::move(other._elems);
	}

	virtual ~SearchTree() {
		clear();
	}
};
// SearchTree Template =====================================================
// END

CUSTOM_END