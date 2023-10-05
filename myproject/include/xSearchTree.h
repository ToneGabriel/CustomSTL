#pragma once
#include "xMemory.h"
#include "Pair.h"
#include "Utility.h"
#include "Iterator.h"
#include "Functional.h"	// for custom::Less


CUSTOM_BEGIN

enum class _TreeChild : char
{
	Left,
	Right
};

template<class _NodePtr>
struct _TreeNodeID 			// Indicator for child placement
{							
	_NodePtr _Parent 	= nullptr;
	_TreeChild _Child 	= _TreeChild::Left;
};

template<class Traits>
struct _SearchTreeData
{
	using KeyType           = typename Traits::KeyType;				// Type of Key
    using MappedType        = typename Traits::MappedType;			// Type of Mapped _Value
	using KeyCompare 		= typename Traits::KeyCompare;
	using ValueType			= typename Traits::ValueType;			// Type of values stored in container
	using Reference 		= typename Traits::Reference;
	using ConstReference 	= typename Traits::ConstReference;
	using Pointer 			= typename Traits::Pointer;
	using ConstPointer 		= typename Traits::ConstPointer;
	using AllocatorType 	= typename Traits::AllocatorType;

	using _AllocTraits		= AllocatorTraits<AllocatorType>;
	using _Node				= node::_TreeNode<ValueType>;
	using _AllocNode		= typename _AllocTraits::template RebindAlloc<_Node>;
	using _AllocNodeTraits	= AllocatorTraits<_AllocNode>;
	using _NodePtr			= _AllocNodeTraits::Pointer;

	size_t _Size 			= 0;									// Number of Nodes held
	_NodePtr _Head 			= nullptr;								// Helper node used to link min and max elems for iteration (root == head->parent)

	_NodePtr leftmost(_NodePtr node) const {						// return leftmost node in subtree at node
		while (!node->_Left->_IsNil)
			node = node->_Left;

		return node;
	}

	_NodePtr rightmost(_NodePtr node) const {						// return rightmost node in subtree at node
		while (!node->_Right->_IsNil)
			node = node->_Right;

		return node;
	}
};

template<class _SearchTreeData>
class _SearchTreeConstIterator
{
private:
	using _Data				= _SearchTreeData;
	using _NodePtr 			= typename _Data::_NodePtr;

public:
    using IteratorCategory 	= BidirectionalIteratorTag;
	using ValueType 		= typename _Data::ValueType;
	using Reference			= typename _Data::ConstReference;
	using Pointer			= typename _Data::ConstPointer;

	_NodePtr _Ptr 			= nullptr;
	const _Data* _RefData	= nullptr;

public:

	_SearchTreeConstIterator() noexcept = default;

	explicit _SearchTreeConstIterator(_NodePtr ptr, const _Data* data) noexcept
		:_Ptr(ptr), _RefData(data) { /*Empty*/ }

	_SearchTreeConstIterator& operator++() noexcept {
		CUSTOM_ASSERT(_Ptr != _RefData->_Head, "Cannot increment end iterator...");

		if (_Ptr->_Right->_IsNil)
		{
			_NodePtr node = _Ptr->_Parent;
			while (!node->_IsNil && _Ptr == node->_Right)
			{
				_Ptr = node;
				node = _Ptr->_Parent;
			}
			_Ptr = node;
		}
		else
			_Ptr = _RefData->leftmost(_Ptr->_Right);

		return *this;
	}

	_SearchTreeConstIterator operator++(int) noexcept {
		_SearchTreeConstIterator temp = *this;
		++(*this);
		return temp;
	}

	_SearchTreeConstIterator& operator--() noexcept {
		CUSTOM_ASSERT(_Ptr != _RefData->_Head->_Left, "Cannot decrement begin iterator...");

		if (_Ptr->_IsNil)
			_Ptr = _Ptr->_Right;
		else if (_Ptr->_Left->_IsNil)
		{
			_NodePtr node = _Ptr->_Parent;
			while (!node->_IsNil && _Ptr == node->_Left)
			{
				_Ptr = node;
				node = _Ptr->_Parent;
			}

			if (!_Ptr->_IsNil)	// decrement non-begin
				_Ptr = node;
		}
		else
			_Ptr = _RefData->rightmost(_Ptr->_Left);

		return *this;
	}

