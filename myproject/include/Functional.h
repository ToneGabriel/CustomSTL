#pragma once
#include "xFunctional.h"
#include "Utility.h"
#include "Tuple.h"


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


#pragma region Hash

#ifdef _WIN64
constexpr size_t _FNVOffsetBasis    = 14695981039346656037ULL;
constexpr size_t _FNVPrime          = 1099511628211ULL;
#else
constexpr size_t _FNVOffsetBasis    = 2166136261U;
constexpr size_t _FNVPrime          = 16777619U;
#endif // _WIN64

// accumulate range [first, first + count) into partial FNV-1a hash val
inline size_t _fnv1a_append_bytes(  size_t val,
                                    const unsigned char* const first,
                                    const size_t count) noexcept {

    for (size_t i = 0; i < count; ++i)
    {
        val ^= static_cast<size_t>(first[i]);
        val *= _FNVPrime;
    }

    return val;
}

// accumulate range [first, last) into partial FNV-1a hash val
template<class Ty>
size_t _fnv1a_append_range( const size_t val,
                            const Ty* const first,
                            const Ty* const last) noexcept {

    static_assert(IsTrivial_v<Ty>, "Only trivial types can be directly hashed.");

    const auto firstAsUChar = reinterpret_cast<const unsigned char*>(first);
    const auto lastAsUChar  = reinterpret_cast<const unsigned char*>(last);

    return _fnv1a_append_bytes(val, firstAsUChar, static_cast<size_t>(lastAsUChar - firstAsUChar));
}

// accumulate key into partial FNV-1a hash val
template<class Key>
size_t _fnv1a_append_value( const size_t val,
                            const Key& key) noexcept {

    static_assert(IsTrivial_v<Key>, "Only trivial types can be directly hashed.");
    return _fnv1a_append_bytes(val, &reinterpret_cast<const unsigned char&>(key), sizeof(Key));
}

// bitwise hashes the representation of a key
template<class Key>
size_t _hash_representation(const Key& key) noexcept {
    return _fnv1a_append_value(_FNVOffsetBasis, key);
}

// bitwise hashes the representation of an array
template<class Key>
size_t _hash_array_representation(  const Key* const first,
                                    const size_t count) noexcept {

    static_assert(IsTrivial_v<Key>, "Only trivial types can be directly hashed.");
    return _fnv1a_append_bytes(_FNVOffsetBasis, reinterpret_cast<const unsigned char*>(first), count * sizeof(Key));
}

template<class Key>
struct Hash;

template<class Key, bool Enabled>
struct _BaseHashEnabler  // conditionally enabled Hash base
{
    size_t operator()(const Key& key) const
    noexcept(noexcept(Hash<Key>::compute_hash(key))) {
        return Hash<Key>::compute_hash(key);
    }
};

template<class Key>
struct _BaseHashEnabler<Key, false>  // conditionally disabled Hash base
{
    _BaseHashEnabler()                                      = delete;
    _BaseHashEnabler(const _BaseHashEnabler&)               = delete;
    _BaseHashEnabler(_BaseHashEnabler&&)                    = delete;
    _BaseHashEnabler& operator=(const _BaseHashEnabler&)    = delete;
    _BaseHashEnabler& operator=(_BaseHashEnabler&&)         = delete;
};

template<class Key>
struct Hash : _BaseHashEnabler<Key, (!IsConst_v<Key> &&
                                     !IsVolatile_v<Key> &&
                                     (IsEnum_v<Key> || IsIntegral_v<Key> || IsPointer_v<Key>))>
{
    static size_t compute_hash(const Key& key) noexcept {
        return _hash_representation(key);
    }
};  // END Hash


// Hash specializations
template<>
struct Hash<float>
{
    size_t operator()(const float key) const noexcept {
        return _hash_representation(key == 0.0F ? 0.0F : key);  // map -0 to 0
    }
};

template<>
struct Hash<double>
{
    size_t operator()(const double key) const noexcept {
        return _hash_representation(key == 0.0 ? 0.0 : key);    // map -0 to 0
    }
};

template<>
struct Hash<long double>
{
    size_t operator()(const long double key) const noexcept {
        return _hash_representation(key == 0.0L ? 0.0L : key); // map -0 to 0
    }
};

template<>
struct Hash<std::nullptr_t>
{
    size_t operator()(std::nullptr_t) const noexcept {
        void* voidPtr{};
        return _hash_representation(voidPtr);
    }
};

#pragma endregion Hash


