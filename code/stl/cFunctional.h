#pragma once
#include "xFunctional.h"
#include "cUtility.h"
#include "cTuple.h"
#include "cMemory.h"


CUSTOM_BEGIN

#pragma region Unary/Binary Operations

// unary function
template<class Arg, class Res>
struct UnaryFunction
{
    using ArgumentType  = Arg;
    using ResultType    = Res;
};

// binary function
template<class Arg1, class Arg2, class Res>
struct BinaryFunction
{
    using FirstArgumentType     = Arg1;
    using SecondArgumentType    = Arg2;
    using ResultType            = Res;
};

// equal to
template<class Ty = void>
struct EqualTo : public BinaryFunction<Ty, Ty, bool>
{
    constexpr bool operator()(const Ty& left, const Ty& right) const {
        return left == right;
    }
};

template<>
struct EqualTo<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) == custom::forward<Ty2>(right)))
    -> decltype(custom::forward<Ty1>(left) == custom::forward<Ty2>(right)) {
        return custom::forward<Ty1>(left) == custom::forward<Ty2>(right);
    }

    using IsTransparent = int;
};

// not equal to
template<class Ty = void>
struct NotEqualTo : public BinaryFunction<Ty, Ty, bool>
{
    constexpr bool operator()(const Ty& left, const Ty& right) const {
        return left != right;
    }
};

template<>
struct NotEqualTo<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) != custom::forward<Ty2>(right)))
    -> decltype(custom::forward<Ty1>(left) != custom::forward<Ty2>(right)) {
        return custom::forward<Ty1>(left) != custom::forward<Ty2>(right);
    }

    using IsTransparent = int;
};

// greater
template<class Ty = void>
struct Greater : public BinaryFunction<Ty, Ty, bool>
{
    constexpr bool operator()(const Ty& left, const Ty& right) const {
        return left > right;
    }
};

template<>
struct Greater<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) > custom::forward<Ty2>(right)))
    -> decltype(custom::forward<Ty1>(left) > custom::forward<Ty2>(right)) {
        return custom::forward<Ty1>(left) > custom::forward<Ty2>(right);
    }

    using IsTransparent = int;
};

// greater equal
template<class Ty = void>
struct GreaterEqual : public BinaryFunction<Ty, Ty, bool>
{
    constexpr bool operator()(const Ty& left, const Ty& right) const {
        return left >= right;
    }
};

template<>
struct GreaterEqual<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) >= custom::forward<Ty2>(right)))
    -> decltype(custom::forward<Ty1>(left) >= custom::forward<Ty2>(right)) {
        return custom::forward<Ty1>(left) >= custom::forward<Ty2>(right);
    }

    using IsTransparent = int;
};

// less
template<class Ty = void>
struct Less : public BinaryFunction<Ty, Ty, bool>
{
    constexpr bool operator()(const Ty& left, const Ty& right) const {
        return left < right;
    }
};

template<>
struct Less<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) < custom::forward<Ty2>(right)))
    -> decltype(custom::forward<Ty1>(left) < custom::forward<Ty2>(right)) {
        return custom::forward<Ty1>(left) < custom::forward<Ty2>(right);
    }

    using IsTransparent = int;
};

// less equal
template<class Ty = void>
struct LessEqual : public BinaryFunction<Ty, Ty, bool>
{
    constexpr bool operator()(const Ty& left, const Ty& right) const {
        return left <= right;
    }
};

template<>
struct LessEqual<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) <= custom::forward<Ty2>(right)))
    -> decltype(custom::forward<Ty1>(left) <= custom::forward<Ty2>(right)) {
        return custom::forward<Ty1>(left) <= custom::forward<Ty2>(right);
    }

    using IsTransparent = int;
};

// plus
template<class Ty = void>
struct Plus : public BinaryFunction<Ty, Ty, Ty>
{
    constexpr Ty operator()(const Ty& left, const Ty& right) const {
        return left + right;
    }
};

