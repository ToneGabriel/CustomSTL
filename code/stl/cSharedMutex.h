#pragma once

#if defined __GNUG__
#include "xLock.h"


CUSTOM_BEGIN

class SharedMutex   // Adaptor for pthread_rwlock_t (read-write mutex)
{
public:
    using NativeHandleType = pthread_rwlock_t;

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

    void lock() {
        pthread_rwlock_wrlock(&_rwMutex);               // write
    }

    bool try_lock() {
        switch (pthread_rwlock_trywrlock(&_rwMutex))    // write
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

    void lock_shared() {
        pthread_rwlock_rdlock(&_rwMutex);               // read
    }

    bool try_lock_shared() {
        switch (pthread_rwlock_tryrdlock(&_rwMutex))    // read
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

    NativeHandleType native_handle() {
        return _rwMutex;
    }
};  // END SharedMutex


class SharedTimedMutex
{
    // TODO: implement
};  // END SharedTimedMutex

CUSTOM_END

#elif defined _MSC_VER
#error NO SharedMutex implementation
#endif