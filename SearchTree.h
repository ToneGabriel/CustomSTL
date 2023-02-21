#pragma once
#include "List.h"
#include "Pair.h"
#include "Utility.h"

#include <iostream>
#include <cmath>		// std::ceil
#include <functional>	// std::less


CUSTOM_BEGIN

// Headings =================================================================================

template<class Type>
struct SearchTreeIterationData {	// Data used for iterating SearchTree
	Type* _Begin	= nullptr;
	Type* _End		= nullptr;

	SearchTreeIterationData() = default;
	~SearchTreeIterationData();
};


template<class SearchTree>
class SearchTreeIterator			// SearchTree Iterator
{
public:
	using ValueType = typename SearchTree::ValueType;
	using IterType	= typename SearchTree::IterType;
	using Data		= SearchTreeIterationData<IterType>;

	IterType* _Ptr	= nullptr;
	Data* _Data		= nullptr;

public:
	explicit SearchTreeIterator(IterType* ptr, Data* data);
	~SearchTreeIterator();

	SearchTreeIterator& operator++();							// ++it
	SearchTreeIterator operator++(int);							// it++

	SearchTreeIterator& operator--();							// --it
	SearchTreeIterator operator--(int);							// it--

	IterType* operator->();
	ValueType& operator*();

	bool operator==(const SearchTreeIterator& other) const;
	bool operator!=(const SearchTreeIterator& other) const;
}; // END SearchTree Iterator


template<class Traits>
class SearchTree			// SearchTree Template implemented as Red-Black Tree
{
protected:
    using KeyType       = typename Traits::KeyType;				// Type of Key
    using MappedType    = typename Traits::MappedType;			// Type of Mapped _Value
    using KeyCompare    = typename Traits::KeyCompare;			// Comparison struct

public:
    using ValueType     = typename Traits::ValueType;				// Type of values stored in container
	using Node 			= TreeNode<ValueType>;						// Node component from Tree
	using IterType		= Node;										// Type of iterating element
	using Iterator		= SearchTreeIterator<SearchTree<Traits>>;	// Iterator type
	using Data			= typename Iterator::Data;					// Iteration data

protected:
	size_t _size		= 0;										// Number of Nodes held by this
	Node* _head			= nullptr;									// Head of the Tree
    KeyCompare _less;												// Used for comparison

	mutable Data _data;
	mutable Node* _workspaceNode = nullptr;							// Auxiliary Node for work

protected:
	// Constructors

	SearchTree();
	SearchTree(const SearchTree& other);
	SearchTree(SearchTree&& other) noexcept;
	virtual ~SearchTree();

protected:
	// Operators

	SearchTree& operator=(const SearchTree& other);
	SearchTree& operator=(SearchTree&& other) noexcept;

	bool operator==(const SearchTree& other) const;					// Contains the same elems, but not the same tree
	bool operator!=(const SearchTree& other) const;

public:
    // Main functions

	static Node* leftmost(Node* node) noexcept;						// return leftmost node in subtree at node
	static Node* rightmost(Node* node) noexcept;					// return leftmost node in subtree at node

    template<class... Args>
	Iterator emplace(Args&&... args);								// Constructs Node first with any given arguments
	Iterator erase(const KeyType& key);
	Iterator erase(const Iterator& iterator);
	Iterator find(const KeyType& key) const;
	
	void clear();
	size_t size() const;
	bool empty() const;

	void print_details() const;
public:
	// Iterator functions

	Iterator begin();
	const Iterator begin() const;

	Iterator end();
	const Iterator end() const;

protected:
	// Others

	template<class _KeyType, class... Args>
	Iterator _try_emplace(_KeyType&& key, Args&&... args);			// Force construction with known key and given arguments for object

	const MappedType& _at(const KeyType& key) const;				// Access _Value at key with check
	MappedType& _at(const KeyType& key);

private:
	// Helpers

	void _print_graph(const size_t& ident, Node* root, const custom::String& rlFlag) const;

	void _rotate_left(Node* subroot);								// promotes subroot right
	void _rotate_right(Node* subroot);								// promotes subroot left

	void _insert(Node* newNode);
	void _insert_raw(Node* newNode);
	void _fix_inserted(Node* newNode);

