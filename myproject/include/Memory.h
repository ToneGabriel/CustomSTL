#pragma once
#include "Utility.h"
#include "xMemory.h"

#include <atomic>
#include <typeinfo>

CUSTOM_BEGIN

#pragma region Deleters

template<class Type>
struct DefaultDelete                                        // default deleter for UniquePtr
{
    constexpr DefaultDelete() noexcept = default;

    template<class Type2, EnableIf_t<IsConvertible_v<Type2*, Type*>, bool> = true>
    constexpr DefaultDelete(const DefaultDelete<Type2>&) noexcept { /*Empty*/ }

    constexpr void operator()(Type* ptr) const noexcept
    {
        static_assert(0 < sizeof(Type), "can't delete an incomplete type");
        delete ptr;
    }
}; // END DefaultDelete

template<class Type>
struct DefaultDelete<Type[]>                                // default deleter for UniquePtr to array of unknown size
{
    constexpr DefaultDelete() noexcept = default;

    template<class Type2, EnableIf_t<IsConvertible_v<Type2(*)[], Type(*)[]>, bool> = true>
    constexpr DefaultDelete(const DefaultDelete<Type2[]>&) noexcept { /*Empty*/ }

    template<class Type2, EnableIf_t<IsConvertible_v<Type2(*)[], Type(*)[]>, bool> = true>
    constexpr void operator()(Type2* ptr) const noexcept
    {
        static_assert(0 < sizeof(Type2), "can't delete an incomplete type");
        delete[] ptr;
    }
}; // END DefaultDelete[]

#pragma endregion Deleters


#pragma region UniquePtr

// Helpers
template<class Ty, class DeleterNoref, class = void>
struct _GetDeleterPointerType { using Type = Ty*; };

template<class Ty, class DeleterNoref>
struct _GetDeleterPointerType<Ty, DeleterNoref, Void_t<typename DeleterNoref::Pointer>>
{
    // the "custom" deleter must have public Pointer alias (it's accessed here)
    using Type = typename DeleterNoref::Pointer;
};

template<class Deleter>
using _UniquePtrEnableDefault_t = EnableIf_t<Conjunction_v<
                                                Negation<IsPointer<Deleter>>,
                                                IsDefaultConstructible<Deleter>>, bool>;


template<class Type, class Deleter = DefaultDelete<Type>>
class UniquePtr                                             // non-copyable pointer to an object
{
public:
    using ElementType   = Type;
    using DeleterType   = Deleter;
    using Pointer       = typename _GetDeleterPointerType<Type, RemoveReference_t<Deleter>>::Type;

private:
    Pointer _ptr        = nullptr;
    Deleter _deleter;

public:
    // Constructors

    template<class Del = Deleter, _UniquePtrEnableDefault_t<Del> = true>
    constexpr UniquePtr() noexcept { /*Empty*/ }

    template<class Del = Deleter, _UniquePtrEnableDefault_t<Del> = true>
    constexpr UniquePtr(std::nullptr_t) noexcept { /*Empty*/ }

    template<class Del = Deleter, _UniquePtrEnableDefault_t<Del> = true>
    constexpr explicit UniquePtr(Pointer ptr) noexcept
        : _ptr(ptr) { /*Empty*/ }

    template<class Del = Deleter,
    EnableIf_t<IsConstructible_v<Del, const Del&>, bool> = true>
    constexpr UniquePtr(Pointer ptr, const Deleter& del) noexcept
        : _ptr(ptr), _deleter(del) { /*Empty*/ }

    template<class Del = Deleter,
    EnableIf_t<Conjunction_v<
                    Negation<IsReference<Del>>,
                    IsConstructible<Del, Del>>, bool> = true>
    constexpr UniquePtr(Pointer ptr, Deleter&& del) noexcept
        : _ptr(ptr), _deleter(custom::move(del)) { /*Empty*/ }

    template<class Del = Deleter,
    EnableIf_t<Conjunction_v<
                    IsReference<Del>,
                    IsConstructible<Del, RemoveReference_t<Del>>>, bool> = true>
    UniquePtr(Pointer, RemoveReference_t<Deleter>&&) = delete;

    template<class Del = Deleter,
    EnableIf_t<IsMoveConstructible_v<Del>, bool> = true>
    constexpr UniquePtr(UniquePtr&& other) noexcept
        : _ptr(other.release()), _deleter(custom::forward<Deleter>(other.get_deleter())) { /*Empty*/ }

