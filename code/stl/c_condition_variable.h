#pragma once

#if defined __GNUG__
#include "c_mutex.h"
#include "c_memory.h"


CUSTOM_BEGIN

enum class cv_status
{
    no_timeout,
    timeout
};

class condition_variable         // Condition variable adaptor for pthread_cond_t
{
public:
    using native_handle_type = pthread_cond_t;

private:
    pthread_cond_t _cv;

public:
    // Constructors & Operators

    condition_variable() {
        pthread_cond_init(&_cv, nullptr);
    }

    ~condition_variable() {
        pthread_cond_destroy(&_cv);
    }

    condition_variable(const condition_variable&)            = delete;
    condition_variable& operator=(const condition_variable&) = delete;

public:
    // Main functions

    void notify_one() noexcept {
        pthread_cond_signal(&_cv);
    }

    void notify_all() noexcept {
        pthread_cond_broadcast(&_cv);
    }

    void wait(unique_lock<mutex>& lock) {
        pthread_cond_wait(&_cv, &lock.mutex()->_mutex);
    }

    template<class Predicate>
    void wait(unique_lock<mutex>& lock, Predicate pred) {
        while (!pred())
            pthread_cond_wait(&_cv, &lock.mutex()->_mutex);
    }

    template<class Clock, class Duration>
    cv_status wait_until(unique_lock<mutex>& lock, const custom::chrono::time_point<Clock, Duration>& absoluteTime) {
        // if absoluteTime duration cast to seconds is 0, then nanoseconds duration will be representative
        // else if absoluteTime duration cast to seconds is > 0, then nanoseconds duration will be 0.
        auto secondsTime    = custom::chrono::time_point_cast<custom::chrono::seconds>(absoluteTime);
	    auto nanoseconds    = custom::chrono::duration_cast<custom::chrono::nanoseconds>(absoluteTime - secondsTime);
        struct timespec ts  =   {
                                    static_cast<std::time_t>(secondsTime.time_since_epoch().count()),
                                    static_cast<long>(nanoseconds.count())
                                };

        pthread_cond_timedwait(&_cv, &lock.mutex()->_mutex, &ts);
        return ((Clock::now() < absoluteTime) ? cv_status::no_timeout : cv_status::timeout);
    }

    template<class Clock, class Duration, class Predicate>
    bool wait_until(unique_lock<mutex>& lock, const custom::chrono::time_point<Clock, Duration>& absoluteTime, Predicate pred) {
        while (!pred())
            if (wait_until(lock, absoluteTime) == cv_status::timeout)
                return pred();

        return true;
    }
    
    template<class Rep, class Period>
    cv_status wait_for(unique_lock<mutex>& lock, const custom::chrono::duration<Rep, Period>& relativeTime) {
	    return wait_until(  lock,
                            custom::chrono::steady_clock::now() + 
                            custom::chrono::ceil<typename custom::chrono::steady_clock::duration>(relativeTime));
    }

    template<class Rep, class Period, class Predicate>
    bool wait_for(unique_lock<mutex>& lock, const custom::chrono::duration<Rep, Period>& relativeTime, Predicate pred) {
	    return wait_until(  lock,
                            custom::chrono::steady_clock::now() + 
                            custom::chrono::ceil<typename custom::chrono::steady_clock::duration>(relativeTime),
                            pred);
    }

    native_handle_type native_handle() {
        return _cv;
    }
}; // END condition_variable


class condition_variable_any
{
private:
    shared_ptr<mutex> _mutexPtr;
    condition_variable _cv;

public:
    // Constructors & Operators

    condition_variable_any()
        : _mutexPtr(custom::make_shared<mutex>()) { /*Empty*/ }

    ~condition_variable_any() = default;

    condition_variable_any(const condition_variable_any&)            = delete;
    condition_variable_any& operator=(const condition_variable_any&) = delete;    

public:
    // Main functions

    void notify_one() noexcept {
        lock_guard<mutex> lock(*_mutexPtr);
        _cv.notify_one();
    }

    void notify_all() noexcept {
        lock_guard<mutex> lock(*_mutexPtr);
        _cv.notify_all();
    }

    template<class Lock>
    void wait(Lock& lock) {
        shared_ptr<mutex> auxMutexPtr = _mutexPtr; // for immunity to *this destruction
        unique_lock<mutex> unqLock(*auxMutexPtr);
        unlock_guard<Lock> unlock(lock);
        // *auxMutexPtr must be unlocked before re-locking "lock" parameter so move
	    // ownership of *auxMutexPtr lock to an object with shorter lifetime.
	    unique_lock<mutex> unqLock2(custom::move(unqLock));
	    _cv.wait(unqLock2);
    }   // relock lock

