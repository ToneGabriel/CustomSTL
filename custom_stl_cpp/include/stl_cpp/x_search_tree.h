#pragma once
#include "stl_cpp/x_memory.h"
#include "stl_cpp/c_pair.h"
#include "stl_cpp/c_utility.h"
#include "stl_cpp/c_iterator.h"
#include "stl_cpp/c_functional.h"	// for custom::Less


CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN

enum class _Tree_Child
{
	_Left,
	_Right
};

template<class _NodePtr>
struct _Tree_Node_ID 			// Indicator for child placement
{							
	_NodePtr _Parent 	= nullptr;
	_Tree_Child _Child 	= _Tree_Child::_Left;
};

template<class Traits>
struct _Search_Tree_Data
{
	using key_type				= typename Traits::key_type;
    using mapped_type			= typename Traits::mapped_type;
	using key_compare			= typename Traits::key_compare;
	using value_type			= typename Traits::value_type;
	using allocator_type		= typename Traits::allocator_type;
	
	using _Alloc_Traits			= allocator_traits<allocator_type>;
	using _Node					= detail::_Tree_Node<value_type>;
	using _Alloc_Node			= typename _Alloc_Traits::template rebind_alloc<_Node>;
	using _Alloc_Node_Traits	= allocator_traits<_Alloc_Node>;
	using _NodePtr				= typename _Alloc_Node_Traits::pointer;

	using difference_type		= typename _Alloc_Traits::difference_type;
	using reference				= typename _Alloc_Traits::reference;
	using const_reference		= typename _Alloc_Traits::const_reference;
	using pointer				= typename _Alloc_Traits::pointer;
	using const_pointer			= typename _Alloc_Traits::const_pointer;

	size_t _Size				= 0;									// Number of Nodes held
	_NodePtr _Head				= nullptr;								// Helper node used to link min and max elems for iteration (root == head->parent)

	_NodePtr leftmost(_NodePtr node) const	// return leftmost node in subtree at node
	{
		while (!node->_Left->_IsNil)
			node = node->_Left;

		return node;
	}

	_NodePtr rightmost(_NodePtr node) const	// return rightmost node in subtree at node
	{
		while (!node->_Right->_IsNil)
			node = node->_Right;

		return node;
	}
};	// END _Search_Tree_Data

template<class SearchTreeData>
class _Search_Tree_Const_Iterator
{
private:
	using _Data				= SearchTreeData;
	using _NodePtr 			= typename _Data::_NodePtr;

public:
    using iterator_category	= bidirectional_iterator_tag;
	using value_type 		= typename _Data::value_type;
	using difference_type	= typename _Data::difference_type;
	using reference			= typename _Data::const_reference;
	using pointer			= typename _Data::const_pointer;

	_NodePtr _Ptr 			= nullptr;
	const _Data* _RefData	= nullptr;

public:

	_Search_Tree_Const_Iterator() noexcept = default;

	explicit _Search_Tree_Const_Iterator(_NodePtr ptr, const _Data* data) noexcept
		:_Ptr(ptr), _RefData(data) { /*Empty*/ }