    template<class Ty, class Del,
    EnableIf_t<Conjunction_v<
                    Negation<IsArray<Ty>>,
                    IsConvertible<typename UniquePtr<Ty, Del>::Pointer, Pointer>,
                    Conditional_t<IsReference_v<Deleter>, IsSame<Del, Deleter>, IsConvertible<Del, Deleter>>>,
    bool> = true>
    constexpr UniquePtr(UniquePtr<Ty, Del>&& other) noexcept
        : _ptr(other.release()), _deleter(custom::forward<Del>(other.get_deleter())) { /*Empty*/ }

    UniquePtr(const UniquePtr&) = delete;

    constexpr ~UniquePtr() noexcept {
        if (_ptr != nullptr)
            _deleter(_ptr);
    }

public:
    // Operators

    constexpr UniquePtr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    template<class Ty, class Del,
    EnableIf_t<Conjunction_v<
                    Negation<IsArray<Ty>>,
                    IsAssignable<Deleter&, Del>,
                    IsConvertible<typename UniquePtr<Ty, Del>::Pointer, Pointer>>,
    bool> = true>
    constexpr UniquePtr& operator=(UniquePtr<Ty, Del>&& other) noexcept {
        reset(other.release());
        _deleter = custom::forward<Del>(other._deleter);

        return *this;
    }

    template<class Del = Deleter,
    EnableIf_t<IsMoveAssignable_v<Del>, bool> = true>
    constexpr UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (_ptr != other._ptr)
        {
            reset(other.release());
            _deleter = custom::forward<Deleter>(other._deleter);
        }

        return *this;
    }

    UniquePtr& operator=(const UniquePtr&) = delete;

    constexpr AddLvalueReference_t<Type> operator*() const noexcept(noexcept(*custom::declval<Pointer>())) {
       return *_ptr;
    }

    constexpr Pointer operator->() const noexcept {
        return _ptr;
    }

    constexpr explicit operator bool() const noexcept {
        return _ptr != nullptr;
    }

public:
    // Main functions

    constexpr Deleter& get_deleter() noexcept {
        return _deleter;
    }

    constexpr const Deleter& get_deleter() const noexcept {
        return _deleter;
    }

    constexpr Pointer get() const noexcept {
        return _ptr;
    }

    constexpr Pointer release() noexcept {                            // Releases the ownership of the managed object, get() returns nullptr after this
        return custom::exchange(_ptr, nullptr);
    }

    constexpr void reset(Pointer ptr = nullptr) noexcept {            // Replaces the managed object and deletes old
        Pointer old = custom::exchange(_ptr, ptr);

        if (old)
            _deleter(old);
    }
}; // END UniquePtr


template<class Type, class Deleter>
class UniquePtr<Type[], Deleter>                            // non-copyable pointer to an array object
{
public:
    using ElementType   = Type;
    using DeleterType   = Deleter;
    using Pointer       = typename _GetDeleterPointerType<Type, RemoveReference_t<Deleter>>::Type;

    template<class Ty, class IsNullptr = IsSame<Ty, std::nullptr_t>>
    using _EnableConstructorReset = EnableIf_t<Disjunction_v<
                                                    IsSame<Ty, Pointer>,
                                                    IsNullptr,
                                                    Conjunction<
                                                        IsSame<Pointer, ElementType*>,
                                                        IsPointer<Ty>,
                                                        IsConvertible<RemovePointer_t<Ty>(*)[], ElementType(*)[]>>>,
                                    bool>;

    template<class Ty, class Del, class More,
    class UpPointer         = typename UniquePtr<Ty, Del>::Pointer,
    class UpValueType       = typename UniquePtr<Ty, Del>::ElementType>
    using _EnableConversion =   EnableIf_t<Conjunction_v<
                                                IsArray<Ty>,
                                                IsSame<Pointer, ElementType*>,
                                                IsSame<UpPointer, UpValueType*>,
                                                IsConvertible<UpValueType(*)[], ElementType(*)[]>,
                                                More>,  // More is used for additional enable traits
                                bool>;

private:
    Pointer _ptr;
    Deleter _deleter;

public:
    // Constructors

    template<class Del = Deleter,
    _UniquePtrEnableDefault_t<Del> = true>
    constexpr UniquePtr() noexcept { /*Empty*/ }

    template<class Ty, class Del = Deleter,
    _UniquePtrEnableDefault_t<Del> = true,
    _EnableConstructorReset<Ty> = true>
    constexpr explicit UniquePtr(Ty ptr) noexcept
        : _ptr(ptr) { /*Empty*/ }

    template<class Ty, class Del = Deleter, 
    EnableIf_t<IsCopyConstructible_v<Del>, bool> = true,
    _EnableConstructorReset<Ty> = true>
    constexpr UniquePtr(Ty ptr, const Deleter& del) noexcept
        : _ptr(ptr), _deleter(del) { /*Empty*/ }

