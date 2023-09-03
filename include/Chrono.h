#pragma once
#include "TypeTraits.h"
#include "Limits.h"
#include "Ratio.h"  // TODO: implement


CUSTOM_BEGIN

#define CUSTOM_CHRONO_BEGIN namespace chrono {
#define CUSTOM_CHRONO_END }

CUSTOM_CHRONO_BEGIN

// duration values
template<class Rep>
struct DurationValues  // gets arithmetic properties of a type
{
    static constexpr Rep zero() noexcept { // get zero value
        return Rep(0);
    }

    static constexpr Rep min() noexcept { // get smallest value
        return NumericLimits<Rep>::lowest();
    }

    static constexpr Rep max () noexcept { // get largest value
        return NumericLimits<Rep>::max();
    }
};

// is clock
//template<class Clock, class = void>
//constexpr bool _IsClock_v = false;
//
//template<class Clock>
//constexpr bool _IsClock_v<Clock, Void_t<  typename Clock::Rep,
//                                          typename Clock::Period,
//                                          typename Clock::Duration,
//                                          typename Clock::TimePoint,
//                                          decltype(Clock::IsSteady),
//                                          decltype(Clock::now())>> = true;
//
//template<class Clock>
//struct IsClock : BoolConstant<_IsClock_v<Clock>> {};
//
//template<class Clock>
//constexpr bool IsClock_v = IsClock::Value;

// Duration
template<class _Rep, class _Period = Ratio<1>>
class Duration;

template<class Type>
constexpr bool IsDuration_v = IsSpecialization_v<Type, Duration>;

template<class To, class _Rep, class _Period, EnableIf_t<IsDuration_v<To>, bool> = true>
constexpr To duration_cast(const Duration<_Rep, _Period>&) noexcept(IsArithmetic_v<_Rep> && IsArithmetic_v<typename To::rep>);
    
template<class _Rep, class _Period>
class Duration  // represents a time duration
{
    static_assert(!IsDuration_v<_Rep>, "duration can't have duration as first template argument");
    static_assert(IsRatio_v<_Period>, "period not an instance of Ratio");
    static_assert(0 < _Period::Num, "period negative or zero");

public:
    using Rep = _Rep;
    using Period = typename _Period::Type;

private:
    _Rep _rep; // the stored rep

public:
    // Constructors

    Duration() = default;

    template<class _Rep2,
    EnableIf_t<  IsConvertible_v<const _Rep2&, _Rep> && 
                    (IsFloatingPoint_v<_Rep> || !IsFloatingPoint_v<_Rep2>),
    bool> = true>
    explicit Duration(const _Rep2& val) noexcept(IsArithmetic_v<_Rep> && IsArithmetic_v<_Rep2>)
        : _rep(static_cast<_Rep>(val)) { /*Empty*/ }

    template<class _Rep2, class _Period2,
    EnableIf_t< IsFloatingPoint_v<_Rep> ||
                (_RatioDivideSfinae<_Period2, _Period>::Den == 1 && !IsFloatingPoint_v<_Rep2>),
    bool> = true>
    constexpr Duration(const Duration<_Rep2, _Period2>& other) noexcept(IsArithmetic_v<_Rep> && IsArithmetic_v<_Rep2>)
        : _rep(custom::chrono::duration_cast<Duration>(other).count()) { /*Empty*/ }

public:
    // Operators

    //constexpr common_type_t<duration> operator+() const
    //    noexcept(IsArithmetic_v<_Rep>) {
    //    return common_type_t<duration>(*this);
    //}

    //constexpr common_type_t<duration> operator-() const
    //    noexcept(IsArithmetic_v<_Rep>) {
    //    return common_type_t<duration>(-_rep);
    //}

    Duration& operator++() noexcept(IsArithmetic_v<_Rep>) {
        ++_rep;
        return *this;
    }

    Duration operator++(int) noexcept(IsArithmetic_v<_Rep>) {
        return Duration(_rep++);
    }

    Duration& operator--() noexcept(IsArithmetic_v<_Rep>) {
        --_rep;
        return *this;
    }

    Duration operator--(int) noexcept(IsArithmetic_v<_Rep>) {
        return Duration(_rep--);
    }

    Duration& operator+=(const Duration& other) noexcept(IsArithmetic_v<_Rep>) {
        _rep += other._rep;
        return *this;
    }

    Duration& operator-=(const Duration& other) noexcept(IsArithmetic_v<_Rep>) {
        _rep -= other._rep;
        return *this;
    }

    Duration& operator*=(const _Rep& other) noexcept(IsArithmetic_v<_Rep>) {
        _rep *= other;
        return *this;
    }

    Duration& operator/=(const _Rep& other) noexcept(IsArithmetic_v<_Rep>) {
        _rep /= other;
        return *this;
    }

    Duration& operator%=(const _Rep& other) noexcept(IsArithmetic_v<_Rep>) {
        _rep %= other;
        return *this;
    }

    Duration& operator%=(const Duration& other) noexcept(IsArithmetic_v<_Rep>) {
        _rep %= other.count();
        return *this;
    }

public:
    // Main functions

    constexpr _Rep count() const noexcept(IsArithmetic_v<_Rep>) {
        return _rep;
    }

    static constexpr Duration zero() noexcept {
        return Duration(DurationValues<_Rep>::zero());
    }

    static constexpr Duration min() noexcept {
        return Duration(DurationValues<_Rep>::min());
    }

    static constexpr Duration max() noexcept {
        return Duration(DurationValues<_Rep>::max());
    }
};


// time units
using Nanoseconds   = Duration<long long, Nano>;
using Microseconds  = Duration<long long, Micro>;
using Milliseconds  = Duration<long long, Milli>;
using Seconds       = Duration<long long>;
using Minutes       = Duration<int, Ratio<60>>;
using Hours         = Duration<int, Ratio<3600>>;

using Days          = Duration<int, RatioMultiply<Ratio<24>, Hours::Period>>;
using Weeks         = Duration<int, RatioMultiply<Ratio<7>, Days::Period>>;
using Years         = Duration<int, RatioMultiply<Ratio<146097, 400>, Days::Period>>;
using Months        = Duration<int, RatioDivide<Years::Period, Ratio<12>>>;

CUSTOM_CHRONO_END

CUSTOM_END