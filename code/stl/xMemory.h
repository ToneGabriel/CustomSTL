#pragma once
#include "cTypeTraits.h"
#include "cLimits.h"
#include "cPair.h"
#include "cIterator.h"


CUSTOM_BEGIN

template<class>
struct PointerTraits;

#pragma region Helpers
// get first parameter in template
template<class Ty>
struct _GetFirstParameter;

template<template<class, class...> class Ty, class First, class... Rest>
struct _GetFirstParameter<Ty<First, Rest...>>                 // given Ty<First, Rest...>, extract First
{
    using Type = First;
};

// replace first parameter in template
template<class NewFirst, class Ty>
struct _ReplaceFirstParameter;

template<class NewFirst, template<class, class...> class Ty, class First, class... Rest>
struct _ReplaceFirstParameter<NewFirst, Ty<First, Rest...>>   // given Ty<First, Rest...>, replace First
{
    using Type = Ty<NewFirst, Rest...>;
};

// get reference type
template<class Ty, class = void>
struct _GetReferenceType
{
    using Type = typename Ty::ValueType&;
};

template<class Ty>
struct _GetReferenceType<Ty, Void_t<typename Ty::Reference>>
{
    using Type = typename Ty::Reference;
};

// get const reference type
template<class Ty, class = void>
struct _GetConstReferenceType
{
    using Type = const typename Ty::ValueType&;
};

template<class Ty>
struct _GetConstReferenceType<Ty, Void_t<typename Ty::ConstReference>>
{
    using Type = typename Ty::ConstReference;
};

// get pointer type
template<class Ty, class = void>
struct _GetPointerType
{
    using Type = typename Ty::ValueType*;
};

template<class Ty>
struct _GetPointerType<Ty, Void_t<typename Ty::Pointer>>
{
    using Type = typename Ty::Pointer;
};

// get ptr difference type
template<class Ty, class = void>
struct _GetPtrDifferenceType
{
    using Type = ptrdiff_t;
};

template<class Ty>
struct _GetPtrDifferenceType<Ty, Void_t<typename Ty::DifferenceType>>
{
    using Type = typename Ty::DifferenceType;
};

// get difference type
template<class Ty, class = void>
struct _GetDifferenceType
{
    using Type = typename PointerTraits<typename _GetPointerType<Ty>::Type>::DifferenceType;
};

template<class Ty>
struct _GetDifferenceType<Ty, Void_t<typename Ty::DifferenceType>>
{
    using Type = typename Ty::DifferenceType;
};

// get const pointer type
template<class Ty, class = void>
struct _GetConstPointerType
{
    using Type = typename PointerTraits<typename _GetPointerType<Ty>::Type>::template Rebind<const typename Ty::ValueType>;
};

template<class Ty>
struct _GetConstPointerType<Ty, Void_t<typename Ty::ConstPointer>>
{
    using Type = typename Ty::ConstPointer;
};

// get void pointer type
template<class Ty, class = void>
struct _GetVoidPointerType
{
    using Type = typename PointerTraits<typename _GetPointerType<Ty>::Type>::template Rebind<void>;
};

template<class Ty>
struct _GetVoidPointerType<Ty, Void_t<typename Ty::VoidPointer>>
{
    using Type = typename Ty::VoidPointer;
};

// get const void pointer type
template<class Ty, class = void>
struct _GetConstVoidPointerType
{
    using Type = typename PointerTraits<typename _GetPointerType<Ty>::Type>::template Rebind<const void>;
};

template<class Ty>
struct _GetConstVoidPointerType<Ty, Void_t<typename Ty::ConstVoidPointer>>
{
    using Type = typename Ty::ConstVoidPointer;
};

// get propagate on container copy
template<class Ty, class = void>
struct _GetPropagateOnContainerCopy
{
    using Type = FalseType;
};

template<class Ty>
struct _GetPropagateOnContainerCopy<Ty, Void_t<typename Ty::PropagateOnContainerCopyAssignment>>
{
    using Type = typename Ty::PropagateOnContainerCopyAssignment;
};

// get propagate on container move
template<class Ty, class = void>
struct _GetPropagateOnContainerMove
{
    using Type = FalseType;
};

template<class Ty>
struct _GetPropagateOnContainerMove<Ty, Void_t<typename Ty::PropagateOnContainerMoveAssignment>>
{
    using Type = typename Ty::PropagateOnContainerMoveAssignment;
};

// get propagate on container swap
template<class Ty, class = void>
struct _GetPropagateOnContainerSwap {
    using Type = FalseType;
};

template<class Ty>
struct _GetPropagateOnContainerSwap<Ty, Void_t<typename Ty::PropagateOnContainerSwap>>
{
    using Type = typename Ty::PropagateOnContainerSwap;
};