    template<class Ty, class Del = Deleter,
    EnableIf_t<Conjunction_v<
                    Negation<IsReference<Del>>,
                    IsMoveConstructible<Del>>, bool> = true,
    _EnableConstructorReset<Ty> = true>
    constexpr UniquePtr(Ty ptr, Deleter&& del) noexcept
        : _ptr(ptr), _deleter(custom::move(del)) { /*Empty*/ }

    template<class Ty, class Del = Deleter,
    EnableIf_t<Conjunction_v<
                    IsReference<Del>,
                    IsConstructible<Del, RemoveReference_t<Del>>>, bool> = true>
    UniquePtr(Ty, RemoveReference_t<Deleter>&&) = delete;

    template<class Del = Deleter,
    EnableIf_t<IsMoveConstructible_v<Del>, bool> = true>
    constexpr UniquePtr(UniquePtr&& other) noexcept
        : _ptr(other.release()), _deleter(custom::forward<Deleter>(other.get_deleter())) { /*Empty*/ }

    UniquePtr(const UniquePtr&) = delete;

    constexpr ~UniquePtr() noexcept {
        if (_ptr != nullptr)
            _deleter(_ptr);
    }

public:
    // Operators

    constexpr UniquePtr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    template<class Ty, class Del,
    _EnableConversion<Ty, Del, IsAssignable<Deleter&, Del>> = true>
    constexpr UniquePtr& operator=(UniquePtr<Ty, Del>&& other) noexcept {
        reset(other.release());
        _deleter = custom::forward<Del>(other._deleter);

        return *this;
    }

    template<class Del = Deleter,
    EnableIf_t<IsMoveAssignable_v<Del>, bool> = true>
    constexpr UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (_ptr != other._ptr)
        {
            reset(other.release());
            _deleter = custom::move(other._deleter);
        }

        return *this;
    }

    UniquePtr& operator=(const UniquePtr&) = delete;

    constexpr Type& operator[](size_t index) const noexcept {
        return _ptr[index];
    }

    constexpr explicit operator bool() const noexcept {
        return _ptr != nullptr;
    }

public:
    // Main functions

    constexpr Deleter& get_deleter() noexcept {
        return _deleter;
    }

    constexpr const Deleter& get_deleter() const noexcept {
        return _deleter;
    }

    constexpr Pointer get() const noexcept {
        return _ptr;
    }

    constexpr Pointer release() noexcept {                                           // Releases the ownership of the managed object, get() returns nullptr after this
        return custom::exchange(_ptr, nullptr);
    }

    template<class Ty, _EnableConstructorReset<Ty, FalseType> = true>               // FalseType (don't allow nullptr ???)
    constexpr void reset(Ty ptr) noexcept {                                         // Replaces the managed object and deletes old
        Pointer old = custom::exchange(_ptr, ptr);

        if (old)
            _deleter(old);
    }

}; // END UniquePtr[]


// build UniquePtr
template<class Ty, class... Args, EnableIf_t<!IsArray_v<Ty>, bool> = true>
constexpr UniquePtr<Ty> make_unique(Args&&... args) {
    return UniquePtr<Ty>(new Ty(custom::forward<Args>(args)...));
}

template<class Ty, EnableIf_t<IsArray_v<Ty> && Extent_v<Ty> == 0, bool> = true>
constexpr UniquePtr<Ty> make_unique(const size_t size) {
    return UniquePtr<Ty>(new RemoveExtent_t<Ty>[size]());
}

template<class Ty, class... Args, EnableIf_t<Extent_v<Ty> != 0, bool> = true>
void make_unique(Args&&...) = delete;

template<class Ty, EnableIf_t<!IsArray_v<Ty>, bool> = true>
constexpr UniquePtr<Ty> make_unique_for_overwrite() {      // make a UniquePtr with default initialization
    return UniquePtr<Ty>(new Ty);
}

template<class Ty, EnableIf_t<IsUnboundedArray_v<Ty>, bool> = true>
constexpr UniquePtr<Ty> make_unique_for_overwrite(const size_t _Size) {    // make a UniquePtr[] with default initialization
    return UniquePtr<Ty>(new RemoveExtent_t<Ty>[_Size]);
}

template<class Ty, class... Unused, EnableIf_t<IsUnboundedArray_v<Ty>, bool> = true>
constexpr void make_unique_for_overwrite(Unused&&...) = delete;

