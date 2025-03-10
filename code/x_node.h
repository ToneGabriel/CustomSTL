#pragma once
#include <c_utility.h>


CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN

template<class Type>
struct _Forward_Node			// Struct that holds data and references to next struct
{
	using value_type = Type;

	value_type _Value;
	_Forward_Node* _Next = nullptr;

	_Forward_Node()									= default;
	~_Forward_Node() 								= default;
	_Forward_Node(const _Forward_Node&)				= delete;
	_Forward_Node& operator=(const _Forward_Node&)	= delete;

	_Forward_Node(const value_type& copyVal)
		:_Value(copyVal) { /*Empty*/ }

	_Forward_Node(value_type&& moveVal)
		:_Value(custom::move(moveVal)) { /*Empty*/ }

	template<class... Args>
	_Forward_Node(Args&&... args)
		:_Value(custom::forward<Args>(args)...) { /*Empty*/ }
}; // END _Forward_Node


template<class Type>
struct _Double_Node			// Struct that holds data and references to next and previous struct
{
	using value_type = Type;

	value_type _Value;
	_Double_Node* _Previous 	= nullptr;
	_Double_Node* _Next 		= nullptr;

	_Double_Node()									= default;
	~_Double_Node() 								= default;
	_Double_Node(const _Double_Node&)				= delete;
	_Double_Node& operator=(const _Double_Node&)	= delete;

	_Double_Node(const value_type& copyVal)
		:_Value(copyVal) { /*Empty*/ }

	_Double_Node(value_type&& moveVal)
		:_Value(custom::move(moveVal)) { /*Empty*/ }

	template<class... Args>
	_Double_Node(Args&&... args)
		: _Value(custom::forward<Args>(args)...) { /*Empty*/ }
}; // END _Double_Node


template<class Type>
struct _Tree_Node			// Used in _Search_Tree
{
	using value_type = Type;
	
	enum class Colors
	{
		Red,
		Black
	};

	value_type _Value;
	_Tree_Node* _Parent	= nullptr;
	_Tree_Node* _Left 	= nullptr;
	_Tree_Node* _Right 	= nullptr;
	bool _IsNil 		= false;			// True for Head only
	Colors _Color 		= Colors::Red;		// Used for balancing

	_Tree_Node()								= default;
	~_Tree_Node()								= default;
	_Tree_Node(const _Tree_Node&)				= delete;
	_Tree_Node& operator=(const _Tree_Node&)	= delete;

	_Tree_Node(const value_type& copyVal)
		:_Value(copyVal) { /*Empty*/ }

	_Tree_Node(value_type&& moveVal)
		:_Value(custom::move(moveVal)) { /*Empty*/ }

	template<class... Args>
	_Tree_Node(Args&&... args)
		: _Value(custom::forward<Args>(args)...) { /*Empty*/ }

	bool is_leaf() const {
		return (_Left->_IsNil && _Right->_IsNil);
	}
}; // END Tree Node

CUSTOM_DETAIL_END

CUSTOM_END