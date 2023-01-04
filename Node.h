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
	ValueType* Value	= nullptr;										// Data
	ForwardNode* Next	= nullptr;										// Reference to next

	~ForwardNode() {
		delete Value;
		Value	= nullptr;
		Next	= nullptr;
	}

	static ForwardNode* create_head() {
		return new ForwardNode();
	}

	template<class... Args>
	static ForwardNode* create_non_head(Args&&... args) {				// Add data using emplace ValueTypepe Constructor
		ForwardNode* temp = new ForwardNode();							// Workaround default Value initialization
		temp->Value = new ValueType(std::forward<Args>(args)...);
		return temp;
	}

private:
	ForwardNode()								= default;
	ForwardNode(const ForwardNode&)				= delete;
	ForwardNode& operator=(const ForwardNode&)	= delete;
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
	ValueType* Value		= nullptr;									// Data
	DoubleNode* Previous	= nullptr;									// Reference to previous 
	DoubleNode* Next		= nullptr;									// Reference to next

	~DoubleNode() {
		delete Value;
		Value		= nullptr;
		Previous	= nullptr;
		Next		= nullptr;
	}

	static DoubleNode* create_head() {
		return new DoubleNode();
	}

	template<class... Args>
	static DoubleNode* create_non_head(Args&&... args) {				// Add data using emplace ValueTypepe Constructor
		DoubleNode* temp = new DoubleNode();							// Workaround default Value initialization
		temp->Value = new ValueType(std::forward<Args>(args)...);
		return temp;
	}

private:
	DoubleNode()								= default;
	DoubleNode(const DoubleNode&)				= delete;
	DoubleNode& operator=(const DoubleNode&)	= delete;
};
// Double Node ========================================================
// END

CUSTOM_END