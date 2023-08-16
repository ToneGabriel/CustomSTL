#pragma once
#include "xMemory.h"

CUSTOM_BEGIN

// UniquePtr implementation
template<class Type, class Deleter = DefaultDelete<Type>>
class UniquePtr                                             // non-copyable pointer to an object
{
public:
    using ValueType     = Type;
    using DeleterType   = Deleter;
    using Pointer       = typename _GetDeleterPointerType<Type, RemoveReference_t<Deleter>>::Type;

private:
    Pointer _ptr;
    Deleter _deleter;

public:
    // Constructors

    template<class Deleter2 = Deleter, _UniquePtrEnableDefault_t<Deleter2> = true>
    UniquePtr() { /*Empty*/ }

    template<class Deleter2 = Deleter, _UniquePtrEnableDefault_t<Deleter2> = true>
    UniquePtr(std::nullptr_t) { /*Empty*/ }

    template<class Deleter2 = Deleter, _UniquePtrEnableDefault_t<Deleter2> = true>
    explicit UniquePtr(Pointer ptr)
        : _ptr(ptr) { /*Empty*/ }

    template<class Deleter2 = Deleter,
    EnableIf_t<IsConstructible_v<Deleter2, const Deleter2&>, bool> = true>
    UniquePtr(Pointer ptr, const Deleter& del)
        : _ptr(ptr), _deleter(del) { /*Empty*/ }

    template<class Deleter2 = Deleter,
    EnableIf_t<Conjunction_v<
                    Negation<IsReference<Deleter2>>,
                    IsConstructible<Deleter2, Deleter2>>, bool> = true>
    UniquePtr(Pointer ptr, Deleter&& del)
        : _ptr(ptr), _deleter(custom::move(del)) { /*Empty*/ }

    template<class Deleter2 = Deleter,
    EnableIf_t<Conjunction_v<
                    IsReference<Deleter2>,
                    IsConstructible<Deleter2, RemoveReference_t<Deleter2>>>, bool> = true>
    UniquePtr(Pointer, RemoveReference_t<Deleter>&&) = delete;

    template<class Deleter2 = Deleter,
    EnableIf_t<IsMoveConstructible_v<Deleter2>, bool> = true>
    UniquePtr(UniquePtr&& other) noexcept
        : _ptr(other.release()), _deleter(custom::forward<Deleter>(other.get_deleter())) { /*Empty*/ }

    template<class Type2, class Deleter2,
    EnableIf_t<Conjunction_v<
                    Negation<IsArray<Type2>>,
                    IsConvertible<typename UniquePtr<Type2, Deleter2>::Pointer, Pointer>,
                    Conditional_t<IsReference_v<Deleter>, IsSame<Deleter2, Deleter>, IsConvertible<Deleter2, Deleter>>>,
    bool> = true>
    UniquePtr(UniquePtr<Type2, Deleter2>&& other) noexcept
        : _ptr(other.release()), _deleter(custom::forward<Deleter2>(other.get_deleter())) { /*Empty*/ }

    UniquePtr(const UniquePtr&) = delete;

    ~UniquePtr() {
        if (_ptr != nullptr)
            _deleter(_ptr);
    }

public:
    // Operators

    UniquePtr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    template<class Type2, class Deleter2,
    EnableIf_t<Conjunction_v<
                    Negation<IsArray<Type2>>,
                    IsAssignable<Deleter&, Deleter2>,
                    IsConvertible<typename UniquePtr<Type2, Deleter2>::Pointer, Pointer>>,
    bool> = true>
    UniquePtr& operator=(UniquePtr<Type2, Deleter2>&& other) noexcept {
        reset(other.release());
        _deleter = custom::forward<Deleter2>(other._deleter);

        return *this;
    }

    template<class Deleter2 = Deleter,
    EnableIf_t<IsMoveAssignable_v<Deleter2>, bool> = true>
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (_ptr != other._ptr) {
            reset(other.release());
            _deleter = custom::forward<Deleter>(other._deleter);
        }

        return *this;
    }

    UniquePtr& operator=(const UniquePtr&)  = delete;

    AddLvalueReference_t<Type> operator*() const noexcept {
       return *_ptr;
    }

    Pointer operator->() const noexcept {
        return _ptr;
    }

    explicit operator bool() const noexcept {
        return _ptr != nullptr;
    }

