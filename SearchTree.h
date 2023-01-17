#pragma once
#include "Common.h"
#include "List.h"
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
	using TreeNode 		= TreeNode<Node*>;						// Node component from Tree

	enum Colors 
	{
		Red,
		Black
	};

public:
    using ValueType     = typename Traits::ValueType;			// Type of values stored in container
    using Iterator      = typename List<ValueType>::Iterator;	// Iterator for this container (identical to List iterator)

protected:
    KeyCompare _comp;											// Used for comparison
    IterList _elems;											// Used to iterate through container

	TreeNode* _head 					= nullptr;				// Head of the Tree
	mutable TreeNode* _workspaceNode 	= nullptr;				// Auxiliary Node for work

protected:
	// Constructors

	SearchTree() {
		// TODO: complete

	}

	SearchTree(const SearchTree& other) {
		_elems = other._elems;
		// TODO: complete

	}

	SearchTree(SearchTree&& other) noexcept {
		_elems = std::move(other._elems);
		// TODO: complete

	}

	virtual ~SearchTree() {
		clear();
	}

protected:
	// Operators

	SearchTree& operator=(const SearchTree& other) {
		if (_elems._head != other._elems._head)
		{
			_elems = other._elems;
			// TODO: complete

		}

		return *this;
	}

	SearchTree& operator=(SearchTree&& other) noexcept {
		if (_elems._head != other._elems._head)
		{
			_elems = std::move(other._elems);
			// TODO: complete

		}

		return *this;
	}

	bool operator==(const SearchTree& other) const {						// Contains the same elems, but not the same hashtable
		if (size() != other.size())
			return false;

		// TODO: complete

		return true;
	}

public:
    // Main functions

    template<class... Args>
	Iterator emplace(Args&&... args) {									// Constructs Node first with any given arguments
		Node* newNode = new Node(std::forward<Args>(args)...);
		const KeyType& newKey = Traits::extract_key(newNode->Value);
		Iterator it = find(newKey);

		if (it != end()) {												// Destroy newly-created Node if key exists
			delete newNode;
			return it;
		}
		else {
			_elems.insert_node_before(_elems._head, newNode);
			
			return Iterator(newNode, _elems.update_iteration_data());
		}
	}

	Iterator find(const KeyType& key) const {
		// TODO: complete

		return Iterator(*it, _elems.update_iteration_data());
	}

	void clear() {
		_elems.clear();													// Delete all Node* with values
		// TODO: complete

	}

	bool empty() const {
		return _elems.empty();
	}

	size_t size() const {
		return _elems.size();
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

	const Iterator end() const {
		return _elems.end();
	}

private:
	// Helpers

	void _rotate_left(TreeNode* subroot) {
		TreeNode* promotionNode = subroot->Right;
		subroot->Right = promotionNode->Left;

		if(promotionNode->Left != nullptr)
			promotionNode->Left->Parent = subroot;		// right-left parent set

		promotionNode->Parent = subroot->Parent;

		if (subroot == _head)
		{
			_head = promotionNode;
			promotionNode->Parent = nullptr;
		}
		else if (subroot == subroot->Parent->Left)
			subroot->Parent->Left = promotionNode;
		else
			subroot->Parent->Right = promotionNode;

		promotionNode->Left = subroot;
		subroot->Parent = promotionNode;
	}

	TreeNode* _rotate_right(TreeNode* subroot) {
		TreeNode* promotionNode = subroot->Left;
		subroot->Left = promotionNode->Right;

		if (promotionNode->Right != nullptr)
			promotionNode->Right->Parent = subroot;		// left-right parent set

		promotionNode->Parent = subroot->Parent;

		if (subroot == _head)
		{
			_head = promotionNode;
			promotionNode->Parent = nullptr;
		}
		else if (subroot == subroot->Parent->Left)
			subroot->Parent->Left = promotionNode;
		else
			subroot->Parent->Right = promotionNode;

		promotionNode->Right = subroot;
		subroot->Parent = promotionNode;
	}

	void insert(TreeNode* newNode, TreeNode* parentOfNew) {
		TreeNode* parentOfParent = nullptr;
		TreeNode* uncleOfNew = nullptr;

		newNode->Color = Red;
		newNode->Parent = parentOfNew;

		if(parentOfNew == nullptr)
		{
			_head = newNode;
			return;
		}

		parentOfNew->Right = newNode;	// TODO: depends on std::less()

		//do
	}

};
// SearchTree Template =====================================================
// END

CUSTOM_END