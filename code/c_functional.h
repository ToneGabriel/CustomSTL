#pragma once
#include <x_functional.h>
#include <c_utility.h>
#include <c_tuple.h>
#include <c_memory.h>


CUSTOM_BEGIN

template<class Signature>
class function;

template<class Functor, class... Args>
class binder;

template<int num>
struct placeholder { static_assert(num > 0, "invalid placeholder index"); };

namespace placeholders
{
    constexpr placeholder<1> _1;
    constexpr placeholder<2> _2;
    constexpr placeholder<3> _3;
    constexpr placeholder<4> _4;
    constexpr placeholder<5> _5;
    constexpr placeholder<6> _6;
    constexpr placeholder<7> _7;
    constexpr placeholder<8> _8;
    constexpr placeholder<9> _9;
    constexpr placeholder<10> _10;
    constexpr placeholder<11> _11;
    constexpr placeholder<12> _12;
    constexpr placeholder<13> _13;
    constexpr placeholder<14> _14;
    constexpr placeholder<15> _15;
    constexpr placeholder<16> _16;
    constexpr placeholder<17> _17;
    constexpr placeholder<18> _18;
    constexpr placeholder<19> _19;
    constexpr placeholder<20> _20;
}   // END placeholders

template<class Ty>
struct is_placeholder;

template<class Ty>
constexpr int is_placeholder_v = is_placeholder<Ty>::value;

template<class Ty>
struct is_bind_expression;

template<class Ty>
constexpr int is_bind_expression_v = is_bind_expression<Ty>::value;

CUSTOM_DETAIL_BEGIN

template<class Arg, class Res>
struct _Unary_Function_Traits
{
    using argument_type = Arg;
    using result_type   = Res;
};

template<class Arg1, class Arg2, class Res>
struct _Binary_Function_Traits
{
    using first_argument_type   = Arg1;
    using second_argument_type  = Arg2;
    using result_type           = Res;
};

template<class Callable>
constexpr bool _Testable_Callable_v = disjunction_v<is_pointer<Callable>,
                                                    is_specialization<Callable, function>,
                                                    is_member_pointer<Callable>>;

template<class Callable>
bool _test_callable(const Callable& obj) noexcept
{
    // determine whether custom::function must store obj

    if constexpr (_Testable_Callable_v<Callable>)
        return !!obj;   // obj != nullptr;
    else
        return true;
}

#if CUSTOM_OPTIMAL_IMPLEMENTATION   // store callable object impl in stack or heap depending on size

static constexpr int _Small_Object_Num_Ptrs = 6 + 16 / sizeof(void*);
static constexpr size_t _Space_Size = (_Small_Object_Num_Ptrs - 1) * sizeof(void*);

template<class _Impl>    // when _Callable_Impl wraps an object(big) that has operator()
static constexpr bool _Is_Large =   sizeof(_Impl) > _Space_Size ||
                                    alignof(_Impl) > alignof(max_align_t) ||
                                    !_Impl::_Nothrow_Move::value;

template<class Ret, class... Args>
class CUSTOM_NOVTABLE_ATTR _Callable_Interface
{
public:
    _Callable_Interface()                                       = default;
    _Callable_Interface(const _Callable_Interface&)             = delete;
    _Callable_Interface& operator=(const _Callable_Interface&)  = delete;
    // destructor non-virtual due to _delete_this()

    virtual _Callable_Interface* _copy(void*) const             = 0;
    virtual _Callable_Interface* _move(void*) noexcept          = 0;
    virtual Ret _call(Args&&...)                                = 0;
    virtual const std::type_info& _target_type() const noexcept = 0;
    virtual void _delete_this(bool) noexcept                    = 0;

    const void* _target(const std::type_info& info) const noexcept
    {
        return _target_type() == info ? _get() : nullptr;
    }

private:
    virtual const void* _get() const noexcept                   = 0;
};  // END _Callable_Interface