public:
    // Main functions

    Deleter& get_deleter() noexcept {
        return _deleter;
    }

    const Deleter& get_deleter() const noexcept {
        return _deleter;
    }

    Pointer get() const noexcept {
        return _ptr;
    }

    Pointer release() noexcept {                            // Releases the ownership of the managed object, get() returns nullptr after this
        return custom::exchange(_ptr, nullptr);
    }

    void reset(Pointer ptr = nullptr) noexcept {            // Replaces the managed object and deletes old
        Pointer old = custom::exchange(_ptr, ptr);

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
    using _EnableConstructorReset = EnableIf_t<Disjunction_v<
                                                    IsSame<Type2, Pointer>,
                                                    IsNullptr,
                                                    Conjunction<
                                                        IsSame<Pointer, ValueType*>,
                                                        IsPointer<Type2>,
                                                        IsConvertible<RemovePointer_t<Type2>(*)[], ValueType(*)[]>>>,
                                    bool>;

    template<class Type2, class Del, class More,
    class UpPointer         = typename UniquePtr<Type2, Del>::Pointer,
    class UpValueType       = typename UniquePtr<Type2, Del>::ValueType>
    using _EnableConversion =   EnableIf_t<Conjunction_v<
                                                IsArray<Type2>,
                                                IsSame<Pointer, ValueType*>,
                                                IsSame<UpPointer, UpValueType*>,
                                                IsConvertible<UpValueType(*)[], ValueType(*)[]>,
                                                More>,  // More is used for additional enable traits
                                bool>;  // TODO: check

private:
    Pointer _ptr;
    Deleter _deleter;

public:
    // Constructors

    template<class Deleter2 = Deleter,
    _UniquePtrEnableDefault_t<Deleter2> = true>
    UniquePtr() { /*Empty*/ }

    template<class Type2, class Deleter2 = Deleter,
    _UniquePtrEnableDefault_t<Deleter2> = true,
    _EnableConstructorReset<Type2> = true>
    explicit UniquePtr(Type2 ptr)
        : _ptr(ptr) { /*Empty*/ }

    template<class Type2, class Deleter2 = Deleter, 
    EnableIf_t<IsCopyConstructible_v<Deleter2>, bool> = true,
    _EnableConstructorReset<Type2> = true>
    UniquePtr(Type2 ptr, const Deleter& del)
        : _ptr(ptr), _deleter(del) { /*Empty*/ }

    template<class Type2, class Deleter2 = Deleter,
    EnableIf_t<Conjunction_v<
                    Negation<IsReference<Deleter2>>,
                    IsMoveConstructible<Deleter2>>, bool> = true,
    _EnableConstructorReset<Type2> = true>
    UniquePtr(Type2 ptr, Deleter&& del) noexcept
        : _ptr(ptr), _deleter(custom::move(del)) { /*Empty*/ }

    template<class Type2, class Deleter2 = Deleter,
    EnableIf_t<Conjunction_v<
                    IsReference<Deleter2>,
                    IsConstructible<Deleter2, RemoveReference_t<Deleter2>>>, bool> = true>
    UniquePtr(Type2, RemoveReference_t<Deleter>&&) = delete;

    template<class Deleter2 = Deleter,
    EnableIf_t<IsMoveConstructible_v<Deleter2>, bool> = true>
    UniquePtr(UniquePtr&& other) noexcept
        : _ptr(other.release()), _deleter(custom::forward<Deleter>(other.get_deleter())) { /*Empty*/ }

    UniquePtr(const UniquePtr&) = delete;

    ~UniquePtr() {
        if (_ptr != nullptr)
            _deleter(_ptr);
    }

public:
    // Operators

    UniquePtr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    template<class Type2, class Deleter2,
    _EnableConversion<Type2, Deleter2, IsAssignable<Deleter&, Deleter2>> = true>
    UniquePtr& operator=(UniquePtr<Type2, Deleter2>&& other) noexcept {
        reset(other.release());
        _deleter = custom::forward<Deleter2>(other._deleter);

        return *this;
    }

    template<class Deleter2 = Deleter,
    EnableIf_t<IsMoveAssignable_v<Deleter2>, bool> = true>
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (_ptr != other._ptr) {
            reset(other.release());
            _deleter = custom::move(other._deleter);
        }

        return *this;
    }

    UniquePtr& operator=(const UniquePtr&) = delete;

    Type& operator[](size_t index) const noexcept {
        return _ptr[index];
    }

    explicit operator bool() const noexcept {
        return _ptr != nullptr;
    }

public:
    // Main functions

    Deleter& get_deleter() noexcept {
        return _deleter;
    }

    const Deleter& get_deleter() const noexcept {
        return _deleter;
    }

    Pointer get() const noexcept {
        return _ptr;
    }

    Pointer release() noexcept {                                                // Releases the ownership of the managed object, get() returns nullptr after this
        return custom::exchange(_ptr, nullptr);
    }

    template<class Type2, _EnableConstructorReset<Type2, FalseType> = true>     // FalseType (don't allow nullptr ???)
    void reset(Type2 ptr) noexcept {                                            // Replaces the managed object and deletes old
        Pointer old = custom::exchange(_ptr, ptr);

        if (old)
            _deleter(old);
    }

}; // END UniquePtr[]


