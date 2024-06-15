#pragma once
#include "c_type_traits.h"
#include "c_limits.h"
#include "c_pair.h"
#include "c_iterator.h"


CUSTOM_BEGIN

template<class>
struct pointer_traits;

#pragma region Helpers
// get first parameter in template
template<class Ty>
struct _GetFirstParameter;

template<template<class, class...> class Ty, class first, class... Rest>
struct _GetFirstParameter<Ty<first, Rest...>>                 // given Ty<first, Rest...>, extract first
{
    using type = first;
};

// replace first parameter in template
template<class NewFirst, class Ty>
struct _ReplaceFirstParameter;

template<class NewFirst, template<class, class...> class Ty, class first, class... Rest>
struct _ReplaceFirstParameter<NewFirst, Ty<first, Rest...>>   // given Ty<first, Rest...>, replace first
{
    using type = Ty<NewFirst, Rest...>;
};

// get reference type
template<class Ty, class = void>
struct _GetReferenceType
{
    using type = typename Ty::value_type&;
};

template<class Ty>
struct _GetReferenceType<Ty, void_t<typename Ty::reference>>
{
    using type = typename Ty::reference;
};

// get const reference type
template<class Ty, class = void>
struct _GetConstReferenceType
{
    using type = const typename Ty::value_type&;
};

template<class Ty>
struct _GetConstReferenceType<Ty, void_t<typename Ty::const_reference>>
{
    using type = typename Ty::const_reference;
};

// get pointer type
template<class Ty, class = void>
struct _GetPointerType
{
    using type = typename Ty::value_type*;
};

template<class Ty>
struct _GetPointerType<Ty, void_t<typename Ty::pointer>>
{
    using type = typename Ty::pointer;
};

// get ptr difference type
template<class Ty, class = void>
struct _GetPtrDifferenceType
{
    using type = ptrdiff_t;
};

template<class Ty>
struct _GetPtrDifferenceType<Ty, void_t<typename Ty::difference_type>>
{
    using type = typename Ty::difference_type;
};

// get difference type
template<class Ty, class = void>
struct _GetDifferenceType
{
    using type = typename pointer_traits<typename _GetPointerType<Ty>::type>::difference_type;
};

template<class Ty>
struct _GetDifferenceType<Ty, void_t<typename Ty::difference_type>>
{
    using type = typename Ty::difference_type;
};

// get const pointer type
template<class Ty, class = void>
struct _GetConstPointerType
{
    using type = typename pointer_traits<typename _GetPointerType<Ty>::type>::template rebind<const typename Ty::value_type>;
};

template<class Ty>
struct _GetConstPointerType<Ty, void_t<typename Ty::const_pointer>>
{
    using type = typename Ty::const_pointer;
};

// get void pointer type
template<class Ty, class = void>
struct _GetVoidPointerType
{
    using type = typename pointer_traits<typename _GetPointerType<Ty>::type>::template rebind<void>;
};

template<class Ty>
struct _GetVoidPointerType<Ty, void_t<typename Ty::void_pointer>>
{
    using type = typename Ty::void_pointer;
};

// get const void pointer type
template<class Ty, class = void>
struct _GetConstVoidPointerType
{
    using type = typename pointer_traits<typename _GetPointerType<Ty>::type>::template rebind<const void>;
};

template<class Ty>
struct _GetConstVoidPointerType<Ty, void_t<typename Ty::const_void_pointer>>
{
    using type = typename Ty::const_void_pointer;
};

// get propagate on container copy
template<class Ty, class = void>
struct _GetPropagateOnContainerCopy
{
    using type = false_type;
};

template<class Ty>
struct _GetPropagateOnContainerCopy<Ty, void_t<typename Ty::propagate_on_container_copy_assignment>>
{
    using type = typename Ty::propagate_on_container_copy_assignment;
};

// get propagate on container move
template<class Ty, class = void>
struct _GetPropagateOnContainerMove
{
    using type = false_type;
};

template<class Ty>
struct _GetPropagateOnContainerMove<Ty, void_t<typename Ty::propagate_on_container_move_assignment>>
{
    using type = typename Ty::propagate_on_container_move_assignment;
};

