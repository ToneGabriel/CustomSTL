#pragma once
#include "Utility.h"


CUSTOM_BEGIN


template<class Type>
struct DefaultDelete        // default deleter for UniquePtr
{
    DefaultDelete() = default;

    template<class Type2, EnableIf_t<IsConvertible_v<Type2*, Type*>, bool> = true>
    DefaultDelete(const DefaultDelete<Type2>&) { /*Empty*/ }

    void operator()(Type* ptr) const noexcept
    {
        static_assert(0 < sizeof(Type), "can't delete an incomplete type");
        delete ptr;
    }
}; // END DefaultDelete


template<class _Ty>
struct DefaultDelete<_Ty[]>     // default deleter for unique_ptr to array of unknown size
{
    DefaultDelete() = default;

    template<class _Uty, custom::EnableIf_t<custom::IsConvertible_v<_Uty(*)[], _Ty(*)[]>, bool> = true>
    DefaultDelete(const DefaultDelete<_Uty[]>&) { /*Empty*/ }

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

template<class Deleter>
using UniquePtrEnableDefault_t = EnableIf_t<Conjunction_v<Negation<IsPointer<Deleter>>, IsDefaultConstructible<Deleter>>, bool>;

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
    template<class Deleter2 = Deleter, UniquePtrEnableDefault_t<Deleter2> = true>
    UniquePtr() { /*Empty*/ }

    template<class Deleter2 = Deleter, UniquePtrEnableDefault_t<Deleter2> = true>
    UniquePtr(std::nullptr_t) { /*Empty*/ }

    template<class Deleter2 = Deleter, UniquePtrEnableDefault_t<Deleter2> = true>
    explicit UniquePtr(Pointer ptr)
        : _data(ptr) { /*Empty*/ }

    template<class Deleter2 = Deleter, EnableIf_t<IsConstructible_v<Deleter2, const Deleter2&>, bool> = true>
    UniquePtr(Pointer ptr, const Deleter& dlt)
        : _data(ptr), _deleter(dlt) { /*Empty*/ }

    template<class Deleter2 = Deleter, EnableIf_t<Conjunction_v<Negation<IsReference<Deleter2>>, IsConstructible<Deleter2, Deleter2>>, bool> = true>
    UniquePtr(Pointer ptr, Deleter&& dlt)
        : _data(ptr), _deleter(custom::move(dlt)) { /*Empty*/ }

    template<class Deleter2 = Deleter,
    EnableIf_t<Conjunction_v<IsReference<Deleter2>, IsConstructible<Deleter2, RemoveReference_t<Deleter2>>>, bool> = true>
    UniquePtr(Pointer, RemoveReference_t<Deleter>&&) = delete;

    template<class Deleter2 = Deleter, EnableIf_t<IsMoveConstructible_v<Deleter2>, bool> = true>
    UniquePtr(UniquePtr&& other) noexcept
        : _data(other.release()), _deleter(custom::forward<Deleter>(other.get_deleter())) { /*Empty*/ }

    template<class Type2, class Deleter2,
    EnableIf_t<
    Conjunction_v<Negation<IsArray<Type2>>, IsConvertible<typename UniquePtr<Type2, Deleter2>::Pointer, Pointer>,
    Conditional_t<IsReference_v<Deleter>, IsSame<Deleter2, Deleter>, IsConvertible<Deleter2, Deleter>>>,
    bool> = true>
    UniquePtr(UniquePtr<Type2, Deleter2>&& other) noexcept
        : _data(other.release()), _deleter(custom::forward<Deleter2>(other.get_deleter())) { /*Empty*/ }

    UniquePtr(const UniquePtr&) = delete;

    ~UniquePtr() {
        if (_data)
            _deleter(_data);
    }

public:
    // Operators
    template<class Type2, class Deleter2,
    EnableIf_t<Conjunction_v<Negation<IsArray<Type2>>, IsAssignable<Deleter&, Deleter2>,
    IsConvertible<typename UniquePtr<Type2, Deleter2>::Pointer, Pointer>>,
    bool> = true>
    UniquePtr& operator=(UniquePtr<Type2, Deleter2>&& other) noexcept {
        reset(other.release());
        _deleter = custom::forward<Deleter2>(other._deleter);

        return *this;
    }

    template<class Deleter2 = Deleter, EnableIf_t<IsMoveAssignable_v<Deleter2>, bool> = true>
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (_data != other._data) {
            reset(other.release());
            _deleter = custom::forward<Deleter>(other._deleter);
        }

        return *this;
    }

    UniquePtr& operator=(const UniquePtr&)  = delete;

public:
    // Main functions

    Deleter& get_deleter() noexcept {
        return _deleter;
    }

    const Deleter& get_deleter() const noexcept {
        return _deleter;
    }

    AddLvalueReference_t<Type> operator*() const noexcept {
       return *_data;
    }

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