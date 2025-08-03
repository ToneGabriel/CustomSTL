#pragma once
#include "custom/type_traits.h"
#include "custom/limits.h"
#include "custom/ratio.h"
#include <ctime>        // std::time_t

#if defined _MSC_VER    // for Clock::now()
#include <windows.h>
#elif defined __GNUG__
#include <sys/time.h>
#endif                  // _MSC_VER and __GNUG__


CUSTOM_BEGIN

#define CUSTOM_CHRONO_BEGIN namespace chrono {
#define CUSTOM_CHRONO_END }

CUSTOM_CHRONO_BEGIN

#if defined _MSC_VER    // for Clock::now()
CUSTOM_DETAIL_BEGIN

#define UNIX_EPOCH 116444736000000000LL

inline long long _get_system_time_precise() noexcept
{
    FILETIME fileTime;
    ULARGE_INTEGER theTime;
    
    GetSystemTimePreciseAsFileTime(&fileTime);
    theTime.LowPart     = fileTime.dwLowDateTime;
    theTime.HighPart    = fileTime.dwHighDateTime;

    return theTime.QuadPart;
}

inline long long _query_performance_counter() noexcept
{
    LARGE_INTEGER itCount{};
    QueryPerformanceCounter(&itCount);
    return itCount.QuadPart;
}

inline long long _query_performance_frequency() noexcept
{
    LARGE_INTEGER itFreq{};
    QueryPerformanceFrequency(&itFreq);
    return itFreq.QuadPart;
}

CUSTOM_DETAIL_END
#endif  // _MSC_VER


// duration values
template<class Rep>
struct duration_values  // gets arithmetic properties of a type
{
    static constexpr Rep zero() noexcept
    {
        return Rep(0);
    }

    static constexpr Rep (min)() noexcept
    {
        return numeric_limits<Rep>::lowest();
    }

    static constexpr Rep (max)() noexcept
    {
        return (numeric_limits<Rep>::max)();
    }
};  // END duration_values


// is clock
template<class Clock, class = void>
constexpr bool is_clock_v = false;

template<class Clock>
constexpr bool is_clock_v<Clock, void_t<typename Clock::Rep,
                                        typename Clock::Period,
                                        typename Clock::duration,
                                        typename Clock::time_point,
                                        decltype(Clock::is_steady),
                                        decltype(Clock::now())>> = true;

template<class Clock>
struct is_clock : bool_constant<is_clock_v<Clock>> {};

CUSTOM_CHRONO_END

CUSTOM_END