// get propagate on container swap
template<class Ty, class = void>
struct _GetPropagateOnContainerSwap {
    using type = false_type;
};

template<class Ty>
struct _GetPropagateOnContainerSwap<Ty, void_t<typename Ty::propagate_on_container_swap>>
{
    using type = typename Ty::propagate_on_container_swap;
};

// get is always equal
template<class Ty, class = void>
struct _GetIsAlwaysEqual
{
    using type = is_empty<Ty>;
};

template<class Ty>
struct _GetIsAlwaysEqual<Ty, void_t<typename Ty::is_always_equal>>
{
    using type = typename Ty::is_always_equal;
};

// get rebind type
template<class Ty, class Other, class = void>
struct _GetRebindType
{
    using type = typename _ReplaceFirstParameter<Other, Ty>::type;
};

template<class Ty, class Other>
struct _GetRebindType<Ty, Other, void_t<typename Ty::template rebind<Other>>>
{
    using type = typename Ty::template rebind<Other>;
};
#pragma endregion Helpers


#pragma region Operations on uninitialized memory
// uninitialized_copy, uninitialized_copy_n
template<class InputIt, class NoThrowForwardIt>
NoThrowForwardIt uninitialized_copy(InputIt first, InputIt last, NoThrowForwardIt destFirst) {
    using value_type = typename iterator_traits<NoThrowForwardIt>::value_type;

    _verify_iteration_range(first, last);
    NoThrowForwardIt current = destFirst;
    
    try
    {
        for (/*Empty*/; first != last; ++first, (void)++current)
            ::new(&(*current)) value_type(*first);

        return current;
    }
    catch (...)
    {
        for (/*Empty*/; destFirst != current; ++destFirst)
            destFirst->~value_type();

        CUSTOM_RERAISE;
    }
}

template<class InputIt, class Size, class NoThrowForwardIt>
NoThrowForwardIt uninitialized_copy_n(InputIt first, Size count, NoThrowForwardIt destFirst) {
    using value_type = typename iterator_traits<NoThrowForwardIt>::value_type;

    NoThrowForwardIt current = destFirst;
    try
    {
        for (/*Empty*/; count > 0; ++first, (void)++current, --count)
            ::new(&(*current)) value_type(*first);
    
        return current;
    }
    catch (...)
    {
        for (/*Empty*/; destFirst != current; ++destFirst)
            destFirst->~value_type();

        CUSTOM_RERAISE;
    }
}
// END uninitialized_copy, uninitialized_copy_n


// uninitialized_fill, uninitialized_fill_n
template<class ForwardIt, class Type>
void uninitialized_fill(ForwardIt first, ForwardIt last, const Type& value) {
    using value_type = typename iterator_traits<ForwardIt>::value_type;

    _verify_iteration_range(first, last);
    ForwardIt current = first;

    try
    {
        for (/*Empty*/; current != last; ++current)
            ::new(&(*current)) value_type(value);
    }
    catch (...)
    {
        for (/*Empty*/; first != current; ++first)
            first->~value_type();

        CUSTOM_RERAISE;
    }
}

template<class ForwardIt, class Size, class Type>
ForwardIt uninitialized_fill_n(ForwardIt first, Size count, const Type& value) {
    using value_type = typename iterator_traits<ForwardIt>::value_type;

    ForwardIt current = first;

    try
    {
        for (/*Empty*/; count > 0; ++current, (void)--count)
            ::new(&(*current)) value_type(value);

        return current;
    }
    catch (...)
    {
        for (/*Empty*/; first != current; ++first)
            first->~value_type();

        CUSTOM_RERAISE;
    }
}
// END uninitialized_fill, uninitialized_fill_n


// uninitialized_move, uninitialized_move_n
template<class InputIt, class NoThrowForwardIt>
NoThrowForwardIt uninitialized_move(InputIt first, InputIt last, NoThrowForwardIt destFirst) {
    using value_type = typename iterator_traits<NoThrowForwardIt>::value_type;

    _verify_iteration_range(first, last);
    NoThrowForwardIt current = destFirst;

    try
    {
        for (/*Empty*/; first != last; ++first, (void)++current)
            ::new(&(*current)) value_type(custom::move(*first));

        return current;
    }
    catch (...)
    {
        for (/*Empty*/; destFirst != current; ++destFirst)
            destFirst->~value_type();

        CUSTOM_RERAISE;
    }
}

