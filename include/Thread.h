#pragma once

#if defined __GNUG__
#include "Utility.h"
#include "Functional.h"
#include "Tuple.h"

#include <pthread.h>
#include <iostream>

#if defined _WIN32
#include <windows.h>    // for hardware_concurrency()
#elif defined __linux__
#include <sys/sysinfo.h>
#endif      // _WIN32 and __linux__

CUSTOM_BEGIN

class Thread        // Thread wrapper for pthread_t
{
public:
    class ID;
    using NativeHandleType  = pthread_t;

private:
    using Invoker       = void*(*)(void*);
    pthread_t _thread   = 0;

private:
    // Core functions

    template <class CallableTuple, size_t... Indices>
    static void* _invoke_impl(void* args) {
        CallableTuple* callable = static_cast<CallableTuple*>(args);
        std::invoke(custom::move(custom::get<Indices>(*callable))...);     // TODO: check std::invoke
        delete callable;

        return nullptr;
    }

    template <class CallableTuple, size_t... Indices>
    static constexpr Invoker _get_invoke_impl(IndexSequence<Indices...>) noexcept {
        return reinterpret_cast<Invoker>(_invoke_impl<CallableTuple, Indices...>);
    }

public:
    // Constructors

    Thread()                = default;
    Thread(const Thread&)   = delete;

    template<class Functor, class... Args, EnableIf_t<!IsSame<Decay_t<Functor>, Thread>::Value, bool> = true>
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

    static unsigned int hardware_concurrency() {
        #if defined _WIN32
        SYSTEM_INFO sysinfo;
        GetSystemInfo(&sysinfo);
        return sysinfo.dwNumberOfProcessors;
        #elif defined __linux__
        return get_nprocs();
        #endif
    }

    bool joinable() const {
        return _thread != 0;
    }

    void join() {
        if (!joinable())
            throw std::runtime_error("Thread not joinable...");

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

    ID get_id() const {
        return _thread;         // calls private constructor of Thread::ID
    }

    NativeHandleType native_handle() {
        return _thread;
    }

public:
    // ID definition
    
    class ID
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

        friend Thread::ID Thread::get_id() const;

    private:

        ID(const pthread_t& thr) : _threadID(thr) { /*Empty*/ }
    };

private:
    // Helpers

    void _move(Thread&& other) {
        _thread         = other._thread;
        other._thread   = 0;
    }
}; // END Thread

CUSTOM_END

#elif defined _MSC_VER
#error NO Thread implementation
#endif      // __GNUG__ and _MSC_VER