#pragma once

#if defined __GNUG__
#include "cThread.h"

CUSTOM_BEGIN

CUSTOM_DETAIL_BEGIN     // lock impl

template<size_t... Indices, class... Locks>
void _lock_from_locks(const int target, IndexSequence<Indices...>, Locks&... locks) { // lock locks[target]
    // use this to mimic a loop aver Indices
    int ignored[] = {((static_cast<int>(Indices) == target ? (void)locks.lock() : void()), 0)...};
    (void)ignored; // suppress warning on unused variable
}

template<size_t... Indices, class... Locks>
bool _try_lock_from_locks(const int target, IndexSequence<Indices...>, Locks&... locks) { // try to lock locks[target]
    bool result;
    int ignored[] = {((static_cast<int>(Indices) == target ? (void)(result = locks.try_lock()) : void()), 0)...};
    (void)ignored;
    return result;
}

template<size_t... Indices, class... Locks>
void _unlock_locks(const int first, const int last, IndexSequence<Indices...>, Locks&... locks) noexcept {
    // terminates
    // unlock locks in locks[first, last)
    int ignored[] = {((first <= static_cast<int>(Indices) && static_cast<int>(Indices) < last ? (void)locks.unlock() : void()), 0)...};
    (void)ignored;
}

template<class... Locks>
int _try_lock_range(const int first, const int last, Locks&... locks) {
    using Indices = IndexSequenceFor<Locks...>;

    int next = first;   // initialized here because of catch
    try
    {
        for (; next != last; ++next)
            if (!_try_lock_from_locks(next, Indices{}, locks...))   // try_lock failed, backout
            {
                _unlock_locks(first, next, Indices{}, locks...);
                return next;
            }
    }
    catch (...)
    {
        _unlock_locks(first, next, Indices{}, locks...);
        throw;      // terminates
    }

    return -1;
}

template<class... Locks>
int _lock_attempt(const int hardLock, Locks&... locks) {
    // attempt to lock locks, starting by locking locks[hardLock] and trying to lock the rest

    using Indices = IndexSequenceFor<Locks...>;

    _lock_from_locks(hardLock, Indices{}, locks...);
    int failed          = -1;
    int backoutStart    = hardLock; // that is, unlock hardLock

    try
    {
        failed = _try_lock_range(0, hardLock, locks...);
        if (failed == -1)
        {
            backoutStart    = 0;    // that is, unlock [0, hardLock] if the next throws
            failed          = _try_lock_range(hardLock + 1, sizeof...(Locks), locks...);
            
            if (failed == -1)       // we got all the locks
                return -1;
        }
    }
    catch (...)
    {
        _unlock_locks(backoutStart, hardLock + 1, Indices{}, locks...);
        throw;  // terminates
    }

    // didn't get all the locks, backout
    _unlock_locks(backoutStart, hardLock + 1, Indices{}, locks...);
    custom::this_thread::yield();
    return failed;
}

CUSTOM_DETAIL_END   // lock impl


template<class... Locks>
void lock(Locks&... locks) {    // lock multiple locks, without deadlock
    int hardLock = 0;
    while (hardLock != -1)
        hardLock = detail::_lock_attempt(hardLock, locks...);
}

template<class... Locks>
int try_lock(Locks&... locks) { // try to lock multiple locks
    return detail::_try_lock_range(0, sizeof...(Locks), locks...);
}


// tag struct declarations for construction of UniqueLock objects
struct AdoptLock_t { explicit AdoptLock_t() = default; };
struct DeferLock_t { explicit DeferLock_t() = default; };
struct TryToLock_t { explicit TryToLock_t() = default; };

constexpr AdoptLock_t AdoptLock = AdoptLock_t();
constexpr DeferLock_t DeferLock = DeferLock_t();
constexpr TryToLock_t TryToLock = TryToLock_t();


template<class Mtx>
class LockGuard         // automatic unlock
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

    LockGuard(MutexType& mtx, AdoptLock_t) : _mutex(mtx) { /*Empty*/ } // construct but don't lock

    ~LockGuard() {
        _mutex.unlock();
    }
    
    LockGuard(const LockGuard&)             = delete;
    LockGuard& operator=(const LockGuard&)  = delete;
}; // END LockGuard


template<class Mtx>
struct UnlockGuard      // automatic lock
{
public:
    using MutexType = Mtx;

private:
    MutexType& _mutex;

public:
    // Constructors & Operators

    explicit UnlockGuard(MutexType& mtx) : _mutex(mtx) {
        _mutex.unlock();
    }

    ~UnlockGuard() {
        // relock mutex or terminate()
        // ConditionVariableAny wait functions are required to terminate if
        // the mutex cannot be relocked;
        _mutex.lock();
    }

