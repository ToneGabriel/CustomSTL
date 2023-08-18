#pragma once
#include "Utility.h"


CUSTOM_BEGIN

namespace node
{

template<class Type>
struct _ForwardNode			// Struct that holds data and references to next struct
{
	using ValueType = Type;

	ValueType _Value;
	_ForwardNode* _Next;

	_ForwardNode()								= default;
	_ForwardNode(const _ForwardNode&)				= delete;
	_ForwardNode& operator=(const _ForwardNode&)	= delete;

	_ForwardNode(const ValueType& copyVal)
		:_Value(copyVal) { /*Empty*/ }

	_ForwardNode(ValueType&& moveVal)
		:_Value(custom::move(moveVal)) { /*Empty*/ }

	template<class... Args>
	_ForwardNode(Args&&... args)
		:_Value(custom::forward<Args>(args)...) { /*Empty*/ }

	~_ForwardNode() {
		_Next = nullptr;
	}
}; // END _ForwardNode


template<class Type>
struct _DoubleNode			// Struct that holds data and references to next and previous struct
{
	using ValueType = Type;

	ValueType _Value;
	_DoubleNode* _Previous;
	_DoubleNode* _Next;

	_DoubleNode()								= default;
	_DoubleNode(const _DoubleNode&)				= delete;
	_DoubleNode& operator=(const _DoubleNode&)	= delete;

	_DoubleNode(const ValueType& copyVal)
		:_Value(copyVal) { /*Empty*/ }

	_DoubleNode(ValueType&& moveVal)
		:_Value(custom::move(moveVal)) { /*Empty*/ }

	template<class... Args>
	_DoubleNode(Args&&... args)
		: _Value(custom::forward<Args>(args)...) { /*Empty*/ }

	~_DoubleNode() {
		_Previous	= nullptr;
		_Next		= nullptr;
	}
}; // END _DoubleNode


template<class Type>
struct _TreeNode				// Used in SearchTree
{
	using ValueType = Type;
	
	ValueType _Value;
	_TreeNode* _Parent;
	_TreeNode* _Left;
	_TreeNode* _Right;
	char _Color;			// Used for balancing
	bool _IsNil;			// True for Head only

	enum Colors : char
	{
		Red,
		Black
	};

	_TreeNode()								= default;
	_TreeNode(const _TreeNode&)				= delete;
	_TreeNode& operator=(const _TreeNode&)	= delete;

	_TreeNode(const ValueType& copyVal)
		:_Value(copyVal) { /*Empty*/ }

	_TreeNode(ValueType&& moveVal)
		:_Value(custom::move(moveVal)) { /*Empty*/ }

	template<class... Args>
	_TreeNode(Args&&... args)
		: _Value(custom::forward<Args>(args)...) { /*Empty*/ }

	~_TreeNode() {
		_Parent	= nullptr;
		_Left	= nullptr;
		_Right	= nullptr;
	}

	bool is_leaf() const {
		return (_Left->_IsNil && _Right->_IsNil);
	}
}; // END Tree Node

}	// END namespace node
CUSTOM_END