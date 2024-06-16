#pragma once
#include "c_utility.h"
#include "x_memory.h"

#include <atomic>
#include <typeinfo>

CUSTOM_BEGIN

#pragma region Deleters

template<class Type>
struct default_delete                                        // default deleter for unique_ptr
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
struct default_delete<Type[]>                                // default deleter for unique_ptr to array of unknown size
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


#pragma region Temporay Owner

template<class Type>
struct _Temporary_Owner
{
    Type* _Ptr;

    explicit _Temporary_Owner(Type* const ptr) noexcept 
        : _Ptr(ptr) { /*Empty*/ }

    _Temporary_Owner(const _Temporary_Owner&)             = delete;
    _Temporary_Owner& operator=(const _Temporary_Owner&)  = delete;

    ~_Temporary_Owner() {
        delete _Ptr;
    }
};  // END _Temporary_Owner

template<class TypePtr, class Deleter>
struct _Temporary_Owner_Del
{
    TypePtr _Ptr;
    Deleter& _Deleter;
    bool _CallDeleter = true;   // modified by user when needed

    explicit _Temporary_Owner_Del(const TypePtr ptr, Deleter& del) noexcept 
        : _Ptr(ptr), _Deleter(del) { /*Empty*/ }

    _Temporary_Owner_Del(const _Temporary_Owner_Del&)               = delete;
    _Temporary_Owner_Del& operator=(const _Temporary_Owner_Del&)    = delete;

    ~_Temporary_Owner_Del() {
        if (_CallDeleter)
            _Deleter(_Ptr);
    }
};  // END _Temporary_Owner_Del

#pragma endregion Temporay Owner


#pragma region UniquePtr

// Helpers
template<class Ty, class DeleterNoref, class = void>
struct _GetDeleterPointerType { using type = Ty*; };

template<class Ty, class DeleterNoref>
struct _GetDeleterPointerType<Ty, DeleterNoref, void_t<typename DeleterNoref::pointer>>
{
    // the "custom" deleter must have public pointer alias (it's accessed here)
    using type = typename DeleterNoref::pointer;
};

template<class Deleter>
using _UniquePtrEnableDefault_t = enable_if_t<conjunction_v<
                                                negation<is_pointer<Deleter>>,
                                                is_default_constructible<Deleter>>, bool>;


template<class Type, class Deleter = default_delete<Type>>
class unique_ptr                                             // non-copyable pointer to an object
{
public:
    using element_type  = Type;
    using deleter_type  = Deleter;
    using pointer       = typename _GetDeleterPointerType<Type, remove_reference_t<Deleter>>::type;

private:
    pointer _ptr        = nullptr;
    Deleter _deleter;

public:
    // Constructors

    template<class Del = Deleter,
    _UniquePtrEnableDefault_t<Del> = true>
    constexpr unique_ptr() noexcept { /*Empty*/ }

    template<class Del = Deleter,
    _UniquePtrEnableDefault_t<Del> = true>
    constexpr unique_ptr(std::nullptr_t) noexcept { /*Empty*/ }

    template<class Del = Deleter,
    _UniquePtrEnableDefault_t<Del> = true>
    constexpr explicit unique_ptr(pointer ptr) noexcept
        : _ptr(ptr) { /*Empty*/ }

    template<class Del = Deleter,
    enable_if_t<is_constructible_v<Del, const Del&>, bool> = true>
    constexpr unique_ptr(pointer ptr, const Deleter& del) noexcept
        : _ptr(ptr), _deleter(del) { /*Empty*/ }

    template<class Del = Deleter,
    enable_if_t<conjunction_v<
                    negation<is_reference<Del>>,
                    is_constructible<Del, Del>>, bool> = true>
    constexpr unique_ptr(pointer ptr, Deleter&& del) noexcept
        : _ptr(ptr), _deleter(custom::move(del)) { /*Empty*/ }

    template<class Del = Deleter,
    enable_if_t<conjunction_v<
                    is_reference<Del>,
                    is_constructible<Del, remove_reference_t<Del>>>, bool> = true>
    unique_ptr(pointer, remove_reference_t<Deleter>&&) = delete;

    template<class Del = Deleter,
    enable_if_t<is_move_constructible_v<Del>, bool> = true>
    constexpr unique_ptr(unique_ptr&& other) noexcept
        : _ptr(other.release()), _deleter(custom::forward<Deleter>(other.get_deleter())) { /*Empty*/ }

    template<class Ty, class Del,
    enable_if_t<conjunction_v<
                    negation<is_array<Ty>>,
                    is_convertible<typename unique_ptr<Ty, Del>::pointer, pointer>,
                    conditional_t<is_reference_v<Deleter>, is_same<Del, Deleter>, is_convertible<Del, Deleter>>>,
    bool> = true>
    constexpr unique_ptr(unique_ptr<Ty, Del>&& other) noexcept
        : _ptr(other.release()), _deleter(custom::forward<Del>(other.get_deleter())) { /*Empty*/ }

