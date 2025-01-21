#pragma once
#include "c_type_traits.h"
#include "c_limits.h"
#include "c_pair.h"
#include "c_iterator.h"


CUSTOM_BEGIN

template<class>
struct pointer_traits;

#pragma region Helpers

CUSTOM_DETAIL_BEGIN

// get first parameter in template
template<class Ty>
struct _Get_First_Parameter;

template<template<class, class...> class Ty, class first, class... Rest>
struct _Get_First_Parameter<Ty<first, Rest...>>                 // given Ty<first, Rest...>, extract first
{
    using type = first;
};

// replace first parameter in template
template<class NewFirst, class Ty>
struct _Replace_First_Parameter;

template<class NewFirst, template<class, class...> class Ty, class first, class... Rest>
struct _Replace_First_Parameter<NewFirst, Ty<first, Rest...>>   // given Ty<first, Rest...>, replace first
{
    using type = Ty<NewFirst, Rest...>;
};

// get reference type
template<class Ty, class = void>
struct _Get_Reference_Type
{
    using type = typename Ty::value_type&;
};

template<class Ty>
struct _Get_Reference_Type<Ty, void_t<typename Ty::reference>>
{
    using type = typename Ty::reference;
};

// get const reference type
template<class Ty, class = void>
struct _Get_Const_Reference_Type
{
    using type = const typename Ty::value_type&;
};

template<class Ty>
struct _Get_Const_Reference_Type<Ty, void_t<typename Ty::const_reference>>
{
    using type = typename Ty::const_reference;
};

// get pointer type
template<class Ty, class = void>
struct _Get_Pointer_Type
{
    using type = typename Ty::value_type*;
};

template<class Ty>
struct _Get_Pointer_Type<Ty, void_t<typename Ty::pointer>>
{
    using type = typename Ty::pointer;
};

// get const pointer type
template<class Ty, class = void>
struct _Get_Const_Pointer_Type
{
    using type = typename pointer_traits<typename _Get_Pointer_Type<Ty>::type>::template rebind<const typename Ty::value_type>;
};

template<class Ty>
struct _Get_Const_Pointer_Type<Ty, void_t<typename Ty::const_pointer>>
{
    using type = typename Ty::const_pointer;
};

// get ptr difference type
template<class Ty, class = void>
struct _Get_Ptr_Difference_Type
{
    using type = ptrdiff_t;
};

template<class Ty>
struct _Get_Ptr_Difference_Type<Ty, void_t<typename Ty::difference_type>>
{
    using type = typename Ty::difference_type;
};

// get difference type
template<class Ty, class = void>
struct _Get_Difference_Type
{
    using type = typename pointer_traits<typename _Get_Pointer_Type<Ty>::type>::difference_type;
};

template<class Ty>
struct _Get_Difference_Type<Ty, void_t<typename Ty::difference_type>>
{
    using type = typename Ty::difference_type;
};

// get void pointer type
template<class Ty, class = void>
struct _Get_Void_Pointer_Type
{
    using type = typename pointer_traits<typename _Get_Pointer_Type<Ty>::type>::template rebind<void>;
};

template<class Ty>
struct _Get_Void_Pointer_Type<Ty, void_t<typename Ty::void_pointer>>
{
    using type = typename Ty::void_pointer;
};

// get const void pointer type
template<class Ty, class = void>
struct _Get_Const_Void_Pointer_Type
{
    using type = typename pointer_traits<typename _Get_Pointer_Type<Ty>::type>::template rebind<const void>;
};

template<class Ty>
struct _Get_Const_Void_Pointer_Type<Ty, void_t<typename Ty::const_void_pointer>>
{
    using type = typename Ty::const_void_pointer;
};

// get propagate on container copy
template<class Ty, class = void>
struct _Get_Propagate_On_Container_Copy
{
    using type = false_type;
};

template<class Ty>
struct _Get_Propagate_On_Container_Copy<Ty, void_t<typename Ty::propagate_on_container_copy_assignment>>
{
    using type = typename Ty::propagate_on_container_copy_assignment;
};

// get propagate on container move
template<class Ty, class = void>
struct _Get_Propagate_On_Container_Move
{
    using type = false_type;
};