template<class Callable, class Ret, class... Args>
class _Callable_Impl final : public _Callable_Interface<Ret, Args...>
{
public:
    using _Base         = _Callable_Interface<Ret, Args...>;
    using _Nothrow_Move = is_nothrow_move_constructible<Callable>;

private:
    Callable _callable;

public:
    template<class OtherCallable,
    enable_if_t<!is_same_v<decay_t<OtherCallable>, _Callable_Impl>, bool> = true>
    explicit _Callable_Impl(OtherCallable&& val)
        : _callable(custom::forward<OtherCallable>(val)) { /*Empty*/ }

private:
    _Base* _copy(void* address) const override
    {
        if constexpr (_Is_Large<_Callable_Impl>)
            return new _Callable_Impl(_callable);
        else
            return ::new(address) _Callable_Impl(_callable);
    }

    _Base* _move(void* address) noexcept override
    {
        if constexpr (_Is_Large<_Callable_Impl>)
            return nullptr;
        else
            return ::new(address) _Callable_Impl(custom::move(_callable));
    }

    Ret _call(Args&&... args) override
    {
        if constexpr (is_void_v<Ret>)
            (void)custom::invoke(_callable, custom::forward<Args>(args)...);
        else
            return custom::invoke(_callable, custom::forward<Args>(args)...);
    }

    const std::type_info& _target_type() const noexcept override
    {
        return typeid(Callable);
    }

    const void* _get() const noexcept override
    {
        return &_callable;
    }

    void _delete_this(bool dealloc) noexcept override
    {
        this->~_Callable_Impl();

        if (dealloc)
            ::operator delete(this, sizeof(_Callable_Impl));
    }
};  // END _Callable_Impl

#else   // CUSTOM_OPTIMAL_IMPLEMENTATION - store callable object impl in heap regardless of size

template<class Ret, class... Args>
class CUSTOM_NOVTABLE_ATTR _Callable_Interface
{
public:
    _Callable_Interface()                                       = default;
    _Callable_Interface(const _Callable_Interface&)             = delete;
    _Callable_Interface& operator=(const _Callable_Interface&)  = delete;

    virtual ~_Callable_Interface() {}
    virtual _Callable_Interface* _copy() const                  = 0;
    virtual Ret _call(Args&&...)                                = 0;
    virtual const std::type_info& _target_type() const noexcept = 0;

    const void* _target(const std::type_info& info) const noexcept
    {
        return _target_type() == info ? _get() : nullptr;
    }

private:
    virtual const void* _get() const noexcept                   = 0;
};  // END _Callable_Interface

template<class Callable, class Ret, class... Args>
class _Callable_Impl final : public _Callable_Interface<Ret, Args...>
{
private:
    using _Base = _Callable_Interface<Ret, Args...>;

    Callable _callable;

public:

    template<class OtherCallable,
    enable_if_t<!is_same_v<decay_t<OtherCallable>, _Callable_Impl>, bool> = true>
    explicit _Callable_Impl(OtherCallable&& val)
        : _callable(custom::forward<OtherCallable>(val)) { /*Empty*/ }

private:

    _Base* _copy() const override
    {
        return new _Callable_Impl(_callable);
    }

    Ret _call(Args&&... args) override
    {
        if constexpr (is_void_v<Ret>)
            (void)custom::invoke(_callable, custom::forward<Args>(args)...);
        else
            return custom::invoke(_callable, custom::forward<Args>(args)...);
    }

    const std::type_info& _target_type() const noexcept override
    {
        return typeid(Callable);
    }

    const void* _get() const noexcept override
    {
        return &_callable;
    }
};  // END _Callable_Impl

#endif  // CUSTOM_OPTIMAL_IMPLEMENTATION

// bind fixers & callers
template<   class BoundArgType,
            bool = is_specialization_v<remove_cv_t<BoundArgType>, reference_wrapper>,
            bool = is_bind_expression_v<BoundArgType>,
            int  = is_placeholder_v<BoundArgType>>
struct _Bound_Arg_Fixer;

template<class BoundArgType>
struct _Bound_Arg_Fixer<BoundArgType, true, false, 0>         // reference_wrapper fixer
{
    template<class UnboundTuple>
    static constexpr typename BoundArgType::type& fix(BoundArgType& boundArg, UnboundTuple&&) noexcept
    {
        // unwrap a reference_wrapper
        return boundArg.get();
    }
};

