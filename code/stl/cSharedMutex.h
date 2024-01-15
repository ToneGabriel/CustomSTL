#pragma once

#if defined __GNUG__
#include "cThread.h"
#include "xLock.h"

// TODO: try implement using pthread_rwlock_t

CUSTOM_BEGIN


class SharedMutex
{
public:
    using NativeHandleType = pthread_rwlock_t;

private:
    friend class SharedTimedMutex;

    pthread_rwlock_t _rwLock = PTHREAD_RWLOCK_INITIALIZER;

public:
    // Constructors & Operators

public:
    // Main functions

// Exclusive ownership
    void lock();
    bool try_lock();
    void unlock();

// Shared ownership
    void lock_shared();
    bool try_lock_shared();
    void unlock_shared();

};  // END SharedMutex


class SharedTimedMutex
{

};  // END SharedTimedMutex


template<class Mtx>
class SharedLock        // try implement in xLock.h (add static_assert for type of mutex)
{

};  // END SharedLock

CUSTOM_END

#elif defined _MSC_VER
#error NO SharedMutex implementation
#endif