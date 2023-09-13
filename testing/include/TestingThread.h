#pragma once

#include "__CTMThreadInclude.h"
#include "__STDThreadInclude.h"
#include "TestingCommon.h"



TEST_BEGIN

#define THREAD_HELP_BEGIN namespace thread_help {
#define THREAD_HELP_END }

THREAD_HELP_BEGIN

struct Employee
{
    std::string _ID;
    custom::Vector<std::string> _LunchPartners;
    custom::Mutex _Mutex;
    
    Employee(std::string id) : _ID(id) { /*Empty*/ }

    std::string output() const;
};  // END Employee

void send_mail(Employee &, Employee &);
void assign_lunch_partner(Employee &e1, Employee &e2);

THREAD_HELP_END

void lock_locks_test();
void thread_test();

TEST_END