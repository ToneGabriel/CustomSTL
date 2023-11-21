#pragma once

#if defined __GNUG__    // thread tests available only on __GNUG__

#include "__CTMThreadInclude.h"
#include "__STDThreadInclude.h"
#include "Testing.h"

#define TEST_BOOST_BEGIN namespace test_boost {
#define TEST_BOOST_END }


TEST_BOOST_BEGIN

struct _Employee
{
    custom::String _ID;
    custom::Vector<custom::String> _LunchPartners;
    custom::Mutex _Mutex;
    
    _Employee(custom::String id) : _ID(id) { /*Empty*/ }

    custom::String output() const;
};  // END _Employee


void _send_mail(_Employee &, _Employee &);
void _assign_lunch_partner(_Employee &e1, _Employee &e2);
void _semaphore_test_task(int id, custom::CountingSemaphore<>& sem);
void _timed_mutex_test_task(int id, custom::TimedMutex& mtx);
void _recursive_timed_mutex_test_task(int id, int depth, custom::RecursiveTimedMutex& rmtx);
void _cva_producer(custom::ConditionVariableAny& cv, custom::Mutex& mtx, bool& dataReady);
void _cva_consumer(custom::ConditionVariableAny& cv, custom::Mutex& mtx, bool& dataReady);

void lock_locks_test();
void thread_test();
void semaphore_test();
void timed_mutex_test();
void recursive_timed_mutex_test();
void condition_variable_any_test();

TEST_BOOST_END

#endif  // __GNUG__