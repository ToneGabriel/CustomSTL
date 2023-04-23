#pragma once
#include "Utility.h"


CUSTOM_BEGIN


template<class Type>
struct DefaultDelete        // default deleter for UniquePtr
{
    constexpr DefaultDelete() noexcept = default;

    template<class Type, custom::EnableIf_t<custom::IsConvertible_v<Type*, Type*>, bool> = true>
    DefaultDelete(const DefaultDelete<Type>&) noexcept { /*Empty*/ }

    void operator()(Type* ptr) const noexcept
    {
        static_assert(0 < sizeof(Type), "can't delete an incomplete type");
        delete ptr;
    }
}; // END DefaultDelete


template<class _Ty>
struct DefaultDelete<_Ty[]>     // default deleter for unique_ptr to array of unknown size
{
    constexpr DefaultDelete() noexcept = default;

    template<class _Uty, custom::EnableIf_t<custom::IsConvertible_v<_Uty(*)[], _Ty(*)[]>, bool> = true>
    DefaultDelete(const DefaultDelete<_Uty[]>&) noexcept { /*Empty*/ }

    template<class _Uty, custom::EnableIf_t<custom::IsConvertible_v<_Uty(*)[], _Ty(*)[]>, bool> = true>
    void operator()(_Uty* _Ptr) const noexcept
    {
        static_assert(0 < sizeof(_Uty), "can't delete an incomplete type");
        delete[] _Ptr;
    }
}; // END DefaultDelete[]


//template<class Type, class DeleteNoref, class = void>      // provide fallback
//struct _Get_deleter_pointer_type { using Type = Type*; };
//
//template<class _Ty, class _Dx_noref>                       // get _Dx_noref::pointer
//struct _Get_deleter_pointer_type<_Ty, _Dx_noref, Void_t<typename _Dx_noref::pointer>> { using type = typename _Dx_noref::pointer; };

template<class Type, class Deleter = DefaultDelete<Type>>
class UniquePtr                                             // non-copyable pointer to an object
{
public:
    using ValueType     = Type;
    using DeleterType   = Deleter;
    using Pointer       = ValueType*;

private:
    Pointer _data;
    Deleter _deleter;

public:
    // Constructors

    UniquePtr() = default;

    ~UniquePtr() {
        if (_data)
            _deleter(_data);
    }

public:
    // Operators

    UniquePtr(const UniquePtr&)             = delete;
    UniquePtr& operator=(const UniquePtr&)  = delete;

public:
    // Main functions

    Deleter& get_deleter() noexcept {
        return _deleter;
    }

    const Deleter& get_deleter() const noexcept {
        return _deleter;
    }

    //add_lvalue_reference_t<_Ty> operator*() const noexcept(noexcept(*_STD declval<pointer>())) {
    //    return *_Mypair._Myval2;
    //}

    Pointer operator->() const noexcept {
        return _data;
    }

    Pointer get() const noexcept {
        return _data;
    }

    explicit operator bool() const noexcept {
        return _data != nullptr;
    }

    Pointer release() noexcept {
        return std::exchange(_data, nullptr);
    }

    void reset(Pointer ptr = nullptr) noexcept {
        Pointer old = std::exchange(_data, ptr);

        if (old)
            _deleter(old);
    }
}; // END UniquePtr


template<class Ty, class Deleter>
class UniquePtr<Ty[], Deleter>      // non-copyable pointer to an array object
{

}; // END UniquePtr[]

CUSTOM_END