	Node* _find_in_tree(const KeyType& key) const;

	bool _has_parent(Node* node);
	bool _has_grandparent(Node* node);

	Data* _update_iteration_data() const;							// Update the data used in Iterator
}; // END SearchTree Template


// Definitions =================================================================================

// SearchTree Iterator
template<class Type>
SearchTreeIterationData<Type>::~SearchTreeIterationData() {
	_Begin	= nullptr;
	_End	= nullptr;
}

template<class SearchTree>
SearchTreeIterator<SearchTree>::SearchTreeIterator(IterType* ptr, Data* data)
	:_Ptr(ptr), _Data(data) { /*Empty*/ }

template<class SearchTree>
SearchTreeIterator<SearchTree>::~SearchTreeIterator() {
	_Ptr	= nullptr;
	_Data	= nullptr;
}

template<class SearchTree>
SearchTreeIterator<SearchTree>& SearchTreeIterator<SearchTree>::operator++() {
	return *this;
}

template<class SearchTree>
SearchTreeIterator<SearchTree> SearchTreeIterator<SearchTree>::operator++(int) {
	SearchTreeIterator temp = *this;
	++(*this);
	return temp;
}

template<class SearchTree>
SearchTreeIterator<SearchTree>& SearchTreeIterator<SearchTree>::operator--() {
	return *this;
}

template<class SearchTree>
SearchTreeIterator<SearchTree> SearchTreeIterator<SearchTree>::operator--(int) {
	SearchTreeIterator temp = *this;
	--(*this);
	return temp;
}

template<class SearchTree>
typename SearchTreeIterator<SearchTree>::IterType* SearchTreeIterator<SearchTree>::operator->() {
	return _Ptr;
}

template<class SearchTree>
typename SearchTreeIterator<SearchTree>::ValueType& SearchTreeIterator<SearchTree>::operator*() {
	return _Ptr->_Value;
}

template<class SearchTree>
bool SearchTreeIterator<SearchTree>::operator==(const SearchTreeIterator& other) const {
	return _Ptr == other._Ptr;
}

template<class SearchTree>
bool SearchTreeIterator<SearchTree>::operator!=(const SearchTreeIterator& other) const {
	return !(*this == other);
}
// END SearchTree Iterator

// SearchTree Template
template<class Traits>
SearchTree<Traits>::SearchTree() {
	// TODO: complete
}

template<class Traits>
SearchTree<Traits>::SearchTree(const SearchTree& other) {
	// TODO: complete
}

template<class Traits>
SearchTree<Traits>::SearchTree(SearchTree&& other) noexcept {
	// TODO: complete
}

template<class Traits>
SearchTree<Traits>::~SearchTree() {
	clear();
}

template<class Traits>
SearchTree<Traits>& SearchTree<Traits>::operator=(const SearchTree& other) {
	// TODO
	return *this;
}

template<class Traits>
SearchTree<Traits>& SearchTree<Traits>::operator=(SearchTree&& other) noexcept {
	// TODO
	return *this;
}

template<class Traits>
bool SearchTree<Traits>::operator==(const SearchTree& other) const {
	if (size() != other.size())
		return false;

	// TODO: complete

	return true;
}

template<class Traits>
bool SearchTree<Traits>::operator!=(const SearchTree& other) const {
	return !(*this == other);
}

template<class Traits>
typename SearchTree<Traits>::Node* SearchTree<Traits>::leftmost(Node* node) noexcept {
	while (node->_Left != nullptr)
		node = node->_Left;

	return node;
}

template<class Traits>
typename SearchTree<Traits>::Node* SearchTree<Traits>::rightmost(Node* node) noexcept {
	while (node->_Right != nullptr)
		node = node->_Right;

	return node;
}

template<class Traits>
template<class... Args>
typename SearchTree<Traits>::Iterator SearchTree<Traits>::emplace(Args&&... args) {
	Node* newNode = new Node(custom::forward<Args>(args)...);
	const KeyType& newKey = Traits::extract_key(newNode->_Value);

	_insert(newNode);	// TODO: check
	return Iterator(newNode, &_data);
}