template<class BoundArgType>
struct _Bound_Arg_Fixer<BoundArgType, false, false, 0>        // identity fixer
{
    template<class UnboundTuple>
    static constexpr BoundArgType& fix(BoundArgType& boundArg, UnboundTuple&&) noexcept
    {
        // pass a bound argument as an Lvalue
        return boundArg;
    }
};

template<class BoundArgType>
struct _Bound_Arg_Fixer<BoundArgType, false, true, 0>         // nested bind fixer
{
private:
    template<class UnboundTuple, size_t... Ix>
    static constexpr auto _fix_impl(BoundArgType& boundArg, UnboundTuple&& unboundTuple, index_sequence<Ix...>) noexcept
    -> decltype(boundArg(custom::get<Ix>(custom::move(unboundTuple))...))
    {
        return boundArg(custom::get<Ix>(custom::move(unboundTuple))...);
    }

public:
    template<class UnboundTuple>
    static constexpr auto fix(BoundArgType& boundArg, UnboundTuple&& unboundTuple) noexcept
    -> decltype(_fix_impl(boundArg, custom::move(unboundTuple), make_index_sequence<tuple_size_v<UnboundTuple>>{}))
    {
        return _fix_impl(boundArg, custom::move(unboundTuple), make_index_sequence<tuple_size_v<UnboundTuple>>{});
    }
};

template<class BoundArgType, int Ix>
struct _Bound_Arg_Fixer<BoundArgType, false, false, Ix>      // placeholder fixer
{
    static_assert(Ix > 0, "invalid is_placeholder value");

    template<class UnboundTuple>
    static constexpr auto fix(BoundArgType&, UnboundTuple&& unboundTuple) noexcept
    -> decltype(custom::get<Ix - 1>(custom::move(unboundTuple)))
    {
        // choose the Ix'th unbound argument (1-based indexing)
        return custom::get<Ix - 1>(custom::move(unboundTuple));
    }
};

template<class BoundArgType, class UnboundTuple>
constexpr auto _fix_one_arg(BoundArgType& boundArg, UnboundTuple&& unboundTuple) noexcept
-> decltype(_Bound_Arg_Fixer<BoundArgType>::fix(boundArg, custom::move(unboundTuple)))
{
    // translate an argument for bind
    return _Bound_Arg_Fixer<BoundArgType>::fix(boundArg, custom::move(unboundTuple));
}

template<class Functor, class BoundTuple, size_t... Ix, class UnboundTuple>
constexpr auto _call_binder(Functor& func, BoundTuple& boundTuple, index_sequence<Ix...>, UnboundTuple&& unboundTuple) noexcept
-> decltype(custom::invoke(func, _fix_one_arg(custom::get<Ix>(boundTuple), custom::move(unboundTuple))...))
{
    // _fix_one_arg is called for every boundTuple item (YES: unboundTuple is moved every call)
    return custom::invoke(func, _fix_one_arg(custom::get<Ix>(boundTuple), custom::move(unboundTuple))...);
}

CUSTOM_DETAIL_END

// equal to
template<class Ty = void>
struct equal_to : public detail::_Binary_Function_Traits<Ty, Ty, bool>
{
    constexpr bool operator()(const Ty& left, const Ty& right) const
    {
        return left == right;
    }
};

template<>
struct equal_to<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) == custom::forward<Ty2>(right)))
    -> decltype(custom::forward<Ty1>(left) == custom::forward<Ty2>(right))
    {
        return custom::forward<Ty1>(left) == custom::forward<Ty2>(right);
    }

    using is_transparent = int;
};

// not equal to
template<class Ty = void>
struct not_equal_to : public detail::_Binary_Function_Traits<Ty, Ty, bool>
{
    constexpr bool operator()(const Ty& left, const Ty& right) const
    {
        return left != right;
    }
};

template<>
struct not_equal_to<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) != custom::forward<Ty2>(right)))
    -> decltype(custom::forward<Ty1>(left) != custom::forward<Ty2>(right))
    {
        return custom::forward<Ty1>(left) != custom::forward<Ty2>(right);
    }

    using is_transparent = int;
};

