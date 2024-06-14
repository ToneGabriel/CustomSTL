#pragma once

#if defined __GNUG__
#include "c_utility.h"
#include "c_tuple.h"
#include "cMemory.h"
#include "c_chrono.h"

#include <pthread.h>
#if defined _WIN32
#include <windows.h>    // for hardware_concurrency()
#elif defined __linux__
#include <sys/sysinfo.h>
#endif      // _WIN32 and __linux__


CUSTOM_BEGIN

class thread        // thread adaptor for pthread_t
{
public:
    class id;
    using native_handle_type  = pthread_t;

private:
    using Invoker           = void*(*)(void*);
    pthread_t _thread       = 0;

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
    static constexpr Invoker _get_invoke_impl(index_sequence<Indices...>) noexcept {
        return reinterpret_cast<Invoker>(_invoke_impl<CallableTuple, Indices...>);
    }

public:
    // Constructors

    thread()                = default;
    thread(const thread&)   = delete;

    template<class Functor, class... Args, 
    enable_if_t<!is_same_v<decay_t<Functor>, thread>, bool> = true>
    explicit thread(Functor&& func, Args&&... args) {
        // forward functor and arguments as tuple pointer to match "pthread_create" procedure
        using CallableTuple = tuple<decay_t<Functor>, decay_t<Args>...>;

        Invoker invoker = _get_invoke_impl<CallableTuple>(make_index_sequence<1 + sizeof...(Args)>{});
        auto callable   = custom::make_unique<CallableTuple>(custom::forward<Functor>(func), custom::forward<Args>(args)...);

        if (pthread_create(&_thread, nullptr, invoker, callable.get()) == 0)
            (void)callable.release();     // ownership transferred to the thread
        else
        {
            _thread = 0;
            throw std::runtime_error("thread creation failed.");
        }
    }

    thread(thread&& other) noexcept {
        _move(custom::move(other));
    }

    ~thread() {
        if (joinable())
            std::terminate();
    }

public:
    // Operators

    thread& operator=(const thread&) = delete;

    thread& operator=(thread&& other) noexcept {
        if (*this != other)
        {
            if (joinable())
                std::terminate();

            _move(custom::move(other));
        }

        return *this;
    }

    bool operator==(const thread& other) const {
        return (pthread_equal(_thread, other._thread) != 0);
    }

    bool operator!=(const thread& other) const {
        return !(*this == other);
    }

public:
    // Main functions

    bool joinable() const noexcept {
        return _thread != 0;
    }

    void join() {
        if (!joinable())
            throw std::runtime_error("thread not joinable.");

        if (_thread == pthread_self())
            throw std::runtime_error("Resource deadlock would occur.");

        if (pthread_join(_thread, nullptr) != 0)
            throw std::runtime_error("thread join failed.");

        _thread = 0;
    }

    void detach() {
        if (!joinable())
            throw std::runtime_error("thread not joinable.");

        if (pthread_detach(_thread) != 0)
            throw std::runtime_error("thread detach failed.");

        _thread = 0;
    }

    id get_id() const noexcept;

    native_handle_type native_handle() noexcept {
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

    void _move(thread&& other) {
        _thread = custom::exchange(other._thread, 0);
    }
}; // END thread


namespace this_thread
{
    thread::id get_id() noexcept;

    inline void yield() noexcept {
        sched_yield();
    }

    template<class Rep, class Period>
    void sleep_for(const custom::chrono::duration<Rep, Period>& relativeTime) {
        if (relativeTime <= relativeTime.zero())
	        return;

        // if relativeTime duration cast to seconds is 0, then nanoseconds duration will be representative
        // else if relativeTime duration cast to seconds is > 0, then nanoseconds duration will be 0.
        auto seconds        = custom::chrono::duration_cast<custom::chrono::seconds>(relativeTime);
	    auto nanoseconds    = custom::chrono::duration_cast<custom::chrono::nanoseconds>(relativeTime - seconds);
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
    void sleep_until(const custom::chrono::time_point<Clock, Duration>& absoluteTime) {
        auto now = Clock::now();

        while (now < absoluteTime)
        {
            sleep_for(absoluteTime - now);
            now = Clock::now();
        }
    }

} // END namespace this_thread


class thread::id
{
private:
    pthread_t _threadID;

public:

    id() : _threadID(0) { /*Empty*/ }

    friend thread::id thread::get_id() const noexcept;
    friend thread::id this_thread::get_id() noexcept;
    friend std::ostream& operator<<(std::ostream& os, const thread::id& id);
    friend bool operator==(thread::id left, thread::id right) noexcept;

private:

    id(const pthread_t& thr) : _threadID(thr) { /*Empty*/ }
}; // END thread::id


// other definitions
inline thread::id thread::get_id() const noexcept {
    return _thread;             // calls private constructor of thread::id
}

inline thread::id this_thread::get_id() noexcept {
    return pthread_self();      // calls private constructor of thread::id
}

inline bool operator==(thread::id left, thread::id right) noexcept {
    return (pthread_equal(left._threadID, right._threadID) != 0);
}

inline bool operator!=(thread::id left, thread::id right) noexcept {
    return !(left == right);
}

inline std::ostream& operator<<(std::ostream& os, const thread::id& id) {
    os << id._threadID;
    return os;
}

CUSTOM_END

#elif defined _MSC_VER
#error NO Thread implementation
#endif      // __GNUG__ and _MSC_VER