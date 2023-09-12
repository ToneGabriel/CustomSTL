#pragma once

#if defined __GNUG__    // thread tests available only on __GNUG__
#include "__xTesting.h"
#include "__LocalTestingThreadInclude.h"
#include "__STDTestingThreadInclude.h"


TEST_BEGIN

struct Employee
{
    std::string id;
    custom::Vector<std::string> lunch_partners;
    custom::Mutex m;
    
    Employee(std::string id) : id(id) {}

    std::string output() const
    {
        std::string ret = "Employee " + id + " has lunch partners: ";
        for( const auto& partner : lunch_partners )
            ret += partner + " ";
        return ret;
    }
};
 
void send_mail(Employee &, Employee &)
{
    // simulate a time-consuming messaging operation
    custom::this_thread::sleep_for(custom::chrono::Seconds(1));
}

void assign_lunch_partner(Employee &e1, Employee &e2)
{
    static custom::Mutex io_mutex;
    {
        custom::LockGuard<custom::Mutex> lk(io_mutex);
        std::cout << e1.id << " and " << e2.id << " are waiting for locks" << std::endl;
    }
 
    // use custom::lock to acquire two locks without worrying about 
    // other calls to assign_lunch_partner deadlocking us
    {
        custom::lock(e1.m, e2.m);
        custom::LockGuard<custom::Mutex> lk1(e1.m, custom::AdoptLock);
        custom::LockGuard<custom::Mutex> lk2(e2.m, custom::AdoptLock);
// Equivalent code (if UniqueLocks are needed, e.g. for condition variables)
		// custom::UniqueLock<custom::Mutex> lk1(e1.m, custom::DeferLock);
		// custom::UniqueLock<custom::Mutex> lk2(e2.m, custom::DeferLock);
		// custom::lock(lk1, lk2);
// Superior solution available in C++17
		// custom::ScopedLock lk(e1.m, e2.m);
        {
            custom::LockGuard<custom::Mutex> lk(io_mutex);
            std::cout << e1.id << " and " << e2.id << " got locks" << std::endl;
        }
        e1.lunch_partners.push_back(e2.id);
        e2.lunch_partners.push_back(e1.id);
    }

    send_mail(e1, e2);
    send_mail(e2, e1);
}



void lock_locks_test() {
    Employee alice("alice"), bob("bob"), christina("christina"), dave("dave");
 
    // assign in parallel threads because mailing users about lunch assignments
    // takes a long time
    custom::Vector<custom::Thread> threads;
    threads.emplace_back(assign_lunch_partner, custom::ref(alice), custom::ref(bob));
    threads.emplace_back(assign_lunch_partner, custom::ref(christina), custom::ref(bob));
    threads.emplace_back(assign_lunch_partner, custom::ref(christina), custom::ref(alice));
    threads.emplace_back(assign_lunch_partner, custom::ref(dave), custom::ref(bob));
 
    for (auto &thread : threads) thread.join();
    std::cout << alice.output() << '\n'  << bob.output() << '\n' << christina.output() << '\n' << dave.output() << '\n';
	
}

void thread_test() {
	std::cout << custom::this_thread::get_id();
	// custom::Thread t(deque_test);
	// t.join();
}

TEST_END

#endif	// __GNUG__ for thread tests