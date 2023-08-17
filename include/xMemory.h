#pragma once
#include "Utility.h"

#include <atomic>
#include <typeinfo>

CUSTOM_BEGIN

template<class Ty, class DeleterNoref, class = void>
struct _GetDeleterPointerType { using Type = Ty*; };

template<class Ty, class DeleterNoref>
struct _GetDeleterPointerType<Ty, DeleterNoref, Void_t<typename DeleterNoref::Pointer>> { using Type = typename DeleterNoref::Pointer; };

template<class Deleter>
using _UniquePtrEnableDefault_t = EnableIf_t<Conjunction_v<
                                                Negation<IsPointer<Deleter>>,
                                                IsDefaultConstructible<Deleter>>, bool>;


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
struct DefaultDelete<Type[]>                                // default deleter for UniquePtr to array of unknown size
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


// Temporary owner helper
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
};

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
};


class /*__declspec(novtable)*/ _RefCountBase     // Helper base class for ref counting // TODO: why __declspec(novtable) ???
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

    void _incref() noexcept {           // increment use count
        ++_uses;
    }

    void _incwref() noexcept {          // increment weak reference count
        ++_weaks;
    }

    void _decref() noexcept {           // decrement use count
        if (--_uses == 0)
        {
            _destroy();
            _decwref();
        }
    }

    void _decwref() noexcept {          // decrement weak reference count
        if (--_weaks == 0)
            _delete_this();
    }

    long _use_count() const noexcept {
        return static_cast<long>(_uses);
    }

    virtual void* _get_deleter(const std::type_info& ti) const noexcept {    // TODO: wtf is this?
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
class SharedPtr;

template<class Type>
class WeakPtr;

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
        return _rep ? _rep->_use_count() : 0;
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
            _rep->_incwref();
        }
        else
            CUSTOM_ASSERT(!_ptr && !_rep, "Pointer components for this should remain nullptr");
    }

    // _Refcount operations
    void _incref() const noexcept {
        if (_rep)
            _rep->_incref();
    }

    void _decref() noexcept {
        if (_rep)
            _rep->_decref();
    }

    void _incwref() const noexcept {
        if (_rep)
            _rep->_incwref();
    }

    void _decwref() noexcept {
        if (_rep)
            _rep->_decwref();
    }
}; // END _SharedWeakBase

CUSTOM_END