template<>
struct Plus<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) + custom::forward<Ty1>(right)))
    -> decltype(custom::forward<Ty1>(left) + custom::forward<Ty1>(right)) {
        return custom::forward<Ty1>(left) + custom::forward<Ty1>(right);
    }

    using IsTransparent = int;
};

// minus
template<class Ty = void>
struct Minus : public BinaryFunction<Ty, Ty, Ty>
{
    constexpr Ty operator()(const Ty& left, const Ty& right) const {
        return left - right;
    }
};

template<>
struct Minus<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) - custom::forward<Ty1>(right)))
    -> decltype(custom::forward<Ty1>(left) - custom::forward<Ty1>(right)) {
        return custom::forward<Ty1>(left) - custom::forward<Ty1>(right);
    }

    using IsTransparent = int;
};

// multiplies
template<class Ty = void>
struct Multiplies : public BinaryFunction<Ty, Ty, Ty>
{
    constexpr Ty operator()(const Ty& left, const Ty& right) const {
        return left * right;
    }
};

template<>
struct Multiplies<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) * custom::forward<Ty1>(right)))
    -> decltype(custom::forward<Ty1>(left) * custom::forward<Ty1>(right)) {
        return custom::forward<Ty1>(left) * custom::forward<Ty1>(right);
    }

    using IsTransparent = int;
};

// divides
template<class Ty = void>
struct Divides : public BinaryFunction<Ty, Ty, Ty>
{
    constexpr Ty operator()(const Ty& left, const Ty& right) const {
        return left / right;
    }
};

template<>
struct Divides<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) / custom::forward<Ty1>(right)))
    -> decltype(custom::forward<Ty1>(left) / custom::forward<Ty1>(right)) {
        return custom::forward<Ty1>(left) / custom::forward<Ty1>(right);
    }

    using IsTransparent = int;
};

// modulus
template<class Ty = void>
struct Modulus : public BinaryFunction<Ty, Ty, Ty>
{
    constexpr Ty operator()(const Ty& left, const Ty& right) const {
        return left % right;
    }
};

template<>
struct Modulus<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& left, Ty2&& right) const
    noexcept(noexcept(custom::forward<Ty1>(left) % custom::forward<Ty1>(right)))
    -> decltype(custom::forward<Ty1>(left) % custom::forward<Ty1>(right)) {
        return custom::forward<Ty1>(left) % custom::forward<Ty1>(right);
    }

    using IsTransparent = int;
};

// negate
template<class Ty = void>
struct Negate : public UnaryFunction<Ty, Ty>
{
    constexpr Ty operator()(const Ty& val) const {
        return -val;
    }
};

template<>
struct Negate<void>
{
    template<class Ty1, class Ty2>
    constexpr auto operator()(Ty1&& val) const
    noexcept(noexcept(-custom::forward<Ty1>(val)))
    -> decltype(-custom::forward<Ty1>(val)) {
        return -custom::forward<Ty1>(val);
    }

    using IsTransparent = int;
};

#pragma endregion Unary/Binary Operations


#pragma region Function

template<class Signature>
class Function;

class BadFunctionCall : public std::exception // exception thrown when an empty custom::Function is called
{
public:
    BadFunctionCall() noexcept { /*Empty*/ }

    const char* what() const noexcept override {
        return "Bad function call.";
    }
};  // END BadFunctionCall

template<class Callable>
constexpr bool _TestableCallable_v = Disjunction_v<IsPointer<Callable>,
                                                   IsSpecialization<Callable, Function>,
                                                   IsMemberPointer<Callable>>;

template<class Callable>
bool _test_callable(const Callable& obj) noexcept { // determine whether custom::Function must store obj
    if constexpr (_TestableCallable_v<Callable>)
        return !!obj;   // obj != nullptr;
    else
        return true;
}


#if CUSTOM_OPTIMAL_IMPLEMENTATION   // store callable object impl in stack or heap depending on size

static constexpr int _SmallObjectNumPtrs = 6 + 16 / sizeof(void*);
static constexpr size_t _SpaceSize = (_SmallObjectNumPtrs - 1) * sizeof(void*);

