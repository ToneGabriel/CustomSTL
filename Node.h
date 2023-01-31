#pragma once
#include "Common.h"

CUSTOM_BEGIN

// Forward Node ========================================================
template<class Type>
struct ForwardNode														// Struct that holds data and references to next struct
{
public:
	using ValueType = Type;

	ValueType Value;													// Data
	ForwardNode* Next = nullptr;										// Reference to next

public:

	ForwardNode()
		:Value() { /*Empty*/ }

	ForwardNode(const ValueType& copyValue)
		:Value(copyValue) { /*Empty*/ }

	ForwardNode(ValueType&& moveValue)
		:Value(std::move(moveValue)) { /*Empty*/ }

	template<class... Args>
	ForwardNode(Args&&... args)
		:Value(std::forward<Args>(args)...) { /*Empty*/ }

	ForwardNode(const ForwardNode&)				= delete;
	ForwardNode& operator=(const ForwardNode&)	= delete;

	~ForwardNode() {
		Next = nullptr;
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

	ValueType Value;													// Data
	DoubleNode* Previous	= nullptr;									// Reference to previous 
	DoubleNode* Next		= nullptr;									// Reference to next

public:

	DoubleNode() 
		:Value() { /*Empty*/ }

	DoubleNode(const ValueType& copyValue)
		:Value(copyValue) { /*Empty*/ }

	DoubleNode(ValueType&& moveValue)
		:Value(std::move(moveValue)) { /*Empty*/ }

	template<class... Args>
	DoubleNode(Args&&... args)
		: Value(std::forward<Args>(args)...) { /*Empty*/ }

	DoubleNode(const DoubleNode&)				= delete;
	DoubleNode& operator=(const DoubleNode&)	= delete;

	~DoubleNode() {
		Previous	= nullptr;
		Next		= nullptr;
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

	ValueType Value;												// Data
	TreeNode* Parent	= nullptr;									// Reference to parent
	TreeNode* Left		= nullptr;									// Reference to left
	TreeNode* Right		= nullptr;									// Reference to right
	char Color;

	enum Colors 
	{
		Red,
		Black
	};

public:

	TreeNode()
		:Value(), Color(Black) { /*Empty*/ }

	TreeNode(const ValueType& copyValue)
		:Value(copyValue), Color(Black) { /*Empty*/ }

	TreeNode(ValueType&& moveValue)
		:Value(std::move(moveValue)), Color(Black) { /*Empty*/ }

	template<class... Args>
	TreeNode(Args&&... args)
		: Value(std::forward<Args>(args)...), Color(Black) { /*Empty*/ }

	TreeNode(const TreeNode&)				= delete;
	TreeNode& operator=(const TreeNode&)	= delete;

	~TreeNode() {
		Parent	= nullptr;
		Left	= nullptr;
		Right	= nullptr;
	}
};
// Tree Node ========================================================
// END
CUSTOM_END