#pragma once

#if defined __GNUG__
#include "c_thread.h"


CUSTOM_BEGIN

class Mutex;
class RecursiveMutex;
class TimedMutex;
class RecursiveTimedMutex;

class SharedMutex;
class SharedTimedMutex;

class ConditionVariable;
class ConditionVariableAny;


CUSTOM_DETAIL_BEGIN     // lock impl

template<size_t... Indices, class... Locks>
void _lock_one_from_locks(const int target, index_sequence<Indices...>, Locks&... locks) { // lock locks[target]
    // ((static_cast<int>(Indices) == target ? (void)locks.lock() : void()), ...);
    
    // Without the array initializer, the fold expression could potentially be optimized away
    // by the compiler if it determines that the result of the expression isn't needed.
    // This optimization might lead to unexpected behavior because
    // the actions inside the fold expression wouldn't be performed.

    // By introducing the array initializer, you force the compiler to evaluate
    // the fold expression for each element in the pack and ensure that the actions are executed,
    // regardless of whether their results are actually used.
    // This guarantees that the critical operations performed are executed as intended.

    int ignored[] = {((static_cast<int>(Indices) == target ? (void)locks.lock() : void()), 0)...};
    (void)ignored; // suppress warning for unused variable
}

template<size_t... Indices, class... Locks>
bool _try_lock_one_from_locks(const int target, index_sequence<Indices...>, Locks&... locks) { // try to lock locks[target]
    bool result;
    int ignored[] = {((static_cast<int>(Indices) == target ? (void)(result = locks.try_lock()) : void()), 0)...};
    (void)ignored;
    return result;
}

template<size_t... Indices, class... Locks>
void _unlock_locks_range(const int first, const int last, index_sequence<Indices...>, Locks&... locks) noexcept {
    // unlock locks in locks[first, last)
    int ignored[] = {((first <= static_cast<int>(Indices) && static_cast<int>(Indices) < last ? (void)locks.unlock() : void()), 0)...};
    (void)ignored;
}

template<class... Locks>
int _try_lock_range(const int first, const int last, Locks&... locks) {
    using Indices = index_sequence_for<Locks...>;

    int next = first;   // initialized here because of catch
    try
    {
        for (/*Empty*/; next != last; ++next)
            if (!_try_lock_one_from_locks(next, Indices{}, locks...))   // try_lock failed, backout
            {
                _unlock_locks_range(first, next, Indices{}, locks...);
                return next;
            }
    }
    catch (...)
    {
        _unlock_locks_range(first, next, Indices{}, locks...);
        CUSTOM_RERAISE;      // terminates
    }

    return -1;
}

CUSTOM_DETAIL_END   // lock impl


template<class... Locks>
void lock(Locks&... locks) {    // lock multiple locks, without deadlock
    using Indices = index_sequence_for<Locks...>;

    // hardLockIndex stores the position of the last failed lock (-1 is ok)
    // attempt lock -> applies try_lock for [0, hardLockIndex)
    // then lock ONLY for hardLockIndex
    // then again try_lock for rest (hardLockIndex, rest...]

    const int lockCount = sizeof...(Locks);
    int hardLockIndex   = 0;
    int failedLockIndex;
    int backoutStart;

    while (hardLockIndex != -1)  // attempt lock
    {
        // lock the one that failed in previous iteration (or the first if none exists)
        detail::_lock_one_from_locks(hardLockIndex, Indices{}, locks...);

        backoutStart    = hardLockIndex;    // unlock just hardLockIndex if fail (at the end)
        failedLockIndex = -1;

        try
        {
            // try to lock from 0 to the one that failed
            failedLockIndex = detail::_try_lock_range(0, hardLockIndex, locks...);

            if (failedLockIndex == -1)  // ok so far
            {
                backoutStart    = 0;   // unlock [0, hardLockIndex] if the next throws
                failedLockIndex = detail::_try_lock_range(hardLockIndex + 1, lockCount, locks...);  // try to lock rest...
                
                if (failedLockIndex == -1)  // we got all the locks
                    return;
            }
        }
        catch (...)
        {
            detail::_unlock_locks_range(backoutStart, hardLockIndex + 1, Indices{}, locks...);
            CUSTOM_RERAISE;  // terminates
        }

        // didn't get all the locks, backout
        detail::_unlock_locks_range(backoutStart, hardLockIndex + 1, Indices{}, locks...);
        custom::this_thread::yield();
        hardLockIndex = failedLockIndex;
    }
}

template<class... Locks>
int try_lock(Locks&... locks) {
    // try to lock multiple locks
    // -1 on success, or ​0​-based index value of the object that failed to lock.
    return detail::_try_lock_range(0, sizeof...(Locks), locks...);
}