    unique_ptr(const unique_ptr&) = delete;

    constexpr ~unique_ptr() noexcept {
        if (_ptr != nullptr)
            _deleter(_ptr);
    }

public:
    // Operators

    constexpr unique_ptr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    template<class Ty, class Del,
    enable_if_t<conjunction_v<
                    negation<is_array<Ty>>,
                    is_assignable<Deleter&, Del>,
                    is_convertible<typename unique_ptr<Ty, Del>::pointer, pointer>>,
    bool> = true>
    constexpr unique_ptr& operator=(unique_ptr<Ty, Del>&& other) noexcept {
        reset(other.release());
        _deleter = custom::forward<Del>(other._deleter);

        return *this;
    }

    template<class Del = Deleter,
    enable_if_t<is_move_assignable_v<Del>, bool> = true>
    constexpr unique_ptr& operator=(unique_ptr&& other) noexcept {
        if (_ptr != other._ptr)
        {
            reset(other.release());
            _deleter = custom::forward<Deleter>(other._deleter);
        }

        return *this;
    }

    unique_ptr& operator=(const unique_ptr&) = delete;

    constexpr add_lvalue_reference_t<Type> operator*() const noexcept(noexcept(*custom::declval<pointer>())) {
       return *_ptr;
    }

    constexpr pointer operator->() const noexcept {
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

    constexpr pointer get() const noexcept {
        return _ptr;
    }

    constexpr pointer release() noexcept {                            // Releases the ownership of the managed object, get() returns nullptr after this
        return custom::exchange(_ptr, nullptr);
    }

    constexpr void reset(pointer ptr = nullptr) noexcept {            // Replaces the managed object and deletes old
        pointer old = custom::exchange(_ptr, ptr);

        if (old)
            _deleter(old);
    }
}; // END unique_ptr


template<class Type, class Deleter>
class unique_ptr<Type[], Deleter>                            // non-copyable pointer to an array object
{
public:
    using element_type   = Type;
    using deleter_type   = Deleter;
    using pointer       = typename _GetDeleterPointerType<Type, remove_reference_t<Deleter>>::type;

    template<class Ty, class IsNullptr = is_same<Ty, std::nullptr_t>>
    using _EnableConstructorAndReset =  enable_if_t<disjunction_v<
                                                    is_same<Ty, pointer>,
                                                    IsNullptr,
                                                    conjunction<
                                                        is_same<pointer, element_type*>,
                                                        is_pointer<Ty>,
                                                        is_convertible<remove_pointer_t<Ty>(*)[], element_type(*)[]>>>,
                                        bool>;

    template<class Ty, class Del, class More,
    class UPPointer         = typename unique_ptr<Ty, Del>::pointer,
    class UPElementType     = typename unique_ptr<Ty, Del>::element_type>
    using _EnableConversion =   enable_if_t<conjunction_v<
                                                is_array<Ty>,
                                                is_same<pointer, element_type*>,
                                                is_same<UPPointer, UPElementType*>,
                                                is_convertible<UPElementType(*)[], element_type(*)[]>,
                                                More>,  // More is used for additional enable traits
                                bool>;

private:
    pointer _ptr;
    Deleter _deleter;

public:
    // Constructors

    template<class Del = Deleter,
    _UniquePtrEnableDefault_t<Del> = true>
    constexpr unique_ptr() noexcept { /*Empty*/ }

    template<class Ty, class Del = Deleter,
    _UniquePtrEnableDefault_t<Del> = true,
    _EnableConstructorAndReset<Ty> = true>
    constexpr explicit unique_ptr(Ty ptr) noexcept
        : _ptr(ptr) { /*Empty*/ }

    template<class Ty, class Del = Deleter, 
    enable_if_t<is_copy_constructible_v<Del>, bool> = true,
    _EnableConstructorAndReset<Ty> = true>
    constexpr unique_ptr(Ty ptr, const Deleter& del) noexcept
        : _ptr(ptr), _deleter(del) { /*Empty*/ }

    template<class Ty, class Del = Deleter,
    enable_if_t<conjunction_v<
                    negation<is_reference<Del>>,
                    is_move_constructible<Del>>, bool> = true,
    _EnableConstructorAndReset<Ty> = true>
    constexpr unique_ptr(Ty ptr, Deleter&& del) noexcept
        : _ptr(ptr), _deleter(custom::move(del)) { /*Empty*/ }

    template<class Ty, class Del = Deleter,
    enable_if_t<conjunction_v<
                    is_reference<Del>,
                    is_constructible<Del, remove_reference_t<Del>>>, bool> = true>
    unique_ptr(Ty, remove_reference_t<Deleter>&&) = delete;