// greater
template<class Ty = void>
struct greater : public detail::_Binary_Function_Traits<Ty, Ty, bool>
{
    constexpr bool operator()(const Ty& left, const Ty& right) const
    {
        return left > right;
    }
};

template<>
struct greater<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) > custom::forward<Ty2>(right)))
    -> decltype(custom::forward<Ty1>(left) > custom::forward<Ty2>(right))
    {
        return custom::forward<Ty1>(left) > custom::forward<Ty2>(right);
    }

    using is_transparent = int;
};

// greater equal
template<class Ty = void>
struct greater_equal : public detail::_Binary_Function_Traits<Ty, Ty, bool>
{
    constexpr bool operator()(const Ty& left, const Ty& right) const
    {
        return left >= right;
    }
};

template<>
struct greater_equal<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) >= custom::forward<Ty2>(right)))
    -> decltype(custom::forward<Ty1>(left) >= custom::forward<Ty2>(right))
    {
        return custom::forward<Ty1>(left) >= custom::forward<Ty2>(right);
    }

    using is_transparent = int;
};

// less
template<class Ty = void>
struct less : public detail::_Binary_Function_Traits<Ty, Ty, bool>
{
    constexpr bool operator()(const Ty& left, const Ty& right) const
    {
        return left < right;
    }
};

template<>
struct less<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) < custom::forward<Ty2>(right)))
    -> decltype(custom::forward<Ty1>(left) < custom::forward<Ty2>(right))
    {
        return custom::forward<Ty1>(left) < custom::forward<Ty2>(right);
    }

    using is_transparent = int;
};

// less equal
template<class Ty = void>
struct less_equal : public detail::_Binary_Function_Traits<Ty, Ty, bool>
{
    constexpr bool operator()(const Ty& left, const Ty& right) const
    {
        return left <= right;
    }
};

template<>
struct less_equal<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) <= custom::forward<Ty2>(right)))
    -> decltype(custom::forward<Ty1>(left) <= custom::forward<Ty2>(right))
    {
        return custom::forward<Ty1>(left) <= custom::forward<Ty2>(right);
    }

    using is_transparent = int;
};

// plus
template<class Ty = void>
struct plus : public detail::_Binary_Function_Traits<Ty, Ty, Ty>
{
    constexpr Ty operator()(const Ty& left, const Ty& right) const
    {
        return left + right;
    }
};

template<>
struct plus<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) + custom::forward<Ty1>(right)))
    -> decltype(custom::forward<Ty1>(left) + custom::forward<Ty1>(right))
    {
        return custom::forward<Ty1>(left) + custom::forward<Ty1>(right);
    }

    using is_transparent = int;
};

// minus
template<class Ty = void>
struct minus : public detail::_Binary_Function_Traits<Ty, Ty, Ty>
{
    constexpr Ty operator()(const Ty& left, const Ty& right) const
    {
        return left - right;
    }
};

template<>
struct minus<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) - custom::forward<Ty1>(right)))
    -> decltype(custom::forward<Ty1>(left) - custom::forward<Ty1>(right))
    {
        return custom::forward<Ty1>(left) - custom::forward<Ty1>(right);
    }

    using is_transparent = int;
};

// multiplies
template<class Ty = void>
struct multiplies : public detail::_Binary_Function_Traits<Ty, Ty, Ty>
{
    constexpr Ty operator()(const Ty& left, const Ty& right) const
    {
        return left * right;
    }
};

template<>
struct multiplies<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) * custom::forward<Ty1>(right)))
    -> decltype(custom::forward<Ty1>(left) * custom::forward<Ty1>(right))
    {
        return custom::forward<Ty1>(left) * custom::forward<Ty1>(right);
    }

    using is_transparent = int;
};

// divides
template<class Ty = void>
struct divides : public detail::_Binary_Function_Traits<Ty, Ty, Ty>
{
    constexpr Ty operator()(const Ty& left, const Ty& right) const
    {
        return left / right;
    }
};

template<>
struct divides<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) / custom::forward<Ty1>(right)))
    -> decltype(custom::forward<Ty1>(left) / custom::forward<Ty1>(right))
    {
        return custom::forward<Ty1>(left) / custom::forward<Ty1>(right);
    }

    using is_transparent = int;
};

