#pragma once

#if defined __GNUG__
#include "cMutex.h"
#include "cMemory.h"


CUSTOM_BEGIN

enum class CVStatus
{
    NoTimeout,
    Timeout
};

class ConditionVariable         // Condition variable adaptor for pthread_cond_t
{
public:
    using native_handle_type = pthread_cond_t;

private:
    pthread_cond_t _conditionVar;

public:
    // Constructors & Operators

    ConditionVariable() {
        pthread_cond_init(&_conditionVar, nullptr);
    }

    ~ConditionVariable() {
        pthread_cond_destroy(&_conditionVar);
    }

    ConditionVariable(const ConditionVariable&)            = delete;
    ConditionVariable& operator=(const ConditionVariable&) = delete;

public:
    // Main functions

    void notify_one() noexcept {
        pthread_cond_signal(&_conditionVar);
    }

    void notify_all() noexcept {
        pthread_cond_broadcast(&_conditionVar);
    }

    void wait(UniqueLock<Mutex>& lock) {
        pthread_cond_wait(&_conditionVar, &lock.mutex()->_mutex);
    }

    template<class Predicate>
    void wait(UniqueLock<Mutex>& lock, Predicate pred) {
        while (!pred())
            pthread_cond_wait(&_conditionVar, &lock.mutex()->_mutex);
    }

    template<class Clock, class duration>
    CVStatus wait_until(UniqueLock<Mutex>& lock, const custom::chrono::time_point<Clock, duration>& absoluteTime) {
        // if absoluteTime duration cast to seconds is 0, then nanoseconds duration will be representative
        // else if absoluteTime duration cast to seconds is > 0, then nanoseconds duration will be 0.
        auto secondsTime    = custom::chrono::time_point_cast<custom::chrono::Seconds>(absoluteTime);
	    auto nanoseconds    = custom::chrono::duration_cast<custom::chrono::Nanoseconds>(absoluteTime - secondsTime);
        struct timespec ts  =   {
                                    static_cast<std::time_t>(secondsTime.time_since_epoch().count()),
                                    static_cast<long>(nanoseconds.count())
                                };

        pthread_cond_timedwait(&_conditionVar, &lock.mutex()->_mutex, &ts);
        return ((Clock::now() < absoluteTime) ? CVStatus::NoTimeout : CVStatus::Timeout);
    }

    template<class Clock, class duration, class Predicate>
    bool wait_until(UniqueLock<Mutex>& lock, const custom::chrono::time_point<Clock, duration>& absoluteTime, Predicate pred) {
        while (!pred())
            if (wait_until(lock, absoluteTime) == CVStatus::Timeout)
                return pred();

        return true;
    }
    
    template<class Rep, class Period>
    CVStatus wait_for(UniqueLock<Mutex>& lock, const custom::chrono::duration<Rep, Period>& relativeTime) {
	    return wait_until(  lock,
                            custom::chrono::steady_clock::now() + 
                            custom::chrono::ceil<typename custom::chrono::steady_clock::duration>(relativeTime));
    }

    template<class Rep, class Period, class Predicate>
    bool wait_for(UniqueLock<Mutex>& lock, const custom::chrono::duration<Rep, Period>& relativeTime, Predicate pred) {
	    return wait_until(  lock,
                            custom::chrono::steady_clock::now() + 
                            custom::chrono::ceil<typename custom::chrono::steady_clock::duration>(relativeTime),
                            pred);
    }

    native_handle_type native_handle() {
        return _conditionVar;
    }
}; // END ConditionVariable


class ConditionVariableAny
{
private:
    SharedPtr<Mutex> _mutexPtr;
    ConditionVariable _cond;

public:
    // Constructors & Operators

    ConditionVariableAny()
        : _mutexPtr(custom::make_shared<Mutex>()) { /*Empty*/ }

    ~ConditionVariableAny() = default;

    ConditionVariableAny(const ConditionVariableAny&)            = delete;
    ConditionVariableAny& operator=(const ConditionVariableAny&) = delete;    

public:
    // Main functions

    void notify_one() noexcept {
        LockGuard<Mutex> lock(*_mutexPtr);
        _cond.notify_one();
    }

    void notify_all() noexcept {
        LockGuard<Mutex> lock(*_mutexPtr);
        _cond.notify_all();
    }

    template<class Lock>
    void wait(Lock& lock) {
        SharedPtr<Mutex> auxMutexPtr = _mutexPtr; // for immunity to *this destruction
        UniqueLock<Mutex> unqLock(*auxMutexPtr);
        UnlockGuard<Lock> unlock(lock);
        // *auxMutexPtr must be unlocked before re-locking "lock" parameter so move
	    // ownership of *auxMutexPtr lock to an object with shorter lifetime.
	    UniqueLock<Mutex> unqLock2(custom::move(unqLock));
	    _cond.wait(unqLock2);
    }   // relock lock

    template<class Lock, class Predicate>
    void wait(Lock& lock, Predicate pred) {
        while (!pred())
            wait(lock);
    }

    template<class Lock, class Clock, class duration>
    CVStatus wait_until(Lock& lock, const custom::chrono::time_point<Clock, duration>& absoluteTime) {
        SharedPtr<Mutex> auxMutexPtr = _mutexPtr;
        UniqueLock<Mutex> unqLock(*auxMutexPtr);
        UnlockGuard<Lock> unlock(lock);

	    UniqueLock<Mutex> unqLock2(custom::move(unqLock));
	    return _cond.wait_until(unqLock2, absoluteTime);
    }