	_Search_Tree_Const_Iterator& operator++() noexcept
	{
		CUSTOM_ASSERT(_Ptr != _RefData->_Head, "Cannot increment end iterator.");

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

	_Search_Tree_Const_Iterator operator++(int) noexcept
	{
		_Search_Tree_Const_Iterator temp = *this;
		++(*this);
		return temp;
	}

	_Search_Tree_Const_Iterator& operator--() noexcept
	{
		CUSTOM_ASSERT(_Ptr != _RefData->_Head->_Left, "Cannot decrement begin iterator.");

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

	_Search_Tree_Const_Iterator operator--(int) noexcept
	{
		_Search_Tree_Const_Iterator temp = *this;
		--(*this);
		return temp;
	}

	pointer operator->() const noexcept
	{
		CUSTOM_ASSERT(_Ptr != _RefData->_Head, "Cannot access end iterator.");
        return pointer_traits<pointer>::pointer_to(**this);	// return &(**this);
	}

	reference operator*() const noexcept
	{
		CUSTOM_ASSERT(_Ptr != _RefData->_Head, "Cannot dereference end iterator.");
		return _Ptr->_Value;
	}

	bool operator==(const _Search_Tree_Const_Iterator& other) const noexcept
	{
		return _Ptr == other._Ptr;
	}

	bool operator!=(const _Search_Tree_Const_Iterator& other) const noexcept
	{
		return !(*this == other);
	}

	friend void _verify_range(const _Search_Tree_Const_Iterator& first, const _Search_Tree_Const_Iterator& last) noexcept
	{
		CUSTOM_ASSERT(first._RefData == last._RefData, "_Search_Tree iterators in range are from different containers");
		// No possible way to determine order.
	}
}; // END _Search_Tree_Const_Iterator

template<class SearchTreeData>
class _Search_Tree_Iterator : public _Search_Tree_Const_Iterator<SearchTreeData>			// _Search_Tree iterator
{
private:
	using _Base				= _Search_Tree_Const_Iterator<SearchTreeData>;
	using _Data 			= SearchTreeData;
	using _NodePtr			= typename _Data::_NodePtr;

public:
    using iterator_category	= bidirectional_iterator_tag;
	using value_type 		= typename _Data::value_type;
	using difference_type	= typename _Data::difference_type;
	using reference 		= typename _Data::reference;
	using pointer 			= typename _Data::pointer;

public:

	_Search_Tree_Iterator() noexcept = default;

	explicit _Search_Tree_Iterator(_NodePtr ptr, const _Data* data) noexcept
		:_Base(ptr, data) { /*Empty*/ }

	_Search_Tree_Iterator& operator++() noexcept
	{
		_Base::operator++();
		return *this;
	}

	_Search_Tree_Iterator operator++(int) noexcept
	{
		_Search_Tree_Iterator temp = *this;
		_Base::operator++();
		return temp;
	}

	_Search_Tree_Iterator& operator--() noexcept
	{
		_Base::operator--();
		return *this;
	}

	_Search_Tree_Iterator operator--(int) noexcept
	{
		_Search_Tree_Iterator temp = *this;
		_Base::operator--();
		return temp;
	}

	pointer operator->() const noexcept
	{
		return const_cast<pointer>(_Base::operator->());
	}

	reference operator*() const noexcept
	{
		return const_cast<reference>(_Base::operator*());
	}
}; // END _Search_Tree_Iterator


template<class Traits>
class _Search_Tree			// _Search_Tree Template implemented as Red-Black Tree
{
private:
	using _Data						= _Search_Tree_Data<Traits>;
	using _Alloc_Traits				= typename _Data::_Alloc_Traits;
	using _Node						= typename _Data::_Node;
	using _Alloc_Node				= typename _Data::_Alloc_Node;
	using _Alloc_Node_Traits		= typename _Data::_Alloc_Node_Traits;
	using _NodePtr					= typename _Data::_NodePtr;
	
protected:
    using key_type					= typename _Data::key_type;
    using mapped_type				= typename _Data::mapped_type;
    using key_compare				= typename _Data::key_compare;
	using value_type				= typename _Data::value_type;
	using difference_type			= typename _Data::difference_type;
	using reference					= typename _Data::reference;
	using const_reference			= typename _Data::const_reference;
	using pointer					= typename _Data::pointer;
	using const_pointer				= typename _Data::const_pointer;
	using allocator_type			= typename _Data::allocator_type;

	using iterator					= _Search_Tree_Iterator<_Data>;
	using const_iterator			= _Search_Tree_Const_Iterator<_Data>;
	using reverse_iterator			= custom::reverse_iterator<iterator>;
	using const_reverse_iterator	= custom::reverse_iterator<const_iterator>;

protected:
	_Data _data;
	_Alloc_Node _alloc;
    key_compare _less;			// Used for comparison

protected:
	// Constructors

	_Search_Tree()
	{
		_create_head();
	}

	_Search_Tree(std::initializer_list<value_type> list) : _Search_Tree()
	{
		for (const auto& val : list)
			emplace(val);
	}

	_Search_Tree(const _Search_Tree& other) : _Search_Tree()
	{
		_copy(other);
	}

	_Search_Tree(_Search_Tree&& other) noexcept : _Search_Tree()
	{
		_move(custom::move(other));
	}

	virtual ~_Search_Tree()
	{
		_destroy_all(_data._Head->_Parent);
		_free_head();
	}

protected:
	// Operators

	_Search_Tree& operator=(const _Search_Tree& other)
	{
		if (_data._Head != other._data._Head)
		{
			clear();
			_copy(other);
		}

		return *this;
	}

	_Search_Tree& operator=(_Search_Tree&& other) noexcept
	{
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
	iterator emplace(Args&&... args)
	{
		_NodePtr newNode 		= _create_common_node(custom::forward<Args>(args)...);
		const key_type& newKey 	= Traits::extract_key(newNode->_Value);
		iterator it 			= find(newKey);

		if (it != end())
		{
			_free_common_node_default(newNode);
			return it;
		}
		else
		{
			auto insertPosition = _find_insertion_slot(newNode);
			_insert(newNode, insertPosition);
			return iterator(newNode, &_data);
		}
	}

	iterator erase(const key_type& key)
	{
		iterator it = find(key);

		if (it == end())
			return it;

		_NodePtr nodeToErase 	= it._Ptr;
		iterator nextIterator 	= ++iterator(nodeToErase, &_data);
		_destroy(nodeToErase);

		return nextIterator;
	}

	iterator erase(const_iterator where)
	{
		if (where == end())
			throw std::out_of_range("map erase iterator outside range.");

		return erase(Traits::extract_key(where._Ptr->_Value));
	}

	iterator erase(iterator where)
	{
		if (where == end())
			throw std::out_of_range("map erase iterator outside range.");

		return erase(Traits::extract_key(where._Ptr->_Value));
	}

	const_iterator find (const key_type& key) const
	{
		return const_iterator(_find_in_tree(key), &_data);
	}

	iterator find(const key_type& key)
	{
		return iterator(_find_in_tree(key), &_data);
	}

	size_t size() const noexcept
	{
		return _data._Size;
	}

	size_t max_size() const noexcept
	{
		return (custom::min)(static_cast<size_t>((	numeric_limits<difference_type>::max)()),
													_Alloc_Node_Traits::max_size(_alloc));
	}

	bool empty() const noexcept
	{
		return _data._Size == 0;
	}

	void clear()
	{
		_destroy_all(_data._Head->_Parent);
		_data._Head->_Parent 	= _data._Head;
		_data._Head->_Left 		= _data._Head;
		_data._Head->_Right		= _data._Head;
		_data._Size				= 0;
	}

	void print_details() const
	{
		std::cout << "Size= " << _data._Size << '\n';
		std::cout << "first= " << Traits::extract_key(_data._Head->_Left->_Value) << '\n';
		std::cout << "Last= " << Traits::extract_key(_data._Head->_Right->_Value) << '\n';
		_print_graph(0, _data._Head->_Parent, "HEAD");
	}

public:
	// iterator functions

	iterator begin()
	{
		return iterator(_data._Head->_Left, &_data);
	}

	const_iterator begin() const
	{
		return const_iterator(_data._Head->_Left, &_data);
	}

	reverse_iterator rbegin()
	{
		return reverse_iterator(end());
	}

	const_reverse_iterator rbegin() const
	{
		return const_reverse_iterator(end());
	}

	iterator end()
	{
		return iterator(_data._Head, &_data);
	}

	const_iterator end() const
	{
		return const_iterator(_data._Head, &_data);
	}

	reverse_iterator rend()
	{
		return reverse_iterator(begin());
	}

	const_reverse_iterator rend() const
	{
		return const_reverse_iterator(begin());
	}

protected:
	// Others

	template<class _KeyType, class... Args>
	pair<iterator, bool> _try_emplace(_KeyType&& key, Args&&... args)	// Force construction with known key and given arguments for object
	{
		iterator it = find(key);

		if (it != end())
			return {it, false};
		else 
		{
			_NodePtr newNode = _create_common_node(
										custom::piecewise_construct,
										custom::forward_as_tuple(custom::forward<_KeyType>(key)),
										custom::forward_as_tuple(custom::forward<Args>(args)...));

			auto insertPosition = _find_insertion_slot(newNode);
			_insert(newNode, insertPosition);
			return {iterator(newNode, &_data), true};
		}
	}

	const mapped_type& _at(const key_type& key) const	// Access value at key with check
	{
		const_iterator it = find(key);
		if (it == end())
			throw std::out_of_range("Invalid key.");

		return Traits::extract_mapval(*it);
	}

	mapped_type& _at(const key_type& key)
	{
		iterator it = find(key);
		if (it == end())
			throw std::out_of_range("Invalid key.");

		return const_cast<mapped_type&>(Traits::extract_mapval(*it));
	}

private:
	// Helpers

	void _print_graph(const size_t ident, _NodePtr root, const custom::string& rlFlag) const
	{
		custom::string str;
		str.append(ident, '\t');

		if (!root->_IsNil)
			std::cout << str << Traits::extract_key(root->_Value) << " [" << ((int)root->_Color ? "black" : "red") << " " << rlFlag << "]\n";

		if (!root->_Left->_IsNil)
			_print_graph(ident + 1, root->_Left, "LEFT");

		if (!root->_Right->_IsNil)
			_print_graph(ident + 1, root->_Right, "RIGHT");
	}
	
	void _rotate_left(_NodePtr subroot)	// promotes subroot right
	{
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

	void _rotate_right(_NodePtr subroot)	// promotes subroot left
	{
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

	_NodePtr _copy_all(_NodePtr subroot)
	{
		if (subroot->_IsNil)
			return _data._Head;

		_NodePtr newNode = _alloc.allocate(1);
		_Alloc_Node_Traits::construct(_alloc, &(newNode->_Value), subroot->_Value);

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

	void _destroy_all(_NodePtr subroot)
	{
		if (subroot->_IsNil)
			return;

		_destroy_all(subroot->_Left);
		_destroy_all(subroot->_Right);

		_free_common_node_default(subroot);
	}

	_NodePtr _in_order_successor(_NodePtr node) const
	{
		if (!node->_Right->_IsNil)
			node = _data.leftmost(node->_Right);
		else
			node = _data.leftmost(node);

		return node;
	}

	_NodePtr _find_in_tree(const key_type& key) const
	{
		_NodePtr found = _data._Head;

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

	_Tree_Node_ID<_NodePtr> _find_insertion_slot(_NodePtr newNode) const	// Find parent for newly created node
	{
		_Tree_Node_ID<_NodePtr> position;

		if (_data._Head->_Parent == _data._Head)					// first node
			position._Parent = _data._Head;
		else
			for (_NodePtr iterNode = _data._Head->_Parent; !iterNode->_IsNil; /*Empty*/)
			{
				position._Parent = iterNode;
				if (_less(Traits::extract_key(newNode->_Value), Traits::extract_key(iterNode->_Value)))
				{
					iterNode = iterNode->_Left;
					if (iterNode->_IsNil)
						position._Child = _Tree_Child::_Left;
				}
				else
				{
					iterNode = iterNode->_Right;
					if (iterNode->_IsNil)
						position._Child = _Tree_Child::_Right;
				}
			}

		return position;
	}

	void _insert(_NodePtr newNode, const _Tree_Node_ID<_NodePtr>& position)
	{
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
		else if (position._Child == _Tree_Child::_Left)				// add to left
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

	void _destroy(_NodePtr oldNode)
	{
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

			for (/*Empty*/; fixNode != _data._Head->_Parent && fixNode->_Color == _Node::Colors::Black; fixNodeParent = fixNode->_Parent)
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

	void _transplant(_NodePtr first, _NodePtr second)
	{
		if (first == second)
			return;

		_swap_parents(first, second);	
		_swap_children(first, second);
		custom::swap(first->_Color, second->_Color);
	}

	void _swap_parents(_NodePtr first, _NodePtr second)
	{
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

	void _swap_children(_NodePtr first, _NodePtr second)
	{
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

	void _create_head()
	{
		// don't construct value, it's not needed
		_data._Head 			= _alloc.allocate(1);
		_data._Head->_Parent	= _data._Head;
		_data._Head->_Left		= _data._Head;
		_data._Head->_Right		= _data._Head;
		_data._Head->_IsNil		= true;
		_data._Head->_Color		= _Node::Colors::Black;
	}

	void _free_head()
	{
		// don't destroy value, it's not constructed
		_data._Head->_Parent	= nullptr;
		_data._Head->_Left		= nullptr;
		_data._Head->_Right		= nullptr;
		_alloc.deallocate(_data._Head, 1);
	}

	template<class... Args>
	_NodePtr _create_common_node(Args&&... args)
	{
		_NodePtr newNode 	= _alloc.allocate(1);
		_Alloc_Node_Traits::construct(_alloc, &(newNode->_Value), custom::forward<Args>(args)...);
		newNode->_Parent	= _data._Head;
		newNode->_Left		= _data._Head;
		newNode->_Right		= _data._Head;
		newNode->_IsNil		= false;
		newNode->_Color		= _Node::Colors::Red;

		return newNode;
	}

	void _free_common_node_default(_NodePtr oldNode)
	{
		oldNode->_Parent	= nullptr;
		oldNode->_Left		= nullptr;
		oldNode->_Right		= nullptr;
		_Alloc_Node_Traits::destroy(_alloc, &(oldNode->_Value));
		_alloc.deallocate(oldNode, 1);
	}

	void _free_common_node(_NodePtr oldNode)
	{
		// detach parent from node
		if (oldNode == _data._Head->_Parent)
			_data._Head->_Parent = _data._Head;
		else if (oldNode == oldNode->_Parent->_Left)
			oldNode->_Parent->_Left = _data._Head;
		else
			oldNode->_Parent->_Right = _data._Head;

		_free_common_node_default(oldNode);
	}

	void _copy(const _Search_Tree& other)
	{
		_data._Head->_Parent 			= _copy_all(other._data._Head->_Parent);	// copy from root
		_data._Head->_Left 				= _data.leftmost(_data._Head->_Parent);
		_data._Head->_Right 			= _data.rightmost(_data._Head->_Parent);
		_data._Head->_Parent->_Parent 	= _data._Head;
		_data._Size 					= other._data._Size;
	}

	void _move(_Search_Tree&& other)
	{
		custom::swap(_data._Head, other._data._Head);
		_data._Size = custom::exchange(other._data._Size, 0);
	}
}; // END _Search_Tree Template


// _Search_Tree binary operators

// Contains the same elems, same order, but not the same tree
template<class Traits>
bool operator==(const _Search_Tree<Traits>& left, const _Search_Tree<Traits>& right)
{
	if (left.size() != right.size())
		return false;

	return custom::equal(left.begin(), left.end(), right.begin());
}

template<class Traits>
bool operator!=(const _Search_Tree<Traits>& left, const _Search_Tree<Traits>& right)
{
	return !(left == right);
}

CUSTOM_DETAIL_END

CUSTOM_END