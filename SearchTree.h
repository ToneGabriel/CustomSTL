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
    using KeyType       = typename Traits::KeyType;					// Type of Key
    using MappedType    = typename Traits::MappedType;				// Type of Mapped _Value
    using KeyCompare    = typename Traits::KeyCompare;				// Comparison struct

public:
    using ValueType     = typename Traits::ValueType;				// Type of values stored in container
	using Node 			= TreeNode<ValueType>;						// Node component from Tree
	using IterType		= Node;										// Type of iterating element
	using Iterator		= SearchTreeIterator<SearchTree<Traits>>;	// Iterator type
	using Alloc			= Allocator<Node>;							// Allocator for Node type
	using Data			= typename Iterator::Data;					// Iteration data

protected:
	size_t _size		= 0;										// Number of Nodes held by this
	Node* _head			= nullptr;									// Helper node used to link min and max elems for iteration (root == head->parent)
    KeyCompare _less;												// Used for comparison

	mutable Data _data;
	mutable Alloc _alloc;

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
	
	size_t size() const;
	bool empty() const;
	void clear();

	void print_details() const;
	void test();

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
	void _raw_insert(Node* newNode);
	void _fix_insert(Node* newNode);

	void _destroy_all(Node* subroot);								// DFS Postorder
	void _destroy(Node* oldNode);
	void _fix_destroy(Node* node);
	void _transplant(Node* first, Node* second);
	void _swap_parents(Node* first, Node* second);
	void _swap_children(Node* first, Node* second);
	void _swap_colors(Node* first, Node* second);
	void _detach_parent(Node* node);

	Node* _in_order_successor(Node* node);
	Node* _find_in_tree(const KeyType& key) const;

	Node* _parent(Node* node);
	Node* _grandparent(Node* node);
	Node* _sibling(Node* node);
	Node* _uncle(Node* node);
	bool _has_parent(Node* node);
	bool _has_grandparent(Node* node);
	bool _is_red(Node* node);
	bool _is_black(Node* node);
	bool _is_leaf(Node* node);

	void _create_head();
	void _free_head();

	void _copy(const SearchTree& other);
	void _move(SearchTree&& other);

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
SearchTreeIterator<SearchTree>& SearchTreeIterator<SearchTree>::operator++() {	// TODO: check and implement Link Node
	if (_Ptr == _Data->_End)
		throw std::out_of_range("Cannot increment end iterator...");

	if (_Ptr->_Right == nullptr)
	{
		IterType* node = _Ptr->_Parent;
		while (node != nullptr && _Ptr == node->_Right)
		{
			_Ptr = node;
			node = _Ptr->_Parent;
		}
		_Ptr = node;
	}
	else
		_Ptr = SearchTree::leftmost(_Ptr->_Right);

	return *this;
}

template<class SearchTree>
SearchTreeIterator<SearchTree> SearchTreeIterator<SearchTree>::operator++(int) {
	SearchTreeIterator temp = *this;
	++(*this);
	return temp;
}

template<class SearchTree>
SearchTreeIterator<SearchTree>& SearchTreeIterator<SearchTree>::operator--() {	// TODO: check and implement Link Node
	if (_Ptr == _Data->_End)
		_Ptr = _Ptr->_Right;
	else if (_Ptr->_Left == nullptr)
	{
		IterType* node = _Ptr->_Parent;
		while (node != nullptr && _Ptr == node->_Left)
		{
			_Ptr = node;
			node = _Ptr->_Parent;
		}

		if (_Ptr != nullptr)	// decrement non-begin
			_Ptr = node;
	}
	else
		_Ptr = SearchTree::rightmost(_Ptr->_Left);

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
	// TODO: implement check
	return _Ptr;
}

template<class SearchTree>
typename SearchTreeIterator<SearchTree>::ValueType& SearchTreeIterator<SearchTree>::operator*() {
	// TODO: implement check
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
	_create_head();
}

template<class Traits>
SearchTree<Traits>::SearchTree(const SearchTree& other) : SearchTree() {
	_copy(other);
}

template<class Traits>
SearchTree<Traits>::SearchTree(SearchTree&& other) noexcept : SearchTree() {
	_move(custom::move(other));
}

