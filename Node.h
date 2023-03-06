#pragma once
#include "Utility.h"


CUSTOM_BEGIN

// Forward Node ========================================================
template<class Type>
struct ForwardNode														// Struct that holds data and references to next struct
{
	using ValueType = Type;
	ValueType _Value;		// Data
	ForwardNode* _Next;		// Reference to next

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
};
// Forward Node ========================================================
// END


// Double Node ========================================================
template<class Type>
struct DoubleNode														// Struct that holds data and references to next and previous struct
{
	using ValueType = Type;
	ValueType _Value;		// Data
	DoubleNode* _Previous;	// Reference to previous 
	DoubleNode* _Next;		// Reference to next

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
};
// Double Node ========================================================
// END


// Tree Node ========================================================
template<class Type>
struct TreeNode
{
	using ValueType = Type;
	ValueType _Value;		// Data
	TreeNode* _Parent;		// Reference to parent
	TreeNode* _Left;		// Reference to left
	TreeNode* _Right;		// Reference to right
	char _Color;			// Used for balancing
	bool _IsNil;			// True for Head only

	enum Colors 
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
};
// Tree Node ========================================================
// END
CUSTOM_END