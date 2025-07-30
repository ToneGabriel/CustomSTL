#pragma once

#if defined __GNUG__
#include "custom/thread.h"


CUSTOM_BEGIN

class mutex;
class recursive_mutex;
class timed_mutex;
class recursive_timed_mutex;

class shared_mutex;
class shared_timed_mutex;

class condition_variable;
class condition_variable_any;


CUSTOM_DETAIL_BEGIN     // lock impl

// lock locks[target]
template<size_t... Indices, class... Locks>
void _lock_one_from_locks(const int target, index_sequence<Indices...>, Locks&... locks)
{
    // ((static_cast<int>(Indices) == target ? (void)locks.lock() : void()), ...);
    
    // Without the array initializer, the fold expression could potentially be optimized away
    // by the compiler if it determines that the result of the expression isn't needed.
    // This optimization might lead to unexpected behavior because
    // the actions inside the fold expression wouldn't be performed.

    // By introducing the array initializer, you force the compiler to evaluate
    // the fold expression for each element in the pack and ensure that the actions are executed,
    // regardless of whether their results are actually used.
    // This guarantees that the critical operations performed are executed as intended.

#define _FOLD_EXPRESSION (static_cast<int>(Indices) == target ? (void)locks.lock() : void())

    int ignored[] = {(_FOLD_EXPRESSION, 0)...};
    (void)ignored; // suppress warning for unused variable

#undef _FOLD_EXPRESSION
}

// try to lock locks[target]
template<size_t... Indices, class... Locks>
bool _try_lock_one_from_locks(const int target, index_sequence<Indices...>, Locks&... locks)
{
    bool result;

#define _FOLD_EXPRESSION (static_cast<int>(Indices) == target ? (void)(result = locks.try_lock()) : void())

    int ignored[] = {(_FOLD_EXPRESSION, 0)...};
    (void)ignored;

#undef _FOLD_EXPRESSION

    return result;
}

// unlock locks in locks[first, last)
template<size_t... Indices, class... Locks>
void _unlock_locks_range(const int first, const int last, index_sequence<Indices...>, Locks&... locks) noexcept
{
#define _FOLD_EXPRESSION (first <= static_cast<int>(Indices) && static_cast<int>(Indices) < last ? (void)locks.unlock() : void())

    int ignored[] = {(_FOLD_EXPRESSION, 0)...};
    (void)ignored;

#undef _FOLD_EXPRESSION
}

