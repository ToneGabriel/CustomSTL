#pragma once

namespace custom {

	// Forward Node ========================================================
	template<class Type>
	struct ForwardNode														// Struct that holds data and references to next struct
	{
	private:
		using ValueType = Type;

	public:
		ValueType* Value	= nullptr;										// Data
		ForwardNode* Next	= nullptr;										// Reference to next

		ForwardNode()								= default;
		ForwardNode(const ForwardNode&)				= delete;
		ForwardNode& operator=(const ForwardNode&)	= delete;

		~ForwardNode() {
			delete Value;
			Value	= nullptr;
			Next	= nullptr;
		}

		template<class... Args>
		void init_non_head(Args&&... args) {								// Add data using emplace ValueTypepe Constructor
			Value = new ValueType(std::forward<Args>(args)...);
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
		ValueType* Value		= nullptr;									// Data
		DoubleNode* Previous	= nullptr;									// Reference to previous 
		DoubleNode* Next		= nullptr;									// Reference to next

	public:

		DoubleNode()								= default;
		DoubleNode(const DoubleNode&)				= delete;
		DoubleNode& operator=(const DoubleNode&)	= delete;

		~DoubleNode() {
			delete Value;
			Value		= nullptr;
			Previous	= nullptr;
			Next		= nullptr;
		}

		template<class... Args>
		void init_non_head(Args&&... args) {								// Add data using emplace ValueTypepe Constructor
			Value = new ValueType(std::forward<Args>(args)...);
		}
	};
	// Double Node ========================================================
	// END
}