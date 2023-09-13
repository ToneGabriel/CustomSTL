#pragma once
#include "TypeTraits.h"


CUSTOM_BEGIN

#pragma region Operations on uninitialized memory
// TODO: implement

// uninitialized_copy
template<class InputIt, class NoThrowForwardIt>
NoThrowForwardIt uninitialized_copy(InputIt first, InputIt last, NoThrowForwardIt d_first) {
    using Type = typename std::iterator_traits<NoThrowForwardIt>::value_type;

    NoThrowForwardIt current = d_first;
    
    try
    {
        for (; first != last; ++first, (void) ++current)
            ::new (static_cast<void*>(&(*current))) Type(*first);

        return current;
    }
    catch (...)
    {
        for (; d_first != current; ++d_first)
            d_first->~Type();

        throw;
    }
}
// END uninitialized_copy


// destroy, destroy_at, destroy_n
template<class Type>
constexpr void destroy_at(Type* p) {
    if (IsArray_v<Type>)
        for (auto &elem : *p)
            destroy_at(&(elem));
    else
        p->~Type(); 
}

template<class ForwardIt>
constexpr void destroy(ForwardIt first, ForwardIt last) {
    for (; first != last; ++first)
        custom::destroy_at(&(*first));
}

template<class ForwardIt, class Size>
constexpr ForwardIt destroy_n(ForwardIt first, Size n) {
    for (; n > 0; (void) ++first, --n)
        custom::destroy_at(&(*first));

    return first;
}
// END destroy, destroy_at, destroy_n
#pragma endregion Operations on uninitialized memory


#pragma region Allocator
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

	Type* allocate(const size_t& capacity) {
		static_assert(sizeof(Type) > 0, "Type must be complete before calling alloc");
		return static_cast<Type*>(::operator new(capacity * sizeof(Type)));
	}

	void deallocate(Type* address, const size_t& capacity) {
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
}; // END Allocator
#pragma endregion Allocator

CUSTOM_END