#pragma once
#include "List.h"
#include "Pair.h"
#include "Utility.h"

#include <iostream>
#include <functional>	// std::less


CUSTOM_BEGIN

enum class TreeChild : char
{
	Left,
	Right
};

template<class Node>
struct TreeNodeID 			// Indicator for child placement
{							
	Node* _Parent		= nullptr;
	TreeChild _Child	= TreeChild::Left;
};

template<class Traits>
struct SearchTreeData
{
	using ValueType		= typename Traits::ValueType;			// Type for stored values
	using Node			= TreeNode<ValueType>;					// Node type
	using Alloc			= Allocator<Node>;						// Allocator type

	size_t _Size		= 0;									// Number of Nodes held
	Node* _Head			= nullptr;								// Helper node used to link min and max elems for iteration (root == head->parent)

	Node* leftmost(Node* node) const {							// return leftmost node in subtree at node
		while (!node->_Left->_IsNil)
			node = node->_Left;

		return node;
	}

	Node* rightmost(Node* node) const {							// return leftmost node in subtree at node
		while (!node->_Right->_IsNil)
			node = node->_Right;

		return node;
	}
};

template<class SearchTree>
class ConstSearchTreeIterator
{
public:
	using Data			= typename SearchTree::Data;
	using ValueType 	= typename SearchTree::ValueType;
	using Node			= typename SearchTree::Node;
	using Reference		= const ValueType&;
	using Pointer		= const Node*;

	Node* _Ptr			= nullptr;
	const Data* _Data	= nullptr;

public:
	explicit ConstSearchTreeIterator(Node* ptr, const Data* data)
		:_Ptr(ptr), _Data(data) { /*Empty*/ }

	ConstSearchTreeIterator& operator++() {
		if (_Ptr == _Data->_Head)
			throw std::out_of_range("Cannot increment end iterator...");

		if (_Ptr->_Right->_IsNil)
		{
			Node* node = _Ptr->_Parent;
			while (!node->_IsNil && _Ptr == node->_Right)
			{
				_Ptr = node;
				node = _Ptr->_Parent;
			}
			_Ptr = node;
		}
		else
			_Ptr = _Data->leftmost(_Ptr->_Right);

		return *this;
	}

	ConstSearchTreeIterator operator++(int) {
		ConstSearchTreeIterator temp = *this;
		++(*this);
		return temp;
	}

	ConstSearchTreeIterator& operator--() {
		if (_Ptr == _Data->_Head->_Left)
			throw std::out_of_range("Cannot decrement begin iterator...");

		if (_Ptr->_IsNil)
			_Ptr = _Ptr->_Right;
		else if (_Ptr->_Left->_IsNil)
		{
			Node* node = _Ptr->_Parent;
			while (!node->_IsNil && _Ptr == node->_Left)
			{
				_Ptr = node;
				node = _Ptr->_Parent;
			}

			if (!_Ptr->_IsNil)	// decrement non-begin
				_Ptr = node;
		}
		else
			_Ptr = _Data->rightmost(_Ptr->_Left);

		return *this;
	}

	ConstSearchTreeIterator operator--(int) {
		ConstSearchTreeIterator temp = *this;
		--(*this);
		return temp;
	}

	Pointer operator->() {
		if (_Ptr == _Data->_Head)
			throw std::out_of_range("Cannot access end iterator...");

		return _Ptr;
	}

	Reference operator*() {
		if (_Ptr == _Data->_Head)
			throw std::out_of_range("Cannot dereference end iterator...");

		return _Ptr->_Value;
	}

	bool operator==(const ConstSearchTreeIterator& other) const {
		return _Ptr == other._Ptr;
	}

	bool operator!=(const ConstSearchTreeIterator& other) const {
		return !(*this == other);
	}
};

template<class SearchTree>
class SearchTreeIterator : public ConstSearchTreeIterator<SearchTree>			// SearchTree Iterator
{
private:
	using Base		= ConstSearchTreeIterator<SearchTree>;

public:
	using Data		= typename SearchTree::Data;
	using ValueType = typename SearchTree::ValueType;
	using Node		= typename SearchTree::Node;
	using Reference	= ValueType&;
	using Pointer	= Node*;

public:

	explicit SearchTreeIterator(Node* ptr, const Data* data)
		:Base(ptr, data) { /*Empty*/ }

	SearchTreeIterator& operator++() {
		Base::operator++();
		return *this;
	}