template<class Impl>    // when _CallableImpl wraps an object(big) that has operator()
static constexpr bool _IsLarge =    sizeof(Impl) > _SpaceSize ||
                                    alignof(Impl) > alignof(MaxAlign_t) ||
                                    !Impl::_NothrowMove::Value;


template<class RetType, class... Args>
class CUSTOM_NOVTABLE_ATTR _CallableInterface
{
public:
    _CallableInterface()                                        = default;
    _CallableInterface(const _CallableInterface&)               = delete;
    _CallableInterface& operator=(const _CallableInterface&)    = delete;
    // destructor non-virtual due to _delete_this()

    virtual _CallableInterface* _copy(void*) const              = 0;
    virtual _CallableInterface* _move(void*) noexcept           = 0;
    virtual RetType _call(Args&&...)                            = 0;
    virtual const std::type_info& _target_type() const noexcept = 0;
    virtual void _delete_this(bool) noexcept                    = 0;

    const void* _target(const std::type_info& info) const noexcept {
        return _target_type() == info ? _get() : nullptr;
    }

private:
    virtual const void* _get() const noexcept                   = 0;
};  // END _CallableInterface


template<class Callable, class RetType, class... Args>
class _CallableImpl final : public _CallableInterface<RetType, Args...>
{
public:
    using _Base         = _CallableInterface<RetType, Args...>;
    using _NothrowMove  = IsNothrowMoveConstructible<Callable>;

private:
    Callable _callable;

public:
    template<class OtherCallable,
    EnableIf_t<!IsSame_v<Decay_t<OtherCallable>, _CallableImpl>, bool> = true>
    explicit _CallableImpl(OtherCallable&& val)
        : _callable(custom::forward<OtherCallable>(val)) { /*Empty*/ }

private:
    _Base* _copy(void* address) const override {
        if constexpr (_IsLarge<_CallableImpl>)
            return new _CallableImpl(_callable);
        else
            return ::new(address) _CallableImpl(_callable);
    }

    _Base* _move(void* address) noexcept override {
        if constexpr (_IsLarge<_CallableImpl>)
            return nullptr;
        else
            return ::new(address) _CallableImpl(custom::move(_callable));
    }

    RetType _call(Args&&... args) override {
        if constexpr (IsVoid_v<RetType>)
            (void)custom::invoke(_callable, custom::forward<Args>(args)...);
        else
            return custom::invoke(_callable, custom::forward<Args>(args)...);
    }

    const std::type_info& _target_type() const noexcept override {
        return typeid(Callable);
    }

    const void* _get() const noexcept override {
        return &_callable;
    }

    void _delete_this(bool dealloc) noexcept override { // destroy self
        this->~_CallableImpl();

        if (dealloc)
            ::operator delete(this, sizeof(_CallableImpl));
    }
};  // END _CallableImpl


template<class RetType, class... Args>
class Function<RetType(Args...)>                // wrapper for callable objects
{
public:
    using ResultType    = RetType;
    using Impl          = _CallableInterface<RetType, Args...>;

private:
    union _Storage  // storage for small objects
    {
        MaxAlign_t _Val;                        // for maximum alignment
        char Pad[_SpaceSize];                   // to permit aliasing
        Impl* _Ptrs[_SmallObjectNumPtrs];       // _Ptrs[_SmallObjectNumPtrs - 1] is reserved
    };

    _Storage _storage;

public:
    // Constructors

    Function() noexcept {
        _set_impl(nullptr);
    }

    Function(std::nullptr_t) noexcept : Function() { /*Empty*/ }

    Function(const Function& other) {
        _reset_copy(other);
    }

    Function(Function&& other) noexcept {
        _reset_move(custom::move(other));
    }

    template<class Callable, EnableIf_t<!IsSame_v<Decay_t<Callable>, Function>, bool> = true>
    Function(Callable&& val) {
        _reset(custom::forward<Callable>(val));
    }

    ~Function() noexcept {
        _clean_up_storage();
    }

public:
    // Operators

    Function& operator=(std::nullptr_t) noexcept {
        _clean_up_storage();
        return *this;
    }

    Function& operator=(const Function& other) {
        Function(other).swap(*this);
        return *this;
    }