    template<class Lock, class Predicate>
    void wait(Lock& lock, Predicate pred) {
        while (!pred())
            wait(lock);
    }

    template<class Lock, class Clock, class Duration>
    cv_status wait_until(Lock& lock, const custom::chrono::time_point<Clock, Duration>& absoluteTime) {
        shared_ptr<mutex> auxMutexPtr = _mutexPtr;
        unique_lock<mutex> unqLock(*auxMutexPtr);
        unlock_guard<Lock> unlock(lock);

	    unique_lock<mutex> unqLock2(custom::move(unqLock));
	    return _cv.wait_until(unqLock2, absoluteTime);
    }

    template<class Lock, class Clock, class Duration, class Predicate>
    bool wait_until(Lock& lock, const custom::chrono::time_point<Clock, Duration>& absoluteTime, Predicate pred) {
        while (!pred())
            if (wait_until(lock, absoluteTime) == cv_status::timeout)
                return pred();

        return true;
    }

    template<class Lock, class Rep, class Period>
    cv_status wait_for(Lock& lock, const custom::chrono::duration<Rep, Period>& relativeTime) {
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
}; // END condition_variable_any


struct _UINT_Is_Zero   // used as predicate in timed_mutex/recursive_timed_mutex::try_lock_until
{
    const unsigned int& _UInt;

    bool operator()() const {
        return _UInt == 0;
    }
};

class timed_mutex
{
public:
    using native_handle_type = typename mutex::native_handle_type;

private:
    mutex _mutex;
    condition_variable _cv;
    unsigned int _locked;

public:
    // Constructors & Operators

    timed_mutex() noexcept
        : _locked(0) { /*Empty*/ }

    timed_mutex(const timed_mutex&)            = delete;
    timed_mutex& operator=(const timed_mutex&) = delete;

public:
    // Main functions

    void lock() {
        unique_lock<mutex> lock(_mutex);

        while (_locked != 0)
            _cv.wait(lock);

        _locked = UINT_MAX;
    }

    bool try_lock() noexcept {
        lock_guard<mutex> lock(_mutex);

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
            lock_guard<mutex> lock(_mutex);
            _locked = 0;
        }

        _cv.notify_one();
    }

    template<class Clock, class Duration>
    bool try_lock_until(const custom::chrono::time_point<Clock, Duration>& absoluteTime) {
        unique_lock<mutex> lock(_mutex);

        if (_cv.wait_until(lock, absoluteTime, _UINT_Is_Zero{_locked}) == false)
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
}; // END timed_mutex


class recursive_timed_mutex
{
public:
    using native_handle_type = typename mutex::native_handle_type;

private:
    mutex _mutex;
    condition_variable _cv;
    unsigned int _locked;
    thread::id _owner;

public:
    // Constructors & Operators

    recursive_timed_mutex() noexcept
        : _locked(0) { /*Empty*/ }

    recursive_timed_mutex(const recursive_timed_mutex&)            = delete;
    recursive_timed_mutex& operator=(const recursive_timed_mutex&) = delete;

public:
    // Main functions

    void lock() {
        const thread::id tid = custom::this_thread::get_id();
        unique_lock<mutex> lock(_mutex);

        if (tid == _owner)
            if (_locked < UINT_MAX)
                ++_locked;
            else
                throw std::runtime_error("Resource is busy.");
        else
        {
            while (_locked != 0)
                _cv.wait(lock);

            _locked = 1;
            _owner  = tid;
        }
    }

    bool try_lock() noexcept {
        const thread::id tid = custom::this_thread::get_id();
        lock_guard<mutex> lock(_mutex);

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
            lock_guard<mutex> lock(_mutex);
            --_locked;
            if (_locked == 0)
            {
                notify = true;
                _owner  = thread::id();
            }
        }

        if (notify)
            _cv.notify_one();
    }

    template<class Clock, class Duration>
    bool try_lock_until(const custom::chrono::time_point<Clock, Duration>& absoluteTime) {
        const thread::id tid = custom::this_thread::get_id();
        unique_lock<mutex> lock(_mutex);

        if (tid == _owner)
            if (_locked < UINT_MAX)
                ++_locked;
            else
                return false;
        else
        {
            if (!_cv.wait_until(lock, absoluteTime, _UINT_Is_Zero{_locked}))
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
}; // END recursive_timed_mutex

CUSTOM_END

#elif defined _MSC_VER
#error NO Condition Variable implementation
#endif