    UnlockGuard(const UnlockGuard&)             = delete;
    UnlockGuard& operator=(const UnlockGuard&)  = delete;
}; // END UnlockGuard


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

    UniqueLock(MutexType& mtx) : _ownedMutex(&mtx), _owns(false) {                              // construct and lock
        _ownedMutex->lock();
        _owns = true;
    }

    UniqueLock(MutexType& mtx, AdoptLock_t)                                                     // construct and assume already locked
        : _ownedMutex(&mtx), _owns(true) { /*Empty*/ }

    UniqueLock(MutexType& mtx, DeferLock_t) noexcept                                            // construct but don't lock
        : _ownedMutex(&mtx), _owns(false) { /*Empty*/ }

    UniqueLock(MutexType& mtx, TryToLock_t)                                                     // construct and try to lock
        : _ownedMutex(&mtx), _owns(_ownedMutex->try_lock()) { /*Empty*/ }

    template<class Rep, class Period>
    UniqueLock(MutexType& mtx, const custom::chrono::Duration<Rep, Period>& relativeTime)          // construct and lock with timeout
        : _ownedMutex(&mtx), _owns(_ownedMutex->try_lock_for(relativeTime)) { /*Empty*/ }

    template<class Clock, class Duration>
    UniqueLock(MutexType& mtx, const custom::chrono::TimePoint<Clock, Duration>& absoluteTime)    // construct and lock with timeout
        : _ownedMutex(&mtx), _owns(_ownedMutex->try_lock_until(absoluteTime)) { /*Empty*/ }

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

    UniqueLock& operator=(UniqueLock&& other) {
        if (_ownedMutex != other._ownedMutex)
        {
            if (_owns)
                _ownedMutex->unlock();  // can throw

            _ownedMutex = custom::exchange(other._ownedMutex, nullptr);
            _owns       = custom::exchange(other._owns, false);
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

    template<class Rep, class Period>
    bool try_lock_for(const custom::chrono::Duration<Rep, Period>& relativeTime) {
        _validate();
        _owns = _ownedMutex->try_lock_for(relativeTime);
        return _owns;
    }

    template<class Clock, class Duration>
    bool try_lock_until(const custom::chrono::TimePoint<Clock, Duration>& absoluteTime) {
        _validate();
        _owns = _ownedMutex->try_lock_until(absoluteTime);
        return _owns;
    }

    void unlock() {
        if (_ownedMutex == nullptr || _owns == false)
            throw std::logic_error("No mutex owned.");

        _ownedMutex->unlock();
        _owns = false;
    }

    MutexType* release() {
        MutexType* temp = _ownedMutex;
        _ownedMutex     = nullptr;
        _owns           = false;

        return temp;
    }

    MutexType* mutex() const noexcept{
        return _ownedMutex;
    }

    bool owns_lock() const {
        return _owns;
    }

private:
    // Helpers

    void _validate() const {        // check if mutex can be locked
        if (_ownedMutex == nullptr)
            throw std::logic_error("No mutex owned.");

        if (_owns)
            throw std::logic_error("Mutex already locked.");
    }
}; // END UniqueLock


template<class... Mutexes>
class ScopedLock            // locks/unlocks multiple mutexes
{
private:
    Tuple<Mutexes&...> _ownedMutexes;

public:
    // Constructors & Operators

    explicit ScopedLock(Mutexes&... mtxes): _ownedMutexes(mtxes...) { // construct and lock
        custom::lock(mtxes...);
    }

    explicit ScopedLock(AdoptLock_t, Mutexes&... mtxes)
        : _ownedMutexes(mtxes...) { /*Empty*/ } // construct but don't lock

    ~ScopedLock() {
        auto reverseUnlock = [](Mutexes&... mtxes) {
            // (args, ...) left folding   = first -> last
            // (..., args) right folding  = last -> first

            (..., (void)mtxes.unlock());
        };

        custom::apply(reverseUnlock, _ownedMutexes);
    }

    ScopedLock(const ScopedLock&)            = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;
}; // END ScopedLock multiple


template<class Mtx>
class ScopedLock<Mtx>
{
public:
    using MutexType = Mtx;

private:
    MutexType& _ownedMutex;
    
public:
    // Constructors & Operators

    explicit ScopedLock(MutexType& mtx) : _ownedMutex(mtx) { // construct and lock
        _ownedMutex.lock();
    }

    explicit ScopedLock(AdoptLock_t, MutexType& mtx)
        : _ownedMutex(mtx) { /*Empty*/ } // construct but don't lock

    ~ScopedLock() {
        _ownedMutex.unlock();
    }

    ScopedLock(const ScopedLock&)            = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;
}; // END ScopedLock one

template<>
class ScopedLock<>
{
public:
    // Constructors & Operators

    explicit ScopedLock() { /*Empty*/ }
    explicit ScopedLock(AdoptLock_t) { /*Empty*/ }
    ~ScopedLock() { /*Empty*/ }

    ScopedLock(const ScopedLock&)            = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;
}; // END ScopedLock empty


CUSTOM_END

#elif defined _MSC_VER
#error NO Locks implementation
#endif