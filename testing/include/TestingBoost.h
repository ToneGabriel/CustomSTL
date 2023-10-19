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
    std::string _ID;
    custom::Vector<std::string> _LunchPartners;
    custom::Mutex _Mutex;
    
    _Employee(std::string id) : _ID(id) { /*Empty*/ }

    std::string output() const;
};  // END _Employee


void _send_mail(_Employee &, _Employee &);
void _assign_lunch_partner(_Employee &e1, _Employee &e2);

void lock_locks_test();
void thread_test();

TEST_BOOST_END

#endif  // __GNUG__