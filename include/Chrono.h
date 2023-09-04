#pragma once
#include "TypeTraits.h"
#include "Limits.h"
#include "Ratio.h"  // TODO: implement


CUSTOM_BEGIN

#define CUSTOM_CHRONO_BEGIN namespace chrono {
#define CUSTOM_CHRONO_END }

CUSTOM_CHRONO_BEGIN

#pragma region Duration
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


// Duration
template<class RepType, class PeriodRatio = Ratio<1>>
class Duration;

template<class Type>
constexpr bool IsDuration_v = IsSpecialization_v<Type, Duration>;

template<class ToDur, class Rep, class Period, EnableIf_t<IsDuration_v<ToDur>, bool> = true>
constexpr ToDur duration_cast(const Duration<Rep, Period>&) noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<typename ToDur::Rep>);


template<class RepType, class PeriodRatio>
class Duration  // represents a time duration
{
    static_assert(!IsDuration_v<RepType>, "duration can't have duration as first template argument");
    static_assert(IsRatio_v<PeriodRatio>, "period not an instance of Ratio");
    static_assert(0 < PeriodRatio::Num, "period negative or zero");

public:
    using Rep       = RepType;
    using Period    = typename PeriodRatio::Type;

private:
    Rep _rep;       // the stored rep

public:
    // Constructors

    Duration() = default;

    template<class _Rep,
    EnableIf_t< IsConvertible_v<const _Rep&, Rep> && 
                (IsFloatingPoint_v<Rep> || !IsFloatingPoint_v<_Rep>), bool> = true>
    explicit Duration(const _Rep& val) noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<_Rep>)
        : _rep(static_cast<Rep>(val)) { /*Empty*/ }

    template<class _Rep, class _Period,
    EnableIf_t< IsFloatingPoint_v<Rep> ||
                (_RatioDivideSfinae<_Period, Period>::Den == 1 && !IsFloatingPoint_v<_Rep>), bool> = true>
    Duration(const Duration<_Rep, _Period>& other) noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<_Rep>)
        : _rep(custom::chrono::duration_cast<Duration>(other).count()) { /*Empty*/ }

public:
    // Operators

    Duration& operator=(const Duration& other) = default;

    CommonType_t<Duration> operator+() const noexcept(IsArithmetic_v<Rep>) {
        return CommonType_t<Duration>(*this);
    }

    CommonType_t<Duration> operator-() const noexcept(IsArithmetic_v<Rep>) {
        return CommonType_t<Duration>(-_rep);
    }

    Duration& operator++() noexcept(IsArithmetic_v<Rep>) {
        ++_rep;
        return *this;
    }

    Duration operator++(int) noexcept(IsArithmetic_v<Rep>) {
        return Duration(_rep++);
    }

    Duration& operator--() noexcept(IsArithmetic_v<Rep>) {
        --_rep;
        return *this;
    }

    Duration operator--(int) noexcept(IsArithmetic_v<Rep>) {
        return Duration(_rep--);
    }

    Duration& operator+=(const Duration& other) noexcept(IsArithmetic_v<Rep>) {
        _rep += other._rep;
        return *this;
    }

    Duration& operator-=(const Duration& other) noexcept(IsArithmetic_v<Rep>) {
        _rep -= other._rep;
        return *this;
    }

    Duration& operator*=(const Rep& otherRep) noexcept(IsArithmetic_v<Rep>) {
        _rep *= otherRep;
        return *this;
    }

    Duration& operator/=(const Rep& otherRep) noexcept(IsArithmetic_v<Rep>) {
        _rep /= otherRep;
        return *this;
    }

    Duration& operator%=(const Rep& otherRep) noexcept(IsArithmetic_v<Rep>) {
        _rep %= otherRep;
        return *this;
    }

    Duration& operator%=(const Duration& other) noexcept(IsArithmetic_v<Rep>) {
        _rep %= other.count();
        return *this;
    }

public:
    // Main functions

    Rep count() const noexcept(IsArithmetic_v<Rep>) {
        return _rep;
    }

    static constexpr Duration zero() noexcept {
        return Duration(DurationValues<Rep>::zero());
    }

    static constexpr Duration min() noexcept {
        return Duration(DurationValues<Rep>::min());
    }

    static constexpr Duration max() noexcept {
        return Duration(DurationValues<Rep>::max());
    }
};  // END Duration


// duration cast impl
template<class ToDur, class Rep, class Period, EnableIf_t<IsDuration_v<ToDur>, bool> /* = true (redefinition) */>
constexpr ToDur duration_cast(const Duration<Rep, Period>& duration) noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<typename ToDur::Rep>) {
    // convert duration to another duration (truncate)

    using ToDurRep  = typename ToDur::Rep;
    using RatioType = RatioDivide<Period, typename ToDur::Period>;
    using RepType   = CommonType_t<ToDurRep, Rep, intmax_t>;

    // avoid multiplications and divisions where possible (last return is the complete cast)
    if (RatioType::Den == 1)
    {
        if (RatioType::Num == 1)
            return static_cast<ToDur>(static_cast<ToDurRep>(duration.count()));
        else
            return static_cast<ToDur>(static_cast<ToDurRep>(static_cast<RepType>(duration.count()) * static_cast<RepType>(RatioType::Num)));
    }
    else
    {
        if (RatioType::Num == 1)
            return static_cast<ToDur>(static_cast<ToDurRep>(static_cast<RepType>(duration.count()) / static_cast<RepType>(RatioType::Den)));
        else
            return static_cast<ToDur>(static_cast<ToDurRep>(static_cast<RepType>(duration.count()) * static_cast<RepType>(RatioType::Num) / static_cast<RepType>(RatioType::Den)));
    }
}

