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
    custom::Mutex _Mutex;
    custom::String _ID;
    custom::Vector<custom::String> _LunchPartners;
    
    _Employee(custom::String id) : _ID(id) { /*Empty*/ }

    custom::String output() const;

    static void send_mail(_Employee&, _Employee&);
    static void assign_lunch_partner(_Employee& e1, _Employee& e2);
};  // END _Employee


void lock_locks_test();
void thread_test();
void semaphore_test();
void timed_mutex_test();
void recursive_timed_mutex_test();
void condition_variable_any_test();

TEST_BOOST_END

#endif  // __GNUG__