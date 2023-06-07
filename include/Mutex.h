#pragma once

#if defined __GNUG__
#include "Utility.h"
#include "Thread.h"

#include <chrono>


CUSTOM_BEGIN

class ConditionVariable;

class MutexBase         // Mutex adaptor for pthread_mutex_t
{
public:
    using NativeHandleType = pthread_mutex_t;

private:
    friend ConditionVariable;

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
                throw std::runtime_error("Mutex lock failed...");
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


// tag struct declarations for construction of UniqueLock objects
struct AdoptLock_t { explicit AdoptLock_t() = default; };
struct DeferLock_t { explicit DeferLock_t() = default; };
struct TryToLock_t { explicit TryToLock_t() = default; };

constexpr AdoptLock_t AdoptLock = AdoptLock_t();
constexpr DeferLock_t DeferLock = DeferLock_t();
constexpr TryToLock_t TryToLock = TryToLock_t();


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
    UniqueLock(MutexType& mtx, const std::chrono::duration<Rep, Period>& relativeTime)          // construct and lock with timeout
        : _ownedMutex(&mtx), _owns(_ownedMutex->try_lock_for(relativeTime)) { /*Empty*/ }

    template<class Clock, class Duration>
    UniqueLock(MutexType& mtx, const std::chrono::time_point<Clock, Duration>& absoluteTime)    // construct and lock with timeout
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

    template<class Rep, class Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& relativeTime) {
        _validate();
        _owns = _ownedMutex->try_lock_for(relativeTime);
        return _owns;
    }

    template<class Clock, class Duration>
    bool try_lock_until(const std::chrono::time_point<Clock, Duration>& absoluteTime) {
        _validate();
        _owns = _ownedMutex->try_lock_until(absoluteTime);
        return _owns;
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
}; // END UniqueLock


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
}; // END LockGuard


// lock
template<size_t... Indices, class... Locks>
void _lock_from_locks(const int target, IndexSequence<Indices...>, Locks&... locks) { // lock locks[target]
    int ignored[] = {((static_cast<int>(Indices) == target ? (void) locks.lock() : void()), 0)...};
    (void) ignored;
}

template<size_t... Indices, class... Locks>
bool _try_lock_from_locks(const int target, IndexSequence<Indices...>, Locks&... locks) { // try to lock locks[target]
    bool result;
    int ignored[] = {((static_cast<int>(Indices) == target ? (void) (result = locks.try_lock()) : void()), 0)...};
    (void) ignored;
    return result;
}

template<size_t... Indices, class... Locks>
void _unlock_locks(const int first, const int last, IndexSequence<Indices...>, Locks&... locks) noexcept {
    // terminates
    // unlock locks in locks[first, last)
    int ignored[] = {((first <= static_cast<int>(Indices) && static_cast<int>(Indices) < last ? (void) locks.unlock() : void()), 0)...};
    (void) ignored;
}

template<class... Locks>
int _try_lock_range(const int first, const int last, Locks&... locks) {
    using Indices = IndexSequenceFor<Locks...>;

    int next = first;
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
        // _RERAISE; // TODO: ???
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
        // _RERAISE; // TODO: ???
    }

    // didn't get all the locks, backout
    _unlock_locks(backoutStart, hardLock + 1, Indices{}, locks...);
    custom::this_thread::yield();
    return failed;
}

template<class... Locks>
void lock(Locks&... locks) {    // lock multiple locks, without deadlock
    int hardLock = 0;
    while (hardLock != -1)
        hardLock = _lock_attempt(hardLock, locks...);
}

template<class... Locks>
int try_lock(Locks&... locks) { // try to lock multiple locks
    return _try_lock_range(0, sizeof...(Locks), locks...);
}


template<class... Mutexes>
class ScopedLock            // locks/unlocks multiple mutexes
{
private:
    Tuple<Mutexes&...> _ownedMutexes;

public:
    // Constructors & Operators

    explicit ScopedLock(Mutexes&... mtxes): _ownedMutexes(mtxes...) { // construct and lock
        custom::lock(mtxes...);    // TODO: check
    }