    Function& operator=(Function&& other) noexcept {
        if (this != &other)
        {
            _clean_up_storage();
            _reset_move(custom::move(other));
        }

        return *this;
    }

    template<class Callable, EnableIf_t<!IsSame_v<Decay_t<Callable>, Function>, bool> = true>
    Function& operator=(Callable&& val) {
        Function(custom::forward<Callable>(val)).swap(*this);
        return *this;
    }

    template<class Callable>
    Function& operator=(ReferenceWrapper<Callable> refVal) noexcept {
        _clean_up_storage();
        _reset(refVal);
        return *this;
    }

    ResultType operator()(Args... args) const {
        if (_empty())
            throw custom::BadFunctionCall();

        return _get_impl()->_call(custom::forward<Args>(args)...);
    }

    explicit operator bool() const noexcept {
        return !_empty();
    }

public:
    // Main functions

    void swap(Function& other) noexcept {
        if (!_local() && !other._local())   // just swap pointers
        {
            auto _Temp = _get_impl();
            _set_impl(other._get_impl());
            other._set_impl(_Temp);
        }
        else                                // do three-way move
        {
            Function _Temp;
            _Temp._reset_move(custom::move(*this));
            _reset_move(custom::move(other));
            other._reset_move(custom::move(_Temp));
        }
    }

    const std::type_info& target_type() const noexcept {
        return _get_impl() ? _get_impl()->_target_type() : typeid(void);
    }

    template<class Callable>
    Callable* target() noexcept {
        return reinterpret_cast<Callable*>(const_cast<void*>(_target(typeid(Callable))));
    }

    template<class Callable>
    const Callable* target() const noexcept {
        return reinterpret_cast<const Callable*>(_target(typeid(Callable)));
    }

private:
    // Helpers

    void _reset_copy(const Function& other) {           // copy other's stored object
        if (!other._empty())
            _set_impl(other._get_impl()->_copy(&_storage));  // copy to _storage[0]
    }

    void _reset_move(Function&& other) noexcept {       // move other's stored object
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
    void _reset(Callable&& val) { // store copy of val
        if (!_test_callable(val))   // null member pointer/function pointer/custom::Function
            return;                 // already empty

        using OtherImpl = _CallableImpl<Decay_t<Callable>, RetType, Args...>;

        if constexpr (_IsLarge<OtherImpl>)  // dynamically allocate val
            _set_impl(new OtherImpl(custom::forward<Callable>(val)));
        else                                // store val in-situ
            _set_impl(::new(&_storage) OtherImpl(custom::forward<Callable>(val)));
    }

    Impl* _get_impl() const noexcept {
        return _storage._Ptrs[_SmallObjectNumPtrs - 1];
    }

    void _set_impl(Impl* ptr) noexcept {
        _storage._Ptrs[_SmallObjectNumPtrs - 1] = ptr;
    }

    bool _empty() const noexcept {
        return _get_impl() == nullptr;
    }

    bool _local() const noexcept {                  // test for locally stored copy of object
        return _get_impl() == static_cast<const void*>(&_storage);  // true ONLY for small obj
    }

    void _clean_up_storage() noexcept {             // destroy callable object and maybe delete it
        if (!_empty())
        {
            _get_impl()->_delete_this(!_local());   // dealloc only if !small
            _set_impl(nullptr);
        }
    }

    const void* _target(const std::type_info& info) const noexcept {
        return _get_impl() ? _get_impl()->_target(info) : nullptr;
    }
};  // END Function

#else   // CUSTOM_OPTIMAL_IMPLEMENTATION - store callable object impl in heap regardless of size

template<class RetType, class... Args>
class CUSTOM_NOVTABLE_ATTR _CallableInterface
{
public:
    _CallableInterface()                                        = default;
    _CallableInterface(const _CallableInterface&)               = delete;
    _CallableInterface& operator=(const _CallableInterface&)    = delete;

    virtual ~_CallableInterface() {}
    virtual _CallableInterface* _copy() const                   = 0;
    virtual RetType _call(Args&&...)                            = 0;
    virtual const std::type_info& _target_type() const noexcept = 0;