// UniquePtr binary operators
template<class Ty1, class Del1, class Ty2, class Del2>
constexpr bool operator==(const UniquePtr<Ty1, Del1>& left, const UniquePtr<Ty2, Del2>& right) {
    return left.get() == right.get();
}

template<class Ty1, class Del1, class Ty2, class Del2>
constexpr bool operator!=(const UniquePtr<Ty1, Del1>& left, const UniquePtr<Ty2, Del2>& right) {
    return !(left == right);
}
#pragma endregion UniquePtr


#pragma region SharedPtr
template<class Type>
class SharedPtr;

template<class Type>
class WeakPtr;

// Shared/Weak Ptr helpers
template<class Ty, class = void>
struct _CanScalarDelete : FalseType {};

template<class Ty>
struct _CanScalarDelete<Ty, Void_t<decltype(delete custom::declval<Ty*>())>> : TrueType {};

template<class Ty, class = void>
struct _CanArrayDelete : FalseType {};

template<class Ty>
struct _CanArrayDelete<Ty, Void_t<decltype(delete[] custom::declval<Ty*>())>> : TrueType {};

template<class Func, class Arg, class = void>
struct _CanCallFunctionObject : FalseType {};

template<class Func, class Arg>
struct _CanCallFunctionObject<Func, Arg, Void_t<decltype(custom::declval<Func>()(custom::declval<Arg>()))>> : TrueType {};

template<class Ty1, class Ty2>
struct _SharedPtrConvertible : IsConvertible<Ty1*, Ty2*>::Type {};

template<class Ty1, class Ty2>
struct _SharedPtrConvertible<Ty1, Ty2[]> : IsConvertible<Ty1(*)[], Ty2(*)[]>::Type {};

template<class Ty1, class Ty2, size_t Nx>
struct _SharedPtrConvertible<Ty1, Ty2[Nx]> : IsConvertible<Ty1(*)[Nx], Ty2(*)[Nx]>::Type {};

// a pointer type Ty1* is said to be compatible with a pointer type Ty2*
// when either Ty1* is convertible to Ty2*
// or Ty1 is U[N] and Ty2 is cv U[].
template<class Ty1, class Ty2>
struct _SharedPtrCompatible : IsConvertible<Ty1*, Ty2*>::Type {};

template<class Ty, size_t Nx>
struct _SharedPtrCompatible<Ty[Nx], Ty[]> : TrueType {};

template<class Ty, size_t Nx>
struct _SharedPtrCompatible<Ty[Nx], const Ty[]> : TrueType {};

template<class Ty, size_t Nx>
struct _SharedPtrCompatible<Ty[Nx], volatile Ty[]> : TrueType {};

template<class Ty, size_t Nx>
struct _SharedPtrCompatible<Ty[Nx], const volatile Ty[]> : TrueType {};

template<class Ty, class = void>
struct _CanEnableShared : FalseType {}; // detect unambiguous and accessible inheritance from EnableSharedFromThis

template<class Ty>
struct _CanEnableShared<Ty, Void_t<typename Ty::Esft_t>>
    : IsConvertible<RemoveCV_t<Ty>*, typename Ty::Esft_t*>::Type {};


// Temporary owner helpers
template<class Type>
struct _TemporaryOwner
{
    Type* _Ptr;

    explicit _TemporaryOwner(Type* const ptr) noexcept 
        : _Ptr(ptr) { /*Empty*/ }

    _TemporaryOwner(const _TemporaryOwner&)             = delete;
    _TemporaryOwner& operator=(const _TemporaryOwner&)  = delete;

    ~_TemporaryOwner() {
        delete _Ptr;
    }
};  // END _TemporaryOwner

template<class TypePtr, class Deleter>  // TODO: why _UxptrOrNullptr and not just Type?
struct _TemporaryOwnerDel
{
    TypePtr _Ptr;
    Deleter& _Deleter;
    bool _CallDeleter = true;   // modified by user when needed

    explicit _TemporaryOwnerDel(const TypePtr ptr, Deleter& deleter) noexcept 
        : _Ptr(ptr), _Deleter(deleter) { /*Empty*/ }

    _TemporaryOwnerDel(const _TemporaryOwnerDel&)               = delete;
    _TemporaryOwnerDel& operator=(const _TemporaryOwnerDel&)    = delete;

    ~_TemporaryOwnerDel() {
        if (_CallDeleter) {
            _Deleter(_Ptr);
        }
    }
};  // END _TemporaryOwnerDel


class USE_NOVTABLE_ATTR _RefCountBase     // Helper base class for ref counting
{
private:
    std::atomic<long> _uses;
    std::atomic<long> _weaks;

protected:
    _RefCountBase() noexcept = default;    // non-atomic initializations

public:
    // Constructors & Operators
    
