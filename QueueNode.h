#pragma once

template<class Queue>
struct QueueNode                                                             // Struct that holds data and references to next struct
{
public:
	using ValueType = typename Queue::ValueType;

	ValueType Value;                                                         // Data
	QueueNode* Next = nullptr;                                               // Reference to next

	template<class... Args>
	QueueNode(Args&&... args) {                                              // Add data using emplace type Constructor
		new(&Value) ValueType(std::forward<Args>(args)...);
	}

	QueueNode(const ValueType& value) {                                      // Add data using reference type Constructor
		new(&Value) ValueType(value);
	}

	QueueNode(ValueType&& value) {                                           // Add data using temporary type Constructor
		new(&Value) ValueType(std::move(value));
	}
};