template<class Traits>
SearchTree<Traits>::~SearchTree() {
	clear();
	_free_head();
}

template<class Traits>
SearchTree<Traits>& SearchTree<Traits>::operator=(const SearchTree& other) {
	if (_head != other._head)
	{
		clear();
		_copy(other);
	}

	return *this;
}

template<class Traits>
SearchTree<Traits>& SearchTree<Traits>::operator=(SearchTree&& other) noexcept {
	if (_head != other._head)
	{
		clear();
		_move(custom::move(other));
	}

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
	if (node == nullptr)
		return node;

	while (node->_Left != nullptr)
		node = node->_Left;

	return node;
}

template<class Traits>
typename SearchTree<Traits>::Node* SearchTree<Traits>::rightmost(Node* node) noexcept {
	if (node == nullptr)
		return node;

	while (node->_Right != nullptr)
		node = node->_Right;

	return node;
}

template<class Traits>
template<class... Args>
typename SearchTree<Traits>::Iterator SearchTree<Traits>::emplace(Args&&... args) {
	Node* newNode = new Node(custom::forward<Args>(args)...);
	const KeyType& newKey = Traits::extract_key(newNode->_Value);
	Iterator it = find(newKey);

	if (it != end())
	{
		delete newNode;
		return it;
	}
	else
	{
		_insert(newNode);
		return Iterator(newNode, _update_iteration_data());
	}
}

template<class Traits>
typename SearchTree<Traits>::Iterator SearchTree<Traits>::erase(const KeyType& key) {
	Node* nodeToErase = _find_in_tree(key);
	_destroy(nodeToErase);

	return Iterator(_head, _update_iteration_data());	// TODO: not ok
}

template<class Traits>
typename SearchTree<Traits>::Iterator SearchTree<Traits>::erase(const Iterator& iterator) {	// TODO: check
	if (iterator == end())
		throw std::out_of_range("Map erase iterator outside range...");

	return erase(Traits::extract_key(iterator._Ptr->_Value));
}

template<class Traits>
typename SearchTree<Traits>::Iterator SearchTree<Traits>::find(const KeyType& key) const {	// TODO: check
	Node* foundNode = _find_in_tree(key);
	if (foundNode != nullptr)
		return Iterator(foundNode, _update_iteration_data());

	return end();
}

template<class Traits>
void SearchTree<Traits>::clear() {
	_destroy_all(_head->_Parent);
	_head->_Parent = _head;
	_head->_Left = _head;
	_head->_Right = _head;
	_size = 0;
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
	_print_graph(0, _head->_Parent, "HEAD");
}

template<class Traits>
void SearchTree<Traits>::test() {	// TODO: remove after testing
	Node* f = _find_in_tree(19);
	Node* s = _find_in_tree(7);
	_transplant(f,s);
}

template<class Traits>
typename SearchTree<Traits>::Iterator SearchTree<Traits>::begin() {	// TODO: implement
	return Iterator(_head->_Left, _update_iteration_data());
}

template<class Traits>
const typename SearchTree<Traits>::Iterator SearchTree<Traits>::begin() const {
	return Iterator(_head->_Left, _update_iteration_data());
}

template<class Traits>
typename SearchTree<Traits>::Iterator SearchTree<Traits>::end() {
	return Iterator(_head, _update_iteration_data());
}

template<class Traits>
const typename SearchTree<Traits>::Iterator SearchTree<Traits>::end() const {
	return Iterator(_head, _update_iteration_data());
}

template<class Traits>
template<class _KeyType, class... Args>
typename SearchTree<Traits>::Iterator SearchTree<Traits>::_try_emplace(_KeyType&& key, Args&&... args) {
	Iterator it = find(key);

	if (it != end())
		return it;
	else 
	{
		Node* newNode = new Node(
			custom::piecewise_construct,
			custom::forward_as_tuple(custom::forward<_KeyType>(key)),
			custom::forward_as_tuple(custom::forward<Args>(args)...)
		);
		const KeyType& newKey = Traits::extract_key(newNode->_Value);

		_insert(newNode);
		return Iterator(newNode, _update_iteration_data());
	}
}