    template<class Del = Deleter,
    enable_if_t<is_move_constructible_v<Del>, bool> = true>
    constexpr unique_ptr(unique_ptr&& other) noexcept
        : _ptr(other.release()), _deleter(custom::forward<Deleter>(other.get_deleter())) { /*Empty*/ }

    unique_ptr(const unique_ptr&) = delete;

    constexpr ~unique_ptr() noexcept {
        if (_ptr != nullptr)
            _deleter(_ptr);
    }

public:
    // Operators

    constexpr unique_ptr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    template<class Ty, class Del,
    _EnableConversion<Ty, Del, is_assignable<Deleter&, Del>> = true>
    constexpr unique_ptr& operator=(unique_ptr<Ty, Del>&& other) noexcept {
        reset(other.release());
        _deleter = custom::forward<Del>(other._deleter);

        return *this;
    }

    template<class Del = Deleter,
    enable_if_t<is_move_assignable_v<Del>, bool> = true>
    constexpr unique_ptr& operator=(unique_ptr&& other) noexcept {
        if (_ptr != other._ptr)
        {
            reset(other.release());
            _deleter = custom::move(other._deleter);
        }

        return *this;
    }

    unique_ptr& operator=(const unique_ptr&) = delete;

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

    constexpr pointer get() const noexcept {
        return _ptr;
    }

    constexpr pointer release() noexcept {                                           // Releases the ownership of the managed object, get() returns nullptr after this
        return custom::exchange(_ptr, nullptr);
    }

    template<class Ty, _EnableConstructorAndReset<Ty, false_type> = true>            // false_type (don't allow nullptr ???)
    constexpr void reset(Ty ptr) noexcept {                                         // Replaces the managed object and deletes old
        pointer old = custom::exchange(_ptr, ptr);

        if (old)
            _deleter(old);
    }

}; // END unique_ptr[]


// build unique_ptr
template<class Ty, class... Args, enable_if_t<!is_array_v<Ty>, bool> = true>
constexpr unique_ptr<Ty> make_unique(Args&&... args) {
    return unique_ptr<Ty>(new Ty(custom::forward<Args>(args)...));
}

template<class Ty, enable_if_t<is_array_v<Ty> && extent_v<Ty> == 0, bool> = true>
constexpr unique_ptr<Ty> make_unique(const size_t size) {
    return unique_ptr<Ty>(new remove_extent_t<Ty>[size]());
}

template<class Ty, class... Args, enable_if_t<extent_v<Ty> != 0, bool> = true>
void make_unique(Args&&...) = delete;

template<class Ty, enable_if_t<!is_array_v<Ty>, bool> = true>
constexpr unique_ptr<Ty> make_unique_for_overwrite() {      // make a unique_ptr with default initialization
    return unique_ptr<Ty>(new Ty());
}

template<class Ty, enable_if_t<is_unbounded_array_v<Ty>, bool> = true>
constexpr unique_ptr<Ty> make_unique_for_overwrite(const size_t _Size) {    // make a unique_ptr[] with default initialization
    return unique_ptr<Ty>(new remove_extent_t<Ty>[_Size]);
}

template<class Ty, class... Unused, enable_if_t<is_unbounded_array_v<Ty>, bool> = true>
constexpr void make_unique_for_overwrite(Unused&&...) = delete;

// unique_ptr binary operators
template<class Ty1, class Del1, class Ty2, class Del2>
constexpr bool operator==(const unique_ptr<Ty1, Del1>& left, const unique_ptr<Ty2, Del2>& right) {
    return left.get() == right.get();
}

template<class Ty1, class Del1, class Ty2, class Del2>
constexpr bool operator!=(const unique_ptr<Ty1, Del1>& left, const unique_ptr<Ty2, Del2>& right) {
    return !(left == right);
}
#pragma endregion unique_ptr


#pragma region shared_ptr
template<class Type>
class shared_ptr;

template<class Type>
class weak_ptr;

// Shared/Weak Ptr helpers
template<class Ty, class = void>
struct _CanScalarDelete : false_type {};

template<class Ty>
struct _CanScalarDelete<Ty, void_t<decltype(delete custom::declval<Ty*>())>> : true_type {};

template<class Ty, class = void>
struct _CanArrayDelete : false_type {};

template<class Ty>
struct _CanArrayDelete<Ty, void_t<decltype(delete[] custom::declval<Ty*>())>> : true_type {};

template<class Func, class Arg, class = void>
struct _CanCallFunctionObject : false_type {};

template<class Func, class Arg>
struct _CanCallFunctionObject<Func, Arg, void_t<decltype(custom::declval<Func>()(custom::declval<Arg>()))>> : true_type {};

template<class Ty1, class Ty2>
struct _SharedPtrConvertible : is_convertible<Ty1*, Ty2*>::type {};

template<class Ty1, class Ty2>
struct _SharedPtrConvertible<Ty1, Ty2[]> : is_convertible<Ty1(*)[], Ty2(*)[]>::type {};

template<class Ty1, class Ty2, size_t Nx>
struct _SharedPtrConvertible<Ty1, Ty2[Nx]> : is_convertible<Ty1(*)[Nx], Ty2(*)[Nx]>::type {};

// a pointer type Ty1* is said to be compatible with a pointer type Ty2*
// when either Ty1* is convertible to Ty2*
// or Ty1 is U[N] and Ty2 is cv U[].
template<class Ty1, class Ty2>
struct _SharedPtrCompatible : is_convertible<Ty1*, Ty2*>::type {};

template<class Ty, size_t Nx>
struct _SharedPtrCompatible<Ty[Nx], Ty[]> : true_type {};

template<class Ty, size_t Nx>
struct _SharedPtrCompatible<Ty[Nx], const Ty[]> : true_type {};

template<class Ty, size_t Nx>
struct _SharedPtrCompatible<Ty[Nx], volatile Ty[]> : true_type {};

template<class Ty, size_t Nx>
struct _SharedPtrCompatible<Ty[Nx], const volatile Ty[]> : true_type {};

template<class Ty, class = void>
struct _CanEnableShared : false_type {}; // detect unambiguous and accessible inheritance from enable_shared_from_this

template<class Ty>
struct _CanEnableShared<Ty, void_t<typename Ty::_Esft_t>>
    : is_convertible<remove_cv_t<Ty>*, typename Ty::_Esft_t*>::type {};


class CUSTOM_NOVTABLE_ATTR _Ref_Count_Base     // Helper base class for ref counting
{
private:
    std::atomic<long> _uses     = 1;
    std::atomic<long> _weaks    = 1;

protected:
    _Ref_Count_Base() noexcept = default;    // non-atomic initializations

public:
    // Constructors & Operators
    
