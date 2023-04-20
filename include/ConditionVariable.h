#pragma once

#if defined __GNUG__
#include "Utility.h"
#include "Thread.h"

#include <chrono>


CUSTOM_BEGIN

class ConditionVariable         // Condition variable wrapper for pthread_cond_t
{
    // TODO: implement

public:
    using NativeHandleType = pthread_cond_t;

private:
    pthread_cond_t _conditionVar;

public:
    // Constructors & Operators

    ConditionVariable() = default;

    ~ConditionVariable() = default;

    ConditionVariable(const ConditionVariable&)            = delete;
    ConditionVariable& operator=(const ConditionVariable&) = delete;

public:
    // Main functions

    void notify_one();

    void notify_all();

    void wait();

    bool wait_for();

    bool wait_until();

    NativeHandleType native_handle();

}; // END ConditionVariable

CUSTOM_END

#elif defined _MSC_VER
#error NO Condition Variable implementation
#endif