#include "../include/TestingBoost.h"


#if defined __GNUG__    // thread tests available only on __GNUG__

TEST_BOOST_BEGIN

custom::String _Employee::output() const
{
    custom::String ret = "Employee " + _ID + " has lunch partners: ";

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

void _semaphore_test_task(int id, custom::CountingSemaphore<>& sem) {
    // custom::chrono::SystemClock::now() + custom::chrono::Milliseconds(2000);
    // custom::chrono::Milliseconds(2000);

    if (sem.try_acquire_for(custom::chrono::Milliseconds(2000)))
    {
        // If the semaphore is acquired within the timeout
        std::cout << "Task " << id << " acquired the semaphore." << std::endl;
        custom::this_thread::sleep_for(custom::chrono::Milliseconds(5000)); // Simulating work
        sem.release(); // Release the semaphore after work is done
        std::cout << "Task " << id << " released the semaphore." << std::endl;
    }
    else
    {
        // If unable to acquire the semaphore within the timeout
        std::cout << "Task " << id << " couldn't acquire the semaphore within the timeout." << std::endl;
    }
}

void _timed_mutex_test_task(int id, custom::TimedMutex& mtx) {
    // Try to acquire the timed mutex for 3 seconds
    if (mtx.try_lock_for(custom::chrono::Seconds(3)))
    {
        std::cout << "Thread " << id << " acquired the timed mutex." << std::endl;
        // Simulating some work
        custom::this_thread::sleep_for(custom::chrono::Seconds(2));
        // Release the mutex
        mtx.unlock();
        std::cout << "Thread " << id << " released the timed mutex." << std::endl;
    }
    else
        std::cout << "Thread " << id << " couldn't acquire the timed mutex within 3 seconds." << std::endl;
}

void _recursive_timed_mutex_test_task(int id, int depth, std::recursive_timed_mutex& rmtx) {
    if (depth <= 0)
        return;

    // Try to acquire the recursive timed mutex for 3 seconds
    if (rmtx.try_lock_for(std::chrono::seconds(3)))
    {
        std::cout << "Thread " << id << " acquired the recursive timed mutex." << std::endl;
        // Simulating some work
        std::this_thread::sleep_for(std::chrono::seconds(1));
        // Call the function recursively with a decreased depth
        _recursive_timed_mutex_test_task(id, depth - 1, rmtx);
        // Release the mutex
        rmtx.unlock();
        std::cout << "Thread " << id << " released the recursive timed mutex." << std::endl;
    }
    else
        std::cout << "Thread " << id << " couldn't acquire the recursive timed mutex within 3 seconds." << std::endl;
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

void semaphore_test() {
	const int numTasks = 3;
    custom::CountingSemaphore<> semaphore(2); // Initialize semaphore with a count of 2

    custom::Thread tasks[numTasks];
    
    // Start multiple tasks that attempt to acquire the semaphore
    for (int i = 0; i < numTasks; ++i)
        tasks[i] = custom::Thread(_semaphore_test_task, i + 1, custom::ref(semaphore));

    // Join all threads to the main thread
    for (int i = 0; i < numTasks; ++i)
        tasks[i].join();
}

void timed_mutex_test() {
    custom::TimedMutex mtx;
    custom::Thread t1(_timed_mutex_test_task, 1, custom::ref(mtx));
    custom::Thread t2(_timed_mutex_test_task, 2, custom::ref(mtx));

    t1.join();
    t2.join();
}

void recursive_timed_mutex_test() {
    std::recursive_timed_mutex rmtx;
    std::thread t1(_recursive_timed_mutex_test_task, 1, 3, std::ref(rmtx));
    std::thread t2(_recursive_timed_mutex_test_task, 2, 2, std::ref(rmtx));

    t1.join();
    t2.join();
}

TEST_BOOST_END

#endif		// __GNUG__