    _Ref_Count_Base(const _Ref_Count_Base&)               = delete;
    _Ref_Count_Base& operator=(const _Ref_Count_Base&)    = delete;

    virtual ~_Ref_Count_Base() { /*Empty*/ }

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

private:
    // Helpers

    virtual void* _get_deleter(const std::type_info& ti) const noexcept = 0;
    virtual void _destroy() noexcept = 0;
    virtual void _delete_this() noexcept = 0;
}; // END _Ref_Count_Base


template<class TypePtr, class Deleter>
class _Ref_Count_Deleter : public _Ref_Count_Base     // handle reference counting for object with deleter
{
private:
    TypePtr _ptr;
    Deleter _deleter;

public:

    explicit _Ref_Count_Deleter(TypePtr ptr, Deleter del)
        : _Ref_Count_Base(), _ptr(ptr), _deleter(custom::move(del)) { /*Empty*/ }

private:
    void* _get_deleter(const std::type_info& ti) const noexcept override {
        if (ti == typeid(Deleter))
            return const_cast<Deleter*>(&_deleter);

        return nullptr;
    }

    void _destroy() noexcept override { // destroy managed resource
        _deleter(_ptr);
    }

    void _delete_this() noexcept override { // destroy self
        delete this;
    }
}; // END _Ref_Count_Deleter


template<class TypePtr, class Deleter, class Alloc>
class _Ref_Count_Deleter_Alloc : public _Ref_Count_Base     // handle reference counting for object with deleter and allocator
{
private:
    TypePtr _ptr;
    Deleter _deleter;
    Alloc _alloc;

public:
    explicit _Ref_Count_Deleter_Alloc(TypePtr ptr, Deleter del, const Alloc& alloc)
        : _Ref_Count_Base(), _ptr(ptr), _deleter(custom::move(del)), _alloc(alloc) { /*Empty*/ }

private:
    void* _get_deleter(const std::type_info& ti) const noexcept override {
        if (ti == typeid(Deleter))
            return const_cast<Deleter*>(&_deleter);

        return nullptr;
    }

    void _destroy() noexcept override { // destroy managed resource
        _deleter(_ptr);
    }

    void _delete_this() noexcept override { // destroy self
        using _AllocRefCount        = typename allocator_traits<Alloc>::template rebind_alloc<_Ref_Count_Deleter_Alloc>;
        using _AllocRefCountTraits  = allocator_traits<_AllocRefCount>;

        _AllocRefCount alref(_alloc);
		_AllocRefCountTraits::destroy(alref, this);
        alref.deallocate(this, 1);
    }
};  // END _Ref_Count_Deleter_Alloc


template<class Type>
class _Shared_Weak_Base        // base class for shared_ptr and weak_ptr
{
public:
    using element_type = remove_extent_t<Type>;

protected:
    element_type* _ptr      = nullptr;
    _Ref_Count_Base* _rep   = nullptr;

public:
    // Constructors & Operators