// get is always equal
template<class Ty, class = void>
struct _GetIsAlwaysEqual
{
    using Type = IsEmpty<Ty>;
};

template<class Ty>
struct _GetIsAlwaysEqual<Ty, Void_t<typename Ty::IsAlwaysEqual>>
{
    using Type = typename Ty::IsAlwaysEqual;
};

// get rebind type
template<class Ty, class Other, class = void>
struct _GetRebindType
{
    using Type = typename _ReplaceFirstParameter<Other, Ty>::Type;
};

template<class Ty, class Other>
struct _GetRebindType<Ty, Other, Void_t<typename Ty::template Rebind<Other>>>
{
    using Type = typename Ty::template Rebind<Other>;
};
#pragma endregion Helpers


#pragma region Operations on uninitialized memory
// uninitialized_copy, uninitialized_copy_n
template<class InputIt, class NoThrowForwardIt>
NoThrowForwardIt uninitialized_copy(InputIt first, InputIt last, NoThrowForwardIt destFirst) {
    using ValueType = typename IteratorTraits<NoThrowForwardIt>::ValueType;

    _verify_iteration_range(first, last);
    NoThrowForwardIt current = destFirst;
    
    try
    {
        for (/*Empty*/; first != last; ++first, (void)++current)
            ::new(&(*current)) ValueType(*first);

        return current;
    }
    catch (...)
    {
        for (/*Empty*/; destFirst != current; ++destFirst)
            destFirst->~ValueType();

        CUSTOM_RERAISE;
    }
}

template<class InputIt, class Size, class NoThrowForwardIt>
NoThrowForwardIt uninitialized_copy_n(InputIt first, Size count, NoThrowForwardIt destFirst) {
    using ValueType = typename IteratorTraits<NoThrowForwardIt>::ValueType;

    NoThrowForwardIt current = destFirst;
    try
    {
        for (/*Empty*/; count > 0; ++first, (void)++current, --count)
            ::new(&(*current)) ValueType(*first);
    
        return current;
    }
    catch (...)
    {
        for (/*Empty*/; destFirst != current; ++destFirst)
            destFirst->~ValueType();

        CUSTOM_RERAISE;
    }
}
// END uninitialized_copy, uninitialized_copy_n


// uninitialized_fill, uninitialized_fill_n
template<class ForwardIt, class Type>
void uninitialized_fill(ForwardIt first, ForwardIt last, const Type& value) {
    using ValueType = typename IteratorTraits<ForwardIt>::ValueType;

    _verify_iteration_range(first, last);
    ForwardIt current = first;

    try
    {
        for (/*Empty*/; current != last; ++current)
            ::new(&(*current)) ValueType(value);
    }
    catch (...)
    {
        for (/*Empty*/; first != current; ++first)
            first->~ValueType();

        CUSTOM_RERAISE;
    }
}

template<class ForwardIt, class Size, class Type>
ForwardIt uninitialized_fill_n(ForwardIt first, Size count, const Type& value) {
    using ValueType = typename IteratorTraits<ForwardIt>::ValueType;

    ForwardIt current = first;

    try
    {
        for (/*Empty*/; count > 0; ++current, (void)--count)
            ::new(&(*current)) ValueType(value);

        return current;
    }
    catch (...)
    {
        for (/*Empty*/; first != current; ++first)
            first->~ValueType();

        CUSTOM_RERAISE;
    }
}
// END uninitialized_fill, uninitialized_fill_n


// uninitialized_move, uninitialized_move_n
template<class InputIt, class NoThrowForwardIt>
NoThrowForwardIt uninitialized_move(InputIt first, InputIt last, NoThrowForwardIt destFirst) {
    using ValueType = typename IteratorTraits<NoThrowForwardIt>::ValueType;

    _verify_iteration_range(first, last);
    NoThrowForwardIt current = destFirst;

    try
    {
        for (/*Empty*/; first != last; ++first, (void)++current)
            ::new(&(*current)) ValueType(custom::move(*first));

        return current;
    }
    catch (...)
    {
        for (/*Empty*/; destFirst != current; ++destFirst)
            destFirst->~ValueType();

        CUSTOM_RERAISE;
    }
}

template<class InputIt, class Size, class NoThrowForwardIt>
custom::Pair<InputIt, NoThrowForwardIt> uninitialized_move_n(InputIt first, Size count, NoThrowForwardIt destFirst) {
    using ValueType = typename IteratorTraits<NoThrowForwardIt>::ValueType;
    
    NoThrowForwardIt current = destFirst;

    try
    {
        for (/*Empty*/; count > 0; ++first, (void)++current, --count)
            ::new(&(*current)) ValueType(custom::move(*first));
    
        return { first, current };
    }
    catch (...)
    {
        for (/*Empty*/; destFirst != current; ++destFirst)
            destFirst->~ValueType();

        CUSTOM_RERAISE;
    }
}
// END uninitialized_move, uninitialized_move_n


