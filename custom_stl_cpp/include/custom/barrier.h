#pragma once

#if defined __GNUG__
#include "custom/thread.h"

CUSTOM_BEGIN

// template<class CompletionFunction /*TODO: check*/>
class barrier                       // barrier adaptor for pthread_barrier_t
{
private:
    pthread_barrier_t _barrier;

public:
    // Constructors & Operators

    explicit barrier(int expected) noexcept
    {
        CUSTOM_ASSERT(expected >= 0, "Invalid expected value.");
        pthread_barrier_init(&_barrier, nullptr, expected);
    }

    ~barrier() noexcept
    {
        pthread_barrier_destroy(&_barrier);
    }

    barrier(const barrier&)            = delete;
    barrier& operator=(const barrier&) = delete;

public:
    // Main functions

    static constexpr int (max)() noexcept
    {
        return INT_MAX;
    }

    void arrive_and_wait() noexcept
    {
        pthread_barrier_wait(&_barrier);
    }

    // void arrive();
    // void wait();
    // void arrive_and_drop();

}; // END barrier

CUSTOM_END

#elif defined _MSC_VER
#error NO Barrier implementation
#endif      // __GNUG__ and _MSC_VER