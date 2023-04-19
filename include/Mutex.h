#pragma once
#include "Utility.h"
#include "Thread.h"

// TODO: implement

CUSTOM_BEGIN

class MutexBase         // Mutex wrapper for pthread_mutex_t
{
private:
    pthread_mutex_t _mutex;
    pthread_mutexattr_t _mutexAttr;

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
                throw std::runtime_error("Mutex lock failed...");
        }
    }

    void unlock() {
        pthread_mutex_unlock(&_mutex);
    }
};


class Mutex : public MutexBase
{
public:
    // Constructors & Operators

    explicit Mutex() noexcept
        : MutexBase(PTHREAD_MUTEX_NORMAL) { /*Empty*/ }

    Mutex(const Mutex&)             = delete;
    Mutex& operator= (const Mutex&) = delete;
};


class RecursiveMutex : public MutexBase
{
public:
    // Constructors & Operators

    explicit RecursiveMutex() noexcept
        : MutexBase(PTHREAD_MUTEX_RECURSIVE) { /*Empty*/ }

    RecursiveMutex(const RecursiveMutex&)             = delete;
    RecursiveMutex& operator= (const RecursiveMutex&) = delete;
};


template<class Mtx>
class UniqueLock
{
public:
    using MutexType = Mtx;

private:
    MutexType* _ownedMutex  = nullptr;
    bool _owns              = false;

public:
    // Constructors

    UniqueLock()                    = default;
    UniqueLock(const UniqueLock&)   = delete;

    UniqueLock(MutexType& mtx) : _ownedMutex(&mtx), _owns(false) {
        _ownedMutex->lock();
        _owns = true;
    }

    UniqueLock(UniqueLock&& other) noexcept : _ownedMutex(other._ownedMutex), _owns(other._owns) {
        other._ownedMutex   = nullptr;
        other._owns         = false;
    }

    ~UniqueLock() {
        if (_owns)
            _ownedMutex->unlock();
    }

public:
    // Operators

    UniqueLock& operator=(const UniqueLock&) = delete;

    UniqueLock& operator=(UniqueLock&& other) noexcept {
        if (_ownedMutex != other._ownedMutex)
        {
            if (_owns)
                _ownedMutex->unlock();

            _ownedMutex         = other._ownedMutex;
            _owns               = other._owns;
            other._ownedMutex   = nullptr;
            other._owns         = false;
        }

        return *this;
    }

    explicit operator bool() const noexcept {
        return _owns;
    }

public:
    // Main functions

    void lock() {
        _validate();
        _ownedMutex->lock();
        _owns = true;
    }

    bool try_lock() {
        _validate();
        _owns = _ownedMutex->try_lock();
        return _owns;
    }

    bool try_lock_for() {
        return false;
        // TODO: implement
    }

    bool try_lock_until() {
        return false;
        // TODO: implement
    }

    void unlock() {
        if (_ownedMutex == nullptr || _owns == false)
            throw std::logic_error("No mutex owned...");

        _ownedMutex->unlock();
        _owns = false;
    }

    MutexType* release() {
        MutexType* temp = _ownedMutex;
        _ownedMutex     = nullptr;
        _owns           = false;

        return temp;
    }

    MutexType* mutex() const {
        return _ownedMutex;
    }

    bool owns_lock() const {
        return _owns;
    }

private:
    // Helpers

    void _validate() const {        // check if mutex can be locked
        if (_ownedMutex == nullptr)
            throw std::logic_error("No mutex owned...");

        if (_owns)
            throw std::logic_error("Mutex already locked...");
    }
};


template<class Mtx>
class LockGuard
{
public:
    using MutexType = Mtx;

private:
    MutexType& _mutex;

public:
    // Constructors & Operators

    explicit LockGuard(MutexType& mtx) : _mutex(mtx) {
        _mutex.lock();
    }

    ~LockGuard() {
        _mutex.unlock();
    }
    
    LockGuard(const LockGuard&)             = delete;
    LockGuard& operator=(const LockGuard&)  = delete;
};


class ScopedLock
{

};

CUSTOM_END