    _RefCountBase(const _RefCountBase&)               = delete;
    _RefCountBase& operator=(const _RefCountBase&)    = delete;

    virtual ~_RefCountBase() { /*Empty*/ }

public:
    // Main functions

    void incref() noexcept {           // increment use count
        ++_uses;
    }

    void incwref() noexcept {          // increment weak reference count
        ++_weaks;
    }

    void decref() noexcept {           // decrement use count
        if (--_uses == 0)
        {
            _destroy();
            decwref();
        }
    }

    void decwref() noexcept {          // decrement weak reference count
        if (--_weaks == 0)
            _delete_this();
    }

    long use_count() const noexcept {
        return static_cast<long>(_uses);
    }

    virtual void* get_deleter(const std::type_info& ti) const noexcept {    // TODO: wtf is this?
        return nullptr;
    }

private:
    // Helpers

    virtual void _destroy() noexcept {
        std::terminate();
    }

    virtual void _delete_this() noexcept {
        std::terminate();
    }
}; // END _RefCountBase

template<class Type>
class _RefCount : public _RefCountBase    // handle reference counting for pointer without deleter
{
private:
    Type* _ptr;

public:

    explicit _RefCount(Type* ptr)
        : _RefCountBase(), _ptr(ptr) { /*Empty*/ }

private:

    void _destroy() noexcept override {     // destroy managed resource
        delete _ptr;
    }

    void _delete_this() noexcept override { // destroy self
        delete this;
    }
}; // END _RefCount

template<class Type, class Deleter>
class _RefCountDeleter : public _RefCountBase     // handle reference counting for object with deleter
{
private:
    Type* _ptr;
    Deleter _deleter;

public:

    _RefCountDeleter(Type* ptr, Deleter& dt)
        : _RefCountBase(), _ptr(ptr), _deleter(dt) { /*Empty*/ }

private:
    void _destroy() noexcept override { // destroy managed resource
        _deleter(_ptr);
    }

    void _delete_this() noexcept override { // destroy self
        delete this;
    }
}; // END _RefCountDeleter


template<class Type>
class _SharedWeakBase        // base class for SharedPtr and WeakPtr
{
public:
    using ElementType = RemoveExtent_t<Type>;

private:
    ElementType* _ptr       = nullptr;
    _RefCountBase* _rep     = nullptr;

    friend SharedPtr<Type>;
    friend WeakPtr<Type>;

public:
    // Constructors & Operators

    _SharedWeakBase(const _SharedWeakBase&)               = delete;
    _SharedWeakBase& operator=(const _SharedWeakBase&)    = delete;

protected:
    
    _SharedWeakBase() noexcept   = default;
    ~_SharedWeakBase()           = default;      // YES: each child of this class manages the destructor itself

public:
    // Main functions

    ElementType* get() const noexcept {
        return _ptr;
    }

    long use_count() const noexcept {
        return _rep ? _rep->use_count() : 0;
    }

    template<class Ty>
    bool owner_before(const _SharedWeakBase<Ty>& other) const noexcept { // compare addresses of manager objects
        return _rep < other._rep;
    }

    void swap(_SharedWeakBase& other) noexcept { // swap pointers
        custom::swap(_ptr, other._ptr);
        custom::swap(_rep, other._rep);
    }

protected:

    // Common for Shared and Weak ptr
    template<class Ty>
    void _move_construct(_SharedWeakBase<Ty>&& other) noexcept {
        _ptr = other._ptr;
        _rep = other._rep;

        other._ptr = nullptr;
        other._rep = nullptr;
    }

    // SharedPtr constructor helpers
    template<class Ty>
    void _copy_construct(const SharedPtr<Ty>& other) noexcept {     // Only SharedPtr can copy
        other._incref();

        _ptr = other._ptr;
        _rep = other._rep;
    }

    template<class Ty>
    void _alias_copy_construct(const SharedPtr<Ty>& other, ElementType* ptr) noexcept {
        other._incref();

        _ptr = ptr;
        _rep = other._rep;
    }

    template<class Ty>
    void _alias_move_construct(SharedPtr<Ty>&& other, ElementType* ptr) noexcept {
        _ptr = ptr;
        _rep = other._rep;

        other._ptr = nullptr;
        other._rep = nullptr;
    }

    template<class Ty>
    bool _construct_from_weak(const WeakPtr<Ty>& other) noexcept {     // TODO: check _incref here
        // implement SharedPtr's constructor from WeakPtr, and WeakPtr::lock()

        // if (other._rep && other._rep->_incref())
        // {
        //     _ptr = other._ptr;
        //     _rep = other._rep;

        //     return true;
        // }

        return false;
    }