template<class Traits>
typename SearchTree<Traits>::Iterator SearchTree<Traits>::erase(const KeyType& key) {
	// TODO: implement
}

template<class Traits>
typename SearchTree<Traits>::Iterator SearchTree<Traits>::erase(const Iterator& iterator) {
	// TODO: implement
}

template<class Traits>
typename SearchTree<Traits>::Iterator SearchTree<Traits>::find(const KeyType& key) const {
	// TODO: complete
}

template<class Traits>
void SearchTree<Traits>::clear() {
	// TODO: complete
}

template<class Traits>
size_t SearchTree<Traits>::size() const {
	return _size;
}

template<class Traits>
bool SearchTree<Traits>::empty() const {
	return _size == 0;
}

template<class Traits>
void SearchTree<Traits>::print_details() const {
	std::cout << "Size= " << _size << '\n';
	_print_graph(0, _head, "HEAD");
}

template<class Traits>
typename SearchTree<Traits>::Iterator SearchTree<Traits>::begin() {	// TODO: implement
	return Iterator(_head, &_data);
}

template<class Traits>
const typename SearchTree<Traits>::Iterator SearchTree<Traits>::begin() const {
	return Iterator(_head, &_data);
}

template<class Traits>
typename SearchTree<Traits>::Iterator SearchTree<Traits>::end() {
	return Iterator(_head, &_data);
}

template<class Traits>
const typename SearchTree<Traits>::Iterator SearchTree<Traits>::end() const {
	return Iterator(_head, &_data);
}

template<class Traits>
void SearchTree<Traits>::_print_graph(const size_t& ident, Node* root, const custom::String& rlFlag) const {
	custom::String str;
	str.append(ident, '\t');

	std::cout << str << Traits::extract_key(root->_Value) << " [" << ((int)root->_Color ? "black" : "red") << " " << rlFlag << "]\n";

	if (root->_Left)
		_print_graph(ident + 1, root->_Left, "LEFT");

	if (root->_Right)
		_print_graph(ident + 1, root->_Right, "RIGHT");
}

template<class Traits>
void SearchTree<Traits>::_rotate_left(Node* subroot) {
	Node* promotedNode = subroot->_Right;
	subroot->_Right = promotedNode->_Left;			// subroot adopt left child of promoted

	if (promotedNode->_Left != nullptr)
		promotedNode->_Left->_Parent = subroot;		// subroot-right-left parent set

	promotedNode->_Parent = subroot->_Parent;		// promoted takes subroot parent

	if (subroot == _head)							// special case when tree root is chosen for rotation
	{
		_head = promotedNode;
		promotedNode->_Parent = nullptr;
	}
	else if (subroot == subroot->_Parent->_Left)	// parent links his new promoted child
		subroot->_Parent->_Left = promotedNode;
	else
		subroot->_Parent->_Right = promotedNode;

	promotedNode->_Left = subroot;					// promoted takes subroot as left child
	subroot->_Parent = promotedNode;				// subroot has promoted as new parent
}

template<class Traits>
void SearchTree<Traits>::_rotate_right(Node* subroot) {
	Node* promotedNode = subroot->_Left;
	subroot->_Left = promotedNode->_Right;			// subroot adopt right child of promoted

	if (promotedNode->_Right != nullptr)
		promotedNode->_Right->_Parent = subroot;	// subroot-left-right parent set

	promotedNode->_Parent = subroot->_Parent;		// promoted takes subroot parent

	if (subroot == _head)							// special case when tree root is chosen for rotation
	{
		_head = promotedNode;
		promotedNode->_Parent = nullptr;
	}
	else if (subroot == subroot->_Parent->_Left)
		subroot->_Parent->_Left = promotedNode;		// parent links his new promoted child
	else
		subroot->_Parent->_Right = promotedNode;

	promotedNode->_Right = subroot;					// promoted takes subroot as right child
	subroot->_Parent = promotedNode;				// subroot has promoted as new parent
}

template<class Traits>
void SearchTree<Traits>::_insert(Node* newNode) {

	if (_head == nullptr)
		_head = newNode;
	else
	{
		_insert_raw(newNode);
		_fix_inserted(newNode);
	}

	++_size;
}

