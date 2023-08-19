#pragma once
#include "TypeTraits.h"


CUSTOM_BEGIN

template<class Type>
class Allocator			// Allocator Template
{
public:
	static_assert(!IsConst_v<Type>, "The C++ Standard forbids containers of const elements ");
	static_assert(!IsFunction_v<Type>, "The C++ Standard forbids allocators for function elements ");
	static_assert(!IsReference_v<Type>, "The C++ Standard forbids allocators for reference elements ");

public:
	// Constructors & Operators

	Allocator() noexcept = default;
	Allocator(const Allocator&) noexcept = default;

	template<class Ty>
	Allocator(const Allocator<Ty>&) noexcept { /*Empty*/ }
	Allocator& operator=(const Allocator&) = default;

	~Allocator() = default;

public:
	// Main functions

	Type* allocate(const size_t& capacity) {										// Allocate memory
		static_assert(sizeof(Type) > 0, "Type must be complete before calling alloc");
		return static_cast<Type*>(::operator new(capacity * sizeof(Type)));
	}

	void dealloc(Type* address, const size_t& capacity) {						// Deallocate memory
		CUSTOM_ASSERT(address != nullptr || capacity > 0, "Invalid block deallocation");
		::operator delete(address, capacity * sizeof(Type));
	}

	template<class... Args>
	void construct(Type* address, Args&&... args) {
		::new(address) Type(custom::forward<Args>(args)...);
	}

	void construct_range(Type* address, const size_t& length) {							// Default value
		for (size_t i = 0; i < length; ++i)
			construct(address + i);
	}

	void construct_range(Type* address, const size_t& length, const Type& value) {		// Copy value
		for (size_t i = 0; i < length; ++i)
			construct(address + i, value);
	}

	void destroy(Type* address) {
		address->~Type();
	}

	void destroy_range(Type* address, const size_t& length) {
		for (size_t i = 0; i < length; ++i)
			destroy(address + i);
	}
}; // END Allocator Template

CUSTOM_END