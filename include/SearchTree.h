#pragma once
#include "List.h"
#include "Pair.h"
#include "Utility.h"

#include <iostream>
#include <functional>	// std::less


CUSTOM_BEGIN

enum class TreeChild {
	Left,
	Right
};

template<class Node>
struct TreeNodeID {
	Node* _Parent		= nullptr;
	TreeChild _Child	= TreeChild::Left;
};

template<class Type>
struct SearchTreeIterationData {	// Data used for iterating SearchTree
	Type* _Begin	= nullptr;
	Type* _End		= nullptr;

	SearchTreeIterationData() = default;
	~SearchTreeIterationData() {
		_Begin	= nullptr;
		_End	= nullptr;
	}
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
	explicit SearchTreeIterator(IterType* ptr, Data* data)
		:_Ptr(ptr), _Data(data) { /*Empty*/ }

	~SearchTreeIterator() {
		_Ptr	= nullptr;
		_Data	= nullptr;
	}

	SearchTreeIterator& operator++() {
		if (_Ptr == _Data->_End)
			throw std::out_of_range("Cannot increment end iterator...");

		if (_Ptr->_Right->_IsNil)
		{
			IterType* node = _Ptr->_Parent;
			while (!node->_IsNil && _Ptr == node->_Right)
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

	SearchTreeIterator operator++(int) {
		SearchTreeIterator temp = *this;
		++(*this);
		return temp;
	}

	SearchTreeIterator& operator--() {
		if (_Ptr == _Data->_Begin)
			throw std::out_of_range("Cannot decrement begin iterator...");

		if (_Ptr->_IsNil)
			_Ptr = _Ptr->_Right;
		else if (_Ptr->_Left->_IsNil)
		{
			IterType* node = _Ptr->_Parent;
			while (!node->_IsNil && _Ptr == node->_Left)
			{
				_Ptr = node;
				node = _Ptr->_Parent;
			}

			if (!_Ptr->_IsNil)	// decrement non-begin
				_Ptr = node;
		}
		else
			_Ptr = SearchTree::rightmost(_Ptr->_Left);

		return *this;
	}

	SearchTreeIterator operator--(int) {
		SearchTreeIterator temp = *this;
		--(*this);
		return temp;
	}

	IterType* operator->() {
		if (_Ptr == _Data->_End)
			throw std::out_of_range("Cannot access end iterator...");

		return _Ptr;
	}

	ValueType& operator*() {
		if (_Ptr == _Data->_End)
			throw std::out_of_range("Cannot dereference end iterator...");

		return _Ptr->_Value;
	}

	bool operator==(const SearchTreeIterator& other) const {
		return _Ptr == other._Ptr;
	}

	bool operator!=(const SearchTreeIterator& other) const {
		return !(*this == other);
	}
}; // END SearchTree Iterator


template<class Traits>
class SearchTree			// SearchTree Template implemented as Red-Black Tree
{
protected:
    using KeyType       	= typename Traits::KeyType;					// Type of Key
    using MappedType    	= typename Traits::MappedType;				// Type of Mapped _Value
    using KeyCompare    	= typename Traits::KeyCompare;				// Comparison struct

public:
    using ValueType     	= typename Traits::ValueType;				// Type of values stored in container
	using Node 				= TreeNode<ValueType>;						// Node component from Tree
	using IterType			= Node;										// Type of iterating element
	using Alloc				= Allocator<Node>;							// Allocator for Node type
	using Iterator			= SearchTreeIterator<SearchTree<Traits>>;	// Iterator type
	using Data				= typename Iterator::Data;					// Iteration data
	using ReverseIterator 	= ReverseIterator<Iterator>;				// ReverseIterator type

protected:
	Alloc _alloc;
	size_t _size			= 0;										// Number of Nodes held by this
	Node* _head				= nullptr;									// Helper node used to link min and max elems for iteration (root == head->parent)
    KeyCompare _less;													// Used for comparison

	mutable Data _data;

protected:
	// Constructors

	SearchTree() {
		_create_head();
	}

	SearchTree(const SearchTree& other) : SearchTree() {
		_copy(other);
	}

	SearchTree(SearchTree&& other) noexcept : SearchTree() {
		_move(custom::move(other));
	}

	virtual ~SearchTree() {
		_destroy_all(_head->_Parent);
		_free_head();
	}

protected:
	// Operators

	SearchTree& operator=(const SearchTree& other) {
		if (_head != other._head)
		{
			clear();
			_copy(other);
		}

		return *this;
	}

	SearchTree& operator=(SearchTree&& other) noexcept {
		if (_head != other._head)
		{
			clear();
			_move(custom::move(other));
		}

		return *this;
	}

	bool operator==(const SearchTree& other) const {				// Contains the same elems, but not the same tree
		if (size() != other.size())
			return false;

		Iterator it1 = begin();
		Iterator it2 = other.begin();
		while (it1 != end())
		{
			if (*it1 != *it2)	// mapping is the same
				return false;

			++it1;
			++it2;
		}

		return true;
	}

	bool operator!=(const SearchTree& other) const {
		return !(*this == other);
	}

public:
    // Main functions

	static Node* leftmost(Node* node) noexcept {					// return leftmost node in subtree at node
		while (!node->_Left->_IsNil)
			node = node->_Left;

		return node;
	}

	static Node* rightmost(Node* node) noexcept {					// return leftmost node in subtree at node
		while (!node->_Right->_IsNil)
			node = node->_Right;

		return node;
	}

    template<class... Args>
	Iterator emplace(Args&&... args) {								// Constructs Node first with any given arguments
		Node* newNode = _create_common_node(custom::forward<Args>(args)...);
		const KeyType& newKey = Traits::extract_key(newNode->_Value);
		Iterator it = find(newKey);

		if (it != end())
		{
			delete newNode;
			return it;
		}
		else
		{
			auto insertPosition = _find_insertion_slot(newNode);
			_insert(newNode, insertPosition);
			return Iterator(newNode, _update_iteration_data());
		}
	}

	Iterator erase(const KeyType& key) {
		Node* nodeToErase = _find_in_tree(key);
		_destroy(nodeToErase);

		return Iterator(_head, _update_iteration_data());	// TODO: not ok
	}

	Iterator erase(const Iterator& iterator) {
		if (iterator == end())
			throw std::out_of_range("Map erase iterator outside range...");

		return erase(Traits::extract_key(iterator._Ptr->_Value));
	}

	Iterator find(const KeyType& key) const {
		Node* foundNode = _find_in_tree(key);
		if (foundNode != nullptr)
			return Iterator(foundNode, _update_iteration_data());

		return end();
	}

	size_t size() const {
		return _size;
	}

	bool empty() const {
		return _size == 0;
	}

	void clear() {
		_destroy_all(_head->_Parent);
		_head->_Parent	= _head;
		_head->_Left	= _head;
		_head->_Right	= _head;
		_size			= 0;
	}

	void print_details() const {									// For Debugging
		std::cout << "Size= " << _size << '\n';
		_print_graph(0, _head->_Parent, "HEAD");
	}

	void test() {	// TODO: remove after testing
		Node* f = _find_in_tree(19);
		Node* s = _find_in_tree(7);
		_transplant(f,s);
	}

public:
	// Iterator functions

	Iterator begin() {
		return Iterator(_head->_Left, _update_iteration_data());
	}

	const Iterator begin() const {
		return Iterator(_head->_Left, _update_iteration_data());
	}

	ReverseIterator rbegin() {
		return ReverseIterator(end());
	}

	const ReverseIterator rbegin() const {
		return ReverseIterator(end());
	}

	Iterator end() {
		return Iterator(_head, _update_iteration_data());
	}

	const Iterator end() const {
		return Iterator(_head, _update_iteration_data());
	}

	ReverseIterator rend() {
		return ReverseIterator(begin());
	}

	const ReverseIterator rend() const {
		return ReverseIterator(begin());
	}

protected:
	// Others

	template<class _KeyType, class... Args>
	Iterator _try_emplace(_KeyType&& key, Args&&... args) {			// Force construction with known key and given arguments for object
		Iterator it = find(key);

		if (it != end())
			return it;
		else 
		{
			Node* newNode = _create_common_node(
				custom::piecewise_construct,
				custom::forward_as_tuple(custom::forward<_KeyType>(key)),
				custom::forward_as_tuple(custom::forward<Args>(args)...)
			);

			auto insertPosition = _find_insertion_slot(newNode);
			_insert(newNode, insertPosition);
			return Iterator(newNode, _update_iteration_data());
		}
	}

	const MappedType& _at(const KeyType& key) const {				// Access _Value at key with check
		Iterator it = find(key);
		if (it == end())
			throw std::out_of_range("Invalid key...");

		return Traits::extract_mapval((*it)->_Value);
	}

	MappedType& _at(const KeyType& key) {
		Iterator it = find(key);
		if (it == end())
			throw std::out_of_range("Invalid key...");

		return Traits::extract_mapval((*it)->_Value);
	}

private:
	// Helpers

	void _print_graph(const size_t& ident, Node* root, const custom::String& rlFlag) const {
		custom::String str;
		str.append(ident, '\t');

		if (!root->_IsNil)
			std::cout << str << Traits::extract_key(root->_Value) << " [" << ((int)root->_Color ? "black" : "red") << " " << rlFlag << "]\n";

		if (!root->_Left->_IsNil)
			_print_graph(ident + 1, root->_Left, "LEFT");

		if (!root->_Right->_IsNil)
			_print_graph(ident + 1, root->_Right, "RIGHT");
	}
	
	void _rotate_left(Node* subroot) {								// promotes subroot right
		Node* promotedNode = subroot->_Right;
		subroot->_Right = promotedNode->_Left;			// subroot adopt left child of promoted

		if (!promotedNode->_Left->_IsNil)
			promotedNode->_Left->_Parent = subroot;		// subroot-right-left parent set

		promotedNode->_Parent = subroot->_Parent;		// promoted takes subroot parent

		if (subroot == _head->_Parent)					// special case when tree root is chosen for rotation
			_head->_Parent = promotedNode;
		else if (subroot == subroot->_Parent->_Left)	// parent links his new promoted child
			subroot->_Parent->_Left = promotedNode;
		else
			subroot->_Parent->_Right = promotedNode;

		promotedNode->_Left = subroot;					// promoted takes subroot as left child
		subroot->_Parent = promotedNode;				// subroot has promoted as new parent
	}

	void _rotate_right(Node* subroot) {								// promotes subroot left
		Node* promotedNode = subroot->_Left;
		subroot->_Left = promotedNode->_Right;			// subroot adopt right child of promoted

		if (!promotedNode->_Right->_IsNil)
			promotedNode->_Right->_Parent = subroot;	// subroot-left-right parent set

		promotedNode->_Parent = subroot->_Parent;		// promoted takes subroot parent

		if (subroot == _head->_Parent)					// special case when tree root is chosen for rotation
			_head->_Parent = promotedNode;
		else if (subroot == subroot->_Parent->_Left)
			subroot->_Parent->_Left = promotedNode;		// parent links his new promoted child
		else
			subroot->_Parent->_Right = promotedNode;

		promotedNode->_Right = subroot;					// promoted takes subroot as right child
		subroot->_Parent = promotedNode;				// subroot has promoted as new parent
	}

	Node* _copy_all(Node* subroot) {								// DFS Preorder
		if (subroot->_IsNil)
			return _head;

		Node* newNode 		= new Node(subroot->_Value);
		newNode->_IsNil		= false;
		newNode->_Color		= subroot->_Color;

		newNode->_Left = _copy_all(subroot->_Left);
		if (!newNode->_Left->_IsNil)
			newNode->_Left->_Parent = newNode;

		newNode->_Right = _copy_all(subroot->_Right);
		if (!newNode->_Right->_IsNil)
			newNode->_Right->_Parent = newNode;

		return newNode;
	}

	void _destroy_all(Node* subroot) {								// DFS Postorder
		if (subroot->_IsNil)
			return;

		_destroy_all(subroot->_Left);
		_destroy_all(subroot->_Right);

		delete subroot;
	}

	bool _is_leaf(Node* node) const {
		return (node->_Left->_IsNil && node->_Right->_IsNil);
	}

	Node* _in_order_successor(Node* node) const {
		while (!_is_leaf(node))
			if (!node->_Right->_IsNil)
				node = leftmost(node->_Right);
			else
				node = leftmost(node);

		return node;
	}

	Node* _find_in_tree(const KeyType& key) const {
		Node* found = nullptr;

		for (Node* iterNode = _head->_Parent; !iterNode->_IsNil; )	// TODO: check warning
		{
			if (key == Traits::extract_key(iterNode->_Value))
			{
				found = iterNode;
				iterNode = _head;
			}
			else if (_less(key, Traits::extract_key(iterNode->_Value)))
				iterNode = iterNode->_Left;
			else
				iterNode = iterNode->_Right;
		}

		return found;
	}

	TreeNodeID<Node> _find_insertion_slot(Node* newNode) const {
		TreeNodeID<Node> position;

		if (_head->_Parent == _head)	// First node
			position._Parent = _head;
		else
			for (Node* iterNode = _head->_Parent; !iterNode->_IsNil;)	// find parent for newly created node
			{
				position._Parent = iterNode;
				if (_less(Traits::extract_key(newNode->_Value), Traits::extract_key(iterNode->_Value)))
				{
					iterNode = iterNode->_Left;
					if (iterNode->_IsNil)
						position._Child = TreeChild::Left;
				}
				else
				{
					iterNode = iterNode->_Right;
					if (iterNode->_IsNil)
						position._Child = TreeChild::Right;
				}
			}

		return position;
	}

	void _insert(Node* newNode, const TreeNodeID<Node>& position) {
		++_size;

		// Raw Insert
		newNode->_Parent = position._Parent;

		if (position._Parent == _head)					// first node
		{
			_head->_Parent	= newNode;
			_head->_Left	= newNode;
			_head->_Right	= newNode;
			newNode->_Color = Node::Colors::Black;
		}
		else if (position._Child == TreeChild::Left)	// add to left
		{
			position._Parent->_Left = newNode;
			if (position._Parent == _head->_Left)
				_head->_Left = newNode;
		}
		else											// add to right
		{
			position._Parent->_Right = newNode;
			if (position._Parent == _head->_Right)
				_head->_Right = newNode;
		}

		// Fix Insert
		Node* uncle = nullptr;
		Node* tempNode = newNode;													// initialize violation with newly inserted node

		while (tempNode->_Parent->_Color == Node::Colors::Red)
		{
			if (tempNode->_Parent == tempNode->_Parent->_Parent->_Left)
			{
				uncle = tempNode->_Parent->_Parent->_Right;
				if (uncle->_Color == Node::Colors::Black)							// uncle black
				{
					if (tempNode == tempNode->_Parent->_Right)						// case 2 = uncle black (triangle)
					{
						tempNode = tempNode->_Parent;
						_rotate_left(tempNode);
					}

					tempNode->_Parent->_Color			= Node::Colors::Black;		// case 3 = uncle black (line)
					tempNode->_Parent->_Parent->_Color	= Node::Colors::Red;
					_rotate_right(tempNode->_Parent->_Parent);
				}
				else																// case 1 = uncle red
				{
					tempNode->_Parent->_Color			= Node::Colors::Black;
					uncle->_Color						= Node::Colors::Black;
					tempNode->_Parent->_Parent->_Color	= Node::Colors::Red;
					tempNode							= tempNode->_Parent->_Parent;
				}
			}
			else																	// simetrical situation
			{
				uncle = tempNode->_Parent->_Parent->_Left;
				if (uncle->_Color == Node::Colors::Black)
				{
					if (tempNode == tempNode->_Parent->_Left)
					{
						tempNode = tempNode->_Parent;
						_rotate_right(tempNode);
					}

					tempNode->_Parent->_Color			= Node::Colors::Black;
					tempNode->_Parent->_Parent->_Color	= Node::Colors::Red;
					_rotate_left(tempNode->_Parent->_Parent);
				}
				else
				{
					tempNode->_Parent->_Color			= Node::Colors::Black;
					uncle->_Color						= Node::Colors::Black;
					tempNode->_Parent->_Parent->_Color	= Node::Colors::Red;
					tempNode							= tempNode->_Parent->_Parent;
				}
			}
		}

		_head->_Parent->_Color = Node::Colors::Black;								// root is black
	}

	void _destroy(Node* oldNode) {
		Node* tempNode = _in_order_successor(oldNode);	// Now tempNode is leaf

		if (tempNode->_Color == Node::Colors::Red)
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

	void _fix_destroy(Node* node) {
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

	void _transplant(Node* first, Node* second) {
		if(first == second)
			return;

		_swap_parents(first, second);	
		_swap_children(first, second);
		_swap_colors(first, second);
	}

	void _swap_parents(Node* first, Node* second) {
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

	void _swap_children(Node* first, Node* second) {
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

	void _swap_colors(Node* first, Node* second) {
		std::swap(first->_Color, second->_Color);
	}

	void _detach_parent(Node* node) {
		if (node == node->_Parent->_Left)
			node->_Parent->_Left = nullptr;
		else
			node->_Parent->_Right = nullptr;

		node->_Parent = nullptr;
	}

	void _create_head() {
		_head 			= _alloc.alloc(1);
		_head->_Parent	= _head;
		_head->_Left	= _head;
		_head->_Right	= _head;
		_head->_IsNil	= true;
		_head->_Color	= Node::Colors::Black;
	}

	void _free_head() {
		_head->_Parent	= nullptr;
		_head->_Left	= nullptr;
		_head->_Right	= nullptr;
		_alloc.dealloc(_head, 1);
	}

	template<class... Args>
	Node* _create_common_node(Args&&... args) {
		Node* newNode 		= new Node(custom::forward<Args>(args)...);
		newNode->_Parent	= _head;
		newNode->_Left		= _head;
		newNode->_Right		= _head;
		newNode->_IsNil		= false;
		newNode->_Color		= Node::Colors::Red;

		return newNode;
	}

	void _copy(const SearchTree& other) {
		_head->_Parent 				= _copy_all(other._head->_Parent);
		_head->_Left 				= leftmost(_head->_Parent);
		_head->_Right 				= rightmost(_head->_Parent);
		_head->_Parent->_Parent 	= _head;
		_size 						= other._size;
		_update_iteration_data();
	}

	void _move(SearchTree&& other) {
		std::swap(_head, other._head);

		_size = other._size;
		other._size = 0;

		_update_iteration_data();
		other._update_iteration_data();
	}

	Data* _update_iteration_data() const {
		_data._Begin	= _head->_Left;
		_data._End		= _head;

		return &_data;
	}
}; // END SearchTree Template

CUSTOM_END