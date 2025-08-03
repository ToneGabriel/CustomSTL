#pragma once

#if defined __GNUG__
#include "custom/_lock.h"


CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN

class _Mutex_Base         // mutex adaptor for pthread_mutex_t
{
public:
    using native_handle_type = pthread_mutex_t;

private:
    friend condition_variable;
    friend condition_variable_any;

    pthread_mutex_t _mutex;
    pthread_mutexattr_t _mutexAttr;     // PTHREAD_MUTEX_NORMAL or PTHREAD_MUTEX_RECURSIVE

public:
    // Constructors & Operators

    explicit _Mutex_Base(int attributeFlag) noexcept
    {
        pthread_mutexattr_init(&_mutexAttr);
        pthread_mutexattr_settype(&_mutexAttr, attributeFlag);
        pthread_mutex_init(&_mutex, &_mutexAttr);
    }

    virtual ~_Mutex_Base()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_mutexattr_destroy(&_mutexAttr);
    }

    _Mutex_Base(const _Mutex_Base&)             = delete;
    _Mutex_Base& operator= (const _Mutex_Base&) = delete;

public:
    // Main functions

    void lock()
    {
        pthread_mutex_lock(&_mutex);
    }

    bool try_lock()
    {
        switch (pthread_mutex_trylock(&_mutex))
        {
            case 0:
                return true;
            case EBUSY:
                return false;
            default:
                throw std::runtime_error("mutex lock failed.");
        }
    }

    void unlock()
    {
        pthread_mutex_unlock(&_mutex);
    }

    native_handle_type native_handle()
    {
        return _mutex;
    }
}; // END _Mutex_Base

CUSTOM_DETAIL_END

class mutex : public detail::_Mutex_Base
{
public:
    // Constructors & Operators

    explicit mutex() noexcept
        : detail::_Mutex_Base(PTHREAD_MUTEX_NORMAL) { /*Empty*/ }

    mutex(const mutex&)             = delete;
    mutex& operator= (const mutex&) = delete;
}; // END mutex


class recursive_mutex : public detail::_Mutex_Base
{
public:
    // Constructors & Operators

    explicit recursive_mutex() noexcept
        : detail::_Mutex_Base(PTHREAD_MUTEX_RECURSIVE) { /*Empty*/ }

    recursive_mutex(const recursive_mutex&)             = delete;
    recursive_mutex& operator= (const recursive_mutex&) = delete;
}; // recursive_mutex

CUSTOM_END

#elif defined _MSC_VER
#error NO Mutex implementation
#endif