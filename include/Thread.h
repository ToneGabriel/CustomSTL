#pragma once
#include "Utility.h"
#include "Tuple.h"

#include <pthread.h>


CUSTOM_BEGIN

class Thread        // Thread wrapper for pthread_t
{
private:
    using Invoker   = void*(*)(void*);
    using ThreadID  = unsigned long long;

    pthread_t _thread;
    ThreadID _id        = 0;
    Invoker _invoker    = nullptr;

private:
    template <class CallableTuple, size_t... _Indices>
    static void* _invoke_impl(void* args) {
        CallableTuple* callable = static_cast<CallableTuple*>(args);
        std::invoke(custom::move(custom::get<_Indices>(*callable))...);     // TODO: check std::invoke
        delete callable;

        return nullptr;
    }

    template <class CallableTuple, size_t... _Indices>
    static constexpr Invoker _get_invoke_impl(IndexSequence<_Indices...>) noexcept {
        return _invoke_impl<CallableTuple, _Indices...>;
    }

public:
    // Constructors

    Thread()                = default;
    Thread(const Thread&)   = delete;

    template<class Functor, class... Args, custom::EnableIf_t<!custom::IsSame<custom::Decay_t<Functor>, Thread>::Value, bool> = true>
    Thread(Functor&& func, Args&&... args) {
        using CallableTuple = custom::Tuple<custom::Decay_t<Functor>, custom::Decay_t<Args>...>;

        // forward functor and arguments as tuple pointer to match "pthread_create" procedure
        CallableTuple* callable = new CallableTuple(custom::forward<Functor>(func), custom::forward<Args>(args)...);
        _invoker = _get_invoke_impl<CallableTuple>(MakeIndexSequence<1 + sizeof...(Args)>{});

        if (pthread_create(&_thread, nullptr, _invoker, callable) == 0)
            _id = static_cast<ThreadID>(_thread);
        else
        {
            _id = 0;
            delete callable;
            throw std::runtime_error("Thread creation failed...");
        }
    }

    Thread(Thread&& other) noexcept {
        // TODO: implement
    }

    ~Thread() {
        if (joinable())
            std::terminate();
    }

public:
    // Operators

    Thread& operator=(Thread&& other) noexcept {
        // TODO: implement
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
        return pthread_num_processors_np();
        // SYSTEM_INFO sysinfo;
        // GetSystemInfo(&sysinfo);
        // return sysinfo.dwNumberOfProcessors;
    }

    bool joinable() const {
        return _id != 0;
    }

    void join() {
        if (!joinable())
            throw std::runtime_error("Thread not joinable...");

        if (pthread_join(_thread, nullptr) != 0)
            throw std::runtime_error("Thread join failed...");

        _id         = 0;
        _invoker    = nullptr;
    }

    void detach() {
        if (!joinable())
            throw std::runtime_error("Thread not joinable...");

        if (pthread_detach(_thread) != 0)
            throw std::runtime_error("Thread detach failed...");

        _id         = 0;
        _invoker    = nullptr;
    }

    ThreadID get_id() const {
        return _id;
    }
}; // END Thread

CUSTOM_END