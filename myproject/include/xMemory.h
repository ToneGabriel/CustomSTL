#pragma once
#include "TypeTraits.h"
#include "Limits.h"


CUSTOM_BEGIN

#pragma region Operations on uninitialized memory
// TODO: implement

// uninitialized_copy
template<class InputIt, class NoThrowForwardIt>
NoThrowForwardIt uninitialized_copy(InputIt first, InputIt last, NoThrowForwardIt d_first) {
    using Type = typename std::iterator_traits<NoThrowForwardIt>::valueType;

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


// construct_at
template<class Type, class... Args>
constexpr Type* construct_at(Type* const address, Args&&... args) {
	return ::new(address) Type(custom::forward<Args>(args)...);
}
// END construct_at


// destroy, destroy_at, destroy_n
template<class Type>
constexpr void destroy_at(Type* const address) {
    if constexpr (IsArray_v<Type>)
        for (auto &elem : *address)
            destroy_at(&(elem));
    else
        address->~Type();
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
	using ValueType = Type;

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


// template<class Alloc, class = void>
// struct _HasConstructMemberFunction : FalseType {};

// template<class Alloc>
// struct _HasConstructMemberFunction<Alloc,
// Void_t<decltype(custom::declval<Alloc>().construct(custom::declval<typename Alloc::ValueType*>()))>> : TrueType {};

template<class Alloc>
struct AllocatorTraits						// AllocatorTraits any
{
    using AllocatorType 						= Alloc;
    using ValueType								= typename Alloc::ValueType;

    // using Pointer								= typename _Get_pointer_type<Alloc>::Type;
    // using ConstPointer							= typename _Get_const_pointer_type<Alloc>::Type;
    // using VoidPointer 							= typename _Get_void_pointer_type<Alloc>::Type;
    // using ConstVoidPointer						= typename _Get_const_void_pointer_type<Alloc>::Type;

    // using PropagateOnContainerCopyAssignment	= typename _Get_propagate_on_container_copy<Alloc>::Type;
    // using PropagateOnContainerMoveAssignment	= typename _Get_propagate_on_container_move<Alloc>::Type;
    // using PropagateOnContainerSwap				= typename _Get_propagate_on_container_swap<Alloc>::Type;
    // using IsAlwaysEqual							= typename _Get_is_always_equal<Alloc>::Type;

    // template<class Other>
    // using RebindAlloc 							= typename _Get_rebind_type<Alloc, Other>::Type;

    // template<class Other>
    // using RebindTraits 							= AllocatorTraits<RebindAlloc<Other>>;
};	// END AllocatorTraits any


template<class Type>
struct AllocatorTraits<Allocator<Type>>		// AllocatorTraits default
{
	using AllocatorType 						= Allocator<Type>;
    using ValueType     						= Type;

    using Reference                             = ValueType&;
    using ConstReference                        = const Reference;
    using Pointer 								= ValueType*;
    using ConstPointer 							= const ValueType*;
    using VoidPointer 							= void*;
    using ConstVoidPointer 						= const void*;

    using PropagateOnContainerCopyAssignment 	= FalseType;
    using PropagateOnContainerMoveAssignment 	= TrueType;
    using PropagateOnContainerSwap 				= FalseType;
    using IsAlwaysEqual 						= TrueType;

    template<class Other>
    using RebindAlloc 							= Allocator<Other>;

    template<class Other>
    using RebindTraits 							= AllocatorTraits<Allocator<Other>>;

	static constexpr Pointer allocate(AllocatorType& al, const size_t& capacity) {
		return al.allocate(capacity);
	}

	static constexpr void deallocate(AllocatorType& al, Pointer address, const size_t& capacity) {
		al.deallocate(address, capacity);
	}

    template<class _Type, class... Args>
    static constexpr void construct(AllocatorType&, _Type* const address, Args&&... args) {
        custom::construct_at(address, custom::forward<Args>(args)...);
    }

    template<class _Type>
    static constexpr void destroy(AllocatorType&, _Type* const address) {
        custom::destroy_at(address);
    }

    static constexpr size_t max_size(const AllocatorType&) noexcept {
        return static_cast<size_t>(-1) / sizeof(ValueType);
    }

    static constexpr AllocatorType select_on_container_copy_construction(const AllocatorType& alloc) {
        return alloc;
    }
};	// END AllocatorTraits default
#pragma endregion Allocator

CUSTOM_END