#pragma once
#include "TypeTraits.h"
#include "Limits.h"
#include "Ratio.h"

#include <ctime>        // std::time_t

#if defined _WIN32
#include <windows.h>    // for Clock::now()

#define UNIX_EPOCH 116444736000000000LL

inline long long _get_system_time_precise() noexcept {
    FILETIME fileTime;
    ULARGE_INTEGER theTime;
    
    GetSystemTimePreciseAsFileTime(&fileTime);
    theTime.LowPart     = fileTime.dwLowDateTime;
    theTime.HighPart    = fileTime.dwHighDateTime;

    return theTime.QuadPart;
}

inline long long _query_performance_counter() noexcept {
    LARGE_INTEGER itCount{};
    QueryPerformanceCounter(&itCount);
    return itCount.QuadPart;
}

inline long long _query_performance_frequency() noexcept {
    LARGE_INTEGER itFreq{};
    QueryPerformanceFrequency(&itFreq);
    return itFreq.QuadPart;
}
#elif defined __linux__
#include <sys/time.h>
#endif      // _WIN32 and __linux__


CUSTOM_BEGIN

#define CUSTOM_CHRONO_BEGIN namespace chrono {
#define CUSTOM_CHRONO_END }

CUSTOM_CHRONO_BEGIN

#pragma region Duration
// duration values
template<class Rep>
struct DurationValues  // gets arithmetic properties of a type
{
    static constexpr Rep zero() noexcept {      // get zero value
        return Rep(0);
    }

    static constexpr Rep (min)() noexcept {     // get smallest value
        return NumericLimits<Rep>::lowest();
    }

    static constexpr Rep (max)() noexcept {     // get largest value
        return (NumericLimits<Rep>::max)();
    }
};


// Duration
template<class RepType, class PeriodRatio = Ratio<1>>
class Duration;

template<class Type>
constexpr bool IsDuration_v = IsSpecialization_v<Type, Duration>;

template<class ToDur, class Rep, class Period, EnableIf_t<IsDuration_v<ToDur>, bool> = true>
constexpr ToDur duration_cast(const Duration<Rep, Period>&)
noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<typename ToDur::Rep>);


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

    constexpr Duration() = default;

    template<class _Rep,
    EnableIf_t< IsConvertible_v<const _Rep&, Rep> && 
                (IsFloatingPoint_v<Rep> || !IsFloatingPoint_v<_Rep>), bool> = true>
    constexpr explicit Duration(const _Rep& val) noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<_Rep>)
        : _rep(static_cast<Rep>(val)) { /*Empty*/ }

    template<class _Rep, class _Period,
    EnableIf_t< IsFloatingPoint_v<Rep> ||
                (_RatioDivideSfinae<_Period, Period>::Den == 1 && !IsFloatingPoint_v<_Rep>), bool> = true>
    constexpr Duration(const Duration<_Rep, _Period>& other) noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<_Rep>)
        : _rep(custom::chrono::duration_cast<Duration>(other).count()) { /*Empty*/ }

public:
    // Operators

    constexpr Duration& operator=(const Duration& other) = default;

    constexpr CommonType_t<Duration> operator+() const noexcept(IsArithmetic_v<Rep>) {
        return CommonType_t<Duration>(*this);
    }

    constexpr CommonType_t<Duration> operator-() const noexcept(IsArithmetic_v<Rep>) {
        return CommonType_t<Duration>(-_rep);
    }

    constexpr Duration& operator++() noexcept(IsArithmetic_v<Rep>) {
        ++_rep;
        return *this;
    }

    constexpr Duration operator++(int) noexcept(IsArithmetic_v<Rep>) {
        return Duration(_rep++);
    }

    constexpr Duration& operator--() noexcept(IsArithmetic_v<Rep>) {
        --_rep;
        return *this;
    }

    constexpr Duration operator--(int) noexcept(IsArithmetic_v<Rep>) {
        return Duration(_rep--);
    }

    constexpr Duration& operator+=(const Duration& other) noexcept(IsArithmetic_v<Rep>) {     // need axact same duration
        _rep += other._rep;
        return *this;
    }

    constexpr Duration& operator-=(const Duration& other) noexcept(IsArithmetic_v<Rep>) {     // need axact same duration
        _rep -= other._rep;
        return *this;
    }

    constexpr Duration& operator*=(const Rep& otherRep) noexcept(IsArithmetic_v<Rep>) {
        _rep *= otherRep;
        return *this;
    }

    constexpr Duration& operator/=(const Rep& otherRep) noexcept(IsArithmetic_v<Rep>) {
        _rep /= otherRep;
        return *this;
    }

    constexpr Duration& operator%=(const Rep& otherRep) noexcept(IsArithmetic_v<Rep>) {
        _rep %= otherRep;
        return *this;
    }

    constexpr Duration& operator%=(const Duration& other) noexcept(IsArithmetic_v<Rep>) {     // need axact same duration
        _rep %= other._rep;
        return *this;
    }

