#pragma once
#include "List.h"
#include "Pair.h"
#include "Utility.h"

#include <functional>	// std::less


CUSTOM_BEGIN

enum class _TreeChild : char
{
	Left,
	Right
};

template<class Node>
struct _TreeNodeID 			// Indicator for child placement
{							
	Node* _Parent		= nullptr;
	_TreeChild _Child	= _TreeChild::Left;
};

template<class Traits>
struct _SearchTreeData
{
	using ValueType		= typename Traits::ValueType;			// Type for stored values
	using Node			= node::_TreeNode<ValueType>;			// Node type
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

template<class _SearchTree>
class _ConstSearchTreeIterator
{
public:
	using Data			= typename _SearchTree::Data;
	using ValueType 	= typename _SearchTree::ValueType;
	using Node			= typename _SearchTree::Node;
	using Reference		= const ValueType&;
	using Pointer		= const Node*;

	Node* _Ptr			= nullptr;
	const Data* _Data	= nullptr;

public:
	explicit _ConstSearchTreeIterator(Node* ptr, const Data* data) noexcept
		:_Ptr(ptr), _Data(data) { /*Empty*/ }

	_ConstSearchTreeIterator& operator++() noexcept {
		CUSTOM_ASSERT(_Ptr != _Data->_Head, "Cannot increment end iterator...");

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

	_ConstSearchTreeIterator operator++(int) noexcept {
		_ConstSearchTreeIterator temp = *this;
		++(*this);
		return temp;
	}

	_ConstSearchTreeIterator& operator--() noexcept {
		CUSTOM_ASSERT(_Ptr != _Data->_Head->_Left, "Cannot decrement begin iterator...");

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

	_ConstSearchTreeIterator operator--(int) noexcept {
		_ConstSearchTreeIterator temp = *this;
		--(*this);
		return temp;
	}

	Pointer operator->() const noexcept{
		CUSTOM_ASSERT(_Ptr != _Data->_Head, "Cannot access end iterator...");
		return _Ptr;
	}

	Reference operator*() const noexcept{
		CUSTOM_ASSERT(_Ptr != _Data->_Head, "Cannot dereference end iterator...");
		return _Ptr->_Value;
	}

	bool operator==(const _ConstSearchTreeIterator& other) const noexcept {
		return _Ptr == other._Ptr;
	}

	bool operator!=(const _ConstSearchTreeIterator& other) const noexcept {
		return !(*this == other);
	}
}; // END _ConstSearchTreeIterator

template<class _SearchTree>
class _SearchTreeIterator : public _ConstSearchTreeIterator<_SearchTree>			// _SearchTree Iterator
{
private:
	using Base		= _ConstSearchTreeIterator<_SearchTree>;

public:
	using Data		= typename _SearchTree::Data;
	using ValueType = typename _SearchTree::ValueType;
	using Node		= typename _SearchTree::Node;
	using Reference	= ValueType&;
	using Pointer	= Node*;

public:

	explicit _SearchTreeIterator(Node* ptr, const Data* data) noexcept
		:Base(ptr, data) { /*Empty*/ }

	_SearchTreeIterator& operator++() noexcept {
		Base::operator++();
		return *this;
	}

	_SearchTreeIterator operator++(int) noexcept {
		_SearchTreeIterator temp = *this;
		Base::operator++();
		return temp;
	}

	_SearchTreeIterator& operator--() noexcept {
		Base::operator--();
		return *this;
	}

	_SearchTreeIterator operator--(int) noexcept {
		_SearchTreeIterator temp = *this;
		Base::operator--();
		return temp;
	}

	Pointer operator->() const noexcept {
		return const_cast<Pointer>(Base::operator->());
	}

	Reference operator*() const noexcept {
		return const_cast<Reference>(Base::operator*());
	}
}; // END _SearchTreeIterator


template<class Traits>
class _SearchTree			// _SearchTree Template implemented as Red-Black Tree
{
public:
    using KeyType       		= typename Traits::KeyType;						// Type of Key
    using MappedType    		= typename Traits::MappedType;					// Type of Mapped values
    using ValueType     		= typename Traits::ValueType;					// Type of values stored in container (raw or pair)
    using KeyCompare    		= typename Traits::KeyCompare;					// Comparison struct

	using Data					= _SearchTreeData<Traits>;
	using Node 					= typename Data::Node;							// Node component from Tree
	using Alloc					= typename Data::Alloc;							// Allocator for Node type
	
	using Iterator				= _SearchTreeIterator<_SearchTree<Traits>>;		// Iterator type
	using ConstIterator			= _ConstSearchTreeIterator<_SearchTree<Traits>>;
	using ReverseIterator 		= custom::ReverseIterator<Iterator>;			// ReverseIterator type
	using ConstReverseIterator 	= custom::ReverseIterator<ConstIterator>;

protected:
	Data _data;
	Alloc _alloc;
    KeyCompare _less;													// Used for comparison

protected:
	// Constructors

	_SearchTree() {
		_create_head();
	}

	_SearchTree(const _SearchTree& other) : _SearchTree() {
		_copy(other);
	}

	_SearchTree(_SearchTree&& other) noexcept : _SearchTree() {
		_move(custom::move(other));
	}

	virtual ~_SearchTree() {
		_destroy_all(_data._Head->_Parent);
		_free_head();
	}

protected:
	// Operators

	_SearchTree& operator=(const _SearchTree& other) {
		if (_data._Head != other._data._Head)
		{
			clear();
			_copy(other);
		}

		return *this;
	}

	_SearchTree& operator=(_SearchTree&& other) noexcept {
		if (_data._Head != other._data._Head)
		{
			clear();
			_move(custom::move(other));
		}

		return *this;
	}

	bool operator==(const _SearchTree& other) const {				// Contains the same elems, but not the same tree
		if (size() != other.size())
			return false;

		auto it1 = begin();
		auto it2 = other.begin();
		while (it1 != end())
			if (*(it1++) != *(it2++))
				return false;

		return true;
	}

	bool operator!=(const _SearchTree& other) const {
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
		Node* nodeToErase 		= _find_in_tree(key);
		Iterator nextIterator 	= ++Iterator(nodeToErase, &_data);
		_destroy(nodeToErase);

		return nextIterator;
	}

	Iterator erase(ConstIterator iterator) {
		if (iterator == end())
			throw std::out_of_range("Map erase iterator outside range...");

		return erase(Traits::extract_key(iterator._Ptr->_Value));
	}

	Iterator erase(Iterator iterator) {
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
	
	void _rotate_left(Node* subroot) {					// promotes subroot right
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

	_TreeNodeID<Node> _find_insertion_slot(Node* newNode) const {	// Find parent for newly created node
		_TreeNodeID<Node> position;

		if (_data._Head->_Parent == _data._Head)					// first node
			position._Parent = _data._Head;
		else
			for (Node* iterNode = _data._Head->_Parent; !iterNode->_IsNil;)
			{
				position._Parent = iterNode;
				if (_less(Traits::extract_key(newNode->_Value), Traits::extract_key(iterNode->_Value)))
				{
					iterNode = iterNode->_Left;
					if (iterNode->_IsNil)
						position._Child = _TreeChild::Left;
				}
				else
				{
					iterNode = iterNode->_Right;
					if (iterNode->_IsNil)
						position._Child = _TreeChild::Right;
				}
			}

		return position;
	}

	void _insert(Node* newNode, const _TreeNodeID<Node>& position) {
		++_data._Size;

		// Raw Insert
		newNode->_Parent = position._Parent;

		if (position._Parent == _data._Head)						// first node
		{
			_data._Head->_Parent	= newNode;
			_data._Head->_Left		= newNode;
			_data._Head->_Right		= newNode;
			newNode->_Color 		= Node::Colors::Black;
		}
		else if (position._Child == _TreeChild::Left)				// add to left
		{
			position._Parent->_Left = newNode;
			if (position._Parent == _data._Head->_Left)
				_data._Head->_Left = newNode;
		}
		else														// add to right
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

		_data._Head->_Parent->_Color = Node::Colors::Black;							// root is black
	}

	void _destroy(Node* oldNode) {
		--_data._Size;

		Node* fixNode; 			// the node to recolor as needed
        Node* fixNodeParent;
		Node* tempNode;

		// Switch old with successor until leaf
		while (!oldNode->is_leaf())
		{
			tempNode = _in_order_successor(oldNode);
			_transplant(oldNode, tempNode);
		}

		// Rebalance only if old color is black
		if (oldNode->_Color == Node::Colors::Black)
		{
			fixNode 		= oldNode;
			fixNodeParent 	= oldNode->_Parent;

			for (; fixNode != _data._Head->_Parent && fixNode->_Color == Node::Colors::Black; fixNodeParent = fixNode->_Parent)
			{
				if (fixNode == fixNodeParent->_Left)	// left subtree
				{
                    tempNode = fixNodeParent->_Right;
                    if (tempNode->_Color == Node::Colors::Red) 
					{
                        tempNode->_Color = Node::Colors::Black;
                        fixNodeParent->_Color = Node::Colors::Red;
                        _rotate_left(fixNodeParent);
                        tempNode = fixNodeParent->_Right;
                    }

                    if (tempNode->_Left->_Color == Node::Colors::Black && tempNode->_Right->_Color == Node::Colors::Black)
					{
                        tempNode->_Color = Node::Colors::Red;
                        fixNode = fixNodeParent;
                    } 
					else
					{
                        if (tempNode->_Right->_Color == Node::Colors::Black)
						{
                            tempNode->_Left->_Color = Node::Colors::Black;
                            tempNode->_Color = Node::Colors::Red;
                            _rotate_right(tempNode);
                            tempNode = fixNodeParent->_Right;
                        }

                        tempNode->_Color 			= fixNodeParent->_Color;
                        fixNodeParent->_Color 		= Node::Colors::Black;
                        tempNode->_Right->_Color 	= Node::Colors::Black;
                        _rotate_left(fixNodeParent);
                        break;	// rebalanced
                    }
                }
				else	// right subtree
				{
                    tempNode = fixNodeParent->_Left;
                    if (tempNode->_Color == Node::Colors::Red)
					{
                        tempNode->_Color = Node::Colors::Black;
                        fixNodeParent->_Color = Node::Colors::Red;
                        _rotate_right(fixNodeParent);
                        tempNode = fixNodeParent->_Left;
                    }

                    if (tempNode->_Right->_Color == Node::Colors::Black && tempNode->_Left->_Color == Node::Colors::Black)
					{
                        tempNode->_Color = Node::Colors::Red;
                        fixNode = fixNodeParent;
                    }
					else
					{
                        if (tempNode->_Left->_Color == Node::Colors::Black)
						{
                            tempNode->_Right->_Color = Node::Colors::Black;
                            tempNode->_Color = Node::Colors::Red;
                            _rotate_left(tempNode);
                            tempNode = fixNodeParent->_Left;
                        }

                        tempNode->_Color 			= fixNodeParent->_Color;
                        fixNodeParent->_Color 		= Node::Colors::Black;
                        tempNode->_Left->_Color 	= Node::Colors::Black;
                        _rotate_right(fixNodeParent);
                        break;	// rebalanced
                    }
                }
			}

            fixNode->_Color = Node::Colors::Black;									// stopping node is black
		}

		_free_common_node(oldNode);
		_data._Head->_Left 		= _data.leftmost(_data._Head->_Parent);
		_data._Head->_Right 	= _data.rightmost(_data._Head->_Parent);
	}

	void _transplant(Node* first, Node* second) {
		if (first == second)
			return;

		_swap_parents(first, second);	
		_swap_children(first, second);
		custom::swap(first->_Color, second->_Color);
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

		custom::swap(first->_Parent, second->_Parent);
	}

	void _swap_children(Node* first, Node* second) {
		Node* aux;

		// left child
		custom::swap(first->_Left, second->_Left);

		if (first->_Left != _data._Head)
			first->_Left->_Parent = first;
		if (second->_Left != _data._Head)
			second->_Left->_Parent = second;

		// right child
		custom::swap(first->_Right, second->_Right);
		
		if (first->_Right != _data._Head)
			first->_Right->_Parent = first;
		if (second->_Right != _data._Head)
			second->_Right->_Parent = second;
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
		// detach parent first
		if (oldNode == _data._Head->_Parent)
			_data._Head->_Parent = _data._Head;
		else if (oldNode == oldNode->_Parent->_Left)
			oldNode->_Parent->_Left = _data._Head;
		else
			oldNode->_Parent->_Right = _data._Head;

		delete oldNode;
	}

	void _copy(const _SearchTree& other) {
		_data._Head->_Parent 			= _copy_all(other._data._Head->_Parent);	// copy from root
		_data._Head->_Left 				= _data.leftmost(_data._Head->_Parent);
		_data._Head->_Right 			= _data.rightmost(_data._Head->_Parent);
		_data._Head->_Parent->_Parent 	= _data._Head;
		_data._Size 					= other._data._Size;
	}

	void _move(_SearchTree&& other) {
		custom::swap(_data._Head, other._data._Head);

		_data._Size 		= other._data._Size;
		other._data._Size 	= 0;
	}
}; // END _SearchTree Template

CUSTOM_END