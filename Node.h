#pragma once
#include "Utility.h"


CUSTOM_BEGIN

// Forward Node ========================================================
template<class Type>
struct ForwardNode														// Struct that holds data and references to next struct
{
	using ValueType = Type;
	ValueType _Value;													// Data
	ForwardNode* _Next = nullptr;										// Reference to next

	ForwardNode()								= default;
	ForwardNode(const ForwardNode&)				= delete;
	ForwardNode& operator=(const ForwardNode&)	= delete;

	template<class... Args>
	ForwardNode(Args&&... args)
		:_Value(custom::forward<Args>(args)...) { /*Empty*/ }

	~ForwardNode() {
		_Next = nullptr;
	}

	template<class AllocNode>
	static ForwardNode* create_head(AllocNode& al) {
		ForwardNode* newNode = al.alloc(1);
		newNode->_Next = newNode;

		return newNode;
	}

	template<class AllocNode>
	static void free_head(AllocNode& al, ForwardNode* node) {
		node->_Next = nullptr;
		al.dealloc(node, 1);
	}
};
// Forward Node ========================================================
// END


// Double Node ========================================================
template<class Type>
struct DoubleNode														// Struct that holds data and references to next and previous struct
{
	using ValueType = Type;
	ValueType _Value;													// Data
	DoubleNode* _Previous	= nullptr;									// Reference to previous 
	DoubleNode* _Next		= nullptr;									// Reference to next

	DoubleNode()								= default;
	DoubleNode(const DoubleNode&)				= delete;
	DoubleNode& operator=(const DoubleNode&)	= delete;

	template<class... Args>
	DoubleNode(Args&&... args)
		: _Value(custom::forward<Args>(args)...) { /*Empty*/ }

	~DoubleNode() {
		_Previous	= nullptr;
		_Next		= nullptr;
	}

	template<class AllocNode>
	static DoubleNode* create_head(AllocNode& al) {
		DoubleNode* newNode = al.alloc(1);
		newNode->_Next = newNode;
		newNode->_Previous = newNode;

		return newNode;
	}

	template<class AllocNode>
	static void free_head(AllocNode& al, DoubleNode* node) {
		node->_Next		= nullptr;
		node->_Previous = nullptr;
		al.dealloc(node, 1);
	}
};
// Double Node ========================================================
// END


// Tree Node ========================================================
template<class Type>
struct TreeNode
{
	using ValueType = Type;
	ValueType _Value;												// Data
	TreeNode* _Parent	= nullptr;									// Reference to parent
	TreeNode* _Left		= nullptr;									// Reference to left
	TreeNode* _Right	= nullptr;									// Reference to right
	char _Color;

	enum Colors 
	{
		Red,
		Black
	};

	TreeNode()
		:_Value(), _Color(Black) { /*Empty*/ }

	TreeNode(const TreeNode&)				= delete;
	TreeNode& operator=(const TreeNode&)	= delete;

	template<class... Args>
	TreeNode(Args&&... args)
		: _Value(custom::forward<Args>(args)...), _Color(Black) { /*Empty*/ }

	~TreeNode() {
		_Parent	= nullptr;
		_Left	= nullptr;
		_Right	= nullptr;
	}

	template<class AllocNode>
	static TreeNode* create_head(AllocNode& al) {
		TreeNode* newNode = al.alloc(1);
		newNode->_Parent = newNode;
		newNode->_Next = newNode;
		newNode->_Previous = newNode;

		return newNode;
	}

	template<class AllocNode>
	static void free_head(AllocNode& al, TreeNode* node) {
		node->_Parent	= nullptr;
		node->_Next		= nullptr;
		node->_Previous = nullptr;
		al.dealloc(node, 1);
	}
};
// Tree Node ========================================================
// END
CUSTOM_END