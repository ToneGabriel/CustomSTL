#pragma once
#include "x_chrono.h"


CUSTOM_BEGIN

CUSTOM_CHRONO_BEGIN

#pragma region Duration
// duration
template<class RepType, class PeriodRatio = ratio<1>>
class duration;

template<class Type>
constexpr bool IsDuration_v = is_specialization_v<Type, duration>;

template<class ToDur, class Rep, class Period, enable_if_t<IsDuration_v<ToDur>, bool> = true>
constexpr ToDur duration_cast(const duration<Rep, Period>&)
noexcept(is_arithmetic_v<Rep> && is_arithmetic_v<typename ToDur::rep>);


template<class RepType, class PeriodRatio>
class duration  // represents a time duration
{
    static_assert(!IsDuration_v<RepType>, "duration can't have duration as first template argument");
    static_assert(is_ratio_v<PeriodRatio>, "period not an instance of ratio");
    static_assert(0 < PeriodRatio::num, "period negative or zero");

public:
    using rep       = RepType;
    using period    = typename PeriodRatio::type;

private:
    rep _rep;       // the stored rep

public:
    // Constructors

    constexpr duration() = default;

    template<class _Rep,
    enable_if_t< is_convertible_v<const _Rep&, rep> && 
                (is_floating_point_v<rep> || !is_floating_point_v<_Rep>), bool> = true>
    constexpr explicit duration(const _Rep& val) noexcept(is_arithmetic_v<rep> && is_arithmetic_v<_Rep>)
        : _rep(static_cast<rep>(val)) { /*Empty*/ }

    template<class _Rep, class _Period,
    enable_if_t< is_floating_point_v<rep> ||
                (_RatioDivideSfinae<_Period, period>::den == 1 && !is_floating_point_v<_Rep>), bool> = true>
    constexpr duration(const duration<_Rep, _Period>& other) noexcept(is_arithmetic_v<rep> && is_arithmetic_v<_Rep>)
        : _rep(custom::chrono::duration_cast<duration>(other).count()) { /*Empty*/ }

public:
    // Operators

    constexpr duration& operator=(const duration& other) = default;

    constexpr common_type_t<duration> operator+() const noexcept(is_arithmetic_v<rep>) {
        return common_type_t<duration>(*this);
    }

    constexpr common_type_t<duration> operator-() const noexcept(is_arithmetic_v<rep>) {
        return common_type_t<duration>(-_rep);
    }

    constexpr duration& operator++() noexcept(is_arithmetic_v<rep>) {
        ++_rep;
        return *this;
    }

    constexpr duration operator++(int) noexcept(is_arithmetic_v<rep>) {
        return duration(_rep++);
    }

    constexpr duration& operator--() noexcept(is_arithmetic_v<rep>) {
        --_rep;
        return *this;
    }

    constexpr duration operator--(int) noexcept(is_arithmetic_v<rep>) {
        return duration(_rep--);
    }

    constexpr duration& operator+=(const duration& other) noexcept(is_arithmetic_v<rep>) {     // need axact same duration
        _rep += other._rep;
        return *this;
    }

    constexpr duration& operator-=(const duration& other) noexcept(is_arithmetic_v<rep>) {     // need axact same duration
        _rep -= other._rep;
        return *this;
    }

    constexpr duration& operator*=(const rep& otherRep) noexcept(is_arithmetic_v<rep>) {
        _rep *= otherRep;
        return *this;
    }

    constexpr duration& operator/=(const rep& otherRep) noexcept(is_arithmetic_v<rep>) {
        _rep /= otherRep;
        return *this;
    }

    constexpr duration& operator%=(const rep& otherRep) noexcept(is_arithmetic_v<rep>) {
        _rep %= otherRep;
        return *this;
    }

    constexpr duration& operator%=(const duration& other) noexcept(is_arithmetic_v<rep>) {     // need axact same duration
        _rep %= other._rep;
        return *this;
    }

public:
    // Main functions

    constexpr rep count() const noexcept(is_arithmetic_v<rep>) {
        return _rep;
    }

    static constexpr duration zero() noexcept {
        return duration(duration_values<rep>::zero());
    }

    static constexpr duration (min)() noexcept {
        return duration((duration_values<rep>::min)());
    }

    static constexpr duration (max)() noexcept {
        return duration((duration_values<rep>::max)());
    }
};  // END duration