template<class Traits>
void SearchTree<Traits>::_insert_raw(Node* newNode) {
	Node* futureParent = nullptr;

	for (_workspaceNode = _head; _workspaceNode != nullptr;)	// find parent for newly created node
	{
		futureParent = _workspaceNode;

		if (_less(Traits::extract_key(newNode->_Value), Traits::extract_key(futureParent->_Value)))
		{
			_workspaceNode = _workspaceNode->_Left;
			if (_workspaceNode == nullptr)
				futureParent->_Left = newNode;
		}
		else
		{
			_workspaceNode = _workspaceNode->_Right;
			if (_workspaceNode == nullptr)
				futureParent->_Right = newNode;
		}
	}

	newNode->_Parent = futureParent;
	newNode->_Color = Node::Colors::Red;
}

template<class Traits>
void SearchTree<Traits>::_fix_inserted(Node* newNode) {		// TODO: check (should work now...)
	Node* uncle = nullptr;
	_workspaceNode = newNode;													// initialize violation with newly inserted node

	while (_has_grandparent(_workspaceNode) && _workspaceNode->_Parent->_Color == Node::Colors::Red)
	{
		if (_workspaceNode->_Parent == _workspaceNode->_Parent->_Parent->_Left)
		{
			uncle = _workspaceNode->_Parent->_Parent->_Right;
			if(uncle == nullptr || uncle->_Color == Node::Colors::Black)		// uncle black
			{
				if (_workspaceNode == _workspaceNode->_Parent->_Right)			// case 2 = uncle black (triangle)
				{
					_workspaceNode = _workspaceNode->_Parent;
					_rotate_left(_workspaceNode);
				}	

				_workspaceNode->_Parent->_Color = Node::Colors::Black;			// case 3 = uncle black (line)
				_workspaceNode->_Parent->_Parent->_Color = Node::Colors::Red;
				_rotate_right(_workspaceNode->_Parent->_Parent);
			}
			else																// case 1 = uncle red
			{
				_workspaceNode->_Parent->_Color = Node::Colors::Black;
				uncle->_Color = Node::Colors::Black;
				_workspaceNode->_Parent->_Parent->_Color = Node::Colors::Red;
				_workspaceNode = _workspaceNode->_Parent->_Parent;
			}
		}
		else																	// simetrical situation
		{
			uncle = _workspaceNode->_Parent->_Parent->_Left;
			if(uncle == nullptr || uncle->_Color == Node::Colors::Black)
			{
				if (_workspaceNode == _workspaceNode->_Parent->_Left)
				{
					_workspaceNode = _workspaceNode->_Parent;
					_rotate_right(_workspaceNode->_Parent);
				}

				_workspaceNode->_Parent->_Color = Node::Colors::Black;
				_workspaceNode->_Parent->_Parent->_Color = Node::Colors::Red;
				_rotate_left(_workspaceNode->_Parent->_Parent);
			}
			else
			{
				_workspaceNode->_Parent->_Color = Node::Colors::Black;
				uncle->_Color = Node::Colors::Black;
				_workspaceNode->_Parent->_Parent->_Color = Node::Colors::Red;
				_workspaceNode = _workspaceNode->_Parent->_Parent;
			}
		}
	}

	_head->_Color = Node::Colors::Black;
}

template<class Traits>
typename SearchTree<Traits>::Node* SearchTree<Traits>::_find_in_tree(const KeyType& key) const {
	Node* found = nullptr;

	for (_workspaceNode = _head; _workspaceNode != nullptr; )
	{
		if (key == Traits::extract_key(_workspaceNode->_Value))
			found = _workspaceNode;
		else if (_less(key, Traits::extract_key(_workspaceNode->_Value)))
		{
			_workspaceNode = _workspaceNode->_Left;
		}
		else
		{
			_workspaceNode = _workspaceNode->_Right;
		}
	}

	return found;
}

template<class Traits>
bool SearchTree<Traits>::_has_parent(Node* node) {
	return node->_Parent != nullptr;
}

template<class Traits>
bool SearchTree<Traits>::_has_grandparent(Node* node) {
	return (node->_Parent != nullptr && node->_Parent->_Parent != nullptr);
}
// END SearchTree Template

CUSTOM_END