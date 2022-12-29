#pragma once

namespace custom {

	// Queue Node ========================================================
	template<class Queue>
	struct QueueNode														// Struct that holds data and references to next struct
	{
	private:
		using ValueType = typename Queue::ValueType;

	public:
		ValueType* Value	= nullptr;										// Data
		QueueNode* Next		= nullptr;										// Reference to next

		QueueNode()								= default;
		QueueNode(const QueueNode&)				= delete;
		QueueNode& operator=(const QueueNode&)	= delete;

		template<class... Args>
		QueueNode(Args&&... args) {											// Add data using emplace type Constructor
			Value = new ValueType(std::forward<Args>(args)...);
		}

		~QueueNode() {
			delete Value;
			Value = nullptr;
			Next = nullptr;
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

	public:
		// Constructors

		Queue() = default;

		Queue(const size_t& newSize, const ValueType& value) {          // Reference type Constructor
			create_until_size(newSize, value);
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
		void emplace(Args&&... args) {                                  // Construct object using arguments (Args) and add it to the tail
			Node* newNode = new Node(std::forward<Args>(args)...);

			if (_head == nullptr)
				_head = _tail = newNode;
			else {
				_tail->Next = newNode;
				_tail = newNode;
			}
			_size++;
		}

		void push(const ValueType& copyValue) {
			emplace(copyValue);
		}

		void push(ValueType&& moveValue) {
			emplace(std::move(moveValue));
		}

		void pop() {                                                   // Return first component and remove it from queue
			if (_head) {
				_workspaceNode = _head;
				_head = _head->Next;

				if (_head == nullptr)
					_tail = nullptr;

				delete _workspaceNode;
				_size--;
			}
		}

		ValueType& front() {
			return *_head->Value;
		}

		const ValueType& front() const {
			return *_head->Value;
		}

		ValueType& back() {
			return *_tail->Value;
		}

		const ValueType& back() const {
			return *_tail->Value;
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
			if (_head != other._head)
			{
				clear();
				copy(other);
			}

			return *this;
		}

		Queue& operator=(Queue<ValueType>&& other) noexcept {          // Assign operator using temporary
			if (_head != other._head)
			{
				clear();
				move(std::move(other));
			}

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
		void create_until_size(const size_t& newSize, Args&&... args) { // Add elements until current size equals newSize
			while (_size < newSize)
				emplace(std::forward<Args>(args)...);
		}
	};
	// Queue ========================================================
	// END

} // END custom::