// uninitialized_default_construct, uninitialized_default_construct_n
template<class ForwardIt>
void uninitialized_default_construct(ForwardIt first, ForwardIt last) {
    using ValueType = typename IteratorTraits<ForwardIt>::ValueType;
    
    _verify_iteration_range(first, last);
    ForwardIt current = first;

    try
    {
        for (/*Empty*/; current != last; ++current)
            ::new(&(*current)) ValueType;
    }
    catch (...)
    {
        for (/*Empty*/; first != current; ++first)
            first->~ValueType();

        CUSTOM_RERAISE;
    }
}

template<class ForwardIt, class Size>
ForwardIt uninitialized_default_construct_n(ForwardIt first, Size n) {
    using ValueType = typename IteratorTraits<ForwardIt>::ValueType;

    ForwardIt current = first;
    
    try
    {
        for (/*Empty*/; n > 0; (void)++current, --n)
            ::new(&(*current)) ValueType;

        return current;
    }
    catch (...)
    {
        for (/*Empty*/; first != current; ++first)
            first->~ValueType();

        CUSTOM_RERAISE;
    }
}
// END uninitialized_default_construct, uninitialized_default_construct_n


// uninitialized_value_construct, uninitialized_value_construct_n
template<class ForwardIt>
void uninitialized_value_construct(ForwardIt first, ForwardIt last) {
    using ValueType = typename IteratorTraits<ForwardIt>::ValueType;

    _verify_iteration_range(first, last);
    ForwardIt current = first;

    try
    {
        for (/*Empty*/; current != last; ++current)
            ::new(&(*current)) ValueType();
    }
    catch (...)
    {
        for (/*Empty*/; first != current; ++first)
            first->~ValueType();

        CUSTOM_RERAISE;
    }
}