	SearchTreeIterator operator++(int) {
		SearchTreeIterator temp = *this;
		Base::operator++();
		return temp;
	}

	SearchTreeIterator& operator--() {
		Base::operator--();
		return *this;
	}

	SearchTreeIterator operator--(int) {
		SearchTreeIterator temp = *this;
		Base::operator--();
		return temp;
	}

	Pointer operator->() {
		return const_cast<Pointer>(Base::operator->());
	}

	Reference operator*() {
		return const_cast<Reference>(Base::operator*());
	}
}; // END SearchTree Iterator


template<class Traits>
class SearchTree			// SearchTree Template implemented as Red-Black Tree
{
public:
    using KeyType       		= typename Traits::KeyType;						// Type of Key
    using MappedType    		= typename Traits::MappedType;					// Type of Mapped values
    using ValueType     		= typename Traits::ValueType;					// Type of values stored in container (raw or pair)
    using KeyCompare    		= typename Traits::KeyCompare;					// Comparison struct

	using Data					= SearchTreeData<Traits>;
	using Node 					= typename Data::Node;							// Node component from Tree
	using Alloc					= typename Data::Alloc;							// Allocator for Node type
	
	using Iterator				= SearchTreeIterator<SearchTree<Traits>>;		// Iterator type
	using ConstIterator			= ConstSearchTreeIterator<SearchTree<Traits>>;
	using ReverseIterator 		= custom::ReverseIterator<Iterator>;			// ReverseIterator type
	using ConstReverseIterator 	= custom::ReverseIterator<ConstIterator>;

protected:
	Data _data;
	Alloc _alloc;
    KeyCompare _less;													// Used for comparison

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
		_destroy_all(_data._Head->_Parent);
		_free_head();
	}

protected:
	// Operators

	SearchTree& operator=(const SearchTree& other) {
		if (_data._Head != other._data._Head)
		{
			clear();
			_copy(other);
		}

		return *this;
	}

	SearchTree& operator=(SearchTree&& other) noexcept {
		if (_data._Head != other._data._Head)
		{
			clear();
			_move(custom::move(other));
		}

		return *this;
	}

	bool operator==(const SearchTree& other) const {				// Contains the same elems, but not the same tree
		if (size() != other.size())
			return false;

		auto it1 = begin();
		auto it2 = other.begin();
		while (it1 != end())
			if (*(it1++) != *(it2++))
				return false;

		return true;
	}

	bool operator!=(const SearchTree& other) const {
		return !(*this == other);
	}

public:
    // Main functions

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
			return Iterator(newNode, &_data);
		}
	}

	Iterator erase(const KeyType& key) {
		Node* nodeToErase = _find_in_tree(key);
		_destroy(nodeToErase);

		return Iterator(_data._Head, &_data);	// TODO: not ok
	}

	Iterator erase(ConstIterator iterator) {
		if (iterator == end())
			throw std::out_of_range("Map erase iterator outside range...");

		return erase(Traits::extract_key(iterator._Ptr->_Value));
	}

	ConstIterator find (const KeyType& key) const {
		Node* foundNode = _find_in_tree(key);
		if (foundNode != nullptr)
			return ConstIterator(foundNode, &_data);

		return end();
	}

	Iterator find(const KeyType& key) {
		Node* foundNode = _find_in_tree(key);
		if (foundNode != nullptr)
			return Iterator(foundNode, &_data);

		return end();
	}

	size_t size() const {
		return _data._Size;
	}

	bool empty() const {
		return _data._Size == 0;
	}

	void clear() {
		_destroy_all(_data._Head->_Parent);
		_data._Head->_Parent 	= _data._Head;
		_data._Head->_Left 		= _data._Head;
		_data._Head->_Right		= _data._Head;
		_data._Size				= 0;
	}

	void print_details() const {									// For Debugging
		std::cout << "Size= " << _data._Size << '\n';
		std::cout << "First= " << Traits::extract_key(_data._Head->_Left->_Value) << '\n';
		std::cout << "Last= " << Traits::extract_key(_data._Head->_Right->_Value) << '\n';
		_print_graph(0, _data._Head->_Parent, "HEAD");
	}

	void test() {	// TODO: remove after testing
		Node* f = _find_in_tree(101);
		Node* s = _find_in_tree(99);
		_transplant(f,s);
	}

