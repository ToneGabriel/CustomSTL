#pragma once

#if defined __GNUG__
#include "Utility.h"
#include "Tuple.h"
#include "Memory.h"
#include "Chrono.h"

#include <pthread.h>

#if defined _WIN32
#include <windows.h>    // for hardware_concurrency()
#elif defined __linux__
#include <sys/sysinfo.h>
#endif      // _WIN32 and __linux__

CUSTOM_BEGIN

class Thread        // Thread adaptor for pthread_t
{
public:
    class ID;
    using NativeHandleType  = pthread_t;

private:
    using Invoker       = void*(*)(void*);
    pthread_t _thread   = 0;

private:
    // Core functions

    template<class CallableTuple, size_t... Indices>
    static void* _invoke_impl(void* args) noexcept {
        const UniquePtr<CallableTuple> callable(static_cast<CallableTuple*>(args));
        CallableTuple& derefCallable = *callable;
        custom::invoke(custom::move(custom::get<Indices>(derefCallable))...);

        return nullptr;
    }

    template<class CallableTuple, size_t... Indices>
    static constexpr Invoker _get_invoke_impl(IndexSequence<Indices...>) noexcept {
        return reinterpret_cast<Invoker>(_invoke_impl<CallableTuple, Indices...>);
    }

public:
    // Constructors

    Thread()                = default;
    Thread(const Thread&)   = delete;

    template<class Functor, class... Args, 
    EnableIf_t<!IsSame_v<Decay_t<Functor>, Thread>, bool> = true>
    Thread(Functor&& func, Args&&... args) {
        // forward functor and arguments as tuple pointer to match "pthread_create" procedure
        using CallableTuple = Tuple<Decay_t<Functor>, Decay_t<Args>...>;

        Invoker invoker = _get_invoke_impl<CallableTuple>(MakeIndexSequence<1 + sizeof...(Args)>{});
        auto callable   = custom::make_unique<CallableTuple>(custom::forward<Functor>(func), custom::forward<Args>(args)...);

        if (pthread_create(&_thread, nullptr, invoker, callable.get()) == 0)
            (void)callable.release();     // ownership transferred to the thread
        else
        {
            _thread = 0;
            throw std::runtime_error("Thread creation failed...");
        }
    }

    Thread(Thread&& other) noexcept {
        _move(custom::move(other));
    }

    ~Thread() {
        if (joinable())
            std::terminate();
    }

public:
    // Operators

    Thread& operator=(const Thread&) = delete;

    Thread& operator=(Thread&& other) noexcept {
        if (*this != other)
        {
            if (joinable())
                std::terminate();

            _move(custom::move(other));
        }

        return *this;
    }

    bool operator==(const Thread& other) const {
        return (pthread_equal(_thread, other._thread) != 0);
    }

    bool operator!=(const Thread& other) const {
        return !(*this == other);
    }

public:
    // Main functions

    bool joinable() const noexcept {
        return _thread != 0;
    }

    void join() {
        if (!joinable())
            throw std::runtime_error("Thread not joinable...");

        if (_thread == pthread_self())
            throw std::runtime_error("Resource deadlock would occur...");

        if (pthread_join(_thread, nullptr) != 0)
            throw std::runtime_error("Thread join failed...");

        _thread = 0;
    }

    void detach() {
        if (!joinable())
            throw std::runtime_error("Thread not joinable...");

        if (pthread_detach(_thread) != 0)
            throw std::runtime_error("Thread detach failed...");

        _thread = 0;
    }

    ID get_id() const noexcept;

    NativeHandleType native_handle() noexcept {
        return _thread;
    }

    static unsigned int hardware_concurrency() noexcept {
#if defined _WIN32
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        return sysinfo.dwNumberOfProcessors;
#elif defined __linux__
        return get_nprocs();
#endif  // _WIN32 and __linux__
    }

private:
    // Helpers

    void _move(Thread&& other) {
        _thread         = other._thread;
        other._thread   = 0;
    }
}; // END Thread


namespace this_thread   // TODO: check
{
    Thread::ID get_id() noexcept;

    inline void yield() noexcept {
        sched_yield();
    }

    template<class Rep, class Period>
    void sleep_for(const custom::chrono::Duration<Rep, Period>& relativeTime) {
        if (relativeTime <= relativeTime.zero())
	        return;

        // if relativeTime duration cast to seconds is 0, then nanoseconds duration will be representative
        // else if relativeTime duration cast to seconds is > 0, then nanoseconds duration will be 0.
        auto seconds        = custom::chrono::duration_cast<custom::chrono::Seconds>(relativeTime);
	    auto nanoseconds    = custom::chrono::duration_cast<custom::chrono::Nanoseconds>(relativeTime - seconds);
        struct timespec ts  =   {
                                    static_cast<std::time_t>(seconds.count()),
                                    static_cast<long>(nanoseconds.count())
                                };

        while (::nanosleep(&ts, &ts) == -1 && errno == EINTR)
        {
            // Empty

            // 1'st param (const) is the given time to sleep;
            // 2'nd param (no const) returns the remaining time if fail;
            // while ::nanosleep keeps failing, it is recalled with the remaining time...
            // ...assigned to given time;
        }
    }

    template<class Clock, class Duration>
    void sleep_until(const custom::chrono::TimePoint<Clock, Duration>& absoluteTime) {
        auto now = Clock::now();

        while (now < absoluteTime)
        {
            sleep_for(absoluteTime - now);
            now = Clock::now();
        }
    }

} // END namespace this_thread


class Thread::ID
{
private:
    pthread_t _threadID;

public:

    ID() : _threadID(0) { /*Empty*/ }

    bool operator==(const ID& other) const {
        return (pthread_equal(_threadID, other._threadID) != 0);
    }

    bool operator!=(const ID& other) const {
        return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& os, const ID& id) {
        os << id._threadID;
        return os;
    }

    friend Thread::ID Thread::get_id() const noexcept;
    friend Thread::ID this_thread::get_id() noexcept;

private:

    ID(const pthread_t& thr) : _threadID(thr) { /*Empty*/ }
}; // END Thread::ID


// other definitions
inline Thread::ID Thread::get_id() const noexcept {
    return _thread;             // calls private constructor of Thread::ID
}

inline Thread::ID this_thread::get_id() noexcept {
    return pthread_self();      // calls private constructor of Thread::ID
}

CUSTOM_END

#elif defined _MSC_VER
#error NO Thread implementation
#endif      // __GNUG__ and _MSC_VER