template<class InputIt, class Size, class NoThrowForwardIt>
custom::pair<InputIt, NoThrowForwardIt> uninitialized_move_n(InputIt first, Size count, NoThrowForwardIt destFirst) {
    using value_type = typename iterator_traits<NoThrowForwardIt>::value_type;
    
    NoThrowForwardIt current = destFirst;

    try
    {
        for (/*Empty*/; count > 0; ++first, (void)++current, --count)
            ::new(&(*current)) value_type(custom::move(*first));
    
        return { first, current };
    }
    catch (...)
    {
        for (/*Empty*/; destFirst != current; ++destFirst)
            destFirst->~value_type();

        CUSTOM_RERAISE;
    }
}
// END uninitialized_move, uninitialized_move_n


// uninitialized_default_construct, uninitialized_default_construct_n
template<class ForwardIt>
void uninitialized_default_construct(ForwardIt first, ForwardIt last) {
    using value_type = typename iterator_traits<ForwardIt>::value_type;
    
    _verify_iteration_range(first, last);
    ForwardIt current = first;

    try
    {
        for (/*Empty*/; current != last; ++current)
            ::new(&(*current)) value_type;
    }
    catch (...)
    {
        for (/*Empty*/; first != current; ++first)
            first->~value_type();

        CUSTOM_RERAISE;
    }
}

template<class ForwardIt, class Size>
ForwardIt uninitialized_default_construct_n(ForwardIt first, Size n) {
    using value_type = typename iterator_traits<ForwardIt>::value_type;

    ForwardIt current = first;
    
    try
    {
        for (/*Empty*/; n > 0; (void)++current, --n)
            ::new(&(*current)) value_type;

        return current;
    }
    catch (...)
    {
        for (/*Empty*/; first != current; ++first)
            first->~value_type();

        CUSTOM_RERAISE;
    }
}
// END uninitialized_default_construct, uninitialized_default_construct_n


// uninitialized_value_construct, uninitialized_value_construct_n
template<class ForwardIt>
void uninitialized_value_construct(ForwardIt first, ForwardIt last) {
    using value_type = typename iterator_traits<ForwardIt>::value_type;

    _verify_iteration_range(first, last);
    ForwardIt current = first;

    try
    {
        for (/*Empty*/; current != last; ++current)
            ::new(&(*current)) value_type();
    }
    catch (...)
    {
        for (/*Empty*/; first != current; ++first)
            first->~value_type();

        CUSTOM_RERAISE;
    }
}

template<class ForwardIt, class Size>
ForwardIt uninitialized_value_construct_n(ForwardIt first, Size n) {
    using value_type = typename iterator_traits<ForwardIt>::value_type;

    ForwardIt current = first;

    try
    {
        for (/*Empty*/; n > 0; (void)++current, --n)
            ::new(&(*current)) value_type();

        return current;
    }
    catch (...)
    {
        for (/*Empty*/; first != current; ++first)
            first->~value_type();

        CUSTOM_RERAISE;
    }
}
// END uninitialized_value_construct, uninitialized_value_construct_n


// construct_at
template<class Type, class... Args>
constexpr Type* construct_at(Type* const address, Args&&... args) {
	return ::new(address) Type(custom::forward<Args>(args)...);
}
// END construct_at


// destroy, destroy_at, destroy_n
template<class Type>
constexpr void destroy_at(Type* const address) {
    if constexpr (is_array_v<Type>)
        for (auto &elem : *address)
            destroy_at(&(elem));
    else
        address->~Type();
}

template<class ForwardIt>
constexpr void destroy(ForwardIt first, ForwardIt last) {
    _verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first)
        custom::destroy_at(&(*first));
}

template<class ForwardIt, class Size>
constexpr ForwardIt destroy_n(ForwardIt first, Size n) {
    for (/*Empty*/; n > 0; (void)++first, --n)
        custom::destroy_at(&(*first));

    return first;
}
// END destroy, destroy_at, destroy_n
#pragma endregion Operations on uninitialized memory