    // WeakPtr contructor helpers
    template<class Ty>
    void _weak_construct(const _SharedWeakBase<Ty>& other) noexcept {
        if (other._rep)
        {
            _ptr = other._ptr;
            _rep = other._rep;
            _rep->incwref();
        }
        else
            CUSTOM_ASSERT(!_ptr && !_rep, "Pointer components for this should remain nullptr");
    }

    // _Refcount operations
    void _incref() const noexcept {
        if (_rep)
            _rep->incref();
    }

    void _decref() noexcept {
        if (_rep)
            _rep->decref();
    }

    void _incwref() const noexcept {
        if (_rep)
            _rep->incwref();
    }

    void _decwref() noexcept {
        if (_rep)
            _rep->decwref();
    }
}; // END _SharedWeakBase


template<class Type>
class EnableSharedFromThis      // provide member functions that create SharedPtr to this
{
public:
    using Esft_t = EnableSharedFromThis;

private:
    template<class Ty>
    friend class SharedPtr;

    mutable WeakPtr<Type> _wptr;

public:

    SharedPtr<Type> shared_from_this() {
        return SharedPtr<Type>(_wptr);
    }

    SharedPtr<const Type> shared_from_this() const {
        return SharedPtr<const Type>(_wptr);
    }

    WeakPtr<Type> weak_from_this() noexcept {
        return _wptr;
    }

    WeakPtr<const Type> weak_from_this() const noexcept {
        return _wptr;
    }

protected:
    constexpr EnableSharedFromThis() noexcept 
        : _wptr() { /*Empty*/ }

    EnableSharedFromThis(const EnableSharedFromThis&) noexcept
        : _wptr() { /*construct (must value-initialize _wptr)*/}

    EnableSharedFromThis& operator=(const EnableSharedFromThis&) noexcept { // assign (must not change _wptr)
        return *this;
    }

    ~EnableSharedFromThis() = default;

};  // END EnableSharedFromThis


template<class Type>
class SharedPtr : public _SharedWeakBase<Type>    // class for reference counted resource management
{
private:
    using Base = _SharedWeakBase<Type>;

public:
    using ElementType   = typename Base::ElementType;
    using WeakType      = WeakPtr<Type>;

public:
    // Constructors

    SharedPtr() noexcept = default;
    SharedPtr(std::nullptr_t) noexcept { /*Empty*/ }

    // TODO: not ready
    template<class Ty,
    EnableIf_t<Conjunction_v<
                    Conditional_t<IsArray_v<Type>, _CanArrayDelete<Ty>, _CanScalarDelete<Ty>>,
                    _SharedPtrConvertible<Ty, Type>>,
    bool> = true>
    explicit SharedPtr(Ty* ptr) {              // construct SharedPtr object that owns ptr
        if constexpr (IsArray_v<Type>)
            _set_pointer_default(ptr, DefaultDelete<Ty[]>{});
        else
        {
            _TemporaryOwner<Ty> temp(ptr);
            _set_ptr_rep_and_enable_shared(temp._Ptr, new _RefCount<Ty>(temp._Ptr));
            temp._Ptr = nullptr;
        }
    }

    template<class Ty, class Del,
    EnableIf_t<Conjunction_v<
                    IsMoveConstructible<Del>, 
                    _CanCallFunctionObject<Del&, Ty*&>,
                    _SharedPtrConvertible<Ty, Type>>,
    bool> = true>
    SharedPtr(Ty* ptr, Del deleter) {           // construct with ptr, deleter
        _set_pointer_default(ptr, custom::move(deleter));
    }

    template<class Del,
    EnableIf_t<Conjunction_v<
                    IsMoveConstructible<Del>, 
                    _CanCallFunctionObject<Del&, std::nullptr_t&>>, 
    bool> = true>
    SharedPtr(std::nullptr_t, Del deleter) {    // construct with nullptr, deleter
        _set_pointer_default(nullptr, custom::move(deleter));
    }

    SharedPtr(const SharedPtr& other) noexcept { // construct SharedPtr object that owns same resource as other
        this->_copy_construct(other);
    }

    template<class Ty, EnableIf_t<_SharedPtrCompatible<Ty, Type>::Value, bool> = true>
    SharedPtr(const SharedPtr<Ty>& other) noexcept {
        this->_copy_construct(other);
    }

    template<class Ty>
    SharedPtr(const SharedPtr<Ty>& other, ElementType* ptr) noexcept {  // copy construct SharedPtr object that aliases other
        this->_alias_copy_construct(other, ptr);
    }

