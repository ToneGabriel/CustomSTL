#pragma once
#include "Utility.h"


CUSTOM_BEGIN

template<class Type>
struct ForwardNode			// Struct that holds data and references to next struct
{
	using ValueType = Type;

	ValueType _Value;
	ForwardNode* _Next;

	ForwardNode()								= default;
	ForwardNode(const ForwardNode&)				= delete;
	ForwardNode& operator=(const ForwardNode&)	= delete;

	ForwardNode(const ValueType& copyVal)
		:_Value(copyVal) { /*Empty*/ }

	ForwardNode(ValueType&& moveVal)
		:_Value(custom::move(moveVal)) { /*Empty*/ }

	template<class... Args>
	ForwardNode(Args&&... args)
		:_Value(custom::forward<Args>(args)...) { /*Empty*/ }

	~ForwardNode() {
		_Next = nullptr;
	}
}; // END ForwardNode


template<class Type>
struct DoubleNode			// Struct that holds data and references to next and previous struct
{
	using ValueType = Type;

	ValueType _Value;
	DoubleNode* _Previous;
	DoubleNode* _Next;

	DoubleNode()								= default;
	DoubleNode(const DoubleNode&)				= delete;
	DoubleNode& operator=(const DoubleNode&)	= delete;

	DoubleNode(const ValueType& copyVal)
		:_Value(copyVal) { /*Empty*/ }

	DoubleNode(ValueType&& moveVal)
		:_Value(custom::move(moveVal)) { /*Empty*/ }

	template<class... Args>
	DoubleNode(Args&&... args)
		: _Value(custom::forward<Args>(args)...) { /*Empty*/ }

	~DoubleNode() {
		_Previous	= nullptr;
		_Next		= nullptr;
	}
}; // END DoubleNode


template<class Type>
struct TreeNode				// Used in SearchTree
{
	using ValueType = Type;
	
	ValueType _Value;
	TreeNode* _Parent;
	TreeNode* _Left;
	TreeNode* _Right;
	char _Color;			// Used for balancing
	bool _IsNil;			// True for Head only

	enum Colors : char
	{
		Red,
		Black
	};

	TreeNode()								= default;
	TreeNode(const TreeNode&)				= delete;
	TreeNode& operator=(const TreeNode&)	= delete;

	TreeNode(const ValueType& copyVal)
		:_Value(copyVal) { /*Empty*/ }

	TreeNode(ValueType&& moveVal)
		:_Value(custom::move(moveVal)) { /*Empty*/ }

	template<class... Args>
	TreeNode(Args&&... args)
		: _Value(custom::forward<Args>(args)...) { /*Empty*/ }

	~TreeNode() {
		_Parent	= nullptr;
		_Left	= nullptr;
		_Right	= nullptr;
	}

	bool is_leaf() const {
		return (_Left->_IsNil && _Right->_IsNil);
	}
}; // END Tree Node

CUSTOM_END