// tag struct declarations for construction of UniqueLock/SharedLock objects
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
    UniqueLock(MutexType& mtx, const custom::chrono::duration<Rep, Period>& relativeTime)          // construct and lock with timeout
        : _ownedMutex(&mtx), _owns(_ownedMutex->try_lock_for(relativeTime)) { /*Empty*/ }

    template<class Clock, class duration>
    UniqueLock(MutexType& mtx, const custom::chrono::time_point<Clock, duration>& absoluteTime)    // construct and lock with timeout
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
    bool try_lock_for(const custom::chrono::duration<Rep, Period>& relativeTime) {
        _validate();
        _owns = _ownedMutex->try_lock_for(relativeTime);
        return _owns;
    }

    template<class Clock, class duration>
    bool try_lock_until(const custom::chrono::time_point<Clock, duration>& absoluteTime) {
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


// is shared mutex
template<class Ty>
constexpr bool IsSharedMutex_v = is_any_of_v<RemoveCV_t<Ty>,  SharedMutex,
                                                            SharedTimedMutex>;

template<class Ty>
struct IsSharedMutex : bool_constant<IsSharedMutex_v<Ty>> {};


template<class Mtx>
class SharedLock
{
public:
    static_assert(IsSharedMutex_v<Mtx>, "SharedLock requires shared mutex type!");
    using MutexType = Mtx;

private:
    MutexType* _ownedSMutex = nullptr;
    bool _owns              = false;

public:
    // Constructors

    SharedLock()                    = default;
    SharedLock(const SharedLock&)   = delete;

    SharedLock(MutexType& mtx) : _ownedSMutex(&mtx), _owns(false) {                              // construct and lock
        _ownedSMutex->lock_shared();
        _owns = true;
    }

    SharedLock(MutexType& mtx, AdoptLock_t)                                                     // construct and assume already locked
        : _ownedSMutex(&mtx), _owns(true) { /*Empty*/ }

    SharedLock(MutexType& mtx, DeferLock_t) noexcept                                            // construct but don't lock
        : _ownedSMutex(&mtx), _owns(false) { /*Empty*/ }

    SharedLock(MutexType& mtx, TryToLock_t)                                                     // construct and try to lock
        : _ownedSMutex(&mtx), _owns(_ownedSMutex->try_lock_shared()) { /*Empty*/ }

    template<class Rep, class Period>
    SharedLock(MutexType& mtx, const custom::chrono::duration<Rep, Period>& relativeTime)          // construct and lock with timeout
        : _ownedSMutex(&mtx), _owns(_ownedSMutex->try_lock_shared_for(relativeTime)) { /*Empty*/ }

    template<class Clock, class duration>
    SharedLock(MutexType& mtx, const custom::chrono::time_point<Clock, duration>& absoluteTime)    // construct and lock with timeout
        : _ownedSMutex(&mtx), _owns(_ownedSMutex->try_lock_shared_until(absoluteTime)) { /*Empty*/ }

    SharedLock(SharedLock&& other) noexcept : _ownedSMutex(other._ownedSMutex), _owns(other._owns) {
        other._ownedSMutex  = nullptr;
        other._owns         = false;
    }

    ~SharedLock() {
        if (_owns)
            _ownedSMutex->unlock_shared();
    }

public:
    // Operators

    SharedLock& operator=(const SharedLock&) = delete;

    SharedLock& operator=(SharedLock&& other) {
        if (_ownedSMutex != other._ownedSMutex)
        {
            if (_owns)
                _ownedSMutex->unlock_shared();  // can throw

            _ownedSMutex    = custom::exchange(other._ownedSMutex, nullptr);
            _owns           = custom::exchange(other._owns, false);
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
        _ownedSMutex->lock_shared();
        _owns = true;
    }

    bool try_lock() {
        _validate();
        _owns = _ownedSMutex->try_lock_shared();
        return _owns;
    }

    template<class Rep, class Period>
    bool try_lock_for(const custom::chrono::duration<Rep, Period>& relativeTime) {
        _validate();
        _owns = _ownedSMutex->try_lock_shared_for(relativeTime);
        return _owns;
    }

    template<class Clock, class duration>
    bool try_lock_until(const custom::chrono::time_point<Clock, duration>& absoluteTime) {
        _validate();
        _owns = _ownedSMutex->try_lock_shared_until(absoluteTime);
        return _owns;
    }

    void unlock() {
        if (_ownedSMutex == nullptr || _owns == false)
            throw std::logic_error("No mutex owned.");

        _ownedSMutex->unlock_shared();
        _owns = false;
    }

    MutexType* release() {
        MutexType* temp = _ownedSMutex;
        _ownedSMutex    = nullptr;
        _owns           = false;

        return temp;
    }

    MutexType* mutex() const noexcept{
        return _ownedSMutex;
    }

    bool owns_lock() const {
        return _owns;
    }

private:
    // Helpers

    void _validate() const {        // check if mutex can be locked
        if (_ownedSMutex == nullptr)
            throw std::logic_error("No mutex owned.");

        if (_owns)
            throw std::logic_error("Mutex already locked.");
    }
};  // END SharedLock


template<class... Mutexes>
class ScopedLock            // locks/unlocks multiple mutexes
{
private:
    tuple<Mutexes&...> _ownedMutexes;

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