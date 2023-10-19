#include "../include/TestingBoost.h"


#if defined __GNUG__    // thread tests available only on __GNUG__

TEST_BOOST_BEGIN

std::string _Employee::output() const
{
    std::string ret = "Employee " + _ID + " has lunch partners: ";

    for(const auto& partner : _LunchPartners)
        ret += partner + " ";

    return ret;
}

void _send_mail(_Employee &, _Employee &)
{
    // simulate a time-consuming messaging operation
    custom::this_thread::sleep_for(custom::chrono::Seconds(1));
}

void _assign_lunch_partner(_Employee &e1, _Employee &e2)
{
    static custom::Mutex io_mutex;

    {
        custom::LockGuard<custom::Mutex> lk(io_mutex);
        std::cout << e1._ID << " and " << e2._ID << " are waiting for locks" << std::endl;
    }
 
    // use custom::lock to acquire two locks without worrying about 
    // other calls to assign_lunch_partner deadlocking us
    {
        custom::lock(e1._Mutex, e2._Mutex);
        custom::LockGuard<custom::Mutex> lk1(e1._Mutex, custom::AdoptLock);
        custom::LockGuard<custom::Mutex> lk2(e2._Mutex, custom::AdoptLock);
        
        // ========= Equivalent code (if UniqueLocks are needed, e.g. for condition variables) =========
        // custom::UniqueLock<custom::Mutex> lk1(e1._Mutex, custom::DeferLock);
		// custom::UniqueLock<custom::Mutex> lk2(e2._Mutex, custom::DeferLock);
		// custom::lock(lk1, lk2);

        // ========= Superior solution available in C++17 =========
        // custom::ScopedLock slk(e1._Mutex, e2._Mutex);

        {
            custom::LockGuard<custom::Mutex> lk(io_mutex);
            std::cout << e1._ID << " and " << e2._ID << " got locks" << std::endl;
        }

        e1._LunchPartners.push_back(e2._ID);
        e2._LunchPartners.push_back(e1._ID);
    }

    _send_mail(e1, e2);
    _send_mail(e2, e1);
}


// actual thread test functions
void lock_locks_test() {
    _Employee alice("alice"), bob("bob"), christina("christina"), dave("dave");
 
    // assign in parallel threads because mailing users about lunch assignments takes a long time
    custom::Vector<custom::Thread> threads;
    threads.emplace_back(_assign_lunch_partner, custom::ref(alice), custom::ref(bob));
    threads.emplace_back(_assign_lunch_partner, custom::ref(christina), custom::ref(bob));
    threads.emplace_back(_assign_lunch_partner, custom::ref(christina), custom::ref(alice));
    threads.emplace_back(_assign_lunch_partner, custom::ref(dave), custom::ref(bob));
 
    for (auto &thread : threads) thread.join();
        std::cout   << alice.output() << '\n'
                    << bob.output() << '\n'
                    << christina.output() << '\n'
                    << dave.output() << '\n';
}

void thread_test() {
	std::cout << custom::this_thread::get_id();
	// custom::Thread t(deque_test);
	// t.join();
}

TEST_BOOST_END

#endif		// __GNUG__