    _Shared_Weak_Base(const _Shared_Weak_Base&)               = delete;
    _Shared_Weak_Base& operator=(const _Shared_Weak_Base&)    = delete;

protected:
    
    _Shared_Weak_Base() noexcept   = default;
    ~_Shared_Weak_Base()           = default;      // YES: each child of this class manages the destructor itself

public:
    // Main functions

    element_type* get() const noexcept {
        return _ptr;
    }

    long use_count() const noexcept {
        return _rep ? _rep->use_count() : 0;
    }

    template<class Ty>
    bool owner_before(const _Shared_Weak_Base<Ty>& other) const noexcept { // compare addresses of manager objects
        return _rep < other._rep;
    }

    void swap(_Shared_Weak_Base& other) noexcept { // swap pointers
        custom::swap(_ptr, other._ptr);
        custom::swap(_rep, other._rep);
    }

protected:
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

    // Common for Shared and Weak ptr
    template<class Ty>
    void _move_construct(_Shared_Weak_Base<Ty>&& other) noexcept {
        _ptr = other._ptr;
        _rep = other._rep;

        other._ptr = nullptr;
        other._rep = nullptr;
    }

    // shared_ptr constructor helpers
    template<class Ty>
    void _copy_construct(const shared_ptr<Ty>& other) noexcept {     // Only shared_ptr can copy
        other._incref();

        _ptr = other._ptr;
        _rep = other._rep;
    }

    template<class Ty>
    void _alias_copy_construct(const shared_ptr<Ty>& other, element_type* ptr) noexcept {
        other._incref();

        _ptr = ptr;
        _rep = other._rep;
    }

    template<class Ty>
    void _alias_move_construct(shared_ptr<Ty>&& other, element_type* ptr) noexcept {
        _ptr = ptr;
        _rep = other._rep;

        other._ptr = nullptr;
        other._rep = nullptr;
    }

    template<class Ty>
    bool _construct_from_weak(const weak_ptr<Ty>& other) noexcept {
        // implement shared_ptr's constructor from weak_ptr, and weak_ptr::lock()

        if (other._rep)
        {
            _ptr = other._ptr;
            _rep = other._rep;
            _rep->incref();

            return true;
        }

        return false;
    }

    // weak_ptr contructor helpers
    template<class Ty>
    void _weak_construct(const _Shared_Weak_Base<Ty>& other) noexcept {
        if (other._rep)
        {
            _ptr = other._ptr;
            _rep = other._rep;
            _rep->incwref();
        }
        else
            CUSTOM_ASSERT(!_ptr && !_rep, "pointer components for this should remain nullptr");
    }
}; // END _Shared_Weak_Base


template<class Type>
class enable_shared_from_this      // provide member functions that create shared_ptr to this
{
public:
    using _Esft_t = enable_shared_from_this;

private:
    template<class Ty>
    friend class shared_ptr;

    mutable weak_ptr<Type> _wptr;

public:

    shared_ptr<Type> shared_from_this() {
        return shared_ptr<Type>(_wptr);
    }

    shared_ptr<const Type> shared_from_this() const {
        return shared_ptr<const Type>(_wptr);
    }

    weak_ptr<Type> weak_from_this() noexcept {
        return _wptr;
    }

    weak_ptr<const Type> weak_from_this() const noexcept {
        return _wptr;
    }

protected:
    constexpr enable_shared_from_this() noexcept 
        : _wptr() { /*Empty*/ }

    enable_shared_from_this(const enable_shared_from_this&) noexcept
        : _wptr() { /*construct (must value-initialize _wptr)*/}

    enable_shared_from_this& operator=(const enable_shared_from_this&) noexcept { // assign (must not change _wptr)
        return *this;
    }

    ~enable_shared_from_this() = default;

};  // END enable_shared_from_this


template<class Type>
class shared_ptr : public _Shared_Weak_Base<Type>    // class for reference counted resource management
{
private:
    using _Base         = _Shared_Weak_Base<Type>;

public:
    using element_type  = typename _Base::element_type;
    using weak_type     = weak_ptr<Type>;

public:
    // Constructors

    shared_ptr() noexcept = default;
    shared_ptr(std::nullptr_t) noexcept { /*Empty*/ }

    template<class Ty,
    enable_if_t<conjunction_v<
                    conditional_t<is_array_v<Type>, _CanArrayDelete<Ty>, _CanScalarDelete<Ty>>,
                    _SharedPtrConvertible<Ty, Type>>,
    bool> = true>
    explicit shared_ptr(Ty* ptr) {              // construct shared_ptr object that owns ptr
        if constexpr (is_array_v<Type>)
            _set_pointer_default(ptr, default_delete<Ty[]>{});
        else
            _set_pointer_default(ptr, default_delete<Ty>{});
    }

