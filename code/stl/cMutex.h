#pragma once

#if defined __GNUG__
#include "xLock.h"


CUSTOM_BEGIN

class MutexBase         // Mutex adaptor for pthread_mutex_t
{
public:
    using NativeHandleType = pthread_mutex_t;

private:
    friend ConditionVariable;
    friend ConditionVariableAny;

    pthread_mutex_t _mutex;
    pthread_mutexattr_t _mutexAttr;     // PTHREAD_MUTEX_NORMAL or PTHREAD_MUTEX_RECURSIVE

public:
    // Constructors & Operators

    explicit MutexBase(int attributeFlag) noexcept {
        pthread_mutexattr_init(&_mutexAttr);
        pthread_mutexattr_settype(&_mutexAttr, attributeFlag);
        pthread_mutex_init(&_mutex, &_mutexAttr);
    }

    virtual ~MutexBase() {
        pthread_mutex_destroy(&_mutex);
        pthread_mutexattr_destroy(&_mutexAttr);
    }

    MutexBase(const MutexBase&)             = delete;
    MutexBase& operator= (const MutexBase&) = delete;

public:
    // Main functions

    void lock() {
        pthread_mutex_lock(&_mutex);
    }

    bool try_lock() {
        switch (pthread_mutex_trylock(&_mutex))
        {
            case 0:
                return true;
            case EBUSY:
                return false;
            default:
                throw std::runtime_error("Mutex lock failed.");
        }
    }

    void unlock() {
        pthread_mutex_unlock(&_mutex);
    }

    NativeHandleType native_handle() {
        return _mutex;
    }
}; // END MutexBase


class Mutex : public MutexBase
{
public:
    // Constructors & Operators

    explicit Mutex() noexcept
        : MutexBase(PTHREAD_MUTEX_NORMAL) { /*Empty*/ }

    Mutex(const Mutex&)             = delete;
    Mutex& operator= (const Mutex&) = delete;
}; // END Mutex


class RecursiveMutex : public MutexBase
{
public:
    // Constructors & Operators

    explicit RecursiveMutex() noexcept
        : MutexBase(PTHREAD_MUTEX_RECURSIVE) { /*Empty*/ }

    RecursiveMutex(const RecursiveMutex&)             = delete;
    RecursiveMutex& operator= (const RecursiveMutex&) = delete;
}; // RecursiveMutex

CUSTOM_END

#elif defined _MSC_VER
#error NO Mutex implementation
#endif