    SharedPtr(SharedPtr&& other) noexcept {                             // construct SharedPtr object that takes resource from other
        this->_move_construct(custom::move(other));
    }

    template<class Ty, EnableIf_t<_SharedPtrCompatible<Ty, Type>::Value, bool> = true>
    SharedPtr(SharedPtr<Ty>&& other) noexcept {
        this->_move_construct(custom::move(other));
    }

    template<class Ty>
    SharedPtr(SharedPtr<Ty>&& other, ElementType* ptr) noexcept {       // move construct SharedPtr object that aliases other
        this->_alias_move_construct(custom::move(other), ptr);
    }

    template<class Ty, EnableIf_t<_SharedPtrCompatible<Ty, Type>::Value, bool> = true>
    explicit SharedPtr(const WeakPtr<Ty>& other) {                      // construct SharedPtr object that owns resource *other
        if (!this->_construct_from_weak(other))
            throw std::runtime_error("Bad weak ptr...");
    }

    template<class Ty, class Del,
    EnableIf_t<Conjunction_v<
                    _SharedPtrCompatible<Ty, Type>,
                    IsConvertible<typename UniquePtr<Ty, Del>::Pointer, ElementType*>>,
    bool> = true>
    SharedPtr(UniquePtr<Ty, Del>&& unique) {    // TODO: check
        using UnqPointer_t  = typename UniquePtr<Ty, Del>::Pointer;
        using UnqRaw_t      = typename UniquePtr<Ty, Del>::ElementType*;
        using UnqDeleter_t  = Conditional_t<IsReference_v<Del>, decltype(custom::ref(unique.get_deleter())), Del>;

        const UnqPointer_t ptr = unique.get();

        if (ptr)
        {
            const UnqRaw_t raw  = ptr;
            const auto refCount = new _RefCountDeleter<UnqPointer_t, UnqDeleter_t>(ptr, custom::forward<Del>(unique.get_deleter()));
            _set_ptr_rep_and_enable_shared(raw, refCount);
            unique.release();
        }
    }

    ~SharedPtr() {                              // release resource
        this->_decref();
    }

public:
    // Operators

    SharedPtr& operator=(const SharedPtr& other) noexcept {
        SharedPtr(other).swap(*this);
        return *this;
    }

    template<class Ty, EnableIf_t<_SharedPtrCompatible<Ty, Type>::Value, bool> = true>
    SharedPtr& operator=(const SharedPtr<Ty>& other) noexcept {
        SharedPtr(other).swap(*this);
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        SharedPtr(custom::move(other)).swap(*this);
        return *this;
    }

    template<class Ty, EnableIf_t<_SharedPtrCompatible<Ty, Type>::Value, bool> = true>
    SharedPtr& operator=(SharedPtr<Ty>&& other) noexcept {
        SharedPtr(custom::move(other)).swap(*this);
        return *this;
    }

    template<class Ty, class Del,
    EnableIf_t<Conjunction_v<
                    _SharedPtrCompatible<Ty, Type>,
                    IsConvertible<typename UniquePtr<Ty, Del>::Pointer, ElementType*>>,
    bool> = true>
    SharedPtr& operator=(UniquePtr<Ty, Del>&& unique) { // move from UniquePtr
        SharedPtr(custom::move(unique)).swap(*this);
        return *this;
    }

    template<EnableIf_t<!Disjunction_v<IsArray<Type>, IsVoid<Type>>, bool> = true>
    Type& operator*() const noexcept {
        return *this->get();
    }

    template<EnableIf_t<!IsArray_v<Type>, bool> = true>
    Type* operator->() const noexcept {
        return this->get();
    }

    explicit operator bool() const noexcept {
        return this->get() != nullptr;
    }

public:
    // Main functions

    // void swap(SharedPtr& other) noexcept {   // TODO: check if needed or can work only with base
    //     this->_swap(other);
    // }

    void reset() noexcept {             // release resource and convert to empty SharedPtr object
        SharedPtr().swap(*this);
    }

    template<class Ty,
    EnableIf_t<Conjunction_v<
                    Conditional_t<IsArray_v<Type>, _CanArrayDelete<Ty>, _CanScalarDelete<Ty>>,
                    _SharedPtrConvertible<Ty, Type>>,
    bool> = true>
    void reset(Ty* ptr) {               // release, take ownership of ptr
        SharedPtr(ptr).swap(*this);
    }