    template<class Ty, class Del,
    enable_if_t<conjunction_v<
                    is_move_constructible<Del>, 
                    _CanCallFunctionObject<Del&, Ty*&>,
                    _SharedPtrConvertible<Ty, Type>>,
    bool> = true>
    shared_ptr(Ty* ptr, Del del) {               // construct with ptr, deleter
        _set_pointer_default(ptr, custom::move(del));
    }

    template<class Del,
    enable_if_t<conjunction_v<
                    is_move_constructible<Del>, 
                    _CanCallFunctionObject<Del&, std::nullptr_t&>>, 
    bool> = true>
    shared_ptr(std::nullptr_t, Del del) {        // construct with nullptr, deleter
        _set_pointer_default(nullptr, custom::move(del));
    }

    template<class Ty, class Del, class Alloc,
    enable_if_t<conjunction_v<
                        is_move_constructible<Del>,
                        _CanCallFunctionObject<Del&, Ty*&>,
                        _SharedPtrConvertible<Ty, Type>>,
    bool> = true>
    shared_ptr(Ty* ptr, Del del, Alloc alloc) { // construct with ptr, deleter, allocator
        _set_pointer_alloc(ptr, custom::move(del), alloc);
    }

    template<class Del, class Alloc,
    enable_if_t<conjunction_v<
                        is_move_constructible<Del>,
                        _CanCallFunctionObject<Del&, std::nullptr_t&>>,
    bool> = true>
    shared_ptr(std::nullptr_t, Del del, Alloc alloc) { // construct with nullptr, deleter, allocator
        _set_pointer_alloc(nullptr, custom::move(del), alloc);
    }

    shared_ptr(const shared_ptr& other) noexcept { // construct shared_ptr object that owns same resource as other
        this->_copy_construct(other);
    }

    template<class Ty, enable_if_t<_SharedPtrCompatible<Ty, Type>::Value, bool> = true>
    shared_ptr(const shared_ptr<Ty>& other) noexcept {
        this->_copy_construct(other);
    }

    template<class Ty>
    shared_ptr(const shared_ptr<Ty>& other, element_type* ptr) noexcept {  // copy construct shared_ptr object that aliases other
        this->_alias_copy_construct(other, ptr);
    }

    shared_ptr(shared_ptr&& other) noexcept {                             // construct shared_ptr object that takes resource from other
        this->_move_construct(custom::move(other));
    }

    template<class Ty, enable_if_t<_SharedPtrCompatible<Ty, Type>::Value, bool> = true>
    shared_ptr(shared_ptr<Ty>&& other) noexcept {
        this->_move_construct(custom::move(other));
    }

    template<class Ty>
    shared_ptr(shared_ptr<Ty>&& other, element_type* ptr) noexcept {       // move construct shared_ptr object that aliases other
        this->_alias_move_construct(custom::move(other), ptr);
    }

    template<class Ty, enable_if_t<_SharedPtrCompatible<Ty, Type>::Value, bool> = true>
    explicit shared_ptr(const weak_ptr<Ty>& other) {                      // construct shared_ptr object that owns resource *other
        if (!this->_construct_from_weak(other))
            throw std::runtime_error("Bad weak ptr.");
    }

    template<class Ty, class Del,
    enable_if_t<conjunction_v<
                    _SharedPtrCompatible<Ty, Type>,
                    is_convertible<typename unique_ptr<Ty, Del>::pointer, element_type*>>,
    bool> = true>
    shared_ptr(unique_ptr<Ty, Del>&& unique) {
        using UnqPointer_t  = typename unique_ptr<Ty, Del>::pointer;
        using UnqRaw_t      = typename unique_ptr<Ty, Del>::element_type*;
        using UnqDeleter_t  = conditional_t<is_reference_v<Del>, decltype(custom::ref(unique.get_deleter())), Del>;

        const UnqPointer_t ptr = unique.get();

        if (ptr)
        {
            const UnqRaw_t raw  = ptr;
            const auto refCount = new _Ref_Count_Deleter<UnqPointer_t, UnqDeleter_t>(ptr, custom::forward<Del>(unique.get_deleter()));
            _set_ptr_rep_and_enable_shared(raw, refCount);
            unique.release();
        }
    }

    ~shared_ptr() {                              // release resource
        this->_decref();
    }

public:
    // Operators

    shared_ptr& operator=(const shared_ptr& other) noexcept {
        shared_ptr(other).swap(*this);
        return *this;
    }