#pragma region Function

template<class Signature>
class Function;

class BadFunctionCall : public std::exception // exception thrown when an empty custom::Function is called
{
public:
    BadFunctionCall() noexcept { /*Empty*/ }

    const char* what() const noexcept override {
        return "Bad function call...";
    }
};  // END BadFunctionCall


static constexpr int _SmallObjectNumPtrs = 6 + 16 / sizeof(void*);
static constexpr size_t _SpaceSize = (_SmallObjectNumPtrs - 1) * sizeof(void*);

template<class Impl>
static constexpr bool _IsLarge =    sizeof(Impl) > _SpaceSize ||
                                    alignof(Impl) > alignof(MaxAlign_t) ||
                                    !Impl::_NothrowMove::Value;

template<class Func>
constexpr bool _TestableCallable_v = Disjunction_v<IsPointer<Func>,
                                                   IsSpecialization<Func, Function>,
                                                   IsMemberPointer<Func>>;

template<class Func>
bool _test_callable(const Func& obj) noexcept { // determine whether custom::Function must store obj
    if constexpr (_TestableCallable_v<Func>)
        return !!obj;   // obj != nullptr;
    else
        return true;
}


template<class RetType, class... Args>
class _CallableInterface
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
    EnableIf_t<!IsSame_v<_CallableImpl, Decay_t<OtherCallable>>, bool> = true>
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

    template<class Func, EnableIf_t<!IsSame_v<Func, Function>, bool> = true>
    Function(Func&& val) {
        _reset(custom::forward<Func>(val));
    }

    ~Function() noexcept {
        _clean_up_storage();
    }

public:
    // Operators

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

    template<class Func, EnableIf_t<!IsSame_v<Func, Function>, bool> = true>
    Function& operator=(Func&& val) {
        Function(custom::forward<Func>(val)).swap(*this);
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

    template<class Func>
    Func* target() noexcept {
        return reinterpret_cast<Func*>(const_cast<void*>(_target(typeid(Func))));
    }

    template<class Func>
    const Func* target() const noexcept {
        return reinterpret_cast<const Func*>(_target(typeid(Func)));
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

    template<class Func>
    void _reset(Func&& val) { // store copy of val
        if (!_test_callable(val))   // null member pointer/function pointer/custom::Function
            return;                 // already empty

        using OtherImpl = _CallableImpl<Decay_t<Func>, RetType, Args...>;

        if constexpr (_IsLarge<OtherImpl>)  // dynamically allocate val
            _set_impl(new OtherImpl(custom::forward<Func>(val)));
        else                                // store val in-situ
            _set_impl(::new(static_cast<void*>(&_storage)) OtherImpl(custom::forward<Func>(val)));
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
auto _call_binder(Functor&& func, BoundTuple&& boundTuple, IndexSequence<Ix...>, UnboundTuple&& unboundTuple)
-> decltype(custom::invoke(  static_cast<Functor&&>(func),
                            _fix_one_arg(custom::get<Ix>(boundTuple), custom::move(unboundTuple))...)) {

    // _fix_one_arg is called for every boundTuple item (YES: unboundTuple is moved every call)
    return custom::invoke(  static_cast<Functor&&>(func),
                            _fix_one_arg(custom::get<Ix>(boundTuple), custom::move(unboundTuple))...);
}

// bind
template<class Functor, class... Args>
class Binder
{
private:
    Decay_t<Functor> _functor;
    Tuple<Decay_t<Args>...> _boundArgs;

public:
    explicit Binder(Functor&& func, Args&&... args)
        : _functor(custom::forward<Functor>(func)), _boundArgs(custom::forward<Args>(args)...) { /*Empty*/ }

    template<class... UnboundArgs>
    auto operator()(UnboundArgs&&... unboundArgs) noexcept
    -> decltype(_call_binder(_functor, _boundArgs, IndexSequenceFor<Args...>{},
                                custom::forward_as_tuple(custom::forward<UnboundArgs>(unboundArgs)...))) {

        return _call_binder(_functor, _boundArgs, IndexSequenceFor<Args...>{},
                                custom::forward_as_tuple(custom::forward<UnboundArgs>(unboundArgs)...));
    }
};

template<class Functor, class... Args>
constexpr Binder<Functor, Args...> bind(Functor&& func, Args&&... args) {
    return Binder<Functor, Args...>(custom::forward<Functor>(func), custom::forward<Args>(args)...);
}

#pragma endregion Bind

CUSTOM_END