    template<class Ty, class Del,
    EnableIf_t<Conjunction_v<
                    IsMoveConstructible<Del>, 
                    _CanCallFunctionObject<Del&, Ty*&>,
                    _SharedPtrConvertible<Ty, Type>>,
    bool> = true>
    void reset(Ty* ptr, Del deleter) { // release, take ownership of ptr, with deleter
        SharedPtr(ptr, deleter).swap(*this);
    }

    bool unique() const noexcept {      // return true if no other SharedPtr object owns this resource
        return this->use_count() == 1;
    }

private:
    // Helpers

    template<class TypePtr, class Deleter>  // TODO: why _UxptrOrNullptr and not Type
    void _set_pointer_default(const TypePtr ptr, Deleter deleter) { // take ownership of ptr, deleter
        _TemporaryOwnerDel<TypePtr, Deleter> temp(ptr, deleter);
        _set_ptr_rep_and_enable_shared(temp._Ptr, new _RefCountDeleter<TypePtr, Deleter>(temp._Ptr, custom::move(deleter)));
        temp._CallDeleter = false;
    }

    template<class Ty>
    void _set_ptr_rep_and_enable_shared(Ty* const ptr, _RefCountBase* const refCount) noexcept { // take ownership of ptr
        this->_ptr = ptr;
        this->_rep = refCount;
        if constexpr (Conjunction_v<Negation<IsArray<Type>>, Negation<IsVolatile<Ty>>, _CanEnableShared<Ty>>)
            if (ptr && ptr->_wptr.expired()) 
                ptr->_wptr = SharedPtr<RemoveCV_t<Ty>>(*this, const_cast<RemoveCV_t<Ty>*>(ptr));
    }

    void _set_ptr_rep_and_enable_shared(std::nullptr_t, _RefCountBase* const refCount) noexcept { // take ownership of nullptr
        this->_Ptr = nullptr;
        this->_Rep = refCount;
    }
}; // END SharedPtr


// build SharedPtr
template<class Ty, class... Args, EnableIf_t<!IsArray_v<Ty>, bool> = true>
constexpr SharedPtr<Ty> make_shared(Args&&... args) {
    return SharedPtr<Ty>(new Ty(custom::forward<Args>(args)...));
}

#pragma endregion SharedPtr


#pragma region WeakPtr
template<class Type>
class WeakPtr : public _SharedWeakBase<Type>    // class for pointer to reference counted resource
{
public:
    // Constructors
    
    WeakPtr() noexcept { /*Empty*/ }

    WeakPtr(const WeakPtr& other) noexcept {
        this->_weak_construct(other); // same type, no conversion
    }

    template<class Ty, EnableIf_t<_SharedPtrCompatible<Ty, Type>::Value, bool> = true>
    WeakPtr(const SharedPtr<Ty>& other) noexcept {
        this->_weak_construct(other); // SharedPtr keeps resource alive during conversion
    }

    WeakPtr(WeakPtr&& other) noexcept {
        this->_weak_construct(custom::move(other));
    }

    ~WeakPtr() {
        this->_decwref();
    }

public:
    // Operators

    WeakPtr& operator=(const WeakPtr& other) noexcept {
        WeakPtr(other).swap(*this);
        return *this;
    }

    template<class Ty, EnableIf_t<_SharedPtrCompatible<Ty, Type>::Value, bool> = true>
    WeakPtr& operator=(const WeakPtr<Ty>& other) noexcept {
        WeakPtr(other).swap(*this);
        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) noexcept {
        WeakPtr(custom::move(other)).swap(*this);
        return *this;
    }

    template<class Ty, EnableIf_t<_SharedPtrCompatible<Ty, Type>::Value, bool> = true>
    WeakPtr& operator=(WeakPtr<Ty>&& other) noexcept {
        WeakPtr(custom::move(other)).swap(*this);
        return *this;
    }

    template<class Ty, EnableIf_t<_SharedPtrCompatible<Ty, Type>::Value, bool> = true>
    WeakPtr& operator=(const SharedPtr<Ty>& other) noexcept {
        WeakPtr(other).swap(*this);
        return *this;
    }

public:
    // Main functions

    void reset() noexcept {     // release resource, convert to null WeakPtr object
        WeakPtr().swap(*this);  // TODO: why WeakPtr{} ??? instead of ()
    }

    // void swap(weak_ptr& other) noexcept {
    //     this->_Swap(other);
    // }

    bool expired() const noexcept {
        return this->use_count() == 0;
    }

    SharedPtr<Type> lock() const noexcept { // convert to SharedPtr
        SharedPtr<Type> shared;
        (void)shared._construct_from_weak(*this);
        return shared;
    }
}; // END WeakPtr
#pragma endregion WeakPtr

CUSTOM_END