#pragma once

#if defined __GNUG__
#include "c_thread.h"
#include <semaphore.h>

CUSTOM_BEGIN

template<int LeastMaxValue = INT_MAX>
class counting_semaphore                 // Semaphore adaptor for sem_t
{
private:
    static_assert((LeastMaxValue >= 0 && LeastMaxValue <= INT_MAX), "Invalid semaphore count.");
    using _ReqClock = custom::chrono::system_clock; // required due to sem_timedwait

    sem_t _semaphore;

public:
    // Constructors & Operators

    explicit counting_semaphore(int desired) noexcept {
        CUSTOM_ASSERT(desired >= 0 && desired <= LeastMaxValue, "Invalid desired value.");
        sem_init(&_semaphore, 0, desired);
    }

    ~counting_semaphore() noexcept {
        sem_destroy(&_semaphore);
    }

    counting_semaphore(const counting_semaphore&)            = delete;
    counting_semaphore& operator=(const counting_semaphore&) = delete;

public:
    // Main functions

    static constexpr int (max)() noexcept {
        return LeastMaxValue;
    }

    void release(int update = 1) noexcept {
        for (/*Empty*/; update != 0; --update)
            sem_post(&_semaphore);     
    }

    void acquire() noexcept {
        sem_wait(&_semaphore);
    }

    bool try_acquire() noexcept {
        return (sem_trywait(&_semaphore) > 0) ? true : false;
    }

    template<class Clock, class Duration,
    enable_if_t<is_same_v<Clock, _ReqClock>, bool> = true>
    bool try_acquire_until(const custom::chrono::time_point<Clock, Duration>& absoluteTime) {
        // if absoluteTime duration cast to seconds is 0, then nanoseconds duration will be representative
        // else if absoluteTime duration cast to seconds is > 0, then nanoseconds duration will be 0.
        auto secondsTime    = custom::chrono::time_point_cast<custom::chrono::seconds>(absoluteTime);
	    auto nanoseconds    = custom::chrono::duration_cast<custom::chrono::nanoseconds>(absoluteTime - secondsTime);
        struct timespec ts  =   {
                                    static_cast<std::time_t>(secondsTime.time_since_epoch().count()),
                                    static_cast<long>(nanoseconds.count())
                                };

        return ((sem_timedwait(&_semaphore, &ts) == 0 ) ? true : false);
    }

    template<class Rep, class Period>
    bool try_acquire_for(const custom::chrono::duration<Rep, Period>& relativeTime) {
        return try_acquire_until(   _ReqClock::now() + 
                                    custom::chrono::ceil<typename _ReqClock::duration>(relativeTime));
    }
}; // END counting_semaphore

using binary_semaphore = counting_semaphore<1>;

CUSTOM_END

#elif defined _MSC_VER
#error NO Semaphore implementation
#endif      // __GNUG__ and _MSC_VER