// floor
// convert duration to another duration; round towards negative infinity
// i.e. the greatest integral result such that the result <= duration
template<class ToDur, class Rep, class Period, EnableIf_t<IsDuration_v<ToDur>, bool> = true>
constexpr ToDur floor(const Duration<Rep, Period>& duration) noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<typename ToDur::Rep>) {
    const ToDur durationCasted = custom::chrono::duration_cast<ToDur>(duration);

    if (durationCasted > duration)
        return ToDur{ durationCasted.count() - static_cast<typename ToDur::Rep>(1) };

    return durationCasted;
}

// ceil
// convert duration to another duration; round towards positive infinity
// i.e. the least integral result such that duration <= the result
template<class ToDur, class Rep, class Period, EnableIf_t<IsDuration_v<ToDur>, bool> = true>
constexpr ToDur ceil(const Duration<Rep, Period>& duration) noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<typename ToDur::Rep>) {
    const ToDur durationCasted = custom::chrono::duration_cast<ToDur>(duration);

    if (durationCasted < duration)
        return ToDur{ durationCasted.count() + static_cast<typename ToDur::Rep>(1) };

    return durationCasted;
}

// round
// convert duration to another duration, round to nearest, ties to even
template<class ToDur, class Rep, class Period,
EnableIf_t<IsDuration_v<ToDur> && !IsFloatingPoint_v<typename ToDur::rep>, bool> = true>
constexpr ToDur round(const Duration<Rep, Period>& duration) noexcept(IsArithmetic_v<Rep>&& IsArithmetic_v<typename ToDur::Rep>) {
    ToDur durationFloored   = custom::chrono::floor<ToDur>(duration);
    ToDur durationCeiled    = durationFloored + ToDur{ 1 };
    auto floorAdjustment    = duration - durationFloored;
    auto ceilAdjustment     = durationCeiled - duration;

    if (floorAdjustment < ceilAdjustment ||
        (floorAdjustment == ceilAdjustment && durationFloored.count() % 2 == 0))
        return durationFloored;

    return durationCeiled;
}

// abs
// create a duration whose count() is the absolute value of duration.count()
template<class Rep, class Period, EnableIf_t<NumericLimits<Rep>::IsSigned, bool> = true>
constexpr Duration<Rep, Period> abs(const Duration<Rep, Period> duration) noexcept(IsArithmetic_v<Rep>) {

    if (duration < Duration<Rep, Period>::zero())
        return Duration<Rep, Period>::zero() - duration;
    else
        return duration;
}
#pragma endregion Duration


#pragma region TimePoint
// TimePoint
template<class ClockType, class DurationType = typename ClockType::Duration>
class TimePoint    // represents a point in time
{
    static_assert(IsDuration_v<DurationType>, "Duration needs to be a specialization of chrono::Duration.");

public:
    using Clock     = ClockType;
    using Duration  = DurationType;
    using Rep       = typename DurationType::Rep;
    using Period    = typename DurationType::Period;

private:
    Duration _dur   = Duration::zero();   // duration since the epoch

public:
    // Constructors

    TimePoint() = default;

    explicit TimePoint(const Duration& val) noexcept(IsArithmetic_v<Rep>)
        : _dur(val) { /*Empty*/ }

    template<class _Duration, EnableIf_t<IsConvertible_v<_Duration, Duration>, bool> = true>
    TimePoint(const TimePoint<Clock, _Duration>& other) noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<typename _Duration::Rep>)
        : _dur(other.time_since_epoch()) { /*Empty*/ }

public:
    // Operators

    TimePoint& operator++() noexcept(IsArithmetic_v<Rep>) {
        ++_dur;
        return *this;
    }

    TimePoint operator++(int) noexcept(IsArithmetic_v<Rep>) {
        return TimePoint(_dur++);
    }

    TimePoint& operator--() noexcept(IsArithmetic_v<Rep>) {
        --_dur;
        return *this;
    }

    TimePoint operator--(int) noexcept(IsArithmetic_v<Rep>) {
        return TimePoint(_dur--);
    }

    TimePoint& operator+=(const Duration& otherDuration) noexcept(IsArithmetic_v<Rep>) {
        _dur += otherDuration;
        return *this;
    }

    TimePoint& operator-=(const Duration& otherDuration) noexcept(IsArithmetic_v<Rep>) {
        _dur -= otherDuration;
        return *this;
    }

public:
    // Main functions

    Duration time_since_epoch() const noexcept(IsArithmetic_v<Rep>) {
        return _dur;
    }

    static constexpr TimePoint min() noexcept {
        return TimePoint(Duration::min());
    }

    static constexpr TimePoint max() noexcept {
        return TimePoint(Duration::max());
    }

};  // END TimePoint
#pragma endregion TimePoint


#pragma region others
// is clock
template<class Clock, class = void>
constexpr bool _IsClock_v = false;

template<class Clock>
constexpr bool _IsClock_v<Clock, Void_t<typename Clock::Rep,
                                        typename Clock::Period,
                                        typename Clock::Duration,
                                        typename Clock::TimePoint,
                                        decltype(Clock::IsSteady),
                                        decltype(Clock::now())>> = true;

template<class Clock>
struct IsClock : BoolConstant<_IsClock_v<Clock>> {};

template<class Clock>
constexpr bool IsClock_v = IsClock::Value;


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
#pragma endregion others

CUSTOM_CHRONO_END

CUSTOM_END