    explicit ScopedLock(AdoptLock_t, Mutexes&... mtxes)
        : _ownedMutexes(mtxes...) { /*Empty*/ } // construct but don't lock

    ~ScopedLock() {
        custom::apply([](Mutexes&... mtxes) { (..., (void) mtxes.unlock()); }, _ownedMutexes);     // TODO: check
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


enum class CVStatus
{
    NoTimeout,
    Timeout
};

class ConditionVariable         // Condition variable adaptor for pthread_cond_t
{
public:
    using NativeHandleType = pthread_cond_t;

private:
    pthread_cond_t _conditionVar;

public:
    // Constructors & Operators

    ConditionVariable() {
        pthread_cond_init(&_conditionVar, nullptr);
    }

    ~ConditionVariable() {
        pthread_cond_destroy(&_conditionVar);
    }

    ConditionVariable(const ConditionVariable&)            = delete;
    ConditionVariable& operator=(const ConditionVariable&) = delete;

public:
    // Main functions

    void notify_one() noexcept {
        pthread_cond_signal(&_conditionVar);
    }

    void notify_all() noexcept {
        pthread_cond_broadcast(&_conditionVar);
    }

    void wait(UniqueLock<Mutex>& lock) {
        pthread_cond_wait(&_conditionVar, &lock.mutex()->_mutex);
    }

    template<class Predicate>
    void wait(UniqueLock<Mutex>& lock, Predicate pred) {
        while (!pred())
            pthread_cond_wait(&_conditionVar, &lock.mutex()->_mutex);
        // TODO: check
    }

    template<class Clock, class Duration>
    CVStatus wait_until(UniqueLock<Mutex>& lock, const std::chrono::time_point<Clock, Duration>& absoluteTime) {
        auto seconds        = std::chrono::time_point_cast<std::chrono::seconds>(absoluteTime);
	    auto nanoseconds    = std::chrono::duration_cast<std::chrono::nanoseconds>(absoluteTime - seconds);
        struct timespec ts  =   {
                                    static_cast<std::time_t>(seconds.time_since_epoch().count()),
                                    static_cast<long>(nanoseconds.count())
                                };

        pthread_cond_timedwait(&_conditionVar, &lock.mutex()->_mutex, &ts);
        return ((Clock::now() < absoluteTime) ? CVStatus::NoTimeout : CVStatus::Timeout);
        // TODO: check return of timedwait
    }

    template<class Clock, class Duration, class Predicate>
    bool wait_until(UniqueLock<Mutex>& lock, const std::chrono::time_point<Clock, Duration>& absoluteTime, Predicate pred) {
        while (!pred())
            if (wait_until(lock, absoluteTime) == CVStatus::Timeout)
                return pred();

        return true;
    }
    
    template<class Rep, class Period>
    CVStatus wait_for(UniqueLock<Mutex>& lock, const std::chrono::duration<Rep, Period>& relativeTime) {
	    return wait_until(  lock,
                            std::chrono::steady_clock::now() + 
                            std::chrono::ceil<typename std::chrono::steady_clock::duration>(relativeTime));
    }

    template<class Rep, class Period, class Predicate>
    bool wait_for(UniqueLock<Mutex>& lock, const std::chrono::duration<Rep, Period>& relativeTime, Predicate pred) {
	    return wait_until(  lock,
                            std::chrono::steady_clock::now() + 
                            std::chrono::ceil<typename std::chrono::steady_clock::duration>(relativeTime),
                            pred);
    }

    NativeHandleType native_handle() {
        return _conditionVar;
    }

}; // END ConditionVariable


struct UINTIsZero   // used as predicate in TimedMutex/RecursiveTimedMutex::try_lock_until
{
    const unsigned int& UInt;

    bool operator()() const {
        return UInt == 0;
    }
};

class TimedMutex    // TODO: check ALL
{
public:
    using NativeHandleType = typename Mutex::NativeHandleType;

private:
    Mutex _mutex;
    ConditionVariable _conditionVar;
    unsigned int _locked;

public:
    // Constructors & Operators

    TimedMutex() noexcept
        : _locked(0) { /*Empty*/ }

    TimedMutex(const TimedMutex&)            = delete;
    TimedMutex& operator=(const TimedMutex&) = delete;

public:
    // Main functions

    void lock() {
        UniqueLock<Mutex> lock(_mutex);

        while (_locked != 0)
            _conditionVar.wait(lock);

        _locked = UINT_MAX;
    }

    bool try_lock() noexcept {
        LockGuard<Mutex> lock(_mutex);

        if (_locked != 0)
            return false;
        else
        {
            _locked = UINT_MAX;
            return true;
        }
    }

    void unlock() {
        {
            LockGuard<Mutex> lock(_mutex);
            _locked = 0;
        }

        _conditionVar.notify_one();
    }

    template<class Clock, class Duration>
    bool try_lock_until(const std::chrono::time_point<Clock, Duration>& absoluteTime) {
        UniqueLock<Mutex> lock(_mutex);

        if (_conditionVar.wait_until(lock, absoluteTime, UINTIsZero{_locked}) == false)
            return false;

        _locked = UINT_MAX;
        return true;
    }

    template<class Rep, class Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& relativeTime) {
        return try_lock_until(  std::chrono::steady_clock::now() + 
                                std::chrono::ceil<typename std::chrono::steady_clock::duration>(relativeTime));
    }

    NativeHandleType native_handle() {
        return _mutex.native_handle();
    }
}; // END TimedMutex


class RecursiveTimedMutex   // TODO: check ALL
{
public:
    using NativeHandleType = typename Mutex::NativeHandleType;

private:
    Mutex _mutex;
    ConditionVariable _conditionVar;
    unsigned int _locked;
    Thread::ID _owner;

public:
    // Constructors & Operators