public:
    // Main functions

    constexpr Rep count() const noexcept(IsArithmetic_v<Rep>) {
        return _rep;
    }

    static constexpr Duration zero() noexcept {
        return Duration(DurationValues<Rep>::zero());
    }

    static constexpr Duration (min)() noexcept {
        return Duration((DurationValues<Rep>::min)());
    }

    static constexpr Duration (max)() noexcept {
        return Duration((DurationValues<Rep>::max)());
    }
};  // END Duration


// Duration binary operators
template<class Rep1, class Period1, class Rep2, class Period2>         // Duration + Duration
constexpr CommonType_t<Duration<Rep1, Period1>, Duration<Rep2, Period2>>
operator+(const Duration<Rep1, Period1>& left, const Duration<Rep2, Period2>& right)
noexcept(IsArithmetic_v<Rep1> && IsArithmetic_v<Rep2>) {
    using CD = CommonType_t<Duration<Rep1, Period1>, Duration<Rep2, Period2>>;
    return CD(CD(left).count() + CD(right).count());
}

template<class Rep1, class Period1, class Rep2, class Period2>         // Duration - Duration
constexpr CommonType_t<Duration<Rep1, Period1>, Duration<Rep2, Period2>>
operator-(const Duration<Rep1, Period1>& left, const Duration<Rep2, Period2>& right)
noexcept(IsArithmetic_v<Rep1> && IsArithmetic_v<Rep2>) {
    using CD = CommonType_t<Duration<Rep1, Period1>, Duration<Rep2, Period2>>;
    return CD(CD(left).count() - CD(right).count());
}

template<class Rep1, class Period1, class Rep2, class Period2>         // Duration / Duration (return rep type)
constexpr CommonType_t<Rep1, Rep2>
operator/(const Duration<Rep1, Period1>& left, const Duration<Rep2, Period2>& right)
noexcept(IsArithmetic_v<Rep1> && IsArithmetic_v<Rep2>) {
    using CD = CommonType_t<Duration<Rep1, Period1>, Duration<Rep2, Period2>>;
    return CD(left).count() / CD(right).count();
}

template<class Rep1, class Period1, class Rep2, class Period2>          // Duration % Duration
constexpr CommonType_t<Duration<Rep1, Period1>, Duration<Rep2, Period2>>
operator%(const Duration<Rep1, Period1>& left, const Duration<Rep2, Period2>& right)
noexcept(IsArithmetic_v<Rep1> && IsArithmetic_v<Rep2>) {
    using CD = CommonType_t<Duration<Rep1, Period1>, Duration<Rep2, Period2>>;
    return CD(CD(left).count() % CD(right).count());
}

template<class Rep, class Period, class RightRep,
EnableIf_t<IsConvertible_v<const RightRep&, CommonType_t<Rep, RightRep>>, bool> = true> // Duration * constant
constexpr Duration<CommonType_t<Rep, RightRep>, Period>
operator*(const Duration<Rep, Period>& duration, const RightRep& rep) 
noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<RightRep>) {
    using CRDur = Duration<CommonType_t<Rep, RightRep>, Period>;
    return CRDur(CRDur(duration).count() * rep);
}

template<class Rep, class Period, class LeftRep,
EnableIf_t<IsConvertible_v<const LeftRep&, CommonType_t<Rep, LeftRep>>, bool> = true>   // constant * Duration
constexpr Duration<CommonType_t<Rep, LeftRep>, Period>
operator*(const LeftRep& rep, const Duration<Rep, Period>& duration)
noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<LeftRep>) {
    return duration * rep;
}

template<class Rep, class Period, class RightRep,
EnableIf_t<IsConvertible_v<const RightRep&, CommonType_t<Rep, RightRep>>, bool> = true> // Duration / constant
constexpr Duration<CommonType_t<Rep, RightRep>, Period>
operator/(const Duration<Rep, Period>& duration, const RightRep& rep)
noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<RightRep>) {
    using CRDur = Duration<CommonType_t<Rep, RightRep>, Period>;
    return CRDur(CRDur(duration).count() / rep);
}