// modulus
template<class Ty = void>
struct modulus : public detail::_Binary_Function_Traits<Ty, Ty, Ty>
{
    constexpr Ty operator()(const Ty& left, const Ty& right) const
    {
        return left % right;
    }
};

template<>
struct modulus<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) % custom::forward<Ty1>(right)))
    -> decltype(custom::forward<Ty1>(left) % custom::forward<Ty1>(right))
    {
        return custom::forward<Ty1>(left) % custom::forward<Ty1>(right);
    }

    using is_transparent = int;
};

// negate
template<class Ty = void>
struct negate : public detail::_Unary_Function_Traits<Ty, Ty>
{
    constexpr Ty operator()(const Ty& val) const
    {
        return -val;
    }
};

template<>
struct negate<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& val) const
    noexcept(noexcept(-custom::forward<Ty1>(val)))
    -> decltype(-custom::forward<Ty1>(val))
    {
        return -custom::forward<Ty1>(val);
    }

    using is_transparent = int;
};

class bad_function_call : public std::exception // exception thrown when an empty custom::function is called
{
public:
    bad_function_call() noexcept { /*Empty*/ }

    const char* what() const noexcept override
    {
        return "Bad function call.";
    }
};  // END bad_function_call

#if CUSTOM_OPTIMAL_IMPLEMENTATION

template<class Ret, class... Args>
class function<Ret(Args...)>                // wrapper for callable objects
{
public:
    using result_type    = Ret;
    using _Impl          = detail::_Callable_Interface<Ret, Args...>;

private:
    union _Storage  // storage for small objects
    {
        max_align_t _Val;                               // for maximum alignment
        char Pad[detail::_Space_Size];                  // to permit aliasing
        _Impl* _Ptrs[detail::_Small_Object_Num_Ptrs];   // _Ptrs[_Small_Object_Num_Ptrs - 1] is reserved
    };

    _Storage _storage;

public:
    // Constructors

    function() noexcept
    {
        _set_impl(nullptr);
    }

    function(std::nullptr_t) noexcept : function() { /*Empty*/ }

    function(const function& other)
    {
        _reset_copy(other);
    }

    function(function&& other) noexcept
    {
        _reset_move(custom::move(other));
    }

    template<class Callable, enable_if_t<!is_same_v<decay_t<Callable>, function>, bool> = true>
    function(Callable&& val)
    {
        _reset(custom::forward<Callable>(val));
    }

    ~function() noexcept
    {
        _clean_up_storage();
    }

public:
    // Operators

    function& operator=(std::nullptr_t) noexcept
    {
        _clean_up_storage();
        return *this;
    }

    function& operator=(const function& other)
    {
        function(other).swap(*this);
        return *this;
    }

    function& operator=(function&& other) noexcept
    {
        if (this != &other)
        {
            _clean_up_storage();
            _reset_move(custom::move(other));
        }

        return *this;
    }

    template<class Callable, enable_if_t<!is_same_v<decay_t<Callable>, function>, bool> = true>
    function& operator=(Callable&& val)
    {
        function(custom::forward<Callable>(val)).swap(*this);
        return *this;
    }

    template<class Callable>
    function& operator=(reference_wrapper<Callable> refVal) noexcept
    {
        _clean_up_storage();
        _reset(refVal);
        return *this;
    }

    result_type operator()(Args... args) const
    {
        if (_empty())
            throw custom::bad_function_call();

        return _get_impl()->_call(custom::forward<Args>(args)...);
    }

    explicit operator bool() const noexcept
    {
        return !_empty();
    }

public:
    // Main functions

    void swap(function& other) noexcept
    {
        if (!_local() && !other._local())   // just swap pointers
        {
            auto _Temp = _get_impl();
            _set_impl(other._get_impl());
            other._set_impl(_Temp);
        }
        else                                // do three-way move
        {
            function _Temp;
            _Temp._reset_move(custom::move(*this));
            _reset_move(custom::move(other));
            other._reset_move(custom::move(_Temp));
        }
    }

    const std::type_info& target_type() const noexcept
    {
        return _get_impl() ? _get_impl()->_target_type() : typeid(void);
    }