template<class... Locks>
int _try_lock_range(const int first, const int last, Locks&... locks)
{
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


// lock multiple locks, without deadlock
template<class... Locks>
void lock(Locks&... locks)
{
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
int try_lock(Locks&... locks)
{
    // try to lock multiple locks
    // -1 on success, or ​0​-based index value of the object that failed to lock.
    return detail::_try_lock_range(0, sizeof...(Locks), locks...);
}


// tag struct declarations for construction of unique_lock/shared_lock objects
struct adopt_lock_t { explicit adopt_lock_t() = default; };
struct defer_lock_t { explicit defer_lock_t() = default; };
struct try_to_lock_t { explicit try_to_lock_t() = default; };

constexpr adopt_lock_t adopt_lock = adopt_lock_t();
constexpr defer_lock_t defer_lock = defer_lock_t();
constexpr try_to_lock_t try_to_lock = try_to_lock_t();


template<class Mtx>
class lock_guard         // automatic unlock
{
public:
    using mutex_type = Mtx;

private:
    mutex_type& _mutex;

public:
    // Constructors & Operators

    explicit lock_guard(mutex_type& mtx) : _mutex(mtx)
    {
        _mutex.lock();
    }

    lock_guard(mutex_type& mtx, adopt_lock_t) : _mutex(mtx) { /*Empty*/ } // construct but don't lock

    ~lock_guard()
    {
        _mutex.unlock();
    }
    
    lock_guard(const lock_guard&)             = delete;
    lock_guard& operator=(const lock_guard&)  = delete;
}; // END lock_guard


template<class Mtx>
struct unlock_guard      // automatic lock
{
public:
    using mutex_type = Mtx;

private:
    mutex_type& _mutex;

public:
    // Constructors & Operators

    explicit unlock_guard(mutex_type& mtx) : _mutex(mtx)
    {
        _mutex.unlock();
    }

    ~unlock_guard()
    {
        // relock mutex or terminate()
        // condition_variable_any wait functions are required to terminate if
        // the mutex cannot be relocked;
        _mutex.lock();
    }

    unlock_guard(const unlock_guard&)             = delete;
    unlock_guard& operator=(const unlock_guard&)  = delete;
}; // END unlock_guard


template<class Mtx>
class unique_lock
{
public:
    using mutex_type = Mtx;

private:
    mutex_type* _ownedMutex = nullptr;
    bool _owns              = false;

public:
    // Constructors

    unique_lock()                   = default;
    unique_lock(const unique_lock&) = delete;

    // construct and lock
    unique_lock(mutex_type& mtx) : _ownedMutex(&mtx), _owns(false)
    {
        _ownedMutex->lock();
        _owns = true;
    }

    unique_lock(mutex_type& mtx, adopt_lock_t)  // construct and assume already locked
        : _ownedMutex(&mtx), _owns(true) { /*Empty*/ }

    unique_lock(mutex_type& mtx, defer_lock_t) noexcept // construct but don't lock
        : _ownedMutex(&mtx), _owns(false) { /*Empty*/ }

    unique_lock(mutex_type& mtx, try_to_lock_t) // construct and try to lock
        : _ownedMutex(&mtx), _owns(_ownedMutex->try_lock()) { /*Empty*/ }

    template<class Rep, class Period>
    unique_lock(mutex_type& mtx, const custom::chrono::duration<Rep, Period>& relativeTime) // construct and lock with timeout
        : _ownedMutex(&mtx), _owns(_ownedMutex->try_lock_for(relativeTime)) { /*Empty*/ }

    template<class Clock, class Duration>
    unique_lock(mutex_type& mtx, const custom::chrono::time_point<Clock, Duration>& absoluteTime)   // construct and lock with timeout
        : _ownedMutex(&mtx), _owns(_ownedMutex->try_lock_until(absoluteTime)) { /*Empty*/ }

    unique_lock(unique_lock&& other) noexcept : _ownedMutex(other._ownedMutex), _owns(other._owns)
    {
        other._ownedMutex   = nullptr;
        other._owns         = false;
    }

    ~unique_lock()
    {
        if (_owns)
            _ownedMutex->unlock();
    }

public:
    // Operators

    unique_lock& operator=(const unique_lock&) = delete;

    unique_lock& operator=(unique_lock&& other)
    {
        if (_ownedMutex != other._ownedMutex)
        {
            if (_owns)
                _ownedMutex->unlock();  // can throw

            _ownedMutex = custom::exchange(other._ownedMutex, nullptr);
            _owns       = custom::exchange(other._owns, false);
        }

        return *this;
    }

    explicit operator bool() const noexcept
    {
        return _owns;
    }

public:
    // Main functions

    void lock()
    {
        _validate();
        _ownedMutex->lock();
        _owns = true;
    }

    bool try_lock()
    {
        _validate();
        _owns = _ownedMutex->try_lock();
        return _owns;
    }

    template<class Rep, class Period>
    bool try_lock_for(const custom::chrono::duration<Rep, Period>& relativeTime)
    {
        _validate();
        _owns = _ownedMutex->try_lock_for(relativeTime);
        return _owns;
    }

    template<class Clock, class Duration>
    bool try_lock_until(const custom::chrono::time_point<Clock, Duration>& absoluteTime)
    {
        _validate();
        _owns = _ownedMutex->try_lock_until(absoluteTime);
        return _owns;
    }

    void unlock()
    {
        if (_ownedMutex == nullptr || _owns == false)
            throw std::logic_error("No mutex owned.");

        _ownedMutex->unlock();
        _owns = false;
    }

    mutex_type* release()
    {
        mutex_type* temp = _ownedMutex;
        _ownedMutex     = nullptr;
        _owns           = false;

        return temp;
    }

    mutex_type* mutex() const noexcept
    {
        return _ownedMutex;
    }

    bool owns_lock() const
    {
        return _owns;
    }

private:
    // Helpers

    // check if mutex can be locked
    void _validate() const
    {
        if (_ownedMutex == nullptr)
            throw std::logic_error("No mutex owned.");

        if (_owns)
            throw std::logic_error("mutex already locked.");
    }
}; // END unique_lock


// is shared mutex
template<class Ty>
constexpr bool is_shared_mutex_v = is_any_of_v<remove_cv_t<Ty>,
                                                        shared_mutex,
                                                        shared_timed_mutex>;

template<class Ty>
struct is_shared_mutex : bool_constant<is_shared_mutex_v<Ty>> {};


template<class Mtx>
class shared_lock
{
public:
    static_assert(is_shared_mutex_v<Mtx>, "shared_lock requires shared mutex type!");
    using mutex_type = Mtx;

private:
    mutex_type* _ownedSMutex    = nullptr;
    bool _owns                  = false;

public:
    // Constructors

    shared_lock()                   = default;
    shared_lock(const shared_lock&) = delete;

    // construct and lock
    shared_lock(mutex_type& mtx) : _ownedSMutex(&mtx), _owns(false)
    {
        _ownedSMutex->lock_shared();
        _owns = true;
    }

    shared_lock(mutex_type& mtx, adopt_lock_t)  // construct and assume already locked
        : _ownedSMutex(&mtx), _owns(true) { /*Empty*/ }

    shared_lock(mutex_type& mtx, defer_lock_t) noexcept // construct but don't lock
        : _ownedSMutex(&mtx), _owns(false) { /*Empty*/ }

    shared_lock(mutex_type& mtx, try_to_lock_t) // construct and try to lock
        : _ownedSMutex(&mtx), _owns(_ownedSMutex->try_lock_shared()) { /*Empty*/ }

    template<class Rep, class Period>
    shared_lock(mutex_type& mtx, const custom::chrono::duration<Rep, Period>& relativeTime) // construct and lock with timeout
        : _ownedSMutex(&mtx), _owns(_ownedSMutex->try_lock_shared_for(relativeTime)) { /*Empty*/ }

    template<class Clock, class Duration>
    shared_lock(mutex_type& mtx, const custom::chrono::time_point<Clock, Duration>& absoluteTime)   // construct and lock with timeout
        : _ownedSMutex(&mtx), _owns(_ownedSMutex->try_lock_shared_until(absoluteTime)) { /*Empty*/ }

    shared_lock(shared_lock&& other) noexcept : _ownedSMutex(other._ownedSMutex), _owns(other._owns)
    {
        other._ownedSMutex  = nullptr;
        other._owns         = false;
    }

    ~shared_lock()
    {
        if (_owns)
            _ownedSMutex->unlock_shared();
    }

public:
    // Operators

    shared_lock& operator=(const shared_lock&) = delete;

    shared_lock& operator=(shared_lock&& other)
    {
        if (_ownedSMutex != other._ownedSMutex)
        {
            if (_owns)
                _ownedSMutex->unlock_shared();  // can throw

            _ownedSMutex    = custom::exchange(other._ownedSMutex, nullptr);
            _owns           = custom::exchange(other._owns, false);
        }

        return *this;
    }

    explicit operator bool() const noexcept
    {
        return _owns;
    }

public:
    // Main functions

    void lock()
    {
        _validate();
        _ownedSMutex->lock_shared();
        _owns = true;
    }

    bool try_lock()
    {
        _validate();
        _owns = _ownedSMutex->try_lock_shared();
        return _owns;
    }

    template<class Rep, class Period>
    bool try_lock_for(const custom::chrono::duration<Rep, Period>& relativeTime)
    {
        _validate();
        _owns = _ownedSMutex->try_lock_shared_for(relativeTime);
        return _owns;
    }

    template<class Clock, class Duration>
    bool try_lock_until(const custom::chrono::time_point<Clock, Duration>& absoluteTime)
    {
        _validate();
        _owns = _ownedSMutex->try_lock_shared_until(absoluteTime);
        return _owns;
    }

    void unlock()
    {
        if (_ownedSMutex == nullptr || _owns == false)
            throw std::logic_error("No mutex owned.");

        _ownedSMutex->unlock_shared();
        _owns = false;
    }

    mutex_type* release()
    {
        mutex_type* temp = _ownedSMutex;
        _ownedSMutex    = nullptr;
        _owns           = false;

        return temp;
    }

    mutex_type* mutex() const noexcept
    {
        return _ownedSMutex;
    }

    bool owns_lock() const
    {
        return _owns;
    }

private:
    // Helpers

    // check if mutex can be locked
    void _validate() const
    {
        if (_ownedSMutex == nullptr)
            throw std::logic_error("No mutex owned.");

        if (_owns)
            throw std::logic_error("mutex already locked.");
    }
};  // END shared_lock


template<class... Mutexes>
class scoped_lock            // locks/unlocks multiple mutexes
{
private:
    tuple<Mutexes&...> _ownedMutexes;

public:
    // Constructors & Operators

    explicit scoped_lock(Mutexes&... mtxes): _ownedMutexes(mtxes...)    // construct and lock
    {
        custom::lock(mtxes...);
    }

    explicit scoped_lock(adopt_lock_t, Mutexes&... mtxes)
        : _ownedMutexes(mtxes...) { /*Empty*/ } // construct but don't lock

    ~scoped_lock()
    {
        auto reverseUnlock = [](Mutexes&... mtxes)
        {
            // (args, ...) left folding   = first -> last
            // (..., args) right folding  = last -> first

            (..., (void)mtxes.unlock());
        };

        custom::apply(reverseUnlock, _ownedMutexes);
    }

    scoped_lock(const scoped_lock&)            = delete;
    scoped_lock& operator=(const scoped_lock&) = delete;
}; // END scoped_lock


template<class Mtx>
class scoped_lock<Mtx>
{
public:
    using mutex_type = Mtx;

private:
    mutex_type& _ownedMutex;
    
public:
    // Constructors & Operators

    explicit scoped_lock(mutex_type& mtx) : _ownedMutex(mtx)    // construct and lock
    {
        _ownedMutex.lock();
    }

    explicit scoped_lock(adopt_lock_t, mutex_type& mtx)
        : _ownedMutex(mtx) { /*Empty*/ } // construct but don't lock

    ~scoped_lock()
    {
        _ownedMutex.unlock();
    }

    scoped_lock(const scoped_lock&)            = delete;
    scoped_lock& operator=(const scoped_lock&) = delete;
}; // END scoped_lock one


template<>
class scoped_lock<>
{
public:
    // Constructors & Operators

    explicit scoped_lock() { /*Empty*/ }
    explicit scoped_lock(adopt_lock_t) { /*Empty*/ }
    ~scoped_lock() { /*Empty*/ }

    scoped_lock(const scoped_lock&)            = delete;
    scoped_lock& operator=(const scoped_lock&) = delete;
}; // END scoped_lock empty


CUSTOM_END

#elif defined _MSC_VER
#error NO Locks implementation
#endif