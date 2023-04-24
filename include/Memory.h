#pragma once
#include "Utility.h"


CUSTOM_BEGIN


template<class Ty, class DeleterNoref, class = void>
struct _GetDeleterPointerType { using Type = Ty*; };

template<class Ty, class DeleterNoref>
struct _GetDeleterPointerType<Ty, DeleterNoref, Void_t<typename DeleterNoref::Pointer>> { using Type = typename DeleterNoref::Pointer; };

template<class Deleter>
using _UniquePtrEnableDefault_t = EnableIf_t<Conjunction_v<Negation<IsPointer<Deleter>>, IsDefaultConstructible<Deleter>>, bool>;


template<class Type>
struct DefaultDelete                                        // default deleter for UniquePtr
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


template<class Type>
struct DefaultDelete<Type[]>                                // default deleter for unique_ptr to array of unknown size
{
    DefaultDelete() = default;

    template<class Type2, EnableIf_t<IsConvertible_v<Type2(*)[], Type(*)[]>, bool> = true>
    DefaultDelete(const DefaultDelete<Type2[]>&) { /*Empty*/ }

    template<class Type2, EnableIf_t<IsConvertible_v<Type2(*)[], Type(*)[]>, bool> = true>
    void operator()(Type2* ptr) const noexcept
    {
        static_assert(0 < sizeof(Type2), "can't delete an incomplete type");
        delete[] ptr;
    }
}; // END DefaultDelete[]


template<class Type, class Deleter = DefaultDelete<Type>>
class UniquePtr                                             // non-copyable pointer to an object
{
public:
    using ValueType     = Type;
    using DeleterType   = Deleter;
    using Pointer       = typename _GetDeleterPointerType<Type, RemoveReference_t<Deleter>>::Type;

private:
    Pointer _data;
    Deleter _deleter;

public:
    // Constructors

    template<class Deleter2 = Deleter, _UniquePtrEnableDefault_t<Deleter2> = true>
    UniquePtr() { /*Empty*/ }

    template<class Deleter2 = Deleter, _UniquePtrEnableDefault_t<Deleter2> = true>
    UniquePtr(std::nullptr_t) { /*Empty*/ }

    template<class Deleter2 = Deleter, _UniquePtrEnableDefault_t<Deleter2> = true>
    explicit UniquePtr(Pointer ptr)
        : _data(ptr) { /*Empty*/ }

    template<class Deleter2 = Deleter, EnableIf_t<IsConstructible_v<Deleter2, const Deleter2&>, bool> = true>
    UniquePtr(Pointer ptr, const Deleter& del)
        : _data(ptr), _deleter(del) { /*Empty*/ }

    template<class Deleter2 = Deleter, EnableIf_t<Conjunction_v<Negation<IsReference<Deleter2>>, IsConstructible<Deleter2, Deleter2>>, bool> = true>
    UniquePtr(Pointer ptr, Deleter&& del)
        : _data(ptr), _deleter(custom::move(del)) { /*Empty*/ }

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
        if (_data != nullptr)
            _deleter(_data);
    }

public:
    // Operators

    UniquePtr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    template<class Type2, class Deleter2,
    EnableIf_t<
    Conjunction_v<Negation<IsArray<Type2>>, IsAssignable<Deleter&, Deleter2>,
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


template<class Type, class Deleter>
class UniquePtr<Type[], Deleter>                            // non-copyable pointer to an array object
{
public:
    using ValueType     = Type;
    using DeleterType   = Deleter;
    using Pointer       = typename _GetDeleterPointerType<Type, RemoveReference_t<Deleter>>::Type;

    template<class Type2, class IsNullptr = IsSame<Type2, std::nullptr_t>>
    using _EnableConstructorReset = EnableIf_t<
                                    IsSame_v<Type2, Pointer> || 
                                    IsNullptr::Value || 
                                    (IsSame_v<Pointer, ValueType*> && IsPointer_v<Type2> && IsConvertible_v<RemovePointer_t<Type2>(*)[], ValueType(*)[]>),
                                    bool>;

    template<class Type2, class Del, class More, 
    class UpPointer         = typename UniquePtr<Type2, Del>::Pointer,
    class UpValueType       = typename UniquePtr<Type2, Del>::ValueType>
    using _EnableConversion =   EnableIf_t<
                                Conjunction_v<IsArray<Type2>, IsSame<Pointer, ValueType*>, IsSame<UpPointer, UpValueType*>,
                                IsConvertible<UpValueType(*)[], ValueType(*)[]>, More>,
                                bool>;

private:
    Pointer _data;
    Deleter _deleter;

public:
    // Constructors

    template<class Deleter2 = Deleter, _UniquePtrEnableDefault_t<Deleter2> = true>
    UniquePtr() { /*Empty*/ }

    template<class Type2, class Deleter2 = Deleter, _UniquePtrEnableDefault_t<Deleter2> = true, _EnableConstructorReset<Type2> = true>
    explicit UniquePtr(Type2 ptr)
        : _data(ptr) { /*Empty*/ }

    template<class Type2, class Deleter2 = Deleter, 
    EnableIf_t<IsConstructible_v<Deleter2, const Deleter2&>, bool> = true,
    _EnableConstructorReset<Type2> = true>
    UniquePtr(Type2 ptr, const Deleter& del)
        : _data(ptr), _deleter(del) { /*Empty*/ }

    // TODO: continue...

    ~UniquePtr() {
        if (_data != nullptr)
            _deleter(_data);
    }

public:
    // Operators

    UniquePtr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    // TODO: continue...

public:
    // Main functions

    // TODO: continue...

    template <class Type2, _EnableConstructorReset<Type2, FalseType> = true>
    void reset(Type2 ptr) noexcept {
        Pointer old = std::exchange(_data, ptr);

        if (old)
            _deleter(old);
    }

}; // END UniquePtr[]

CUSTOM_END