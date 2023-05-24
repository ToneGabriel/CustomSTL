#pragma once
#include "Utility.h"
#include "Tuple.h"


CUSTOM_BEGIN

// TODO: refactor
// This only works with normal function pointers
template<class Signature>
class Function;

template<class RetType, class... ArgTypes>
class Function<RetType(ArgTypes...)>        // Wrapper template to function pointer
{
private:
    using Invoker           = RetType(*)(const Function&, ArgTypes&&...);
    using Copiator          = void*(*)(const Function&);
    using Destructor        = void(*)(const Function&);

private:
    void* _functor          = nullptr;
    Invoker _invoker        = nullptr;
    Copiator _copiator      = nullptr;
    Destructor _destructor  = nullptr;

private:
    // Core functions

    template<class Functor>
    static RetType _invoke_impl(const Function& target, ArgTypes&&... args) {
        return (*reinterpret_cast<Functor*>(target._functor))(custom::forward<ArgTypes>(args)...);
    }

    template<class Functor>
    static void* _copy_impl(const Function& target) {
        return reinterpret_cast<void*>(new Functor(*reinterpret_cast<Functor*>(target._functor)));
    }

    template<class Functor>
    static void _destruct_impl(const Function& target) {
        delete reinterpret_cast<Functor*>(target._functor);
    }

public:
    // Constructors

    Function() = default;

    template<class Functor, EnableIf_t<!IsSame<Decay_t<Functor>, Function>::Value, bool> = true>
    Function(Functor&& f) {
        using FunctorType = Decay_t<Functor>;

        _invoker    = reinterpret_cast<Invoker>(_invoke_impl<FunctorType>);
        _copiator   = reinterpret_cast<Copiator>(_copy_impl<FunctorType>);
        _destructor = reinterpret_cast<Destructor>(_destruct_impl<FunctorType>);
        _functor    = reinterpret_cast<void*>(new FunctorType(custom::forward<Functor>(f)));
    }

    Function(const Function& other) {
        _copy(other);
    }

    Function(Function&& other) noexcept {
        _move(custom::move(other));
    }

    ~Function() {
        _clear();
    }
    
public:
    // Operators

    RetType operator()(ArgTypes&&... args) const {
        if (_functor == nullptr) 
            throw std::runtime_error("call an empty function object");
        
        return _invoker(*this, custom::forward<ArgTypes>(args)...);
    }

    template<class Functor, EnableIf_t<!IsSame<Decay_t<Functor>, Function>::Value, bool> = true>
    Function& operator=(Functor&& func) {
        using FunctorType = Decay_t<Functor>;

        _clear();
        _invoker    = reinterpret_cast<Invoker>(_invoke_impl<FunctorType>);
        _copiator   = reinterpret_cast<Copiator>(_copy_impl<FunctorType>);
        _destructor = reinterpret_cast<Destructor>(_destruct_impl<FunctorType>);
        _functor    = reinterpret_cast<void*>(new FunctorType(custom::forward<Functor>(func)));

        return *this;
    }

    Function& operator=(const Function& other) {
        if (_functor != other._functor)
        {
            _clear();
            _copy(other);
        }

        return *this;
    }

    Function& operator=(Function&& other) {
        if (_functor != other._functor)
        {
            _clear();
            _move(custom::move(other));
        }

        return *this;
    }

    explicit operator bool() const noexcept {
        if (_functor != nullptr)
            return true;

        return false;
    }

private:
    // Helpers

    void _clear() {
        if (_functor != nullptr)
        {
            _destructor(*this);
            _functor = nullptr;
        }
    }

    void _copy(const Function& other) {
        _invoker        = other._invoker;
        _copiator       = other._copiator;
        _destructor     = other._destructor;
        _functor        = other._copiator(other);
    }

    void _move(Function&& other) {
        _invoker        = other._invoker;
        _copiator       = other._copiator;
        _destructor     = other._destructor;
        _functor        = other._functor;

        other._functor  = nullptr;
    }
}; // END Function template


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
            bool = IsBindExpression_v<BoundArgType>,        // TODO: not implemented
            int = IsPlaceholder_v<BoundArgType>>
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
    template<class UnboundTuple>
    static constexpr auto fix(BoundArgType& boundArg, UnboundTuple&& unboundTuple) noexcept
    -> decltype(auto) {
        // TODO: implement
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

CUSTOM_END