// duration binary operators
template<class Rep1, class Period1, class Rep2, class Period2>         // duration + duration
constexpr common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>
operator+(const duration<Rep1, Period1>& left, const duration<Rep2, Period2>& right)
noexcept(is_arithmetic_v<Rep1> && is_arithmetic_v<Rep2>) {
    using CD = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
    return CD(CD(left).count() + CD(right).count());
}

template<class Rep1, class Period1, class Rep2, class Period2>         // duration - duration
constexpr common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>
operator-(const duration<Rep1, Period1>& left, const duration<Rep2, Period2>& right)
noexcept(is_arithmetic_v<Rep1> && is_arithmetic_v<Rep2>) {
    using CD = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
    return CD(CD(left).count() - CD(right).count());
}

template<class Rep1, class Period1, class Rep2, class Period2>         // duration / duration (return rep type)
constexpr common_type_t<Rep1, Rep2>
operator/(const duration<Rep1, Period1>& left, const duration<Rep2, Period2>& right)
noexcept(is_arithmetic_v<Rep1> && is_arithmetic_v<Rep2>) {
    using CD = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
    return CD(left).count() / CD(right).count();
}

template<class Rep1, class Period1, class Rep2, class Period2>          // duration % duration
constexpr common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>
operator%(const duration<Rep1, Period1>& left, const duration<Rep2, Period2>& right)
noexcept(is_arithmetic_v<Rep1> && is_arithmetic_v<Rep2>) {
    using CD = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
    return CD(CD(left).count() % CD(right).count());
}

template<class Rep, class Period, class RightRep,
enable_if_t<is_convertible_v<const RightRep&, common_type_t<Rep, RightRep>>, bool> = true> // duration * constant
constexpr duration<common_type_t<Rep, RightRep>, Period>
operator*(const duration<Rep, Period>& duration, const RightRep& rep) 
noexcept(is_arithmetic_v<Rep> && is_arithmetic_v<RightRep>) {
    using CRDur = duration<common_type_t<Rep, RightRep>, Period>;
    return CRDur(CRDur(duration).count() * rep);
}

template<class Rep, class Period, class LeftRep,
enable_if_t<is_convertible_v<const LeftRep&, common_type_t<Rep, LeftRep>>, bool> = true>   // constant * duration
constexpr duration<common_type_t<Rep, LeftRep>, Period>
operator*(const LeftRep& rep, const duration<Rep, Period>& duration)
noexcept(is_arithmetic_v<Rep> && is_arithmetic_v<LeftRep>) {
    return duration * rep;
}

template<class Rep, class Period, class RightRep,
enable_if_t<is_convertible_v<const RightRep&, common_type_t<Rep, RightRep>>, bool> = true> // duration / constant
constexpr duration<common_type_t<Rep, RightRep>, Period>
operator/(const duration<Rep, Period>& duration, const RightRep& rep)
noexcept(is_arithmetic_v<Rep> && is_arithmetic_v<RightRep>) {
    using CRDur = duration<common_type_t<Rep, RightRep>, Period>;
    return CRDur(CRDur(duration).count() / rep);
}

template<class Rep, class Period, class RightRep,
enable_if_t<is_convertible_v<const RightRep&, common_type_t<Rep, RightRep>>, bool> = true> // duration % constant
constexpr duration<common_type_t<Rep, RightRep>, Period>
operator%(const duration<Rep, Period>& duration, const RightRep& rep)
noexcept(is_arithmetic_v<Rep> && is_arithmetic_v<RightRep>) {
    using CRDur = duration<common_type_t<Rep, RightRep>, Period>;
    return CRDur(CRDur(duration).count() % rep);
}

template<class Rep1, class Period1, class Rep2, class Period2>
constexpr bool operator==(const duration<Rep1, Period1>& left, const duration<Rep2, Period2>& right)
noexcept(is_arithmetic_v<Rep1> && is_arithmetic_v<Rep2>) {
   using CD = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
   return CD(left).count() == CD(right).count();
}

template<class Rep1, class Period1, class Rep2, class Period2>
constexpr bool operator!=(const duration<Rep1, Period1>& left, const duration<Rep2, Period2>& right)
noexcept(is_arithmetic_v<Rep1> && is_arithmetic_v<Rep2>) {
    return !(left == right);
}