    template<class Callable>
    Callable* target() noexcept
    {
        return reinterpret_cast<Callable*>(const_cast<void*>(_target(typeid(Callable))));
    }

    template<class Callable>
    const Callable* target() const noexcept
    {
        return reinterpret_cast<const Callable*>(_target(typeid(Callable)));
    }

private:
    // Helpers

    void _reset_copy(const function& other) // copy other's stored object
    {
        if (!other._empty())
            _set_impl(other._get_impl()->_copy(&_storage));  // copy to _storage[0]
    }

    void _reset_move(function&& other) noexcept // move other's stored object
    {
        if (!other._empty())
        {
            if (other._local()) // move and tidy
            {
                _set_impl(other._get_impl()->_move(&_storage));
                other._clean_up_storage();
            }
            else               // steal from other
            {
                _set_impl(other._get_impl());
                other._set_impl(nullptr);
            }
        }
    }

    template<class Callable>
    void _reset(Callable&& val) // store copy of val
    {
        if (!detail::_test_callable(val))   // null member pointer/function pointer/custom::function
            return;                 // already empty

        using _OtherImpl = detail::_Callable_Impl<decay_t<Callable>, Ret, Args...>;

        if constexpr (detail::_Is_Large<_OtherImpl>)  // dynamically allocate val
            _set_impl(new _OtherImpl(custom::forward<Callable>(val)));
        else                                // store val in-situ
            _set_impl(::new(&_storage) _OtherImpl(custom::forward<Callable>(val)));
    }

    _Impl* _get_impl() const noexcept
    {
        return _storage._Ptrs[detail::_Small_Object_Num_Ptrs - 1];
    }

    void _set_impl(_Impl* ptr) noexcept
    {
        _storage._Ptrs[detail::_Small_Object_Num_Ptrs - 1] = ptr;
    }

    bool _empty() const noexcept
    {
        return _get_impl() == nullptr;
    }

    bool _local() const noexcept    // test for locally stored copy of object
    {
        return _get_impl() == static_cast<const void*>(&_storage);  // true ONLY for small obj
    }

    void _clean_up_storage() noexcept   // destroy callable object and maybe delete it
    {
        if (!_empty())
        {
            _get_impl()->_delete_this(!_local());   // dealloc only if !small
            _set_impl(nullptr);
        }
    }

    const void* _target(const std::type_info& info) const noexcept
    {
        return _get_impl() ? _get_impl()->_target(info) : nullptr;
    }
};  // END function

#else   // CUSTOM_OPTIMAL_IMPLEMENTATION

template<class Ret, class... Args>
class function<Ret(Args...)>                // wrapper for callable objects
{
public:
    using result_type   = Ret;
    using _Impl         = detail::_Callable_Interface<Ret, Args...>;

private:
    unique_ptr<_Impl> _storage;

public:
    // Constructors

    function() noexcept { /*Empty*/}

    function(std::nullptr_t) noexcept { /*Empty*/ }

    function(const function& other) : _storage(other._storage->_copy()) { /*Empty*/ }

    template<class Callable, enable_if_t<!is_same_v<decay_t<Callable>, function>, bool> = true>
    function(Callable&& val) : _storage(nullptr)
    {
        _reset(custom::forward<Callable>(val));
    }

    function(function&& other) noexcept = default;
    ~function() noexcept                = default;

public:
    // Operators

    function& operator=(std::nullptr_t) noexcept
    {
        _storage.reset();
        return *this;
    }

    function& operator=(const function& other)
    {
        if (_storage != other._storage)
            _storage.reset(other._storage->_copy());

        return *this;
    }

    function& operator=(function&& other) noexcept = default;

    template<class Callable, enable_if_t<!is_same_v<decay_t<Callable>, function>, bool> = true>
    function& operator=(Callable&& val)
    {
        _reset(custom::forward<Callable>(val));
        return *this;
    }

    template<class Callable>
    function& operator=(reference_wrapper<Callable> refVal) noexcept
    {
        _reset(refVal);
        return *this;
    }

    result_type operator()(Args... args) const
    {
        if (!_storage)
            throw custom::bad_function_call();

        return _storage->_call(custom::forward<Args>(args)...);
    }