#pragma region Pointer
template<class Type, class Elem>
struct _PointerTraitsBase
{
    using pointer           = Type;
    using element_type      = Elem;
    using difference_type   = typename _GetPtrDifferenceType<Type>::type;

    template<class Other>
    using rebind            = typename _GetRebindType<Type, Other>::type;

    static constexpr pointer pointer_to(element_type& val)
    noexcept(noexcept(element_type::pointer_to(val))) {
        return element_type::pointer_to(val);
    }
};  // END _PointerTraitsBase

template<class, class = void, class = void>
struct _PointerTraitsElemChoice {};

template<class Ty, class Dummy>
struct _PointerTraitsElemChoice<Ty, Dummy, void_t<typename _GetFirstParameter<Ty>::type>>
: _PointerTraitsBase<Ty, typename _GetFirstParameter<Ty>::type> {};

template<class Ty>
struct _PointerTraitsElemChoice<Ty, void_t<typename Ty::element_type>, void>
: _PointerTraitsBase<Ty, typename Ty::element_type> {};

template<class Ty>
struct pointer_traits : _PointerTraitsElemChoice<Ty> {};


template<class Type>
struct pointer_traits<Type*>
{
    using pointer           = Type*;
    using element_type      = Type;
    using difference_type   = ptrdiff_t;

    template<class Other>
    using rebind            = Other*;

    static constexpr pointer pointer_to(element_type& val) noexcept {
        return &val;
    }
};  // END pointer_traits specialization
#pragma endregion Pointer


#pragma region Allocator
template<class Type>
class allocator			// Allocator Template
{
public:
	static_assert(!is_const_v<Type>, "The C++ Standard forbids containers of const elements ");
	static_assert(!is_function_v<Type>, "The C++ Standard forbids allocators for function elements ");
	static_assert(!is_reference_v<Type>, "The C++ Standard forbids allocators for reference elements ");

public:
	using value_type        = Type;
    using differece_type    = ptrdiff_t;

public:
	// Constructors & Operators

	allocator() noexcept = default;
	allocator(const allocator&) noexcept = default;

	template<class Ty>
	allocator(const allocator<Ty>&) noexcept { /*Empty*/ }
	allocator& operator=(const allocator&) = default;

	~allocator() = default;

public:
	// Main functions

	Type* allocate(const size_t capacity) {
		static_assert(sizeof(Type) > 0, "Type must be complete before calling allocate");
		return static_cast<Type*>(::operator new(capacity * sizeof(Type)));
	}

	void deallocate(Type* address, const size_t capacity) {
		CUSTOM_ASSERT(address != nullptr || capacity > 0, "Invalid block deallocation");
		::operator delete(address, capacity * sizeof(Type));
	}
}; // END Allocator


// check construct/destruct existence
template<class Alloc, class Ty, class = void>
struct _HasConstructMemberFunction : false_type {};

template<class Alloc, class Ty>
struct _HasConstructMemberFunction<Alloc, Ty,
void_t<decltype(custom::declval<Alloc>().construct(custom::declval<Ty*>()))>> : true_type {};

template<class Alloc, class Ty, class = void>
struct _HasDestroyMemberFunction : false_type {};

template<class Alloc, class Ty>
struct _HasDestroyMemberFunction<Alloc, Ty,
void_t<decltype(custom::declval<Alloc>().destroy(custom::declval<Ty*>()))>> : true_type {};

// check max_size existence
template<class Alloc, class = void>
struct _HasMaxSizeMemberFunction : false_type {};

template<class Alloc>
struct _HasMaxSizeMemberFunction<Alloc,
void_t<decltype(custom::declval<Alloc>().max_size())>> : true_type {};

// check select_on_container_copy_construction existence
template<class Alloc, class = void>
struct _HasSelectOnContainerCopyConstructionMemberFunction : false_type {};

template<class Alloc>
struct _HasSelectOnContainerCopyConstructionMemberFunction<Alloc,
void_t<decltype(custom::declval<Alloc>().select_on_container_copy_construction())>> : true_type {};