    template<class Ty, enable_if_t<_SharedPtrCompatible<Ty, Type>::Value, bool> = true>
    shared_ptr& operator=(const shared_ptr<Ty>& other) noexcept {
        shared_ptr(other).swap(*this);
        return *this;
    }

    shared_ptr& operator=(shared_ptr&& other) noexcept {
        shared_ptr(custom::move(other)).swap(*this);
        return *this;
    }

    template<class Ty, enable_if_t<_SharedPtrCompatible<Ty, Type>::Value, bool> = true>
    shared_ptr& operator=(shared_ptr<Ty>&& other) noexcept {
        shared_ptr(custom::move(other)).swap(*this);
        return *this;
    }

    template<class Ty, class Del,
    enable_if_t<conjunction_v<
                    _SharedPtrCompatible<Ty, Type>,
                    is_convertible<typename unique_ptr<Ty, Del>::pointer, element_type*>>,
    bool> = true>
    shared_ptr& operator=(unique_ptr<Ty, Del>&& unique) { // move from unique_ptr
        shared_ptr(custom::move(unique)).swap(*this);
        return *this;
    }

    template<class Ty = Type, enable_if_t<!disjunction_v<is_array<Ty>, is_void<Ty>>, bool> = true>
    Ty& operator*() const noexcept {
        return *this->get();
    }

    template<class Ty = Type, enable_if_t<!is_array_v<Ty>, bool> = true>
    Ty* operator->() const noexcept {
        return this->get();
    }

    template<class Ty = Type, class Elem = element_type, enable_if_t<is_array_v<Ty>, bool> = true>
    Elem& operator[](ptrdiff_t index) const noexcept {
        return this->get()[index];
    }

    explicit operator bool() const noexcept {
        return this->get() != nullptr;
    }

public:
    // Main functions

    void reset() noexcept {             // release resource and convert to empty shared_ptr object
        shared_ptr().swap(*this);
    }

    template<class Ty,
    enable_if_t<conjunction_v<
                    conditional_t<is_array_v<Type>, _CanArrayDelete<Ty>, _CanScalarDelete<Ty>>,
                    _SharedPtrConvertible<Ty, Type>>,
    bool> = true>
    void reset(Ty* ptr) {               // release, take ownership of ptr
        shared_ptr(ptr).swap(*this);
    }

    template<class Ty, class Del,
    enable_if_t<conjunction_v<
                    is_move_constructible<Del>, 
                    _CanCallFunctionObject<Del&, Ty*&>,
                    _SharedPtrConvertible<Ty, Type>>,
    bool> = true>
    void reset(Ty* ptr, Del del) {      // release, take ownership of ptr, with deleter
        shared_ptr(ptr, del).swap(*this);
    }

    template<class Ty, class Del, class Alloc,
    enable_if_t<conjunction_v<
                    is_move_constructible<Del>, 
                    _CanCallFunctionObject<Del&, Ty*&>,
                    _SharedPtrConvertible<Ty, Type>>,
    bool> = true>
    void reset(Ty* ptr, Del del, Alloc alloc) {      // release, take ownership of ptr, with deleter and allocator alloc
        shared_ptr(ptr, del, alloc).swap(*this);
    }

    bool unique() const noexcept {      // return true if no other shared_ptr object owns this resource
        return this->use_count() == 1;
    }

private:
    // Helpers

    template<class TypePtr, class Deleter>
    void _set_pointer_default(const TypePtr ptr, Deleter del) { // take ownership of ptr, deleter
        _Temporary_Owner_Del<TypePtr, Deleter> temp(ptr, del);
        _set_ptr_rep_and_enable_shared(temp._Ptr, new _Ref_Count_Deleter<TypePtr, Deleter>(temp._Ptr, custom::move(del)));
        temp._CallDeleter = false;
    }

    template<class TypePtr, class Deleter, class Alloc>
    void _set_pointer_alloc(const TypePtr ptr, Deleter del, Alloc alloc) { // take ownership of ptr, deleter del, allocator alloc
        using _RefCountDelAl            = _Ref_Count_Deleter_Alloc<TypePtr, Deleter, Alloc>;
        using _AllocRefCountDelAl       = typename allocator_traits<Alloc>::template rebind_alloc<_RefCountDelAl>;
        using _AllocRefCountDelAlTraits = allocator_traits<_AllocRefCountDelAl>;

        _Temporary_Owner_Del<TypePtr, Deleter> temp(ptr, del);
        _AllocRefCountDelAl alref(alloc);

        typename _AllocRefCountDelAlTraits::pointer refCountPtr = alref.allocate(1);
		_AllocRefCountDelAlTraits::construct(alref, refCountPtr, temp._Ptr, custom::move(del), alloc);

        _set_ptr_rep_and_enable_shared(temp._Ptr, refCountPtr);
        temp._CallDeleter = false;
    }