template<class Rep, class Period, class RightRep,
EnableIf_t<IsConvertible_v<const RightRep&, CommonType_t<Rep, RightRep>>, bool> = true> // Duration % constant
constexpr Duration<CommonType_t<Rep, RightRep>, Period>
operator%(const Duration<Rep, Period>& duration, const RightRep& rep)
noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<RightRep>) {
    using CRDur = Duration<CommonType_t<Rep, RightRep>, Period>;
    return CRDur(CRDur(duration).count() % rep);
}

template<class Rep1, class Period1, class Rep2, class Period2>
constexpr bool operator==(const Duration<Rep1, Period1>& left, const Duration<Rep2, Period2>& right)
noexcept(IsArithmetic_v<Rep1> && IsArithmetic_v<Rep2>) {
   using CD = CommonType_t<Duration<Rep1, Period1>, Duration<Rep2, Period2>>;
   return CD(left).count() == CD(right).count();
}

template<class Rep1, class Period1, class Rep2, class Period2>
constexpr bool operator!=(const Duration<Rep1, Period1>& left, const Duration<Rep2, Period2>& right)
noexcept(IsArithmetic_v<Rep1> && IsArithmetic_v<Rep2>) {
    return !(left == right);
}

template<class Rep1, class Period1, class Rep2, class Period2>
constexpr bool operator<(const Duration<Rep1, Period1>& left, const Duration<Rep2, Period2>& right)
noexcept(IsArithmetic_v<Rep1> && IsArithmetic_v<Rep2>) {
   using CD = CommonType_t<Duration<Rep1, Period1>, Duration<Rep2, Period2>>;
   return CD(left).count() < CD(right).count();
}

template<class Rep1, class Period1, class Rep2, class Period2>
constexpr bool operator<=(const Duration<Rep1, Period1>& left, const Duration<Rep2, Period2>& right)
noexcept(IsArithmetic_v<Rep1> && IsArithmetic_v<Rep2>) {
   using CD = CommonType_t<Duration<Rep1, Period1>, Duration<Rep2, Period2>>;
   return CD(left).count() <= CD(right).count();
}

template<class Rep1, class Period1, class Rep2, class Period2>
constexpr bool operator>(const Duration<Rep1, Period1>& left, const Duration<Rep2, Period2>& right)
noexcept(IsArithmetic_v<Rep1> && IsArithmetic_v<Rep2>) {
   using CD = CommonType_t<Duration<Rep1, Period1>, Duration<Rep2, Period2>>;
   return CD(left).count() > CD(right).count();
}

template<class Rep1, class Period1, class Rep2, class Period2>
constexpr bool operator>=(const Duration<Rep1, Period1>& left, const Duration<Rep2, Period2>& right)
noexcept(IsArithmetic_v<Rep1> && IsArithmetic_v<Rep2>) {
   using CD = CommonType_t<Duration<Rep1, Period1>, Duration<Rep2, Period2>>;
   return CD(left).count() >= CD(right).count();
}


// duration cast impl
// convert duration to another duration (truncate)
template<class ToDur, class Rep, class Period, EnableIf_t<IsDuration_v<ToDur>, bool> /* = true (redefinition) */>
constexpr ToDur duration_cast(const Duration<Rep, Period>& duration)
noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<typename ToDur::Rep>) {

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

// duration floor
// convert duration to another duration; round towards negative infinity
// i.e. the greatest integral result such that the result <= duration
template<class ToDur, class Rep, class Period, EnableIf_t<IsDuration_v<ToDur>, bool> = true>
constexpr ToDur floor(const Duration<Rep, Period>& duration)
noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<typename ToDur::Rep>) {
    const ToDur durationCasted = custom::chrono::duration_cast<ToDur>(duration);

    if (durationCasted > duration)
        return ToDur{ durationCasted.count() - static_cast<typename ToDur::Rep>(1) };

    return durationCasted;
}

// duration ceil
// convert duration to another duration; round towards positive infinity
// i.e. the least integral result such that duration <= the result
template<class ToDur, class Rep, class Period, EnableIf_t<IsDuration_v<ToDur>, bool> = true>
constexpr ToDur ceil(const Duration<Rep, Period>& duration)
noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<typename ToDur::Rep>) {
    const ToDur durationCasted = custom::chrono::duration_cast<ToDur>(duration);

    if (durationCasted < duration)
        return ToDur{ durationCasted.count() + static_cast<typename ToDur::Rep>(1) };

    return durationCasted;
}