    template<class Lock, class Clock, class duration, class Predicate>
    bool wait_until(Lock& lock, const custom::chrono::time_point<Clock, duration>& absoluteTime, Predicate pred) {
        while (!pred())
            if (wait_until(lock, absoluteTime) == CVStatus::Timeout)
                return pred();

        return true;
    }

    template<class Lock, class Rep, class Period>
    CVStatus wait_for(Lock& lock, const custom::chrono::duration<Rep, Period>& relativeTime) {
	    return wait_until(  lock,
                            custom::chrono::steady_clock::now() + 
                            custom::chrono::ceil<typename custom::chrono::steady_clock::duration>(relativeTime));
    }

    template<class Lock, class Rep, class Period, class Predicate>
    bool wait_for(Lock& lock, const custom::chrono::duration<Rep, Period>& relativeTime, Predicate pred) {
	    return wait_until(  lock,
                            custom::chrono::steady_clock::now() + 
                            custom::chrono::ceil<typename custom::chrono::steady_clock::duration>(relativeTime),
                            pred);
    }
}; // END ConditionVariableAny


struct UINTIsZero   // used as predicate in TimedMutex/RecursiveTimedMutex::try_lock_until
{
    const unsigned int& UInt;

    bool operator()() const {
        return UInt == 0;
    }
};

class TimedMutex
{
public:
    using native_handle_type = typename Mutex::native_handle_type;

private:
    Mutex _mutex;
    ConditionVariable _conditionVar;
    unsigned int _locked;

public:
    // Constructors & Operators

    TimedMutex() noexcept
        : _locked(0) { /*Empty*/ }

    TimedMutex(const TimedMutex&)            = delete;
    TimedMutex& operator=(const TimedMutex&) = delete;

public:
    // Main functions

    void lock() {
        UniqueLock<Mutex> lock(_mutex);

        while (_locked != 0)
            _conditionVar.wait(lock);

        _locked = UINT_MAX;
    }

    bool try_lock() noexcept {
        LockGuard<Mutex> lock(_mutex);

        if (_locked != 0)
            return false;
        else
        {
            _locked = UINT_MAX;
            return true;
        }
    }

    void unlock() {
        {
            LockGuard<Mutex> lock(_mutex);
            _locked = 0;
        }

        _conditionVar.notify_one();
    }

    template<class Clock, class duration>
    bool try_lock_until(const custom::chrono::time_point<Clock, duration>& absoluteTime) {
        UniqueLock<Mutex> lock(_mutex);

        if (_conditionVar.wait_until(lock, absoluteTime, UINTIsZero{_locked}) == false)
            return false;

        _locked = UINT_MAX;
        return true;
    }

    template<class Rep, class Period>
    bool try_lock_for(const custom::chrono::duration<Rep, Period>& relativeTime) {
        return try_lock_until(  custom::chrono::steady_clock::now() + 
                                custom::chrono::ceil<typename custom::chrono::steady_clock::duration>(relativeTime));
    }

    native_handle_type native_handle() {
        return _mutex.native_handle();
    }
}; // END TimedMutex


class RecursiveTimedMutex
{
public:
    using native_handle_type = typename Mutex::native_handle_type;

private:
    Mutex _mutex;
    ConditionVariable _conditionVar;
    unsigned int _locked;
    thread::id _owner;

public:
    // Constructors & Operators

    RecursiveTimedMutex() noexcept
        : _locked(0) { /*Empty*/ }

    RecursiveTimedMutex(const RecursiveTimedMutex&)            = delete;
    RecursiveTimedMutex& operator=(const RecursiveTimedMutex&) = delete;

public:
    // Main functions

    void lock() {
        const thread::id tid = custom::this_thread::get_id();
        UniqueLock<Mutex> lock(_mutex);

        if (tid == _owner)
            if (_locked < UINT_MAX)
                ++_locked;
            else
                throw std::runtime_error("Resource is busy.");
        else
        {
            while (_locked != 0)
                _conditionVar.wait(lock);

            _locked = 1;
            _owner  = tid;
        }
    }

    bool try_lock() noexcept {
        const thread::id tid = custom::this_thread::get_id();
        LockGuard<Mutex> lock(_mutex);

        if (tid == _owner)
            if (_locked < UINT_MAX)
                ++_locked;
            else
                return false;
        else
            if (_locked != 0)
                return false;
            else
            {
                _locked = 1;
                _owner  = tid;
            }
        
        return true;
    }

    void unlock() {
        bool notify = false;

        {
            LockGuard<Mutex> lock(_mutex);
            --_locked;
            if (_locked == 0)
            {
                notify = true;
                _owner  = thread::id();
            }
        }

        if (notify)
            _conditionVar.notify_one();
    }

    template<class Clock, class duration>
    bool try_lock_until(const custom::chrono::time_point<Clock, duration>& absoluteTime) {
        const thread::id tid = custom::this_thread::get_id();
        UniqueLock<Mutex> lock(_mutex);

        if (tid == _owner)
            if (_locked < UINT_MAX)
                ++_locked;
            else
                return false;
        else
        {
            if (!_conditionVar.wait_until(lock, absoluteTime, UINTIsZero{_locked}))
                return false;

            _locked = 1;
            _owner  = tid;
        }

        return true;
    }

    template<class Rep, class Period>
    bool try_lock_for(const custom::chrono::duration<Rep, Period>& relativeTime) {
        return try_lock_until(  custom::chrono::steady_clock::now() + 
                                custom::chrono::ceil<typename custom::chrono::steady_clock::duration>(relativeTime));
    }

    native_handle_type native_handle() {
        return _mutex.native_handle();
    }
}; // END RecursiveTimedMutex

CUSTOM_END

#elif defined _MSC_VER
#error NO Condition Variable implementation
#endif