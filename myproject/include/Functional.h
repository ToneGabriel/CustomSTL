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


template<class>
class Callable;

// TODO: refactor
template<class Signature>
class Function;

// template<class RetType, class... ArgTypes>
// class Function<RetType(ArgTypes...)>        // Wrapper template to function pointer
// {
// private:
//     constexpr size_t StorageSize = std::max(sizeof(Callable<void(*)()>),
//                                             sizeof(ReferenceWrapper<char>));

//     using Storage_t = custom::AlignedStorage_t<StorageSize>;

// }; // END Function template


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