    const void* _target(const std::type_info& info) const noexcept {
        return _target_type() == info ? _get() : nullptr;
    }

private:
    virtual const void* _get() const noexcept                   = 0;
};  // END _CallableInterface


template<class Callable, class RetType, class... Args>
class _CallableImpl final : public _CallableInterface<RetType, Args...>
{
private:
    using _Base = _CallableInterface<RetType, Args...>;

    Callable _callable;

public:

    template<class OtherCallable,
    EnableIf_t<!IsSame_v<Decay_t<OtherCallable>, _CallableImpl>, bool> = true>
    explicit _CallableImpl(OtherCallable&& val)
        : _callable(custom::forward<OtherCallable>(val)) { /*Empty*/ }

private:

    _Base* _copy() const override {
        return new _CallableImpl(_callable);
    }

    RetType _call(Args&&... args) override {
        if constexpr (IsVoid_v<RetType>)
            (void)custom::invoke(_callable, custom::forward<Args>(args)...);
        else
            return custom::invoke(_callable, custom::forward<Args>(args)...);
    }

    const std::type_info& _target_type() const noexcept override {
        return typeid(Callable);
    }

    const void* _get() const noexcept override {
        return &_callable;
    }
};  // END _CallableImpl


template<class RetType, class... Args>
class Function<RetType(Args...)>                // wrapper for callable objects
{
public:
    using ResultType    = RetType;
    using Impl          = _CallableInterface<RetType, Args...>;

private:
    UniquePtr<Impl> _storage;

public:
    // Constructors

    Function() noexcept { /*Empty*/}

    Function(std::nullptr_t) noexcept { /*Empty*/ }

    Function(const Function& other) : _storage(other._storage->_copy()) { /*Empty*/ }

    template<class Callable, EnableIf_t<!IsSame_v<Decay_t<Callable>, Function>, bool> = true>
    Function(Callable&& val) : _storage(nullptr) {
        _reset(custom::forward<Callable>(val));
    }

    Function(Function&& other) noexcept = default;
    ~Function() noexcept                = default;

public:
    // Operators

    Function& operator=(std::nullptr_t) noexcept {
        _storage.reset();
        return *this;
    }

    Function& operator=(const Function& other) {
        if (_storage != other._storage)
            _storage.reset(other._storage->_copy());

        return *this;
    }

    Function& operator=(Function&& other) noexcept = default;

    template<class Callable, EnableIf_t<!IsSame_v<Decay_t<Callable>, Function>, bool> = true>
    Function& operator=(Callable&& val) {
        _reset(custom::forward<Callable>(val));
        return *this;
    }

    template<class Callable>
    Function& operator=(ReferenceWrapper<Callable> refVal) noexcept {
        _reset(refVal);
        return *this;
    }

    ResultType operator()(Args... args) const {
        if (!_storage)
            throw custom::BadFunctionCall();

        return _storage->_call(custom::forward<Args>(args)...);
    }

    explicit operator bool() const noexcept {
        return static_cast<bool>(_storage);
    }

public:
    // Main functions

    void swap(Function& other) noexcept {
        Impl* temp = _storage.release();
        _storage.reset(other._storage.release());
        other._storage.reset(temp);
    }

    const std::type_info& target_type() const noexcept {
        return static_cast<bool>(_storage) ? _storage->_target_type() : typeid(void);
    }

    template<class Callable>
    Callable* target() noexcept {
        return reinterpret_cast<Callable*>(const_cast<void*>(_target(typeid(Callable))));
    }

    template<class Callable>
    const Callable* target() const noexcept {
        return reinterpret_cast<const Callable*>(_target(typeid(Callable)));
    }

private:
    // Helpers

    template<class Callable>
    void _reset(Callable&& val) { // store copy of val
        if (!_test_callable(val))   // null member pointer/function pointer/custom::Function
            return;                 // already empty

        using OtherImpl = _CallableImpl<Decay_t<Callable>, RetType, Args...>;

        _storage.reset(new OtherImpl(custom::forward<Callable>(val)));
    }