    RecursiveTimedMutex() noexcept
        : _locked(0) { /*Empty*/ }

    RecursiveTimedMutex(const RecursiveTimedMutex&)            = delete;
    RecursiveTimedMutex& operator=(const RecursiveTimedMutex&) = delete;

public:
    // Main functions

    void lock() {
        const Thread::ID tid = custom::this_thread::get_id();
        UniqueLock<Mutex> lock(_mutex);

        if (tid == _owner)
            if (_locked < UINT_MAX)
                ++_locked;
            else
                throw std::runtime_error("Resource busy...");
        else
        {
            while (_locked != 0)
                _conditionVar.wait(lock);

            _locked = 1;
            _owner  = tid;
        }
    }

    bool try_lock() noexcept {
        const Thread::ID tid = custom::this_thread::get_id();
        LockGuard<Mutex> lock(_mutex);

        if (tid == _owner)
            if (_locked < UINT_MAX)
                ++_locked;
            else
                return false;
        else
            if (_locked != 0)
                return false;
            else
            {
                _locked = 1;
                _owner  = tid;
            }
        
        return true;
    }

    void unlock() {
        bool notify = false;

        {
            LockGuard<Mutex> lock(_mutex);
            --_locked;
            if (_locked == 0)
            {
                notify = true;
                _owner  = Thread::ID();
            }
        }

        if (notify)
            _conditionVar.notify_one();
    }

    template<class Clock, class Duration>
    bool try_lock_until(const std::chrono::time_point<Clock, Duration>& absoluteTime) {
        const Thread::ID tid = custom::this_thread::get_id();
        UniqueLock<Mutex> lock(_mutex);

        if (tid == _owner)
            if (_locked < UINT_MAX)
                ++_locked;
            else
                return false;
        else
        {
            if (!_conditionVar.wait_until(lock, absoluteTime, UINTIsZero{_locked}))
                return false;

            _locked = 1;
            _owner  = tid;
        }

        return true;
    }

    template<class Rep, class Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& relativeTime) {
        return try_lock_until(  std::chrono::steady_clock::now() + 
                                std::chrono::ceil<typename std::chrono::steady_clock::duration>(relativeTime));
    }

    NativeHandleType native_handle() {
        return _mutex.native_handle();
    }
}; // END RecursiveTimedMutex

CUSTOM_END

#elif defined _MSC_VER
#error NO Mutex implementation
#endif