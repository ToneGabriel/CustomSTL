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

	TreeNode* _rotate_left(TreeNode* subroot) {

		TreeNode* subrootParent = subroot->parent;
		TreeNode* subrootRight = subroot->Right;
		TreeNode* subrootRightLeft = nullptr;

		assert(subrootRight != nullptr); // pointer to true node required

		subrootRightLeft = subrootRight->Left;
		subroot->Right = subrootRightLeft; 
		if (subrootRightLeft != nullptr)
			subrootRightLeft->parent = subroot;

		subrootRight->Left = subroot;
		subroot->parent = subrootRight;
		subrootRight->parent = subrootParent;

		if (subrootParent != nullptr)
			if(subroot == subrootParent->Left)
				subrootParent->Left = subrootRight;
			else
				subrootParent->Right = subrootRight;
				//subrootParent->child[ subroot == subrootParent->right ? RIGHT : LEFT ] = subrootRight;
		else
			_head = subrootRight;

		return subrootRight; // new root of subtree
	}

	void _Lrotate(_Nodeptr _Wherenode) noexcept { // promote right node to root of subtree
		_Nodeptr _Pnode = _Wherenode->_Right;
		_Wherenode->_Right = _Pnode->_Left;

		if (!_Pnode->_Left->_Isnil) {
			_Pnode->_Left->_Parent = _Wherenode;
		}

		_Pnode->_Parent = _Wherenode->_Parent;

		if (_Wherenode == _Myhead->_Parent) {
			_Myhead->_Parent = _Pnode;
		}
		else if (_Wherenode == _Wherenode->_Parent->_Left) {
			_Wherenode->_Parent->_Left = _Pnode;
		}
		else {
			_Wherenode->_Parent->_Right = _Pnode;
		}

		_Pnode->_Left = _Wherenode;
		_Wherenode->_Parent = _Pnode;
	}

	TreeNode* _rotate_right(TreeNode* subroot) {

		TreeNode* subrootParent = subroot->parent;
		TreeNode* subrootLeft = subroot->Left;
		TreeNode* subrootLeftRight = nullptr;

		assert(subrootLeft != nullptr); // pointer to true node required

		subrootLeftRight = subrootLeft->Right;
		subroot->Left = subrootLeftRight;
		if (subrootLeftRight != nullptr)
			subrootLeftRight->parent = subroot;

		subrootLeft->Right = subroot;
		subroot->parent = subrootLeft;
		subrootLeft->parent = subrootParent;

		if (subrootParent != nullptr)
			if(subroot == subrootParent->Left)
				subrootParent->Left = subrootLeft;
			else
				subrootParent->Right = subrootLeft;
				//subrootParent->child[ subroot == subrootParent->right ? RIGHT : LEFT ] = subrootRight;
		else
			_head = subrootLeft;

		return subrootLeft; // new root of subtree
	}

	void _Rrotate(_Nodeptr _Wherenode) noexcept { // promote left node to root of subtree
		_Nodeptr _Pnode = _Wherenode->_Left;
		_Wherenode->_Left = _Pnode->_Right;

		if (!_Pnode->_Right->_Isnil) {
			_Pnode->_Right->_Parent = _Wherenode;
		}

		_Pnode->_Parent = _Wherenode->_Parent;

		if (_Wherenode == _Myhead->_Parent) {
			_Myhead->_Parent = _Pnode;
		}
		else if (_Wherenode == _Wherenode->_Parent->_Right) {
			_Wherenode->_Parent->_Right = _Pnode;
		}
		else {
			_Wherenode->_Parent->_Left = _Pnode;
		}

		_Pnode->_Right = _Wherenode;
		_Wherenode->_Parent = _Pnode;
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

		parentOfNew->Right = newNode;	// TODO: depends of std::less()

		do
		{
			if(parentOfNew->Color == Black)		// case 1
				return;

			if((parentOfParent = parentOfNew->Parent) == nullptr)
				// case 4
			
		}
	}

};
// SearchTree Template =====================================================
// END

CUSTOM_END