// duration round
// convert duration to another duration, round to nearest, ties to even
template<class ToDur, class Rep, class Period,
EnableIf_t<IsDuration_v<ToDur> && !IsFloatingPoint_v<typename ToDur::Rep>, bool> = true>
constexpr ToDur round(const Duration<Rep, Period>& duration)
noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<typename ToDur::Rep>) {
    ToDur durationFloored   = custom::chrono::floor<ToDur>(duration);
    ToDur durationCeiled    = durationFloored + ToDur{ 1 };
    auto floorAdjustment    = duration - durationFloored;
    auto ceilAdjustment     = durationCeiled - duration;

    if (floorAdjustment < ceilAdjustment ||
        (floorAdjustment == ceilAdjustment && durationFloored.count() % 2 == 0))
        return durationFloored;

    return durationCeiled;
}

// duration abs
// create a duration whose count() is the absolute value of duration.count()
template<class Rep, class Period, EnableIf_t<NumericLimits<Rep>::IsSigned, bool> = true>
constexpr Duration<Rep, Period> abs(const Duration<Rep, Period> duration) noexcept(IsArithmetic_v<Rep>) {

    if (duration < Duration<Rep, Period>::zero())
        return Duration<Rep, Period>::zero() - duration;
    else
        return duration;
}

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

    constexpr TimePoint() = default;

    constexpr explicit TimePoint(const Duration& val) noexcept(IsArithmetic_v<Rep>)
        : _dur(val) { /*Empty*/ }

    template<class _Duration, EnableIf_t<IsConvertible_v<_Duration, Duration>, bool> = true>
    constexpr TimePoint(const TimePoint<Clock, _Duration>& other)
    noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<typename _Duration::Rep>)
        : _dur(other._dur) { /*Empty*/ }

public:
    // Operators

    constexpr TimePoint& operator++() noexcept(IsArithmetic_v<Rep>) {
        ++_dur;
        return *this;
    }

    constexpr TimePoint operator++(int) noexcept(IsArithmetic_v<Rep>) {
        return TimePoint(_dur++);
    }

    constexpr TimePoint& operator--() noexcept(IsArithmetic_v<Rep>) {
        --_dur;
        return *this;
    }

    constexpr TimePoint operator--(int) noexcept(IsArithmetic_v<Rep>) {
        return TimePoint(_dur--);
    }

    constexpr TimePoint& operator+=(const Duration& otherDuration) noexcept(IsArithmetic_v<Rep>) {
        _dur += otherDuration;
        return *this;
    }

    constexpr TimePoint& operator-=(const Duration& otherDuration) noexcept(IsArithmetic_v<Rep>) {
        _dur -= otherDuration;
        return *this;
    }

public:
    // Main functions

    constexpr Duration time_since_epoch() const noexcept(IsArithmetic_v<Rep>) {
        return _dur;
    }

    static constexpr TimePoint (min)() noexcept {
        return TimePoint((Duration::min)());
    }

    static constexpr TimePoint (max)() noexcept {
        return TimePoint((Duration::max)());
    }

};  // END TimePoint


// TimePoint binary operators
template<class Clock, class DurationTP, class Rep, class Period>        // TimePoint + Duration (return TP)
constexpr TimePoint<Clock, CommonType_t<DurationTP, Duration<Rep, Period>>>
operator+(const TimePoint<Clock, DurationTP>& time, const Duration<Rep, Period>& duration)
noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<typename DurationTP::Rep>) {
    using CDTime = TimePoint<Clock, CommonType_t<DurationTP, Duration<Rep, Period>>>;
    return CDTime(time.time_since_epoch() + duration);
}

template<class Clock, class DurationTP, class Rep, class Period>        // Duration + TimePoint (return TP)
constexpr TimePoint<Clock, CommonType_t<DurationTP, Duration<Rep, Period>>>
operator+(const Duration<Rep, Period>& duration, const TimePoint<Clock, DurationTP>& time)
noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<typename DurationTP::Rep>) {
    return time + duration;
}

template<class Clock, class DurationTP, class Rep, class Period>        // TimePoint - Duration (return TP)
constexpr TimePoint<Clock, CommonType_t<DurationTP, Duration<Rep, Period>>>
operator-(const TimePoint<Clock, DurationTP>& time, const Duration<Rep, Period>& duration)
noexcept(IsArithmetic_v<Rep> && IsArithmetic_v<typename DurationTP::Rep>) {
    using CDTime = TimePoint<Clock, CommonType_t<DurationTP, Duration<Rep, Period>>>;
    return CDTime(time.time_since_epoch() - duration);
}