template<class Rep1, class Period1, class Rep2, class Period2>
constexpr bool operator<(const duration<Rep1, Period1>& left, const duration<Rep2, Period2>& right)
noexcept(is_arithmetic_v<Rep1> && is_arithmetic_v<Rep2>) {
   using CD = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
   return CD(left).count() < CD(right).count();
}

template<class Rep1, class Period1, class Rep2, class Period2>
constexpr bool operator<=(const duration<Rep1, Period1>& left, const duration<Rep2, Period2>& right)
noexcept(is_arithmetic_v<Rep1> && is_arithmetic_v<Rep2>) {
   using CD = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
   return CD(left).count() <= CD(right).count();
}

template<class Rep1, class Period1, class Rep2, class Period2>
constexpr bool operator>(const duration<Rep1, Period1>& left, const duration<Rep2, Period2>& right)
noexcept(is_arithmetic_v<Rep1> && is_arithmetic_v<Rep2>) {
   using CD = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
   return CD(left).count() > CD(right).count();
}

template<class Rep1, class Period1, class Rep2, class Period2>
constexpr bool operator>=(const duration<Rep1, Period1>& left, const duration<Rep2, Period2>& right)
noexcept(is_arithmetic_v<Rep1> && is_arithmetic_v<Rep2>) {
   using CD = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
   return CD(left).count() >= CD(right).count();
}


// duration cast impl
// convert duration to another duration (truncate)
template<class ToDur, class Rep, class Period, enable_if_t<IsDuration_v<ToDur>, bool> /* = true (redefinition) */>
constexpr ToDur duration_cast(const duration<Rep, Period>& duration)
noexcept(is_arithmetic_v<Rep> && is_arithmetic_v<typename ToDur::rep>) {

    using ToDurRep  = typename ToDur::rep;
    using RatioType = ratio_divide<Period, typename ToDur::period>;
    using RepType   = common_type_t<ToDurRep, Rep, intmax_t>;

    // avoid multiplications and divisions where possible (last return is the complete cast)
    if (RatioType::den == 1)
    {
        if (RatioType::num == 1)
            return static_cast<ToDur>(static_cast<ToDurRep>(duration.count()));
        else
            return static_cast<ToDur>(static_cast<ToDurRep>(static_cast<RepType>(duration.count()) * static_cast<RepType>(RatioType::num)));
    }
    else
    {
        if (RatioType::num == 1)
            return static_cast<ToDur>(static_cast<ToDurRep>(static_cast<RepType>(duration.count()) / static_cast<RepType>(RatioType::den)));
        else
            return static_cast<ToDur>(static_cast<ToDurRep>(static_cast<RepType>(duration.count()) * static_cast<RepType>(RatioType::num) / static_cast<RepType>(RatioType::den)));
    }
}

// duration floor
// convert duration to another duration; round towards negative infinity
// i.e. the greatest integral result such that the result <= duration
template<class ToDur, class Rep, class Period, enable_if_t<IsDuration_v<ToDur>, bool> = true>
constexpr ToDur floor(const duration<Rep, Period>& duration)
noexcept(is_arithmetic_v<Rep> && is_arithmetic_v<typename ToDur::Rep>) {
    const ToDur durationCasted = custom::chrono::duration_cast<ToDur>(duration);

    if (durationCasted > duration)
        return ToDur{ durationCasted.count() - static_cast<typename ToDur::Rep>(1) };

    return durationCasted;
}

// duration ceil
// convert duration to another duration; round towards positive infinity
// i.e. the least integral result such that duration <= the result
template<class ToDur, class Rep, class Period, enable_if_t<IsDuration_v<ToDur>, bool> = true>
constexpr ToDur ceil(const duration<Rep, Period>& duration)
noexcept(is_arithmetic_v<Rep> && is_arithmetic_v<typename ToDur::Rep>) {
    const ToDur durationCasted = custom::chrono::duration_cast<ToDur>(duration);

    if (durationCasted < duration)
        return ToDur{ durationCasted.count() + static_cast<typename ToDur::Rep>(1) };

    return durationCasted;
}

