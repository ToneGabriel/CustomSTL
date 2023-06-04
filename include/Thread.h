#pragma once

#if defined __GNUG__
#include "Utility.h"
#include "Functional.h"
#include "Tuple.h"

#include <pthread.h>
#include <chrono>


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
    static void* _invoke_impl(void* args) {
        CallableTuple* callable = static_cast<CallableTuple*>(args);
        custom::invoke(custom::move(custom::get<Indices>(*callable))...);
        delete callable;

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
    EnableIf_t<!IsSame<Decay_t<Functor>, Thread>::Value, bool> = true>
    Thread(Functor&& func, Args&&... args) {
        // forward functor and arguments as tuple pointer to match "pthread_create" procedure

        using CallableTuple     = Tuple<Decay_t<Functor>, Decay_t<Args>...>;
        Invoker invoker         = _get_invoke_impl<CallableTuple>(MakeIndexSequence<1 + sizeof...(Args)>{});
        CallableTuple* callable = new CallableTuple(custom::forward<Functor>(func), custom::forward<Args>(args)...);

        if (pthread_create(&_thread, nullptr, invoker, callable) != 0)
        {
            _thread = 0;
            delete callable;
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

    bool joinable() const {
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

    static unsigned int hardware_concurrency() {
        #if defined _WIN32
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        return sysinfo.dwNumberOfProcessors;
        #elif defined __linux__
        return get_nprocs();
        #endif
    }

private:
    // Helpers

    void _move(Thread&& other) {
        _thread         = other._thread;
        other._thread   = 0;
    }
}; // END Thread


namespace this_thread
{
    Thread::ID get_id() noexcept;

    void yield() noexcept {
        sched_yield();
    }

    template<class Clock, class Duration>
    void sleep_until(const std::chrono::time_point<Clock, Duration>& absoluteTime) {
        // TODO: implement
    }

    template<class Rep, class Period>
    void sleep_for(const std::chrono::duration<Rep, Period>& relativetime) {
        // if (relativetime <= relativetime.zero())
	    //     return;

        // auto seconds        = std::chrono::time_point_cast<std::chrono::seconds>(relativetime);
	    // auto nanoseconds    = std::chrono::duration_cast<std::chrono::nanoseconds>(relativetime - seconds);
        // struct timespec ts  =   {
        //                             static_cast<std::time_t>(seconds.time_since_epoch().count()),
        //                             static_cast<long>(nanoseconds.count())
        //                         };

        // while (::nanosleep(&ts, &ts) == -1 && errno == EINTR)
        // { /*Empty*/ }

    } // TODO: check

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
Thread::ID Thread::get_id() const noexcept {
    return _thread;             // calls private constructor of Thread::ID
}

Thread::ID this_thread::get_id() noexcept {
    return pthread_self();      // calls private constructor of Thread::ID
}

CUSTOM_END

#elif defined _MSC_VER
#error NO Thread implementation
#endif      // __GNUG__ and _MSC_VER