    const void* _target(const std::type_info& info) const noexcept {
        return static_cast<bool>(_storage) ?  _storage->_target(info) : nullptr;
    }
};  // END Function

#endif  // CUSTOM_OPTIMAL_IMPLEMENTATION
#pragma endregion Function


#pragma region Bind

// placeholder
template<int Num>
struct Placeholder { static_assert(Num > 0, "invalid placeholder index"); };

namespace placeholders
{
    constexpr Placeholder<1> _1;
    constexpr Placeholder<2> _2;
    constexpr Placeholder<3> _3;
    constexpr Placeholder<4> _4;
    constexpr Placeholder<5> _5;
    constexpr Placeholder<6> _6;
    constexpr Placeholder<7> _7;
    constexpr Placeholder<8> _8;
    constexpr Placeholder<9> _9;
    constexpr Placeholder<10> _10;
    constexpr Placeholder<11> _11;
    constexpr Placeholder<12> _12;
    constexpr Placeholder<13> _13;
    constexpr Placeholder<14> _14;
    constexpr Placeholder<15> _15;
    constexpr Placeholder<16> _16;
    constexpr Placeholder<17> _17;
    constexpr Placeholder<18> _18;
    constexpr Placeholder<19> _19;
    constexpr Placeholder<20> _20;
}

template<class Ty>
struct IsPlaceholder : IntegralConstant<int, 0> {}; // Ty is not a placeholder

template<int Num>
struct IsPlaceholder<Placeholder<Num>> : IntegralConstant<int, Num> {}; // Placeholder is ok

template<class Ty>
struct IsPlaceholder<const Ty> : IsPlaceholder<Ty>::Type {}; // ignore cv-qualifiers

template<class Ty>
struct IsPlaceholder<volatile Ty> : IsPlaceholder<Ty>::Type {}; // ignore cv-qualifiers

template<class Ty>
struct IsPlaceholder<const volatile Ty> : IsPlaceholder<Ty>::Type {}; // ignore cv-qualifiers

template<class Ty>
constexpr int IsPlaceholder_v = IsPlaceholder<Ty>::Value;

// bind helpers
template<class Functor, class... Args>
class Binder;

template<class Ty>
struct IsBindExpression : FalseType {}; // Ty is not a bind expression

template<class Functor, class... Args>
struct IsBindExpression<Binder<Functor, Args...>> : TrueType {}; // Binder is a bind expression

template<class Ty>
struct IsBindExpression<const Ty> : IsBindExpression<Ty>::Type {}; // ignore cv-qualifiers

template<class Ty>
struct IsBindExpression<volatile Ty> : IsBindExpression<Ty>::Type {}; // ignore cv-qualifiers

template<class Ty>
struct IsBindExpression<const volatile Ty> : IsBindExpression<Ty>::Type {}; // ignore cv-qualifiers

template<class Ty>
constexpr bool IsBindExpression_v = IsBindExpression<Ty>::Value;


CUSTOM_DETAIL_BEGIN

// bind fixers & callers
template<   class BoundArgType,
            bool = IsSpecialization_v<RemoveCV_t<BoundArgType>, ReferenceWrapper>,
            bool = IsBindExpression_v<BoundArgType>,
            int  = IsPlaceholder_v<BoundArgType>>
struct _BoundArgFixer;

template<class BoundArgType>
struct _BoundArgFixer<BoundArgType, true, false, 0>         // ReferenceWrapper fixer
{
    template<class UnboundTuple>
    static constexpr typename BoundArgType::Type& fix(BoundArgType& boundArg, UnboundTuple&&) noexcept {
        // unwrap a ReferenceWrapper
        return boundArg.get();
    }
};

template<class BoundArgType>
struct _BoundArgFixer<BoundArgType, false, false, 0>        // identity fixer
{
    template<class UnboundTuple>
    static constexpr BoundArgType& fix(BoundArgType& boundArg, UnboundTuple&&) noexcept {
        // pass a bound argument as an Lvalue
        return boundArg;
    }
};

template<class BoundArgType>
struct _BoundArgFixer<BoundArgType, false, true, 0>         // nested bind fixer
{
private:
    template<class UnboundTuple, size_t... Ix>
    static constexpr auto _fix_impl(BoundArgType& boundArg, UnboundTuple&& unboundTuple, IndexSequence<Ix...>) noexcept
    -> decltype(boundArg(custom::get<Ix>(custom::move(unboundTuple))...)) {
        return boundArg(custom::get<Ix>(custom::move(unboundTuple))...);
    }

public:
    template<class UnboundTuple>
    static constexpr auto fix(BoundArgType& boundArg, UnboundTuple&& unboundTuple) noexcept
    -> decltype(_fix_impl(boundArg, custom::move(unboundTuple), MakeIndexSequence<TupleSize_v<UnboundTuple>>{})) {
        return _fix_impl(boundArg, custom::move(unboundTuple), MakeIndexSequence<TupleSize_v<UnboundTuple>>{});
    }
};

template<class BoundArgType, int Ix>
struct _BoundArgFixer<BoundArgType, false, false, Ix>      // placeholder fixer
{
    static_assert(Ix > 0, "invalid IsPlaceholder value");