// duration round
// convert duration to another duration, round to nearest, ties to even
template<class ToDur, class Rep, class Period,
enable_if_t<IsDuration_v<ToDur> && !is_floating_point_v<typename ToDur::Rep>, bool> = true>
constexpr ToDur round(const duration<Rep, Period>& duration)
noexcept(is_arithmetic_v<Rep> && is_arithmetic_v<typename ToDur::Rep>) {
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
template<class Rep, class Period, enable_if_t<numeric_limits<Rep>::is_signed, bool> = true>
constexpr duration<Rep, Period> abs(const duration<Rep, Period> duration) noexcept(is_arithmetic_v<Rep>) {

    if (duration < duration<Rep, Period>::zero())
        return duration<Rep, Period>::zero() - duration;
    else
        return duration;
}

// time units
using nanoseconds   = duration<long long, nano>;
using microseconds  = duration<long long, micro>;
using milliseconds  = duration<long long, milli>;
using seconds       = duration<long long>;
using minutes       = duration<int, ratio<60>>;
using hours         = duration<int, ratio<3600>>;

using days          = duration<int, ratio_multiply<ratio<24>, hours::period>>;
using weeks         = duration<int, ratio_multiply<ratio<7>, days::period>>;
using years         = duration<int, ratio_multiply<ratio<146097, 400>, days::period>>;
using months        = duration<int, ratio_divide<years::period, ratio<12>>>;
#pragma endregion Duration


#pragma region TimePoint
// time_point
template<class ClockType, class DurationType = typename ClockType::duration>
class time_point    // represents a point in time
{
    static_assert(IsDuration_v<DurationType>, "duration needs to be a specialization of chrono::duration.");

public:
    using clock     = ClockType;
    using duration  = DurationType;
    using rep       = typename DurationType::rep;
    using period    = typename DurationType::period;

private:
    duration _dur   = duration::zero();   // duration since the epoch

public:
    // Constructors

    constexpr time_point() = default;

    constexpr explicit time_point(const duration& val) noexcept(is_arithmetic_v<rep>)
        : _dur(val) { /*Empty*/ }

    template<class _Duration, enable_if_t<is_convertible_v<_Duration, duration>, bool> = true>
    constexpr time_point(const time_point<clock, _Duration>& other)
    noexcept(is_arithmetic_v<Rep> && is_arithmetic_v<typename _Duration::Rep>)
        : _dur(other.time_since_epoch()) { /*Empty*/ }

public:
    // Operators

    constexpr time_point& operator++() noexcept(is_arithmetic_v<Rep>) {
        ++_dur;
        return *this;
    }

    constexpr time_point operator++(int) noexcept(is_arithmetic_v<Rep>) {
        return time_point(_dur++);
    }

    constexpr time_point& operator--() noexcept(is_arithmetic_v<Rep>) {
        --_dur;
        return *this;
    }

    constexpr time_point operator--(int) noexcept(is_arithmetic_v<Rep>) {
        return time_point(_dur--);
    }

    constexpr time_point& operator+=(const duration& otherDuration) noexcept(is_arithmetic_v<Rep>) {
        _dur += otherDuration;
        return *this;
    }

    constexpr time_point& operator-=(const duration& otherDuration) noexcept(is_arithmetic_v<Rep>) {
        _dur -= otherDuration;
        return *this;
    }

public:
    // Main functions

    constexpr duration time_since_epoch() const noexcept(is_arithmetic_v<rep>) {
        return _dur;
    }

    static constexpr time_point (min)() noexcept {
        return time_point((duration::min)());
    }

    static constexpr time_point (max)() noexcept {
        return time_point((duration::max)());
    }

};  // END time_point


// time_point binary operators
template<class Clock, class DurationTP, class Rep, class Period>        // time_point + duration (return TP)
constexpr time_point<Clock, common_type_t<DurationTP, duration<Rep, Period>>>
operator+(const time_point<Clock, DurationTP>& time, const duration<Rep, Period>& duration)
noexcept(is_arithmetic_v<Rep> && is_arithmetic_v<typename DurationTP::Rep>) {
    using CDTime = time_point<Clock, common_type_t<DurationTP, duration<Rep, Period>>>;
    return CDTime(time.time_since_epoch() + duration);
}

template<class Clock, class DurationTP, class Rep, class Period>        // duration + time_point (return TP)
constexpr time_point<Clock, common_type_t<DurationTP, duration<Rep, Period>>>
operator+(const duration<Rep, Period>& duration, const time_point<Clock, DurationTP>& time)
noexcept(is_arithmetic_v<Rep> && is_arithmetic_v<typename DurationTP::Rep>) {
    return time + duration;
}

template<class Clock, class DurationTP, class Rep, class Period>        // time_point - duration (return TP)
constexpr time_point<Clock, common_type_t<DurationTP, duration<Rep, Period>>>
operator-(const time_point<Clock, DurationTP>& time, const duration<Rep, Period>& duration)
noexcept(is_arithmetic_v<Rep> && is_arithmetic_v<typename DurationTP::Rep>) {
    using CDTime = time_point<Clock, common_type_t<DurationTP, duration<Rep, Period>>>;
    return CDTime(time.time_since_epoch() - duration);
}

template<class Clock, class Duration1, class Duration2>                 // time_point - time_point (NO "+" between TP)
constexpr common_type_t<Duration1, Duration2>
operator-(const time_point<Clock, Duration1>& left, const time_point<Clock, Duration2>& right)
noexcept(is_arithmetic_v<typename Duration1::Rep> && is_arithmetic_v<typename Duration2::Rep>) {
    return left.time_since_epoch() - right.time_since_epoch();
}

template<class Clock, class Duration1, class Duration2>
constexpr bool operator==(const time_point<Clock, Duration2>& left, const time_point<Clock, Duration2>& right)
noexcept(is_arithmetic_v<typename Duration1::Rep> && is_arithmetic_v<typename Duration2::Rep>) {
    return left.time_since_epoch() == right.time_since_epoch();
}

template<class Clock, class Duration1, class Duration2>
constexpr bool operator!=(const time_point<Clock, Duration1>& left, const time_point<Clock, Duration2>& right)
noexcept(is_arithmetic_v<typename Duration1::Rep> && is_arithmetic_v<typename Duration2::Rep>) {
    return !(left == right);
}

template<class Clock, class Duration1, class Duration2>
constexpr bool operator<(const time_point<Clock, Duration1>& left, const time_point<Clock, Duration2>& right)
noexcept(is_arithmetic_v<typename Duration1::Rep> && is_arithmetic_v<typename Duration2::Rep>) {
    return left.time_since_epoch() < right.time_since_epoch();
}

template<class Clock, class Duration1, class Duration2>
constexpr bool operator<=(const time_point<Clock, Duration1>& left, const time_point<Clock, Duration2>& right)
noexcept(is_arithmetic_v<typename Duration1::Rep> && is_arithmetic_v<typename Duration2::Rep>) {
    return left.time_since_epoch() <= right.time_since_epoch();
}

template<class Clock, class Duration1, class Duration2>
constexpr bool operator>(const time_point<Clock, Duration1>& left, const time_point<Clock, Duration2>& right)
noexcept(is_arithmetic_v<typename Duration1::Rep> && is_arithmetic_v<typename Duration2::Rep>) {
    return left.time_since_epoch() > right.time_since_epoch();
}

template<class Clock, class Duration1, class Duration2>
constexpr bool operator>=(const time_point<Clock, Duration1>& left, const time_point<Clock, Duration2>& right)
noexcept(is_arithmetic_v<typename Duration1::Rep> && is_arithmetic_v<typename Duration2::Rep>) {
    return left.time_since_epoch() >= right.time_since_epoch();
}


// time point cast
// change the duration type of a time_point (truncate)
template<class ToDur, class Clock, class duration, enable_if_t<IsDuration_v<ToDur>, bool> = true>
constexpr time_point<Clock, ToDur> time_point_cast(const time_point<Clock, duration>& time)
noexcept(is_arithmetic_v<typename duration::Rep> && is_arithmetic_v<typename ToDur::Rep>) {
    return time_point<Clock, ToDur>(custom::chrono::duration_cast<ToDur>(time.time_since_epoch()));
}

// time point floor
// change the duration type of a time_point; round towards negative infinity
template<class ToDur, class Clock, class duration, enable_if_t<IsDuration_v<ToDur>, bool> = true>
constexpr time_point<Clock, ToDur> floor(const time_point<Clock, duration>& time)
noexcept(is_arithmetic_v<typename duration::Rep> && is_arithmetic_v<typename ToDur::Rep>) {
    return time_point<Clock, ToDur>(custom::chrono::floor<ToDur>(time.time_since_epoch()));  // use duration floor
}

// time point ceil
// change the duration type of a time_point; round towards positive infinity
template<class ToDur, class Clock, class duration, enable_if_t<IsDuration_v<ToDur>, bool> = true>
constexpr time_point<Clock, ToDur> ceil(const time_point<Clock, duration>& time) noexcept(
    is_arithmetic_v<typename duration::Rep> && is_arithmetic_v<typename ToDur::Rep>) {
    return time_point<Clock, ToDur>(custom::chrono::ceil<ToDur>(time.time_since_epoch()));   // use duration ceil
}

// time point round
// change the duration type of a time_point; round to nearest, ties to even
template<class ToDur, class Clock, class duration,
enable_if_t<IsDuration_v<ToDur> && !is_floating_point_v<typename ToDur::Rep>, bool> = true>
constexpr time_point<Clock, ToDur> round(const time_point<Clock, duration>& time)
noexcept(is_arithmetic_v<typename duration::Rep> && is_arithmetic_v<typename ToDur::Rep>) {
    return time_point<Clock, ToDur>(custom::chrono::round<ToDur>(time.time_since_epoch()));  // use duration round
}
#pragma endregion TimePoint


#pragma region Clock
// system clock
struct system_clock     // wraps GetSystemTimePreciseAsFileTime
{
#if defined _MSC_VER
    using period        = custom::ratio<1, 10'000'000>; // 100 nanoseconds
#elif defined __GNUG__
    using period        = custom::nano;                 // clock_gettime uses nanoseconds
#endif
    using rep           = long long;
    using duration      = custom::chrono::duration<rep, period>;
    using time_point    = custom::chrono::time_point<system_clock>;

    static constexpr bool is_steady = false;


    static time_point now() noexcept { // get current time
#if defined _MSC_VER
        return time_point(duration(detail::_get_system_time_precise() - UNIX_EPOCH));
#elif defined __GNUG__
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        return time_point(duration(seconds(ts.tv_sec) + nanoseconds(ts.tv_nsec)));
#endif
    }

    static std::time_t to_time_t(const time_point& time) noexcept { // convert to std::time_t
        return duration_cast<seconds>(time.time_since_epoch()).count();
    }

    static time_point from_time_t(std::time_t time) noexcept { // convert from std::time_t
        return time_point(seconds(time));
    }
};  // END system_clock


// steady clock
struct steady_clock     // wraps QueryPerformanceCounter
{
    using period    = custom::nano;
    using rep       = long long;
    using duration  = custom::chrono::duration<rep, period>;
    using time_point = custom::chrono::time_point<steady_clock>;

    static constexpr bool is_steady = true;


    static time_point now() noexcept {
#if defined _MSC_VER
        const long long freq    = detail::_query_performance_frequency(); // doesn't change after system boot
        const long long count   = detail::_query_performance_counter();

        static_assert(period::num == 1, "This assumes period::num == 1.");

        // 10 MHz is a very common QPC frequency on modern PCs. Optimizing for
        // this specific frequency can double the performance of this function by
        // avoiding the expensive frequency conversion path.
        
        constexpr long long tenMHz = 10000000LL;
        if (freq == tenMHz)
        {
            static_assert(period::den % tenMHz == 0, "It should never fail.");
            constexpr long long multiplier = period::den / tenMHz;
            return time_point(duration(count * multiplier));
        }
        else
        {
            // Instead of just having "(count * period::den) / freq",
            // the algorithm below prevents overflow when count is sufficiently large.
            // It assumes that freq * period::den does not overflow, which is currently true for nano period.
            // It is not realistic for count to accumulate to large values from zero with this assumption,
            // but the initial value of count could be large.

            const long long whole   = (count / freq) * period::den;
            const long long part    = (count % freq) * period::den / freq;
            return time_point(duration(whole + part));
        }
#elif defined __GNUG__
        timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return time_point(duration(seconds(ts.tv_sec) + nanoseconds(ts.tv_nsec)));
#endif
    }
};  // END steady_clock


#if defined _MSC_VER
using high_resolution_clock = steady_clock;
#elif defined __GNUG__
using high_resolution_clock = system_clock;
#endif

template<class _Duration>
using sys_time      = time_point<system_clock, _Duration>;
using sys_seconds   = sys_time<seconds>;
using sys_days      = sys_time<days>;
#pragma endregion Clock

CUSTOM_CHRONO_END

CUSTOM_END