template<class Ty>
struct _Get_Propagate_On_Container_Move<Ty, void_t<typename Ty::propagate_on_container_move_assignment>>
{
    using type = typename Ty::propagate_on_container_move_assignment;
};

// get propagate on container swap
template<class Ty, class = void>
struct _Get_Propagate_On_Container_Swap
{
    using type = false_type;
};

template<class Ty>
struct _Get_Propagate_On_Container_Swap<Ty, void_t<typename Ty::propagate_on_container_swap>>
{
    using type = typename Ty::propagate_on_container_swap;
};

// get is always equal
template<class Ty, class = void>
struct _Get_Is_Always_Equal
{
    using type = is_empty<Ty>;
};

template<class Ty>
struct _Get_Is_Always_Equal<Ty, void_t<typename Ty::is_always_equal>>
{
    using type = typename Ty::is_always_equal;
};

// get rebind type
template<class Ty, class Other, class = void>
struct _Get_Rebind_Type
{
    using type = typename _Replace_First_Parameter<Other, Ty>::type;
};

template<class Ty, class Other>
struct _Get_Rebind_Type<Ty, Other, void_t<typename Ty::template rebind<Other>>>
{
    using type = typename Ty::template rebind<Other>;
};

template<class Type, class Elem>
struct _Pointer_Traits_Base
{
    using pointer           = Type;
    using element_type      = Elem;
    using difference_type   = typename _Get_Ptr_Difference_Type<Type>::type;

    template<class Other>
    using rebind            = typename _Get_Rebind_Type<Type, Other>::type;

    static constexpr pointer pointer_to(element_type& val)
    noexcept(noexcept(element_type::pointer_to(val))) {
        return element_type::pointer_to(val);
    }
};  // END _Pointer_Traits_Base

template<class, class = void, class = void>
struct _Pointer_Traits_Elem_Choice {};

template<class Ty, class Dummy>
struct _Pointer_Traits_Elem_Choice<Ty, Dummy, void_t<typename _Get_First_Parameter<Ty>::type>>
: _Pointer_Traits_Base<Ty, typename _Get_First_Parameter<Ty>::type> {};

template<class Ty>
struct _Pointer_Traits_Elem_Choice<Ty, void_t<typename Ty::element_type>, void>
: _Pointer_Traits_Base<Ty, typename Ty::element_type> {};

// check construct/destruct existence
template<class Alloc, class Ty, class = void>
struct _Has_Construct_Member_Function : false_type {};

template<class Alloc, class Ty>
struct _Has_Construct_Member_Function<Alloc, Ty,
void_t<decltype(custom::declval<Alloc>().construct(custom::declval<Ty*>()))>> : true_type {};

template<class Alloc, class Ty, class = void>
struct _Has_Destroy_Member_Function : false_type {};

template<class Alloc, class Ty>
struct _Has_Destroy_Member_Function<Alloc, Ty,
void_t<decltype(custom::declval<Alloc>().destroy(custom::declval<Ty*>()))>> : true_type {};

// check max_size existence
template<class Alloc, class = void>
struct _Has_Max_Size_Member_Function : false_type {};

template<class Alloc>
struct _Has_Max_Size_Member_Function<Alloc,
void_t<decltype(custom::declval<Alloc>().max_size())>> : true_type {};

// check select_on_container_copy_construction existence
template<class Alloc, class = void>
struct _Has_Select_On_Container_Copy_Construction_Member_Function : false_type {};

template<class Alloc>
struct _Has_Select_On_Container_Copy_Construction_Member_Function<Alloc,
void_t<decltype(custom::declval<Alloc>().select_on_container_copy_construction())>> : true_type {};

CUSTOM_DETAIL_END

#pragma endregion Helpers


