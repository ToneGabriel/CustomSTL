#pragma once
#include "cTypeTraits.h"
#include "cLimits.h"
#include "cRatio.h"
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

CUSTOM_DETAIL_END
#endif


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

CUSTOM_CHRONO_END

CUSTOM_END