template<class Clock, class Duration1, class Duration2>                 // TimePoint - TimePoint (NO "+" between TP)
constexpr CommonType_t<Duration1, Duration2>
operator-(const TimePoint<Clock, Duration1>& left, const TimePoint<Clock, Duration2>& right)
noexcept(IsArithmetic_v<typename Duration1::Rep> && IsArithmetic_v<typename Duration2::Rep>) {
    return left.time_since_epoch() - right.time_since_epoch();
}

template<class Clock, class Duration1, class Duration2>
constexpr bool operator==(const TimePoint<Clock, Duration2>& left, const TimePoint<Clock, Duration2>& right)
noexcept(IsArithmetic_v<typename Duration1::Rep> && IsArithmetic_v<typename Duration2::Rep>) {
    return left.time_since_epoch() == right.time_since_epoch();
}

template<class Clock, class Duration1, class Duration2>
constexpr bool operator!=(const TimePoint<Clock, Duration1>& left, const TimePoint<Clock, Duration2>& right)
noexcept(IsArithmetic_v<typename Duration1::Rep> && IsArithmetic_v<typename Duration2::Rep>) {
    return !(left == right);
}

template<class Clock, class Duration1, class Duration2>
constexpr bool operator<(const TimePoint<Clock, Duration1>& left, const TimePoint<Clock, Duration2>& right)
noexcept(IsArithmetic_v<typename Duration1::Rep> && IsArithmetic_v<typename Duration2::Rep>) {
    return left.time_since_epoch() < right.time_since_epoch();
}

template<class Clock, class Duration1, class Duration2>
constexpr bool operator<=(const TimePoint<Clock, Duration1>& left, const TimePoint<Clock, Duration2>& right)
noexcept(IsArithmetic_v<typename Duration1::Rep> && IsArithmetic_v<typename Duration2::Rep>) {
    return left.time_since_epoch() <= right.time_since_epoch();
}

template<class Clock, class Duration1, class Duration2>
constexpr bool operator>(const TimePoint<Clock, Duration1>& left, const TimePoint<Clock, Duration2>& right)
noexcept(IsArithmetic_v<typename Duration1::Rep> && IsArithmetic_v<typename Duration2::Rep>) {
    return left.time_since_epoch() > right.time_since_epoch();
}

template<class Clock, class Duration1, class Duration2>
constexpr bool operator>=(const TimePoint<Clock, Duration1>& left, const TimePoint<Clock, Duration2>& right)
noexcept(IsArithmetic_v<typename Duration1::Rep> && IsArithmetic_v<typename Duration2::Rep>) {
    return left.time_since_epoch() >= right.time_since_epoch();
}


// time point cast
// change the duration type of a TimePoint (truncate)
template<class ToDur, class Clock, class Duration, EnableIf_t<IsDuration_v<ToDur>, bool> = true>
constexpr TimePoint<Clock, ToDur> time_point_cast(const TimePoint<Clock, Duration>& time)
noexcept(IsArithmetic_v<typename Duration::Rep> && IsArithmetic_v<typename ToDur::Rep>) {
    return TimePoint<Clock, ToDur>(custom::chrono::duration_cast<ToDur>(time.time_since_epoch()));
}

// time point floor
// change the duration type of a TimePoint; round towards negative infinity
template<class ToDur, class Clock, class Duration, EnableIf_t<IsDuration_v<ToDur>, bool> = true>
constexpr TimePoint<Clock, ToDur> floor(const TimePoint<Clock, Duration>& time)
noexcept(IsArithmetic_v<typename Duration::Rep> && IsArithmetic_v<typename ToDur::Rep>) {
    return TimePoint<Clock, ToDur>(custom::chrono::floor<ToDur>(time.time_since_epoch()));  // use Duration floor
}

// time point ceil
// change the duration type of a TimePoint; round towards positive infinity
template<class ToDur, class Clock, class Duration, EnableIf_t<IsDuration_v<ToDur>, bool> = true>
constexpr TimePoint<Clock, ToDur> ceil(const TimePoint<Clock, Duration>& time) noexcept(
    IsArithmetic_v<typename Duration::Rep> && IsArithmetic_v<typename ToDur::Rep>) {
    return TimePoint<Clock, ToDur>(custom::chrono::ceil<ToDur>(time.time_since_epoch()));   // use Duration ceil
}