#pragma region Operations on uninitialized memory
// uninitialized_copy, uninitialized_copy_n
template<class InputIt, class NoThrowForwardIt>
NoThrowForwardIt uninitialized_copy(InputIt first, InputIt last, NoThrowForwardIt destFirst)
{
    using value_type = typename iterator_traits<NoThrowForwardIt>::value_type;

    detail::_verify_iteration_range(first, last);
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
NoThrowForwardIt uninitialized_copy_n(InputIt first, Size count, NoThrowForwardIt destFirst)
{
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
void uninitialized_fill(ForwardIt first, ForwardIt last, const Type& value)
{
    using value_type = typename iterator_traits<ForwardIt>::value_type;

    detail::_verify_iteration_range(first, last);
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
ForwardIt uninitialized_fill_n(ForwardIt first, Size count, const Type& value)
{
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
NoThrowForwardIt uninitialized_move(InputIt first, InputIt last, NoThrowForwardIt destFirst)
{
    using value_type = typename iterator_traits<NoThrowForwardIt>::value_type;

    detail::_verify_iteration_range(first, last);
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
custom::pair<InputIt, NoThrowForwardIt> uninitialized_move_n(InputIt first, Size count, NoThrowForwardIt destFirst)
{
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
void uninitialized_default_construct(ForwardIt first, ForwardIt last)
{
    using value_type = typename iterator_traits<ForwardIt>::value_type;
    
    detail::_verify_iteration_range(first, last);
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
ForwardIt uninitialized_default_construct_n(ForwardIt first, Size n)
{
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
void uninitialized_value_construct(ForwardIt first, ForwardIt last)
{
    using value_type = typename iterator_traits<ForwardIt>::value_type;

    detail::_verify_iteration_range(first, last);
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
ForwardIt uninitialized_value_construct_n(ForwardIt first, Size n)
{
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
constexpr Type* construct_at(Type* const address, Args&&... args)
{
	return ::new(address) Type(custom::forward<Args>(args)...);
}
// END construct_at


// destroy, destroy_at, destroy_n
template<class Type>
constexpr void destroy_at(Type* const address)
{
    if constexpr (is_array_v<Type>)
        for (auto &elem : *address)
            destroy_at(&(elem));
    else
        address->~Type();
}

template<class ForwardIt>
constexpr void destroy(ForwardIt first, ForwardIt last)
{
    detail::_verify_iteration_range(first, last);

    for (/*Empty*/; first != last; ++first)
        custom::destroy_at(&(*first));
}

template<class ForwardIt, class Size>
constexpr ForwardIt destroy_n(ForwardIt first, Size n)
{
    for (/*Empty*/; n > 0; (void)++first, --n)
        custom::destroy_at(&(*first));

    return first;
}
// END destroy, destroy_at, destroy_n
#pragma endregion Operations on uninitialized memory


#pragma region Pointer
template<class Ty>
struct pointer_traits : detail::_Pointer_Traits_Elem_Choice<Ty> {};

template<class Type>
struct pointer_traits<Type*>
{
    using pointer           = Type*;
    using element_type      = Type;
    using difference_type   = ptrdiff_t;

    template<class Other>
    using rebind            = Other*;

    static constexpr pointer pointer_to(element_type& val) noexcept
    {
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

	Type* allocate(const size_t capacity)
    {
		static_assert(sizeof(Type) > 0, "Type must be complete before calling allocate");
		return static_cast<Type*>(::operator new(capacity * sizeof(Type)));
	}

	void deallocate(Type* address, const size_t capacity)
    {
		CUSTOM_ASSERT(address != nullptr || capacity > 0, "Invalid block deallocation");
		::operator delete(address, capacity * sizeof(Type));
	}
}; // END Allocator

template<class Alloc>
struct allocator_traits						// allocator_traits any
{
    using allocator_type        = Alloc;
    using value_type            = typename Alloc::value_type;
    using difference_type       = typename detail::_Get_Difference_Type<Alloc>::type;
    using reference             = typename detail::_Get_Reference_Type<Alloc>::type;
    using const_reference       = typename detail::_Get_Const_Reference_Type<Alloc>::type;
    using pointer               = typename detail::_Get_Pointer_Type<Alloc>::type;
    using const_pointer         = typename detail::_Get_Const_Pointer_Type<Alloc>::type;
    using void_pointer          = typename detail::_Get_Void_Pointer_Type<Alloc>::type;
    using const_void_pointer    = typename detail::_Get_Const_Void_Pointer_Type<Alloc>::type;

    using propagate_on_container_copy_assignment    = typename detail::_Get_Propagate_On_Container_Copy<Alloc>::type;
    using propagate_on_container_move_assignment    = typename detail::_Get_Propagate_On_Container_Move<Alloc>::type;
    using propagate_on_container_swap               = typename detail::_Get_Propagate_On_Container_Swap<Alloc>::type;
    using is_always_equal                           = typename detail::_Get_Is_Always_Equal<Alloc>::type;

    template<class Other>
    using rebind_alloc  = typename detail::_Get_Rebind_Type<Alloc, Other>::type;  // always uses _Replace_First_Parameter

    template<class Other>
    using rebind_traits = allocator_traits<rebind_alloc<Other>>;

    static constexpr pointer allocate(allocator_type& al, const size_t capacity)
    {
        return al.allocate(capacity);
    }

    static constexpr void deallocate(allocator_type& al, pointer address, const size_t capacity)
    {
        al.deallocate(address, capacity);
    }

    template<class _Type, class... Args>
    static constexpr void construct(allocator_type& al, _Type* const address, Args&&... args)
    {
        if constexpr (detail::_Has_Construct_Member_Function<allocator_type, _Type>::value)
            al.construct(address, custom::forward<Args>(args)...);
        else
            custom::construct_at(address, custom::forward<Args>(args)...);
    }

    template<class _Type>
    static constexpr void destroy(allocator_type& al, _Type* const address)
    {
        if constexpr (detail::_Has_Destroy_Member_Function<allocator_type, _Type>::value)
            al.destroy(address);
        else
            custom::destroy_at(address);
    }

    static constexpr size_t max_size(const allocator_type& al) noexcept
    {
        if constexpr (detail::_Has_Max_Size_Member_Function<allocator_type>::value)
            return al.max_size();

        return static_cast<size_t>(-1) / sizeof(value_type);
    }

    static constexpr allocator_type select_on_container_copy_construction(const allocator_type& al)
    {
        if constexpr (detail::_Has_Select_On_Container_Copy_Construction_Member_Function<allocator_type>::value)
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

	static constexpr pointer allocate(allocator_type& al, const size_t capacity)
    {
		return al.allocate(capacity);
	}

	static constexpr void deallocate(allocator_type& al, pointer address, const size_t capacity)
    {
		al.deallocate(address, capacity);
	}

    template<class _Type, class... Args>
    static constexpr void construct(allocator_type&, _Type* const address, Args&&... args)
    {
        custom::construct_at(address, custom::forward<Args>(args)...);
    }

    template<class _Type>
    static constexpr void destroy(allocator_type&, _Type* const address)
    {
        custom::destroy_at(address);
    }

    static constexpr size_t max_size(const allocator_type&) noexcept
    {
        return static_cast<size_t>(-1) / sizeof(value_type);
    }

    static constexpr allocator_type select_on_container_copy_construction(const allocator_type& al)
    {
        return al;
    }
};	// END allocator_traits default
#pragma endregion Allocator


#pragma region Deleters

template<class Type>
struct default_delete
{
    constexpr default_delete() noexcept = default;

    template<class Type2, enable_if_t<is_convertible_v<Type2*, Type*>, bool> = true>
    constexpr default_delete(const default_delete<Type2>&) noexcept { /*Empty*/ }

    constexpr void operator()(Type* ptr) const noexcept
    {
        static_assert(0 < sizeof(Type), "can't delete an incomplete type");
        delete ptr;
    }
}; // END default_delete

template<class Type>
struct default_delete<Type[]>
{
    constexpr default_delete() noexcept = default;

    template<class Type2, enable_if_t<is_convertible_v<Type2(*)[], Type(*)[]>, bool> = true>
    constexpr default_delete(const default_delete<Type2[]>&) noexcept { /*Empty*/ }

    template<class Type2, enable_if_t<is_convertible_v<Type2(*)[], Type(*)[]>, bool> = true>
    constexpr void operator()(Type2* ptr) const noexcept
    {
        static_assert(0 < sizeof(Type2), "can't delete an incomplete type");
        delete[] ptr;
    }
}; // END default_delete[]

#pragma endregion Deleters

CUSTOM_END