template<class Alloc>
struct allocator_traits						// allocator_traits any
{
    using allocator_type        = Alloc;
    using value_type            = typename Alloc::value_type;
    using difference_type       = typename _GetDifferenceType<Alloc>::type;
    using reference             = typename _GetReferenceType<Alloc>::type;
    using const_reference       = typename _GetConstReferenceType<Alloc>::type;
    using pointer               = typename _GetPointerType<Alloc>::type;
    using const_pointer         = typename _GetConstPointerType<Alloc>::type;
    using void_pointer          = typename _GetVoidPointerType<Alloc>::type;
    using const_void_pointer    = typename _GetConstVoidPointerType<Alloc>::type;

    using propagate_on_container_copy_assignment    = typename _GetPropagateOnContainerCopy<Alloc>::type;
    using propagate_on_container_move_assignment    = typename _GetPropagateOnContainerMove<Alloc>::type;
    using propagate_on_container_swap               = typename _GetPropagateOnContainerSwap<Alloc>::type;
    using is_always_equal                           = typename _GetIsAlwaysEqual<Alloc>::type;

    template<class Other>
    using rebind_alloc  = typename _GetRebindType<Alloc, Other>::type;  // always uses _ReplaceFirstParameter

    template<class Other>
    using rebind_traits = allocator_traits<rebind_alloc<Other>>;

    static constexpr pointer allocate(allocator_type& al, const size_t capacity) {
        return al.allocate(capacity);
    }

    static constexpr void deallocate(allocator_type& al, pointer address, const size_t capacity) {
        al.deallocate(address, capacity);
    }

    template<class _Type, class... Args>
    static constexpr void construct(allocator_type& al, _Type* const address, Args&&... args) {
        if constexpr (_HasConstructMemberFunction<allocator_type, _Type>::Value)
            al.construct(address, custom::forward<Args>(args)...);
        else
            custom::construct_at(address, custom::forward<Args>(args)...);
    }

    template<class _Type>
    static constexpr void destroy(allocator_type& al, _Type* const address) {
        if constexpr (_HasDestroyMemberFunction<allocator_type, _Type>::Value)
            al.destroy(address);
        else
            custom::destroy_at(address);
    }

    static constexpr size_t max_size(const allocator_type& al) noexcept {
        if constexpr (_HasMaxSizeMemberFunction<allocator_type>::Value)
            return al.max_size();

        return static_cast<size_t>(-1) / sizeof(value_type);
    }

    static constexpr allocator_type select_on_container_copy_construction(const allocator_type& al) {
        if constexpr (_HasSelectOnContainerCopyConstructionMemberFunction<allocator_type>::Value)
            return al.select_on_container_copy_construction();

        return al;
    }
};	// END allocator_traits any


template<class Type>
struct allocator_traits<allocator<Type>>		// allocator_traits default
{
	using allocator_type        = allocator<Type>;
    using value_type            = Type;
    using difference_type       = ptrdiff_t;
    using reference             = value_type&;
    using const_reference       = const value_type&;
    using pointer               = value_type*;
    using const_pointer         = const value_type*;
    using void_pointer          = void*;
    using const_void_pointer    = const void*;

    using propagate_on_container_copy_assignment 	= false_type;
    using propagate_on_container_move_assignment 	= true_type;
    using propagate_on_container_swap 				= false_type;
    using is_always_equal                           = true_type;

    template<class Other>
    using rebind_alloc  = allocator<Other>;

    template<class Other>
    using rebind_traits = allocator_traits<allocator<Other>>;

	static constexpr pointer allocate(allocator_type& al, const size_t capacity) {
		return al.allocate(capacity);
	}

	static constexpr void deallocate(allocator_type& al, pointer address, const size_t capacity) {
		al.deallocate(address, capacity);
	}

    template<class _Type, class... Args>
    static constexpr void construct(allocator_type&, _Type* const address, Args&&... args) {
        custom::construct_at(address, custom::forward<Args>(args)...);
    }

    template<class _Type>
    static constexpr void destroy(allocator_type&, _Type* const address) {
        custom::destroy_at(address);
    }

    static constexpr size_t max_size(const allocator_type&) noexcept {
        return static_cast<size_t>(-1) / sizeof(value_type);
    }

    static constexpr allocator_type select_on_container_copy_construction(const allocator_type& al) {
        return al;
    }
};	// END allocator_traits default
#pragma endregion Allocator

CUSTOM_END