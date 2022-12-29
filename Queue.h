#pragma once
#include "Allocator.h"

// Queue Node ========================================================
template<class Queue>
struct QueueNode                                                     // Struct that holds data and references to next struct
{
private:
	using ValueType = typename Queue::ValueType;
	using Alloc		= typename Allocator<ValueType>;

	Alloc _alloc;

public:
	ValueType Value;                                                 // Data
	QueueNode* Next = nullptr;                                       // Reference to next

	template<class... Args>
	QueueNode(Args&&... args) {                                      // Add data using emplace type Constructor
		_alloc.construct(&Value, std::forward<Args>(args)...);
	}
};
// Queue Node ========================================================
// END


// Queue ========================================================
template<class Type>
class Queue
{
public:
	using ValueType = Type;                                         // Type for stored values
	using Node		= typename QueueNode<Queue<ValueType>>;         // Node type

private:
	size_t _size	= 0;                                            // Number of Nodes held by this
	Node* _head		= nullptr;                                      // Head of this list
	Node* _tail		= nullptr;                                      // Tail of this list

	mutable Node* _workspaceNode = nullptr;                         // Auxiliary Node for work
	mutable ValueType _workspaceValue;                              // Auxiliary Value for work

public:
	// Constructors

	Queue() = default;

	template<class... Args>
	Queue(const size_t& newSize, Args&&... args) {                  // Emplace ValueType Constructor
		create_until_size(newSize, std::forward<Args>(args)...);
	}

	Queue(const size_t& newSize, const ValueType& value) {          // Reference type Constructor
		create_until_size(newSize, value);
	}

	Queue(const size_t& newSize, ValueType&& value) {               // Temporary type Constructor
		create_until_size(newSize, std::move(value));
	}

	Queue(const Queue& other) {                                     // Copy Constructor
		copy(other);
	}

	Queue(Queue<ValueType>&& other) noexcept {                      // Move Constructor
		move(std::move(other));
	}

	~Queue() {                                                      // Destructor
		clear();
	}

public:
	// Main functions

	template<class... Args>
	void enqueue(Args&&... args) {                                  // Construct object using arguments (Args) and add it to the tail
		emplace_back(std::forward<Args>(args)...);
	}

	void enqueue(const ValueType& copyValue) {                      // Construct object using reference and add it to the tail
		emplace_back(copyValue);
	}

	void enqueue(ValueType&& moveValue) {                           // Construct object using temporary and add it to the tail
		emplace_back(std::move(moveValue));
	}

	ValueType&& dequeue() {                                         // Return first component and remove it from queue
		if (_head) {
			_workspaceValue = std::move(_head->Value);
			_workspaceNode = _head;
			_head = _head->Next;

			if (_head == nullptr)
				_tail = nullptr;

			delete _workspaceNode;
			_size--;

			return std::move(_workspaceValue);
		}
	}

	const size_t size() const {                                     // Get size
		return _size;
	}

	bool empty() const {                                            // Check if list is empty
		return _size == 0;
	}

	void clear() {                                                  // Remove ALL components
		while (_size) {
			_workspaceNode = _head;
			_head = _head->Next;

			delete _workspaceNode;
			_size--;
		}

		_head = _tail = nullptr;
	}

public:
	// Operators

	Queue& operator=(const Queue<ValueType>& other) {              // Assign operator using reference
		clear();
		copy(other);

		return *this;
	}

	Queue& operator=(Queue<ValueType>&& other) noexcept {          // Assign operator using temporary
		clear();
		move(std::move(other));

		return *this;
	}

private:
	// Others

	void copy(const Queue<ValueType>& other) {
		_workspaceNode = other._head;
		while (_size < other._size) {
			enqueue(_workspaceNode->Value);
			_workspaceNode = _workspaceNode->Next;
		}
	}

	void move(Queue<ValueType>&& other) {
		_head = other._head;
		_tail = other._tail;
		_size = other._size;

		other._head = other._tail = nullptr;
		other._size = 0;
	}

	template<class... Args>
	void emplace_back(Args&&... args) {                             // Construct object using arguments (Args) and add it to the tail
		Node* newNode = new Node(std::forward<Args>(args)...);

		if (_head == nullptr)
			_head = _tail = newNode;
		else {
			_tail->Next = newNode;
			_tail = newNode;
		}
		_size++;
	}

	template<class... Args>
	void create_until_size(const size_t& newSize, Args&&... args) { // Add elements until current size equals newSize
		while (_size < newSize)
			emplace_back(std::forward<Args>(args)...);
	}
};
// Queue ========================================================
// END