#pragma once
#include "Common.h"

CUSTOM_BEGIN

template<class Type>
class Allocator
{
public:

	Allocator() noexcept					= default;
	Allocator(const Allocator&) noexcept	= delete;
	Allocator& operator=(const Allocator&)	= delete;
	~Allocator()							= default;

public:

	Type* alloc(const size_t& capacity);									// Allocate memory
	void dealloc(Type* address, const size_t& capacity);					// Deallocate memory

	template<class... Args>
	void construct(Type* address, Args&&... args);
	void construct_range(Type* address, const size_t& length);							// Default value
	void construct_range(Type* address, const size_t& length, const Type& value);		// Copy value
	void destroy(Type* address);
	void destroy_range(Type* address, const size_t& length);
};



template<class Type>
Type* Allocator<Type>::alloc(const size_t& capacity) {									// Allocate memory
	return static_cast<Type*>(::operator new(capacity * sizeof(Type)));
}

template<class Type>
void Allocator<Type>::dealloc(Type* address, const size_t& capacity) {					// Deallocate memory
	if (address == nullptr || capacity == 0)
		throw std::out_of_range("Invalid block deallocation");

	::operator delete(address, capacity * sizeof(Type));
}

template<class Type>
template<class... Args>
void Allocator<Type>::construct(Type* address, Args&&... args) {
	::new(address) Type(std::forward<Args>(args)...);
}

template<class Type>
void Allocator<Type>::construct_range(Type* address, const size_t& length) {							// Default value
	for (size_t i = 0; i < length; i++)
		construct(address + i);
}

template<class Type>
void Allocator<Type>::construct_range(Type* address, const size_t& length, const Type& value) {		// Copy value
	for (size_t i = 0; i < length; i++)
		construct(address + i, value);
}

template<class Type>
void Allocator<Type>::destroy(Type* address) {
	address->~Type();
}

template<class Type>
void Allocator<Type>::destroy_range(Type* address, const size_t& length) {
	for (size_t i = 0; i < length; i++)
		destroy(address + i);
}

CUSTOM_END