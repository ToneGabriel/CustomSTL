#pragma once
#include "Common.h"

CUSTOM_BEGIN

// Forward Node ========================================================
template<class Type>
struct ForwardNode														// Struct that holds data and references to next struct
{
private:
	using ValueType = Type;

public:
	ValueType Value;													// Data
	ForwardNode* Next = nullptr;										// Reference to next

	ForwardNode()
		:Value() { }

	ForwardNode(const ValueType& copyValue)
		:Value(copyValue) { }

	ForwardNode(ValueType&& moveValue)
		:Value(std::move(moveValue)) { }

	template<class... Args>
	ForwardNode(Args&&... args)
		:Value(std::forward<Args>(args)...) { }

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
private:
	using ValueType = Type;

public:
	ValueType Value;													// Data
	DoubleNode* Previous	= nullptr;									// Reference to previous 
	DoubleNode* Next		= nullptr;									// Reference to next

	DoubleNode() 
		:Value() { }

	DoubleNode(const ValueType& copyValue)
		:Value(copyValue) { }

	DoubleNode(ValueType&& moveValue)
		:Value(std::move(moveValue)) { }

	template<class... Args>
	DoubleNode(Args&&... args)
		: Value(std::forward<Args>(args)...) { }

	DoubleNode(const DoubleNode&)				= delete;
	DoubleNode& operator=(const DoubleNode&)	= delete;

	~DoubleNode() {
		Previous	= nullptr;
		Next		= nullptr;
	}
};
// Double Node ========================================================
// END

CUSTOM_END