	_SearchTreeConstIterator operator--(int) noexcept {
		_SearchTreeConstIterator temp = *this;
		--(*this);
		return temp;
	}

	Pointer operator->() const noexcept {
		CUSTOM_ASSERT(_Ptr != _RefData->_Head, "Cannot access end iterator...");
        return PointerTraits<Pointer>::pointer_to(**this);	// return &(**this);
	}

	Reference operator*() const noexcept {
		CUSTOM_ASSERT(_Ptr != _RefData->_Head, "Cannot dereference end iterator...");
		return _Ptr->_Value;
	}

	bool operator==(const _SearchTreeConstIterator& other) const noexcept {
		return _Ptr == other._Ptr;
	}

	bool operator!=(const _SearchTreeConstIterator& other) const noexcept {
		return !(*this == other);
	}

	friend void _verify_range(const _SearchTreeConstIterator& first, const _SearchTreeConstIterator& last) noexcept {
		CUSTOM_ASSERT(first._RefData == last._RefData, "_SearchTree iterators in range are from different containers");
		// No possible way to determine order.
	}
}; // END _SearchTreeConstIterator

template<class _SearchTreeData>
class _SearchTreeIterator : public _SearchTreeConstIterator<_SearchTreeData>			// _SearchTree Iterator
{
private:
	using _Base				= _SearchTreeConstIterator<_SearchTreeData>;
	using _Data 			= _SearchTreeData;
	using _NodePtr			= typename _Data::_NodePtr;

public:
    using IteratorCategory 	= BidirectionalIteratorTag;
	using ValueType 		= typename _Data::ValueType;
	using Reference 		= typename _Data::Reference;
	using Pointer 			= typename _Data::Pointer;

public:

	_SearchTreeIterator() noexcept = default;

	explicit _SearchTreeIterator(_NodePtr ptr, const _Data* data) noexcept
		:_Base(ptr, data) { /*Empty*/ }

	_SearchTreeIterator& operator++() noexcept {
		_Base::operator++();
		return *this;
	}

	_SearchTreeIterator operator++(int) noexcept {
		_SearchTreeIterator temp = *this;
		_Base::operator++();
		return temp;
	}

	_SearchTreeIterator& operator--() noexcept {
		_Base::operator--();
		return *this;
	}

	_SearchTreeIterator operator--(int) noexcept {
		_SearchTreeIterator temp = *this;
		_Base::operator--();
		return temp;
	}

	Pointer operator->() const noexcept {
		return const_cast<Pointer>(_Base::operator->());
	}

	Reference operator*() const noexcept {
		return const_cast<Reference>(_Base::operator*());
	}
}; // END _SearchTreeIterator


template<class Traits>
class _SearchTree			// _SearchTree Template implemented as Red-Black Tree
{
private:
	using _Data					= _SearchTreeData<Traits>;
	using _AllocTraits			= typename _Data::_AllocTraits;
	using _Node					= typename _Data::_Node;
	using _AllocNode			= typename _Data::_AllocNode;
	using _AllocNodeTraits		= typename _Data::_AllocNodeTraits;
	using _NodePtr				= typename _Data::_NodePtr;
	
protected:
    using KeyType       		= typename _Data::KeyType;
    using MappedType    		= typename _Data::MappedType;
    using KeyCompare    		= typename _Data::KeyCompare;
	using ValueType 			= typename _Data::ValueType;
	using Reference				= typename _Data::Reference;
	using ConstReference		= typename _Data::ConstReference;
	using Pointer				= typename _Data::Pointer;
	using ConstPointer			= typename _Data::ConstPointer;
	using AllocatorType			= typename _Data::AllocatorType;

	using Iterator				= _SearchTreeIterator<_Data>;
	using ConstIterator			= _SearchTreeConstIterator<_Data>;
	using ReverseIterator		= custom::ReverseIterator<Iterator>;
	using ConstReverseIterator	= custom::ReverseIterator<ConstIterator>;

protected:
	_Data _data;
	_AllocNode _alloc;
    KeyCompare _less;			// Used for comparison

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

public:
    // Main functions

