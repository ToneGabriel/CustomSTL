#pragma once
#include "cUtility.h"


CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN

template<class Type>
struct _ForwardNode			// Struct that holds data and references to next struct
{
	using ValueType = Type;

	ValueType _Value;
	_ForwardNode* _Next = nullptr;

	_ForwardNode()									= default;
	~_ForwardNode() 								= default;
	_ForwardNode(const _ForwardNode&)				= delete;
	_ForwardNode& operator=(const _ForwardNode&)	= delete;

	_ForwardNode(const ValueType& copyVal)
		:_Value(copyVal) { /*Empty*/ }

	_ForwardNode(ValueType&& moveVal)
		:_Value(custom::move(moveVal)) { /*Empty*/ }

	template<class... Args>
	_ForwardNode(Args&&... args)
		:_Value(custom::forward<Args>(args)...) { /*Empty*/ }
}; // END _ForwardNode


template<class Type>
struct _DoubleNode			// Struct that holds data and references to next and previous struct
{
	using ValueType = Type;

	ValueType _Value;
	_DoubleNode* _Previous 	= nullptr;
	_DoubleNode* _Next 		= nullptr;

	_DoubleNode()								= default;
	~_DoubleNode() 								= default;
	_DoubleNode(const _DoubleNode&)				= delete;
	_DoubleNode& operator=(const _DoubleNode&)	= delete;

	_DoubleNode(const ValueType& copyVal)
		:_Value(copyVal) { /*Empty*/ }

	_DoubleNode(ValueType&& moveVal)
		:_Value(custom::move(moveVal)) { /*Empty*/ }

	template<class... Args>
	_DoubleNode(Args&&... args)
		: _Value(custom::forward<Args>(args)...) { /*Empty*/ }
}; // END _DoubleNode


template<class Type>
struct _TreeNode			// Used in _SearchTree
{
	using ValueType = Type;
	
	enum class Colors
	{
		Red,
		Black
	};

	ValueType _Value;
	_TreeNode* _Parent 	= nullptr;
	_TreeNode* _Left 	= nullptr;
	_TreeNode* _Right 	= nullptr;
	bool _IsNil 		= false;			// True for Head only
	Colors _Color 		= Colors::Red;		// Used for balancing

	_TreeNode()								= default;
	~_TreeNode() 							= default;
	_TreeNode(const _TreeNode&)				= delete;
	_TreeNode& operator=(const _TreeNode&)	= delete;

	_TreeNode(const ValueType& copyVal)
		:_Value(copyVal) { /*Empty*/ }

	_TreeNode(ValueType&& moveVal)
		:_Value(custom::move(moveVal)) { /*Empty*/ }

	template<class... Args>
	_TreeNode(Args&&... args)
		: _Value(custom::forward<Args>(args)...) { /*Empty*/ }

	bool is_leaf() const {
		return (_Left->_IsNil && _Right->_IsNil);
	}
}; // END Tree Node

CUSTOM_DETAIL_END

CUSTOM_END