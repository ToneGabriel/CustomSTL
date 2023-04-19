#pragma once
#include "Utility.h"
#include "Thread.h"

// TODO: implement

CUSTOM_BEGIN

class Mutex         // Mutex wrapper for pthread_mutex_t
{
private:
    pthread_mutex_t _mutex;

public:
    // Constructors

    Mutex() = default;
    ~Mutex() = default;

public:
    // Main functions

    void lock();

    bool try_lock();

    void unlock();
};


template<class MutexType>
class UniqueLock
{
public:
    // Constructors

    UniqueLock() = default;
    ~UniqueLock() = default;

public:
    // Operators

    explicit operator bool() const noexcept;

public:
    // Main functions

    void lock();

    bool try_lock();

    bool try_lock_for();

    bool try_lock_until();

    void unlock();

    void release();

    MutexType* mutex() const;

    bool owns_lock() const;
};


template<class MutexType>
class LockGuard
{
private:
    MutexType& _mutex;

public:
    // Constructors

    explicit LockGuard(MutexType& mtx) : _mutex(mtx) {
        _mutex.lock();
    }

    LockGuard(const LockGuard&) = delete;

    ~LockGuard() {
        _mutex.unlock();
    }

public:
    // Operators
    
    LockGuard& operator=(const LockGuard&) = delete;
};


class ScopedLock
{

};

CUSTOM_END