// time point round
// change the duration type of a TimePoint; round to nearest, ties to even
template<class ToDur, class Clock, class Duration,
EnableIf_t<IsDuration_v<ToDur> && !IsFloatingPoint_v<typename ToDur::Rep>, bool> = true>
constexpr TimePoint<Clock, ToDur> round(const TimePoint<Clock, Duration>& time)
noexcept(IsArithmetic_v<typename Duration::Rep> && IsArithmetic_v<typename ToDur::Rep>) {
    return TimePoint<Clock, ToDur>(custom::chrono::round<ToDur>(time.time_since_epoch()));  // use Duration round
}
#pragma endregion TimePoint


#pragma region Clock
// system clock
struct SystemClock     // wraps GetSystemTimePreciseAsFileTime/GetSystemTimeAsFileTime
{
    using Rep       = long long;
    using Period    = custom::Nano;
    using Duration  = custom::chrono::Nanoseconds;
    using TimePoint = custom::chrono::TimePoint<SystemClock>;

    static constexpr bool IsSteady = false;

    static TimePoint now() noexcept { // get current time
#if defined _WIN32
        long long sysTime = _get_system_time_precise();
        return TimePoint(Duration(sysTime - UNIX_EPOCH));
#elif defined __linux__
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        return TimePoint(Duration(Seconds(ts.tv_sec) + Nanoseconds(ts.tv_nsec)));
#endif
    }

    static std::time_t to_time_t(const TimePoint& time) noexcept { // convert to std::time_t
        return duration_cast<Seconds>(time.time_since_epoch()).count();
    }

    // TODO: check
    static TimePoint from_time_t(std::time_t time) noexcept { // convert from std::time_t
        return TimePoint(Seconds(time));
    }
};  // END SystemClock


// steady clock
struct SteadyClock     // wraps QueryPerformanceCounter
{
    using Rep       = long long;
    using Period    = custom::Nano;
    using Duration  = custom::chrono::Nanoseconds;
    using TimePoint = custom::chrono::TimePoint<SteadyClock>;

    static constexpr bool IsSteady = true;

    static TimePoint now() noexcept {
#if defined _WIN32
        const long long freq    = _query_performance_frequency(); // doesn't change after system boot
        const long long count   = _query_performance_counter();

        static_assert(Period::Num == 1, "This assumes Period::Num == 1.");

        // 10 MHz is a very common QPC frequency on modern PCs. Optimizing for
        // this specific frequency can double the performance of this function by
        // avoiding the expensive frequency conversion path.
        
        constexpr long long tenMHz = 10000000LL;
        if (freq == tenMHz)
        {
            static_assert(Period::Den % tenMHz == 0, "It should never fail.");
            constexpr long long multiplier = Period::Den / tenMHz;
            return TimePoint(Duration(count * multiplier));
        }
        else
        {
            // Instead of just having "(count * Period::Den) / freq",
            // the algorithm below prevents overflow when count is sufficiently large.
            // It assumes that freq * Period::Den does not overflow, which is currently true for nano period.
            // It is not realistic for count to accumulate to large values from zero with this assumption,
            // but the initial value of count could be large.

            const long long whole   = (count / freq) * Period::Den;
            const long long part    = (count % freq) * Period::Den / freq;
            return TimePoint(Duration(whole + part));
        }
#elif defined __linux__
        timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return TimePoint(Duration(Seconds(ts.tv_sec) + Nanoseconds(ts.tv_nsec)));
#endif
    }
};  // END SteadyClock


using HighResolutionClock = SystemClock;

template<class _Duration>
using SysTime       = TimePoint<SystemClock, _Duration>;
using SysSeconds    = SysTime<Seconds>;
using SysDays       = SysTime<Days>;

// is clock
template<class Clock, class = void>
constexpr bool IsClock_v = false;

template<class Clock>
constexpr bool IsClock_v<Clock, Void_t< typename Clock::Rep,
                                        typename Clock::Period,
                                        typename Clock::Duration,
                                        typename Clock::TimePoint,
                                        decltype(Clock::IsSteady),
                                        decltype(Clock::now())>> = true;

template<class Clock>
struct IsClock : BoolConstant<IsClock_v<Clock>> {};
#pragma endregion Clock

CUSTOM_CHRONO_END

CUSTOM_END