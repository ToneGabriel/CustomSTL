#pragma once

#if defined __GNUG__
#include "Utility.h"
#include "Thread.h"

#include <chrono>


CUSTOM_BEGIN

template<class Mtx>
struct UnlockGuard
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
};


class ConditionVariableAny
{
    // TODO: implement
};

CUSTOM_END

#elif defined _MSC_VER
#error NO Condition Variable implementation
#endif