template<class ForwardIt, class Size>
ForwardIt uninitialized_value_construct_n(ForwardIt first, Size n) {
    using ValueType = typename IteratorTraits<ForwardIt>::ValueType;

    ForwardIt current = first;

    try
    {
        for (/*Empty*/; n > 0; (void)++current, --n)
            ::new(&(*current)) ValueType();

        return current;
    }
    catch (...)
    {
        for (/*Empty*/; first != current; ++first)
            first->~ValueType();

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
    if constexpr (IsArray_v<Type>)
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
    using Pointer           = Type;
    using ElementType       = Elem;
    using DifferenceType    = typename _GetPtrDifferenceType<Type>::Type;

    template<class Other>
    using Rebind            = typename _GetRebindType<Type, Other>::Type;

    static constexpr Pointer pointer_to(ElementType& val)
    noexcept(noexcept(ElementType::pointer_to(val))) {
        return ElementType::pointer_to(val);
    }
};  // END _PointerTraitsBase

template<class, class = void, class = void>
struct _PointerTraitsElemChoice {};

template<class Ty, class Dummy>
struct _PointerTraitsElemChoice<Ty, Dummy, Void_t<typename _GetFirstParameter<Ty>::Type>>
: _PointerTraitsBase<Ty, typename _GetFirstParameter<Ty>::Type> {};

template<class Ty>
struct _PointerTraitsElemChoice<Ty, Void_t<typename Ty::ElementType>, void>
: _PointerTraitsBase<Ty, typename Ty::ElementType> {};

template<class Ty>
struct PointerTraits : _PointerTraitsElemChoice<Ty> {};


template<class Type>
struct PointerTraits<Type*>
{
    using Pointer           = Type*;
    using ElementType       = Type;
    using DifferenceType    = ptrdiff_t;

    template<class Other>
    using Rebind        = Other*;

    static constexpr Pointer pointer_to(ElementType& val) noexcept {
        return &val;
    }
};  // END PointerTraits specialization
#pragma endregion Pointer


#pragma region Allocator
template<class Type>
class Allocator			// Allocator Template
{
public:
	static_assert(!IsConst_v<Type>, "The C++ Standard forbids containers of const elements ");
	static_assert(!IsFunction_v<Type>, "The C++ Standard forbids allocators for function elements ");
	static_assert(!IsReference_v<Type>, "The C++ Standard forbids allocators for reference elements ");

public:
	using ValueType     = Type;
    using DiffereceType = ptrdiff_t;

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
struct _HasConstructMemberFunction : FalseType {};

template<class Alloc, class Ty>
struct _HasConstructMemberFunction<Alloc, Ty,
Void_t<decltype(custom::declval<Alloc>().construct(custom::declval<Ty*>()))>> : TrueType {};

template<class Alloc, class Ty, class = void>
struct _HasDestroyMemberFunction : FalseType {};

template<class Alloc, class Ty>
struct _HasDestroyMemberFunction<Alloc, Ty,
Void_t<decltype(custom::declval<Alloc>().destroy(custom::declval<Ty*>()))>> : TrueType {};

// check max_size existence
template<class Alloc, class = void>
struct _HasMaxSizeMemberFunction : FalseType {};

template<class Alloc>
struct _HasMaxSizeMemberFunction<Alloc,
Void_t<decltype(custom::declval<Alloc>().max_size())>> : TrueType {};

// check select_on_container_copy_construction existence
template<class Alloc, class = void>
struct _HasSelectOnContainerCopyConstructionMemberFunction : FalseType {};

template<class Alloc>
struct _HasSelectOnContainerCopyConstructionMemberFunction<Alloc,
Void_t<decltype(custom::declval<Alloc>().select_on_container_copy_construction())>> : TrueType {};


template<class Alloc>
struct AllocatorTraits						// AllocatorTraits any
{
    using AllocatorType 						= Alloc;
    using ValueType								= typename Alloc::ValueType;
    using DifferenceType                        = typename _GetDifferenceType<Alloc>::Type;
    using Reference                             = typename _GetReferenceType<Alloc>::Type;
    using ConstReference                        = typename _GetConstReferenceType<Alloc>::Type;
    using Pointer								= typename _GetPointerType<Alloc>::Type;
    using ConstPointer							= typename _GetConstPointerType<Alloc>::Type;
    using VoidPointer 							= typename _GetVoidPointerType<Alloc>::Type;
    using ConstVoidPointer						= typename _GetConstVoidPointerType<Alloc>::Type;

    using PropagateOnContainerCopyAssignment	= typename _GetPropagateOnContainerCopy<Alloc>::Type;
    using PropagateOnContainerMoveAssignment	= typename _GetPropagateOnContainerMove<Alloc>::Type;
    using PropagateOnContainerSwap				= typename _GetPropagateOnContainerSwap<Alloc>::Type;
    using IsAlwaysEqual							= typename _GetIsAlwaysEqual<Alloc>::Type;

    template<class Other>
    using RebindAlloc 							= typename _GetRebindType<Alloc, Other>::Type;  // always uses _ReplaceFirstParameter

    template<class Other>
    using RebindTraits 							= AllocatorTraits<RebindAlloc<Other>>;

    static constexpr Pointer allocate(AllocatorType& al, const size_t capacity) {
        return al.allocate(capacity);
    }

    static constexpr void deallocate(AllocatorType& al, Pointer address, const size_t capacity) {
        al.deallocate(address, capacity);
    }

    template<class _Type, class... Args>
    static constexpr void construct(AllocatorType& al, _Type* const address, Args&&... args) {
        if constexpr (_HasConstructMemberFunction<AllocatorType, _Type>::Value)
            al.construct(address, custom::forward<Args>(args)...);
        else
            custom::construct_at(address, custom::forward<Args>(args)...);
    }

    template<class _Type>
    static constexpr void destroy(AllocatorType& al, _Type* const address) {
        if constexpr (_HasDestroyMemberFunction<AllocatorType, _Type>::Value)
            al.destroy(address);
        else
            custom::destroy_at(address);
    }

    static constexpr size_t max_size(const AllocatorType& al) noexcept {
        if constexpr (_HasMaxSizeMemberFunction<AllocatorType>::Value)
            return al.max_size();

        return static_cast<size_t>(-1) / sizeof(ValueType);
    }

    static constexpr AllocatorType select_on_container_copy_construction(const AllocatorType& al) {
        if constexpr (_HasSelectOnContainerCopyConstructionMemberFunction<AllocatorType>::Value)
            return al.select_on_container_copy_construction();

        return al;
    }
};	// END AllocatorTraits any


template<class Type>
struct AllocatorTraits<Allocator<Type>>		// AllocatorTraits default
{
	using AllocatorType 						= Allocator<Type>;
    using ValueType     						= Type;
    using DifferenceType                        = ptrdiff_t;

    using Reference                             = ValueType&;
    using ConstReference                        = const ValueType&;
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

	static constexpr Pointer allocate(AllocatorType& al, const size_t capacity) {
		return al.allocate(capacity);
	}

	static constexpr void deallocate(AllocatorType& al, Pointer address, const size_t capacity) {
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

    static constexpr AllocatorType select_on_container_copy_construction(const AllocatorType& al) {
        return al;
    }
};	// END AllocatorTraits default
#pragma endregion Allocator

CUSTOM_END