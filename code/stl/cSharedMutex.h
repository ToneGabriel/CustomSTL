#pragma once

#if defined __GNUG__
#include "xLock.h"


CUSTOM_BEGIN

class SharedMutex   // Adaptor for pthread_rwlock_t (read-write mutex)
{
public:
    using native_handle_type = pthread_rwlock_t;

private:
    friend class SharedTimedMutex;

    pthread_rwlock_t _rwMutex;

public:
    // Constructors & Operators

    SharedMutex() {
        pthread_rwlock_init(&_rwMutex, nullptr);
    }

    ~SharedMutex() {
        pthread_rwlock_destroy(&_rwMutex);
    }

    SharedMutex(const SharedMutex&)             = delete;
    SharedMutex& operator= (const SharedMutex&) = delete;

public:
    // Main functions

// Exclusive (Write)
    void lock() {
        pthread_rwlock_wrlock(&_rwMutex);
    }

    bool try_lock() {
        switch (pthread_rwlock_trywrlock(&_rwMutex))
        {
            case 0:
                return true;
            case EBUSY:
                return false;
            default:
                throw std::runtime_error("SharedMutex exclusive lock failed.");
        }
    }

    void unlock() {
        pthread_rwlock_unlock(&_rwMutex);               // read / write
    }
// end Exclusive

// Shared (Read)
    void lock_shared() {
        pthread_rwlock_rdlock(&_rwMutex);
    }

    bool try_lock_shared() {
        switch (pthread_rwlock_tryrdlock(&_rwMutex))
        {
            case 0:
                return true;
            case EBUSY:
                return false;
            default:
                throw std::runtime_error("SharedMutex shared lock failed.");
        }
    }

    void unlock_shared() {
        pthread_rwlock_unlock(&_rwMutex);               // same as unlock
    }
// end Shared

    native_handle_type native_handle() {
        return _rwMutex;
    }
};  // END SharedMutex


class SharedTimedMutex : private SharedMutex
{
private:
    using _Base         = SharedMutex;
    
#ifdef _GLIBCXX_USE_PTHREAD_RWLOCK_CLOCKLOCK
    using _ReqClock     = custom::chrono::steady_clock;
#else
    using _ReqClock     = custom::chrono::system_clock;
#endif

public:
    // Constructors & Operators

    SharedTimedMutex()                                      = default;
    ~SharedTimedMutex()                                     = default;

    SharedTimedMutex(const SharedTimedMutex&)               = delete;
    SharedTimedMutex& operator=(const SharedTimedMutex&)    = delete;

public:
    // Main Functions

// Exclusive (Write)
    void lock() {
        _Base::lock();
    }

    bool try_lock() {
        return _Base::try_lock();
    }

    void unlock() {
        _Base::unlock();
    }

    template<class Clock, class duration,
    enable_if_t<is_same_v<Clock, _ReqClock>, bool> = true>
    bool try_lock_until(const custom::chrono::time_point<Clock, duration>& absoluteTime) {
        // if absoluteTime duration cast to seconds is 0, then nanoseconds duration will be representative
        // else if absoluteTime duration cast to seconds is > 0, then nanoseconds duration will be 0.
        auto secondsTime    = custom::chrono::time_point_cast<custom::chrono::Seconds>(absoluteTime);
        auto nanoseconds    = custom::chrono::duration_cast<custom::chrono::Nanoseconds>(absoluteTime - secondsTime);
        struct timespec ts  =   {
                                    static_cast<std::time_t>(secondsTime.time_since_epoch().count()),
                                    static_cast<long>(nanoseconds.count())
                                };

#ifdef _GLIBCXX_USE_PTHREAD_RWLOCK_CLOCKLOCK
        return ((pthread_rwlock_clockwrlock(&_rwMutex, CLOCK_MONOTONIC, &ts) == 0 ) ? true : false);
#else
        return ((pthread_rwlock_timedwrlock(&_rwMutex, &ts) == 0 ) ? true : false);
#endif
    }
    
    template<class Rep, class Period>
    bool try_lock_for(const custom::chrono::duration<Rep, Period>& relativeTime) {
        return try_lock_until(  _ReqClock::now() + 
                                custom::chrono::ceil<typename _ReqClock::duration>(relativeTime));
    }
// end Exclusive

// Shared (Read)
    void lock_shared() {
        _Base::lock_shared();
    }

    bool try_lock_shared() {
        return _Base::try_lock_shared();
    }

    void unlock_shared() {
        _Base::unlock_shared();
    }

    template<class Clock, class duration,
    enable_if_t<is_same_v<Clock, _ReqClock>, bool> = true>
    bool try_lock_shared_until(const custom::chrono::time_point<Clock, duration>& absoluteTime) {
        // if absoluteTime duration cast to seconds is 0, then nanoseconds duration will be representative
        // else if absoluteTime duration cast to seconds is > 0, then nanoseconds duration will be 0.
        auto secondsTime    = custom::chrono::time_point_cast<custom::chrono::Seconds>(absoluteTime);
        auto nanoseconds    = custom::chrono::duration_cast<custom::chrono::Nanoseconds>(absoluteTime - secondsTime);
        struct timespec ts  =   {
                                    static_cast<std::time_t>(secondsTime.time_since_epoch().count()),
                                    static_cast<long>(nanoseconds.count())
                                };

#ifdef _GLIBCXX_USE_PTHREAD_RWLOCK_CLOCKLOCK
        return ((pthread_rwlock_clockrdlock(&_rwMutex, CLOCK_MONOTONIC, &ts) == 0 ) ? true : false);
#else
        return ((pthread_rwlock_timedrdlock(&_rwMutex, &ts) == 0 ) ? true : false);
#endif
    }

    template<class Rep, class Period>
    bool try_lock_shared_for(const custom::chrono::duration<Rep, Period>& relativeTime) {
        return try_lock_shared_until(   _ReqClock::now() + 
                                        custom::chrono::ceil<typename _ReqClock::duration>(relativeTime));
    }
// end Shared
};  // END SharedTimedMutex

CUSTOM_END

#elif defined _MSC_VER
#error NO SharedMutex implementation
#endif