    template<class... Args>
	Iterator emplace(Args&&... args) {								// Constructs Node first with any given arguments
		_NodePtr newNode 		= _create_common_node(custom::forward<Args>(args)...);
		const KeyType& newKey 	= Traits::extract_key(newNode->_Value);
		Iterator it 			= find(newKey);

		if (it != end())
		{
			_free_common_node_default(newNode);
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
		_NodePtr nodeToErase 	= _find_in_tree(key);
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
		_NodePtr foundNode = _find_in_tree(key);
		if (foundNode != nullptr)
			return ConstIterator(foundNode, &_data);

		return end();
	}

	Iterator find(const KeyType& key) {
		_NodePtr foundNode = _find_in_tree(key);
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
	Pair<Iterator, bool> _try_emplace(_KeyType&& key, Args&&... args) {			// Force construction with known key and given arguments for object
		Iterator it = find(key);

		if (it != end())
			return {it, false};
		else 
		{
			_NodePtr newNode = _create_common_node(
										custom::PiecewiseConstruct,
										custom::forward_as_tuple(custom::forward<_KeyType>(key)),
										custom::forward_as_tuple(custom::forward<Args>(args)...));

			auto insertPosition = _find_insertion_slot(newNode);
			_insert(newNode, insertPosition);
			return {Iterator(newNode, &_data), true};
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

	void _print_graph(const size_t& ident, _NodePtr root, const custom::String& rlFlag) const {
		custom::String str;
		str.append(ident, '\t');

		if (!root->_IsNil)
			std::cout << str << Traits::extract_key(root->_Value) << " [" << ((int)root->_Color ? "black" : "red") << " " << rlFlag << "]\n";

		if (!root->_Left->_IsNil)
			_print_graph(ident + 1, root->_Left, "LEFT");

		if (!root->_Right->_IsNil)
			_print_graph(ident + 1, root->_Right, "RIGHT");
	}
	
	void _rotate_left(_NodePtr subroot) {					// promotes subroot right
		_NodePtr promotedNode = subroot->_Right;
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

	void _rotate_right(_NodePtr subroot) {					// promotes subroot left
		_NodePtr promotedNode = subroot->_Left;
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

	_NodePtr _copy_all(_NodePtr subroot) {								// DFS Preorder
		if (subroot->_IsNil)
			return _data._Head;

		_NodePtr newNode = _alloc.allocate(1);
		_AllocNodeTraits::construct(_alloc, &(newNode->_Value), subroot->_Value);

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

	void _destroy_all(_NodePtr subroot) {								// DFS Postorder
		if (subroot->_IsNil)
			return;

		_destroy_all(subroot->_Left);
		_destroy_all(subroot->_Right);

		_free_common_node_default(subroot);
	}

	_NodePtr _in_order_successor(_NodePtr node) const {
		if (!node->_Right->_IsNil)
			node = _data.leftmost(node->_Right);
		else
			node = _data.leftmost(node);

		return node;
	}

	_NodePtr _find_in_tree(const KeyType& key) const {
		_NodePtr found = nullptr;

		for (_NodePtr iterNode = _data._Head->_Parent; !iterNode->_IsNil; )
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

	_TreeNodeID<_NodePtr> _find_insertion_slot(_NodePtr newNode) const {	// Find parent for newly created node
		_TreeNodeID<_NodePtr> position;

		if (_data._Head->_Parent == _data._Head)					// first node
			position._Parent = _data._Head;
		else
			for (_NodePtr iterNode = _data._Head->_Parent; !iterNode->_IsNil;)
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

	void _insert(_NodePtr newNode, const _TreeNodeID<_NodePtr>& position) {
		++_data._Size;

		// Raw Insert
		newNode->_Parent = position._Parent;

		if (position._Parent == _data._Head)						// first node
		{
			_data._Head->_Parent	= newNode;
			_data._Head->_Left		= newNode;
			_data._Head->_Right		= newNode;
			newNode->_Color 		= _Node::Colors::Black;
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
		_NodePtr uncle = nullptr;
		_NodePtr tempNode = newNode;													// initialize violation with newly inserted node

		while (tempNode->_Parent->_Color == _Node::Colors::Red)
		{
			if (tempNode->_Parent == tempNode->_Parent->_Parent->_Left)
			{
				uncle = tempNode->_Parent->_Parent->_Right;
				if (uncle->_Color == _Node::Colors::Black)							// uncle black
				{
					if (tempNode == tempNode->_Parent->_Right)						// case 2 = uncle black (triangle)
					{
						tempNode = tempNode->_Parent;
						_rotate_left(tempNode);
					}

					tempNode->_Parent->_Color			= _Node::Colors::Black;		// case 3 = uncle black (line)
					tempNode->_Parent->_Parent->_Color	= _Node::Colors::Red;
					_rotate_right(tempNode->_Parent->_Parent);
				}
				else																// case 1 = uncle red
				{
					tempNode->_Parent->_Color			= _Node::Colors::Black;
					uncle->_Color						= _Node::Colors::Black;
					tempNode->_Parent->_Parent->_Color	= _Node::Colors::Red;
					tempNode							= tempNode->_Parent->_Parent;
				}
			}
			else																	// simetrical situation
			{
				uncle = tempNode->_Parent->_Parent->_Left;
				if (uncle->_Color == _Node::Colors::Black)
				{
					if (tempNode == tempNode->_Parent->_Left)
					{
						tempNode = tempNode->_Parent;
						_rotate_right(tempNode);
					}

					tempNode->_Parent->_Color			= _Node::Colors::Black;
					tempNode->_Parent->_Parent->_Color	= _Node::Colors::Red;
					_rotate_left(tempNode->_Parent->_Parent);
				}
				else
				{
					tempNode->_Parent->_Color			= _Node::Colors::Black;
					uncle->_Color						= _Node::Colors::Black;
					tempNode->_Parent->_Parent->_Color	= _Node::Colors::Red;
					tempNode							= tempNode->_Parent->_Parent;
				}
			}
		}

		_data._Head->_Parent->_Color = _Node::Colors::Black;							// root is black
	}

	void _destroy(_NodePtr oldNode) {
		--_data._Size;

		_NodePtr fixNode; 			// the node to recolor as needed
        _NodePtr fixNodeParent;
		_NodePtr tempNode;

		// Switch old with successor until leaf
		while (!oldNode->is_leaf())
		{
			tempNode = _in_order_successor(oldNode);
			_transplant(oldNode, tempNode);
		}

		// Rebalance only if old color is black
		if (oldNode->_Color == _Node::Colors::Black)
		{
			fixNode 		= oldNode;
			fixNodeParent 	= oldNode->_Parent;

			for (; fixNode != _data._Head->_Parent && fixNode->_Color == _Node::Colors::Black; fixNodeParent = fixNode->_Parent)
			{
				if (fixNode == fixNodeParent->_Left)	// left subtree
				{
                    tempNode = fixNodeParent->_Right;
                    if (tempNode->_Color == _Node::Colors::Red) 
					{
                        tempNode->_Color = _Node::Colors::Black;
                        fixNodeParent->_Color = _Node::Colors::Red;
                        _rotate_left(fixNodeParent);
                        tempNode = fixNodeParent->_Right;
                    }

                    if (tempNode->_Left->_Color == _Node::Colors::Black && tempNode->_Right->_Color == _Node::Colors::Black)
					{
                        tempNode->_Color = _Node::Colors::Red;
                        fixNode = fixNodeParent;
                    } 
					else
					{
                        if (tempNode->_Right->_Color == _Node::Colors::Black)
						{
                            tempNode->_Left->_Color = _Node::Colors::Black;
                            tempNode->_Color = _Node::Colors::Red;
                            _rotate_right(tempNode);
                            tempNode = fixNodeParent->_Right;
                        }

                        tempNode->_Color 			= fixNodeParent->_Color;
                        fixNodeParent->_Color 		= _Node::Colors::Black;
                        tempNode->_Right->_Color 	= _Node::Colors::Black;
                        _rotate_left(fixNodeParent);
                        break;	// rebalanced
                    }
                }
				else	// right subtree
				{
                    tempNode = fixNodeParent->_Left;
                    if (tempNode->_Color == _Node::Colors::Red)
					{
                        tempNode->_Color = _Node::Colors::Black;
                        fixNodeParent->_Color = _Node::Colors::Red;
                        _rotate_right(fixNodeParent);
                        tempNode = fixNodeParent->_Left;
                    }

                    if (tempNode->_Right->_Color == _Node::Colors::Black && tempNode->_Left->_Color == _Node::Colors::Black)
					{
                        tempNode->_Color = _Node::Colors::Red;
                        fixNode = fixNodeParent;
                    }
					else
					{
                        if (tempNode->_Left->_Color == _Node::Colors::Black)
						{
                            tempNode->_Right->_Color = _Node::Colors::Black;
                            tempNode->_Color = _Node::Colors::Red;
                            _rotate_left(tempNode);
                            tempNode = fixNodeParent->_Left;
                        }

                        tempNode->_Color 			= fixNodeParent->_Color;
                        fixNodeParent->_Color 		= _Node::Colors::Black;
                        tempNode->_Left->_Color 	= _Node::Colors::Black;
                        _rotate_right(fixNodeParent);
                        break;	// rebalanced
                    }
                }
			}

            fixNode->_Color = _Node::Colors::Black;									// stopping node is black
		}

		_free_common_node(oldNode);
		_data._Head->_Left 		= _data.leftmost(_data._Head->_Parent);
		_data._Head->_Right 	= _data.rightmost(_data._Head->_Parent);
	}

	void _transplant(_NodePtr first, _NodePtr second) {
		if (first == second)
			return;

		_swap_parents(first, second);	
		_swap_children(first, second);
		custom::swap(first->_Color, second->_Color);
	}

	void _swap_parents(_NodePtr first, _NodePtr second) {
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

	void _swap_children(_NodePtr first, _NodePtr second) {
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
		// don't construct value, it's not needed
		_data._Head 			= _alloc.allocate(1);
		_data._Head->_Parent	= _data._Head;
		_data._Head->_Left		= _data._Head;
		_data._Head->_Right		= _data._Head;
		_data._Head->_IsNil		= true;
		_data._Head->_Color		= _Node::Colors::Black;
	}

	void _free_head() {
		// don't destroy value, it's not constructed
		_data._Head->_Parent	= nullptr;
		_data._Head->_Left		= nullptr;
		_data._Head->_Right		= nullptr;
		_alloc.deallocate(_data._Head, 1);
	}

	template<class... Args>
	_NodePtr _create_common_node(Args&&... args) {
		_NodePtr newNode 	= _alloc.allocate(1);
		_AllocNodeTraits::construct(_alloc, &(newNode->_Value), custom::forward<Args>(args)...);
		newNode->_Parent	= _data._Head;
		newNode->_Left		= _data._Head;
		newNode->_Right		= _data._Head;
		newNode->_IsNil		= false;
		newNode->_Color		= _Node::Colors::Red;

		return newNode;
	}

	void _free_common_node_default(_NodePtr oldNode) {
		oldNode->_Parent	= nullptr;
		oldNode->_Left		= nullptr;
		oldNode->_Right		= nullptr;
		_AllocNodeTraits::destroy(_alloc, &(oldNode->_Value));
		_alloc.deallocate(oldNode, 1);
	}

	void _free_common_node(_NodePtr oldNode) {
		// detach parent from node
		if (oldNode == _data._Head->_Parent)
			_data._Head->_Parent = _data._Head;
		else if (oldNode == oldNode->_Parent->_Left)
			oldNode->_Parent->_Left = _data._Head;
		else
			oldNode->_Parent->_Right = _data._Head;

		_free_common_node_default(oldNode);
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
		_data._Size = custom::exchange(other._data._Size, 0);
	}
}; // END _SearchTree Template


// _SearchTree binary operators
template<class Traits>
bool operator==(const _SearchTree<Traits>& left, const _SearchTree<Traits>& right) {	// Contains the same elems, same order, but not the same tree
	if (left.size() != right.size())
		return false;

	return custom::equal(left.begin(), left.end(), right.begin());
}

template<class Traits>
bool operator!=(const _SearchTree<Traits>& left, const _SearchTree<Traits>& right) {
	return !(left == right);
}

CUSTOM_END