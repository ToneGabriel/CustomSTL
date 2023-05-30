#pragma once

#if defined __GNUG__
#include "Utility.h"

#include <semaphore.h>

CUSTOM_BEGIN

template<int LeastMaxValue = INT_MAX>
class CountingSemaphore                 // Semaphore adaptor for sem_t
{
private:
    static_assert((LeastMaxValue >= 0 && LeastMaxValue <= INT_MAX), "Invalid semaphore count...");
    sem_t _semaphore;

public:
    // Constructors & Operators

    explicit CountingSemaphore(int desired) {
        assert((void("Invalid desired value..."), (desired >= 0 && desired <= LeastMaxValue)));
        sem_init(&_semaphore, 0, desired);
    }

    ~CountingSemaphore() {
        sem_destroy(&_semaphore);
    }

    CountingSemaphore(const CountingSemaphore&)            = delete;
    CountingSemaphore& operator=(const CountingSemaphore&) = delete;

public:
    // Main functions

    static int max() noexcept {
        return LeastMaxValue;
    }

    void release(int update = 1) noexcept {
        for (; update != 0; --update)
            sem_post(&_semaphore);     
    }

    void acquire() noexcept {
        sem_wait(&_semaphore);
    }

    bool try_acquire() noexcept {
        return (sem_trywait(&_semaphore) > 0) ? true : false;
    }

    template<class Clock, class Duration>
    bool try_acquire_until(const std::chrono::time_point<Clock, Duration>& absoluteTime) {
        auto seconds        = std::chrono::time_point_cast<std::chrono::seconds>(absoluteTime);
	    auto nanoseconds    = std::chrono::duration_cast<std::chrono::nanoseconds>(absoluteTime - seconds);
        struct timespec ts  =   {
                                    static_cast<std::time_t>(seconds.time_since_epoch().count()),
                                    static_cast<long>(nanoseconds.count())
                                };
        return ((sem_timedwait(&_semaphore, &ts) == 0 ) ? true : false);
        // TODO: check return of timedwait
    }

    template<class Rep, class Period>
    bool try_acquire_for(const std::chrono::duration<Rep, Period>& relativeTime) {
        return wait_until(  std::chrono::steady_clock::now() + 
                            std::chrono::ceil<typename std::chrono::steady_clock::duration>(relativeTime));
    }
}; // END CountingSemaphore

using BinarySemaphore = CountingSemaphore<1>;

CUSTOM_END

#elif defined _MSC_VER
#error NO Semaphore implementation
#endif      // __GNUG__ and _MSC_VER