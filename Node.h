#pragma once
#include "Common.h"

CUSTOM_BEGIN

// Forward Node ========================================================
template<class Type>
struct ForwardNode														// Struct that holds data and references to next struct
{
public:
	using ValueType = Type;

	ValueType _Value;													// Data
	ForwardNode* _Next = nullptr;										// Reference to next

public:

	ForwardNode()
		:_Value() { /*Empty*/ }

	ForwardNode(const ValueType& copyValue)
		:_Value(copyValue) { /*Empty*/ }

	ForwardNode(ValueType&& moveValue)
		:_Value(std::move(moveValue)) { /*Empty*/ }

	template<class... Args>
	ForwardNode(Args&&... args)
		:_Value(std::forward<Args>(args)...) { /*Empty*/ }

	ForwardNode(const ForwardNode&)				= delete;
	ForwardNode& operator=(const ForwardNode&)	= delete;

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
public:
	using ValueType = Type;

	ValueType _Value;													// Data
	DoubleNode* _Previous	= nullptr;									// Reference to previous 
	DoubleNode* _Next		= nullptr;									// Reference to next

public:

	DoubleNode() 
		:_Value() { /*Empty*/ }

	DoubleNode(const ValueType& copyValue)
		:_Value(copyValue) { /*Empty*/ }

	DoubleNode(ValueType&& moveValue)
		:_Value(std::move(moveValue)) { /*Empty*/ }

	template<class... Args>
	DoubleNode(Args&&... args)
		: _Value(std::forward<Args>(args)...) { /*Empty*/ }

	DoubleNode(const DoubleNode&)				= delete;
	DoubleNode& operator=(const DoubleNode&)	= delete;

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
public:
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

public:

	TreeNode()
		:_Value(), _Color(Black) { /*Empty*/ }

	TreeNode(const ValueType& copyValue)
		:_Value(copyValue), _Color(Black) { /*Empty*/ }

	TreeNode(ValueType&& moveValue)
		:_Value(std::move(moveValue)), _Color(Black) { /*Empty*/ }

	template<class... Args>
	TreeNode(Args&&... args)
		: _Value(std::forward<Args>(args)...), _Color(Black) { /*Empty*/ }

	TreeNode(const TreeNode&)				= delete;
	TreeNode& operator=(const TreeNode&)	= delete;

	~TreeNode() {
		_Parent	= nullptr;
		_Left	= nullptr;
		_Right	= nullptr;
	}
};
// Tree Node ========================================================
// END
CUSTOM_END