    template<class UnboundTuple>
    static constexpr auto fix(BoundArgType&, UnboundTuple&& unboundTuple) noexcept
    -> decltype(custom::get<Ix - 1>(custom::move(unboundTuple))) {
        // choose the Ix'th unbound argument (1-based indexing)
        return custom::get<Ix - 1>(custom::move(unboundTuple));
    }
};

template<class BoundArgType, class UnboundTuple>
constexpr auto _fix_one_arg(BoundArgType& boundArg, UnboundTuple&& unboundTuple) noexcept
-> decltype(_BoundArgFixer<BoundArgType>::fix(boundArg, custom::move(unboundTuple))) {
    // translate an argument for bind
    return _BoundArgFixer<BoundArgType>::fix(boundArg, custom::move(unboundTuple));
}

template<class Functor, class BoundTuple, size_t... Ix, class UnboundTuple>
constexpr auto _call_binder(Functor& func, BoundTuple& boundTuple, IndexSequence<Ix...>, UnboundTuple&& unboundTuple) noexcept
-> decltype(custom::invoke(func, _fix_one_arg(custom::get<Ix>(boundTuple), custom::move(unboundTuple))...)) {
    // _fix_one_arg is called for every boundTuple item (YES: unboundTuple is moved every call)
    return custom::invoke(func, _fix_one_arg(custom::get<Ix>(boundTuple), custom::move(unboundTuple))...);
}

CUSTOM_DETAIL_END


// bind
template<class Functor, class... Args>
class Binder
{
private:
    Decay_t<Functor>        _functor;
    Tuple<Decay_t<Args>...> _boundArgs;

public:
    constexpr explicit Binder(Functor&& func, Args&&... args)
        :   _functor(custom::forward<Functor>(func)),
            _boundArgs(custom::forward<Args>(args)...) { /*Empty*/ }

// just a shortcut used in operator()
#define _CALL_BINDER                                                            \
    detail::_call_binder(   _functor, _boundArgs, IndexSequenceFor<Args...>{},  \
                            custom::forward_as_tuple(custom::forward<UnboundArgs>(unboundArgs)...))

    template<class... UnboundArgs>
    constexpr auto operator()(UnboundArgs&&... unboundArgs) noexcept(noexcept(_CALL_BINDER))
    -> decltype(_CALL_BINDER) {
        return _CALL_BINDER;
    }

    template<class... UnboundArgs>
    constexpr auto operator()(UnboundArgs&&... unboundArgs) const noexcept(noexcept(_CALL_BINDER))
    -> decltype(_CALL_BINDER) {
        return _CALL_BINDER;
    }

#undef _CALL_BINDER // used only here
};

template<class Functor, class... Args>
constexpr Binder<Functor, Args...> bind(Functor&& func, Args&&... args) {
    return Binder<Functor, Args...>(custom::forward<Functor>(func), custom::forward<Args>(args)...);
}

#pragma endregion Bind

CUSTOM_END