    explicit operator bool() const noexcept
    {
        return static_cast<bool>(_storage);
    }

public:
    // Main functions

    void swap(function& other) noexcept
    {
        _Impl* temp = _storage.release();
        _storage.reset(other._storage.release());
        other._storage.reset(temp);
    }

    const std::type_info& target_type() const noexcept
    {
        return static_cast<bool>(_storage) ? _storage->_target_type() : typeid(void);
    }

    template<class Callable>
    Callable* target() noexcept
    {
        return reinterpret_cast<Callable*>(const_cast<void*>(_target(typeid(Callable))));
    }

    template<class Callable>
    const Callable* target() const noexcept
    {
        return reinterpret_cast<const Callable*>(_target(typeid(Callable)));
    }

private:
    // Helpers

    template<class Callable>
    void _reset(Callable&& val) // store copy of val
    {
        if (!detail::_test_callable(val))   // null member pointer/function pointer/custom::function
            return;                 // already empty

        using _OtherImpl = detail::_Callable_Impl<decay_t<Callable>, Ret, Args...>;

        _storage.reset(new _OtherImpl(custom::forward<Callable>(val)));
    }

    const void* _target(const std::type_info& info) const noexcept
    {
        return static_cast<bool>(_storage) ?  _storage->_target(info) : nullptr;
    }
};  // END function

#endif  // CUSTOM_OPTIMAL_IMPLEMENTATION

// is_placeholder
template<class Ty>
struct is_placeholder : integral_constant<int, 0> {}; // Ty is not a placeholder

template<int num>
struct is_placeholder<placeholder<num>> : integral_constant<int, num> {}; // placeholder is ok

template<class Ty>
struct is_placeholder<const Ty> : is_placeholder<Ty>::type {}; // ignore cv-qualifiers

template<class Ty>
struct is_placeholder<volatile Ty> : is_placeholder<Ty>::type {}; // ignore cv-qualifiers

template<class Ty>
struct is_placeholder<const volatile Ty> : is_placeholder<Ty>::type {}; // ignore cv-qualifiers

// is_bind_expression
template<class Ty>
struct is_bind_expression : false_type {}; // Ty is not a bind expression

template<class Functor, class... Args>
struct is_bind_expression<binder<Functor, Args...>> : true_type {}; // binder is a bind expression

template<class Ty>
struct is_bind_expression<const Ty> : is_bind_expression<Ty>::type {}; // ignore cv-qualifiers

template<class Ty>
struct is_bind_expression<volatile Ty> : is_bind_expression<Ty>::type {}; // ignore cv-qualifiers

template<class Ty>
struct is_bind_expression<const volatile Ty> : is_bind_expression<Ty>::type {}; // ignore cv-qualifiers

// bind
template<class Functor, class... Args>
class binder
{
private:
    decay_t<Functor>        _functor;
    tuple<decay_t<Args>...> _boundArgs;

public:
    constexpr explicit binder(Functor&& func, Args&&... args)
        :   _functor(custom::forward<Functor>(func)),
            _boundArgs(custom::forward<Args>(args)...) { /*Empty*/ }

// just a shortcut used in operator()
#define _CALL_BINDER                                                            \
    detail::_call_binder(   _functor, _boundArgs, index_sequence_for<Args...>{},  \
                            custom::forward_as_tuple(custom::forward<UnboundArgs>(unboundArgs)...))

    template<class... UnboundArgs>
    constexpr auto operator()(UnboundArgs&&... unboundArgs)
    noexcept(noexcept(_CALL_BINDER))
    -> decltype(_CALL_BINDER)
    {
        return _CALL_BINDER;
    }

    template<class... UnboundArgs>
    constexpr auto operator()(UnboundArgs&&... unboundArgs) const
    noexcept(noexcept(_CALL_BINDER))
    -> decltype(_CALL_BINDER)
    {
        return _CALL_BINDER;
    }
#undef _CALL_BINDER // used only here
};  // END binder

template<class Functor, class... Args>
constexpr binder<Functor, Args...> bind(Functor&& func, Args&&... args)
{
    return binder<Functor, Args...>(custom::forward<Functor>(func), custom::forward<Args>(args)...);
}

CUSTOM_END