public:
	// Iterator functions

	Iterator begin() {
		return Iterator(_data._Head->_Left, &_data);
	}

	ConstIterator begin() const {
		return ConstIterator(_data._Head->_Left, &_data);
	}

	ReverseIterator rbegin() {
		return ReverseIterator(end());
	}

	ConstReverseIterator rbegin() const {
		return ConstReverseIterator(end());
	}

	Iterator end() {
		return Iterator(_data._Head, &_data);
	}

	ConstIterator end() const {
		return ConstIterator(_data._Head, &_data);
	}

	ReverseIterator rend() {
		return ReverseIterator(begin());
	}

	ConstReverseIterator rend() const {
		return ConstReverseIterator(begin());
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
				custom::PiecewiseConstruct,
				custom::forward_as_tuple(custom::forward<_KeyType>(key)),
				custom::forward_as_tuple(custom::forward<Args>(args)...)
			);

			auto insertPosition = _find_insertion_slot(newNode);
			_insert(newNode, insertPosition);
			return Iterator(newNode, &_data);
		}
	}

	const MappedType& _at(const KeyType& key) const {				// Access _Value at key with check
		ConstIterator it = find(key);
		if (it == end())
			throw std::out_of_range("Invalid key...");

		return Traits::extract_mapval(*it);
	}

	MappedType& _at(const KeyType& key) {
		Iterator it = find(key);
		if (it == end())
			throw std::out_of_range("Invalid key...");

		return const_cast<MappedType&>(Traits::extract_mapval(*it));
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

		if (subroot == _data._Head->_Parent)			// special case when tree root is chosen for rotation
			_data._Head->_Parent = promotedNode;
		else if (subroot == subroot->_Parent->_Left)	// parent links his new promoted child
			subroot->_Parent->_Left = promotedNode;
		else
			subroot->_Parent->_Right = promotedNode;

		promotedNode->_Left = subroot;					// promoted takes subroot as left child
		subroot->_Parent = promotedNode;				// subroot has promoted as new parent
	}

	void _rotate_right(Node* subroot) {					// promotes subroot left
		Node* promotedNode = subroot->_Left;
		subroot->_Left = promotedNode->_Right;			// subroot adopt right child of promoted

		if (!promotedNode->_Right->_IsNil)
			promotedNode->_Right->_Parent = subroot;	// subroot-left-right parent set

		promotedNode->_Parent = subroot->_Parent;		// promoted takes subroot parent

		if (subroot == _data._Head->_Parent)			// special case when tree root is chosen for rotation
			_data._Head->_Parent = promotedNode;
		else if (subroot == subroot->_Parent->_Left)
			subroot->_Parent->_Left = promotedNode;		// parent links his new promoted child
		else
			subroot->_Parent->_Right = promotedNode;

		promotedNode->_Right = subroot;					// promoted takes subroot as right child
		subroot->_Parent = promotedNode;				// subroot has promoted as new parent
	}

	Node* _copy_all(Node* subroot) {								// DFS Preorder
		if (subroot->_IsNil)
			return _data._Head;

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
		if (!node->_Right->_IsNil)
			node = _data.leftmost(node->_Right);
		else
			node = _data.leftmost(node);

		return node;
	}

	Node* _find_in_tree(const KeyType& key) const {
		Node* found = nullptr;

		for (Node* iterNode = _data._Head->_Parent; !iterNode->_IsNil; )
		{
			if (key == Traits::extract_key(iterNode->_Value))
			{
				found = iterNode;
				iterNode = _data._Head;
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

		if (_data._Head->_Parent == _data._Head)	// First node
			position._Parent = _data._Head;
		else
			for (Node* iterNode = _data._Head->_Parent; !iterNode->_IsNil;)	// find parent for newly created node
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
		++_data._Size;

		// Raw Insert
		newNode->_Parent = position._Parent;

		if (position._Parent == _data._Head)			// first node
		{
			_data._Head->_Parent	= newNode;
			_data._Head->_Left		= newNode;
			_data._Head->_Right		= newNode;
			newNode->_Color 		= Node::Colors::Black;
		}
		else if (position._Child == TreeChild::Left)	// add to left
		{
			position._Parent->_Left = newNode;
			if (position._Parent == _data._Head->_Left)
				_data._Head->_Left = newNode;
		}
		else											// add to right
		{
			position._Parent->_Right = newNode;
			if (position._Parent == _data._Head->_Right)
				_data._Head->_Right = newNode;
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

		_data._Head->_Parent->_Color = Node::Colors::Black;								// root is black
	}

	void _destroy(Node* oldNode) {	// TODO: implement
		--_data._Size;

		// Switch old with successor until leaf
		for (Node* tempNode = _in_order_successor(oldNode); !_is_leaf(oldNode);)
		{
			_transplant(oldNode, tempNode);
			tempNode = _in_order_successor(oldNode);
		}

		// Fix Delete
		Node* oldNodeParent = oldNode->_Parent;
		_free_common_node(oldNode);



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

	void _transplant(Node* first, Node* second) {	// seems OK
		if(first == second)
			return;

		_swap_parents(first, second);	
		_swap_children(first, second);
		_swap_colors(first, second);
	}

	void _swap_parents(Node* first, Node* second) {
		Node* aux;

		// check head first
		if (first->_Parent != _data._Head)
			if (first == first->_Parent->_Left)
				first->_Parent->_Left = second;
			else
				first->_Parent->_Right = second;
		else
			_data._Head->_Parent = second;

		// check head second
		if (second->_Parent != _data._Head)
			if (second == second->_Parent->_Left)
				second->_Parent->_Left = first;
			else
				second->_Parent->_Right = first;
		else
			_data._Head->_Parent = first;

		aux				= first->_Parent;
		first->_Parent	= second->_Parent;
		second->_Parent = aux;
	}

	void _swap_children(Node* first, Node* second) {
		Node* aux;

		// left child
		aux				= first->_Left;
		first->_Left	= second->_Left;
		second->_Left	= aux;

		if (first->_Left != _data._Head)
			first->_Left->_Parent = first;
		if (second->_Left != _data._Head)
			second->_Left->_Parent = second;


		// right child
		aux				= first->_Right;
		first->_Right	= second->_Right;
		second->_Right	= aux;

		if (first->_Right != _data._Head)
			first->_Right->_Parent = first;
		if (second->_Right != _data._Head)
			second->_Right->_Parent = second;

		// head link (if first/second are begin/end)
		if (_data._Head->_Left == first)
			_data._Head->_Left = second;
		else if (_data._Head->_Left == second)
			_data._Head->_Left = first;

		if (_data._Head->_Right == first)
			_data._Head->_Right = second;
		else if (_data._Head->_Right == second)
			_data._Head->_Right = first;
	}

	void _swap_colors(Node* first, Node* second) {
		std::swap(first->_Color, second->_Color);
	}

	void _detach_parent(Node* node) {
		if (node == _data._Head->_Parent)
			_data._Head->_Parent = _data._Head;
		else if (node == node->_Parent->_Left)
			node->_Parent->_Left = _data._Head;
		else
			node->_Parent->_Right = _data._Head;
	}

	void _create_head() {
		_data._Head 			= _alloc.alloc(1);
		_data._Head->_Parent	= _data._Head;
		_data._Head->_Left		= _data._Head;
		_data._Head->_Right		= _data._Head;
		_data._Head->_IsNil		= true;
		_data._Head->_Color		= Node::Colors::Black;
	}

	void _free_head() {
		_data._Head->_Parent	= nullptr;
		_data._Head->_Left		= nullptr;
		_data._Head->_Right		= nullptr;
		_alloc.dealloc(_data._Head, 1);
	}

	template<class... Args>
	Node* _create_common_node(Args&&... args) {
		Node* newNode 		= new Node(custom::forward<Args>(args)...);
		newNode->_Parent	= _data._Head;
		newNode->_Left		= _data._Head;
		newNode->_Right		= _data._Head;
		newNode->_IsNil		= false;
		newNode->_Color		= Node::Colors::Red;

		return newNode;
	}

	void _free_common_node(Node* oldNode) {
		_detach_parent(oldNode);
		delete oldNode;
	}

	void _copy(const SearchTree& other) {
		_data._Head->_Parent 			= _copy_all(other._data._Head->_Parent);	// copy from root
		_data._Head->_Left 				= _data.leftmost(_data._Head->_Parent);
		_data._Head->_Right 			= _data.rightmost(_data._Head->_Parent);
		_data._Head->_Parent->_Parent 	= _data._Head;
		_data._Size 					= other._data._Size;
	}

	void _move(SearchTree&& other) {
		std::swap(_data._Head, other._data._Head);

		_data._Size 		= other._data._Size;
		other._data._Size 	= 0;
	}
}; // END SearchTree Template

CUSTOM_END