// build UniquePtr
template<class Ty, class... Args, EnableIf_t<!IsArray_v<Ty>, bool> = true>
UniquePtr<Ty> make_unique(Args&&... args) {
    return UniquePtr<Ty>(new Ty(custom::forward<Args>(args)...));
}

template<class Ty, EnableIf_t<IsArray_v<Ty> && Extent_v<Ty> == 0, bool> = true>
UniquePtr<Ty> make_unique(const size_t size) {
    return UniquePtr<Ty>(new RemoveExtent_t<Ty>[size]());
}

template<class Ty, class... Args, EnableIf_t<Extent_v<Ty> != 0, bool> = true>
void make_unique(Args&&...) = delete;


// Shared/Weak Ptr implementation
template<class Type>
class EnableSharedFromThis { // provide member functions that create SharedPtr to this
public:
    using Esft_t = EnableSharedFromThis;

private:
    template<class _Type>
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
    constexpr EnableSharedFromThis() noexcept : _wptr() {}

    EnableSharedFromThis(const EnableSharedFromThis&) noexcept : _wptr() {
        // construct (must value-initialize _wptr)
    }

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
    // Constructors & Operators

    SharedPtr() noexcept = default;
    SharedPtr(std::nullptr_t) noexcept { /*Empty*/ }

    // TODO: not ready
    template<class Ty,
    EnableIf_t<Conjunction_v<
                    Conditional_t<IsArray_v<Type>, 
                                _CanArrayDelete<Ty>, 
                                _CanScalarDelete<Ty>>,
                    _SharedPtrConvertible<Ty, Type>>,
    bool> = true>
    explicit SharedPtr(Ty* ptr) {              // construct SharedPtr object that owns ptr
        if (IsArray_v<Type>)
            _set_pointer_default(ptr, DefaultDelete<Ty[]>{});
        else
        {
            _TemporaryOwner<Ty> temp(ptr);
            _set_ptr_rep_and_enable_shared(temp._Ptr, new _RefCount<Ty>(temp._Ptr));
            temp._Ptr = nullptr;
        }
    }

    SharedPtr(const SharedPtr& other) noexcept { // construct SharedPtr object that owns same resource as other
        this->_copy_construct(other);
    }

    SharedPtr(SharedPtr&& other) noexcept {     // construct SharedPtr object that takes resource from other
        this->_move_construct(custom::move(other));
    }

    template<class Ty>
    SharedPtr(const SharedPtr<Ty>& other, ElementType* ptr) noexcept { // copy construct SharedPtr object that aliases other
        this->_alias_copy_construct(other, ptr);
    }

    template<class Ty>
    SharedPtr(SharedPtr<Ty>&& other, ElementType* ptr) noexcept {      // move construct SharedPtr object that aliases other
        this->_alias_move_construct(custom::move(other), ptr);
    }

    ~SharedPtr() {                              // release resource
        this->_decref();
    }

    SharedPtr& operator=(const SharedPtr& other) noexcept {
        SharedPtr(other).swap(*this);
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        SharedPtr(custom::move(other)).swap(*this);
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

    bool unique() const noexcept {      // return true if no other SharedPtr object owns this resource
        return this->use_count() == 1;
    }

private:
    // Helpers

    template<class Ty, class Deleter>  // TODO: why _UxptrOrNullptr and not Type
    void _set_pointer_default(const Ty ptr, Deleter deleter) { // take ownership of ptr, deleter
        _TemporaryOwnerDel<Ty, Deleter> temp(ptr, deleter);
        _set_ptr_rep_and_enable_shared(temp._Ptr, new _RefCountDeleter<Ty, Deleter>(temp._Ptr, custom::move(deleter)));
        temp._CallDeleter = false;
    }

    template<class _Ux> // TODO: _Ux change
    void _set_ptr_rep_and_enable_shared(_Ux* const ptr, _RefCountBase* const refCount) noexcept { // take ownership of ptr
        this->_ptr = ptr;
        this->_rep = refCount;
        if (Conjunction_v<Negation<IsArray<Type>>, Negation<IsVolatile<_Ux>>, _CanEnableShared<_Ux>>)
            if (ptr && ptr->_wptr.expired()) 
                ptr->_wptr = SharedPtr<RemoveCV_t<_Ux>>(*this, const_cast<RemoveCV_t<_Ux>*>(ptr));
    }
}; // END SharedPtr


template<class Type>
class WeakPtr : public _SharedWeakBase<Type>    // class for pointer to reference counted resource
{
public:
    // Constructors & Operators
    
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

    // void swap(weak_ptr& _Other) noexcept {
    //     this->_Swap(_Other);
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

CUSTOM_END