template<class Traits>
const typename SearchTree<Traits>::MappedType& SearchTree<Traits>::_at(const KeyType& key) const {
	Iterator it = find(key);
	if (it == end())
		throw std::out_of_range("Invalid key...");

	return Traits::extract_mapval((*it)->_Value);
}

template<class Traits>
typename SearchTree<Traits>::MappedType& SearchTree<Traits>::_at(const KeyType& key) {
	Iterator it = find(key);
	if (it == end())
		throw std::out_of_range("Invalid key...");

	return Traits::extract_mapval((*it)->_Value);
}

template<class Traits>
void SearchTree<Traits>::_print_graph(const size_t& ident, Node* root, const custom::String& rlFlag) const {
	custom::String str;
	str.append(ident, '\t');

	if (root != _head)
		std::cout << str << Traits::extract_key(root->_Value) << " [" << ((int)root->_Color ? "black" : "red") << " " << rlFlag << "]\n";

	if (root->_Left != nullptr && root->_Left != _head)
		_print_graph(ident + 1, root->_Left, "LEFT");

	if (root->_Right != nullptr && root->_Left != _head)
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
void SearchTree<Traits>::_insert(Node* newNode) {	// TODO

	//if (_head->_Parent == _head)
	//{
	//	newNode->_Parent = _head;

	//	_head->_Parent = newNode;
	//	_head->_Left = newNode;
	//	_head->_Right = newNode;
	//}

	if (_head == nullptr)
		_head = newNode;
	else
	{
		_raw_insert(newNode);
		_fix_insert(newNode);
	}

	++_size;
}

template<class Traits>
void SearchTree<Traits>::_raw_insert(Node* newNode) {
	Node* futureParent = nullptr;

	for (Node* iterNode = _head; iterNode != nullptr;)	// find parent for newly created node
	{
		futureParent = iterNode;

		if (_less(Traits::extract_key(newNode->_Value), Traits::extract_key(futureParent->_Value)))
		{
			iterNode = iterNode->_Left;
			if (iterNode == nullptr)
				futureParent->_Left = newNode;
		}
		else
		{
			iterNode = iterNode->_Right;
			if (iterNode == nullptr)
				futureParent->_Right = newNode;
		}
	}

	newNode->_Parent = futureParent;
	newNode->_Color = Node::Colors::Red;
}

template<class Traits>
void SearchTree<Traits>::_fix_insert(Node* newNode) {		// TODO: check (should work now...)
	Node* uncle = nullptr;
	Node* tempNode = newNode;													// initialize violation with newly inserted node

	while (_has_grandparent(tempNode) && tempNode->_Parent->_Color == Node::Colors::Red)
	{
		if (tempNode->_Parent == tempNode->_Parent->_Parent->_Left)
		{
			uncle = tempNode->_Parent->_Parent->_Right;
			if(uncle == nullptr || uncle->_Color == Node::Colors::Black)		// uncle black
			{
				if (tempNode == tempNode->_Parent->_Right)			// case 2 = uncle black (triangle)
				{
					tempNode = tempNode->_Parent;
					_rotate_left(tempNode);
				}	

				tempNode->_Parent->_Color = Node::Colors::Black;			// case 3 = uncle black (line)
				tempNode->_Parent->_Parent->_Color = Node::Colors::Red;
				_rotate_right(tempNode->_Parent->_Parent);
			}
			else																// case 1 = uncle red
			{
				tempNode->_Parent->_Color = Node::Colors::Black;
				uncle->_Color = Node::Colors::Black;
				tempNode->_Parent->_Parent->_Color = Node::Colors::Red;
				tempNode = tempNode->_Parent->_Parent;
			}
		}
		else																	// simetrical situation
		{
			uncle = tempNode->_Parent->_Parent->_Left;
			if(uncle == nullptr || uncle->_Color == Node::Colors::Black)
			{
				if (tempNode == tempNode->_Parent->_Left)
				{
					tempNode = tempNode->_Parent;
					_rotate_right(tempNode->_Parent);
				}

				tempNode->_Parent->_Color = Node::Colors::Black;
				tempNode->_Parent->_Parent->_Color = Node::Colors::Red;
				_rotate_left(tempNode->_Parent->_Parent);
			}
			else
			{
				tempNode->_Parent->_Color = Node::Colors::Black;
				uncle->_Color = Node::Colors::Black;
				tempNode->_Parent->_Parent->_Color = Node::Colors::Red;
				tempNode = tempNode->_Parent->_Parent;
			}
		}
	}

	_head->_Color = Node::Colors::Black;
}

template<class Traits>
void SearchTree<Traits>::_destroy_all(Node* subroot) {
	if (subroot == nullptr || subroot == _head)
		return;

	_destroy_all(subroot->_Left);
	_destroy_all(subroot->_Right);

	delete subroot;
}

template<class Traits>
void SearchTree<Traits>::_destroy(Node* oldNode) {	// TODO: questionable code...
	Node* tempNode = _in_order_successor(oldNode);	// Now tempNode is leaf

	if (_is_red(tempNode))
	{
		_transplant(oldNode, tempNode);	// TODO: check
		_detach_parent(oldNode);
		delete oldNode;
	}
	else
	{
		// TODO: rebalance
	}

	--_size;
}

template<class Traits>
void SearchTree<Traits>::_fix_destroy(Node* node) {
	// while (node != _head && node->is_black())
	// {
	// 	if (node == node->_Parent->_Left)
	// 	{
	// 		_workspaceNode = node->_Parent->_Right;
	// 		if (_workspaceNode->is_red())
	// 		{
	// 			_workspaceNode->_Color = Node::Colors::Black;
	// 			node->_Parent->_Color = Node::Colors::Red;
	// 			_rotate_left(node->_Parent);
	// 			_workspaceNode = node->_Parent->_Right;
	// 		}
	// 		if (_workspaceNode->_Left->is_black() && _workspaceNode->_Right->is_black())
	// 		{
	// 			_workspaceNode->_Color = Node::Colors::Red;
	// 			node = node->_Parent;
	// 		}
	// 		else
	// 		{
	// 			if (_workspaceNode->_Right->is_black())
	// 			{
	// 				_workspaceNode->_Left->_Color = Node::Colors::Black;
	// 				_workspaceNode->_Color = Node::Colors::Red;
	// 				_rotate_right(_workspaceNode);
	// 				_workspaceNode = node->_Parent->_Right;
	// 			}

	// 			_workspaceNode->_Color = node->_Parent->_Color;
	// 			node->_Parent->_Color = Node::Colors::Black;
	// 			_workspaceNode->_Right->_Color = Node::Colors::Black;
	// 			_rotate_left(node->_Parent);
	// 			node = _head;
	// 		}
	// 	}
	// }

	// node->_Color = Node::Colors::Black;
}

template<class Traits>
void SearchTree<Traits>::_transplant(Node* first, Node* second) {	// TODO: check if needed
	if(first == second)
		return;

	_swap_parents(first, second);	
	_swap_children(first, second);
	_swap_colors(first, second);
}

template<class Traits>
void SearchTree<Traits>::_swap_parents(Node* first, Node* second) {
	Node* aux;

	if (_has_parent(first))		// check head first
		if (first == first->_Parent->_Left)
			first->_Parent->_Left = second;
		else
			first->_Parent->_Right = second;
	else
		_head = second;

	if (_has_parent(second))	// check head second
		if (second == second->_Parent->_Left)
			second->_Parent->_Left = first;
		else
			second->_Parent->_Right = first;
	else
		_head = first;

	aux = first->_Parent;
	first->_Parent = second->_Parent;
	second->_Parent = aux;
}

template<class Traits>
void SearchTree<Traits>::_swap_children(Node* first, Node* second) {
	Node* aux;

	// left child
	aux = first->_Left;
	first->_Left = second->_Left;
	second->_Left = aux;

	if (first->_Left != nullptr)
		first->_Left->_Parent = first;
	if (second->_Left != nullptr)
		second->_Left->_Parent = second;

	// right child
	aux = first->_Right;
	first->_Right = second->_Right;
	second->_Right = aux;

	if (first->_Right != nullptr)
		first->_Right->_Parent = first;
	if (second->_Right != nullptr)
		second->_Right->_Parent = second;
}

template<class Traits>
void SearchTree<Traits>::_swap_colors(Node* first, Node* second) {
	std::swap(first->_Color, second->_Color);
}

template<class Traits>
void SearchTree<Traits>::_detach_parent(Node* node) {
	if (node == node->_Parent->_Left)
		node->_Parent->_Left = nullptr;
	else
		node->_Parent->_Right = nullptr;

	node->_Parent = nullptr;
}

template<class Traits>
typename SearchTree<Traits>::Node* SearchTree<Traits>::_in_order_successor(Node* node) {
	while (!_is_leaf(node))
		if (node->_Right != nullptr)
			node = leftmost(node->_Right);
		else
			node = leftmost(node);

	return node;
}

template<class Traits>
typename SearchTree<Traits>::Node* SearchTree<Traits>::_find_in_tree(const KeyType& key) const {
	Node* found = nullptr;

	for (Node* iterNode = _head; iterNode != nullptr; )
	{
		if (key == Traits::extract_key(iterNode->_Value))
		{
			found = iterNode;
			iterNode = nullptr;
		}
		else if (_less(key, Traits::extract_key(iterNode->_Value)))
			iterNode = iterNode->_Left;
		else
			iterNode = iterNode->_Right;
	}

	return found;
}

template<class Traits>
typename SearchTree<Traits>::Node* SearchTree<Traits>::_parent(Node* node) {
	return node->_Parent;
}

template<class Traits>
typename SearchTree<Traits>::Node* SearchTree<Traits>::_grandparent(Node* node) {
	return node->_Parent->_Parent;
}

template<class Traits>
typename SearchTree<Traits>::Node* SearchTree<Traits>::_sibling(Node* node) {
	return (node == node->_Parent->_Left) ? node->_Parent->_Right : node->_Parent->_Left;
}

template<class Traits>
typename SearchTree<Traits>::Node* SearchTree<Traits>::_uncle(Node* node) {
	return (node->_Parent == node->_Parent->_Parent->_Left) ? node->_Parent->_Parent->_Right : node->_Parent->_Parent->_Left;
}

template<class Traits>
bool SearchTree<Traits>::_has_parent(Node* node) {
	return node->_Parent != nullptr;
}

template<class Traits>
bool SearchTree<Traits>::_has_grandparent(Node* node) {
	return (node->_Parent != nullptr && node->_Parent->_Parent != nullptr);
}

template<class Traits>
bool SearchTree<Traits>::_is_red(Node* node) {
	return (node != nullptr && node->_Color == Node::Colors::Red);
}

template<class Traits>
bool SearchTree<Traits>::_is_black(Node* node) {
	return (node == nullptr || node->_Color == Node::Colors::Black);
}

template<class Traits>
bool SearchTree<Traits>::_is_leaf(Node* node) {
	return (node->_Left == nullptr && node->_Right == nullptr);
}

template<class Traits>
void SearchTree<Traits>::_create_head() {
	_head = _alloc.alloc(1);
	_head->_Parent = _head;
	_head->_Left = _head;
	_head->_Right = _head;
}

template<class Traits>
void SearchTree<Traits>::_free_head() {
	_head->_Parent = nullptr;
	_head->_Left = nullptr;
	_head->_Right = nullptr;
	_alloc.dealloc(_head, 1);
}

template<class Traits>
void SearchTree<Traits>::_copy(const SearchTree& other) {
	// TODO: implement
}

template<class Traits>
void SearchTree<Traits>::_move(SearchTree&& other) {	// TODO: check
	// link current head with the other "body"
	_head->_Parent = other._head->_Parent;
	_head->_Parent->_Parent = _head;
	_head->_Left = other._head->_Left;
	_head->_Left->_Left = _head;
	_head->_Right = other._head->_Right;
	_head->_Right->_Right = _head;
	_size = other._size;
	_data = other._data;

	// link old head with itself
	other._head->_Parent = other._head;
	other._head->_Left = other._head;
	other._head->_Right = other._head;
	other._size = 0;
	other._update_iteration_data();
}

template<class Traits>
typename SearchTree<Traits>::Data* SearchTree<Traits>::_update_iteration_data() const {	// TODO: implement
	_data._Begin = _head->_Left;
	_data._End = _head;

	return &_data;
}
// END SearchTree Template

CUSTOM_END