    template<class Ty>
    void _set_ptr_rep_and_enable_shared(Ty* const ptr, _Ref_Count_Base* const refCount) noexcept { // take ownership of ptr
        this->_ptr = ptr;
        this->_rep = refCount;

        // this is for enable_shared_from_this
        if constexpr (conjunction_v<
                            negation<is_array<Type>>,
                            negation<is_volatile<Ty>>,
                            _CanEnableShared<Ty>>)
            if (ptr && ptr->_wptr.expired()) 
                ptr->_wptr = shared_ptr<remove_cv_t<Ty>>(*this, const_cast<remove_cv_t<Ty>*>(ptr));
    }

    void _set_ptr_rep_and_enable_shared(std::nullptr_t, _Ref_Count_Base* const refCount) noexcept { // take ownership of nullptr
        this->_ptr = nullptr;
        this->_rep = refCount;
    }

private:
    // Friends

    template<class Ty, class... Args>
    friend enable_if_t<!is_array_v<Ty>, shared_ptr<Ty>> make_shared(Args&&... args);

    template<class Ty, class Alloc, class... Args>
    friend enable_if_t<!is_array_v<Ty>, shared_ptr<Ty>> allocate_shared(const Alloc& alloc, Args&&... args);

    template<class Del, class Ty>
    friend Del* get_deleter(const shared_ptr<Ty>& ptr) noexcept;
}; // END shared_ptr

// TODO
// build shared_ptr
template<class Ty, class... Args>
enable_if_t<!is_array_v<Ty>, shared_ptr<Ty>> make_shared(Args&&... args) {
    return shared_ptr<Ty>(new Ty(custom::forward<Args>(args)...));
}

template<class Ty, class Alloc, class... Args>
enable_if_t<!is_array_v<Ty>, shared_ptr<Ty>> allocate_shared(const Alloc& alloc, Args&&... args) {
    return shared_ptr<Ty>();
    //return shared_ptr<Ty>(alloc, std::forward<Args>(args)...);
}

// get_deleter
template<class Del, class Ty>
Del* get_deleter(const shared_ptr<Ty>& ptr) noexcept {   // return pointer to shared_ptr's deleter object if its type is Del
    if (ptr._rep)
        return static_cast<Del*>(ptr._rep->_get_deleter(typeid(Del)));

    return nullptr;
}

#pragma endregion SharedPtr


#pragma region WeakPtr
template<class Type>
class weak_ptr : public _Shared_Weak_Base<Type>    // class for pointer to reference counted resource
{
public:
    // Constructors
    
    weak_ptr() noexcept { /*Empty*/ }

    weak_ptr(const weak_ptr& other) noexcept {
        this->_weak_construct(other); // same type, no conversion
    }

    template<class Ty, enable_if_t<_SharedPtrCompatible<Ty, Type>::Value, bool> = true>
    weak_ptr(const shared_ptr<Ty>& other) noexcept {
        this->_weak_construct(other); // shared_ptr keeps resource alive during conversion
    }

    weak_ptr(weak_ptr&& other) noexcept {
        this->_weak_construct(custom::move(other));
    }

    ~weak_ptr() {
        this->_decwref();
    }

public:
    // Operators

    weak_ptr& operator=(const weak_ptr& other) noexcept {
        weak_ptr(other).swap(*this);
        return *this;
    }

    template<class Ty, enable_if_t<_SharedPtrCompatible<Ty, Type>::Value, bool> = true>
    weak_ptr& operator=(const weak_ptr<Ty>& other) noexcept {
        weak_ptr(other).swap(*this);
        return *this;
    }

    weak_ptr& operator=(weak_ptr&& other) noexcept {
        weak_ptr(custom::move(other)).swap(*this);
        return *this;
    }

    template<class Ty, enable_if_t<_SharedPtrCompatible<Ty, Type>::Value, bool> = true>
    weak_ptr& operator=(weak_ptr<Ty>&& other) noexcept {
        weak_ptr(custom::move(other)).swap(*this);
        return *this;
    }

    template<class Ty, enable_if_t<_SharedPtrCompatible<Ty, Type>::Value, bool> = true>
    weak_ptr& operator=(const shared_ptr<Ty>& other) noexcept {
        weak_ptr(other).swap(*this);
        return *this;
    }

public:
    // Main functions

    void reset() noexcept {     // release resource, convert to null weak_ptr object
        weak_ptr().swap(*this);
    }

    bool expired() const noexcept {
        return this->use_count() == 0;
    }

    shared_ptr<Type> lock() const noexcept { // convert to shared_ptr
        shared_ptr<Type> shared;
        (void)shared._construct_from_weak(*this);
        return shared;
    }
}; // END weak_ptr
#pragma endregion WeakPtr

CUSTOM_END