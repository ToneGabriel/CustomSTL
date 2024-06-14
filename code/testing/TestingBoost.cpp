#include "TestingBoost.h"


#if defined __GNUG__    // thread tests available only on __GNUG__

TEST_BOOST_BEGIN

// !!! OPTIONAL !!!
// mutexes and semaphores can be declared above lambda,
// can be put into capture clause [&mtx] (...)
// if so, they can be removed from lambda parameters


custom::String _Employee::output() const
{
    custom::String ret = "Employee " + _ID + " has lunch partners: ";

    for(const auto& partner : _LunchPartners)
        ret += partner + " ";

    return ret;
}

void _Employee::send_mail(_Employee &, _Employee &)
{
    // simulate a time-consuming messaging operation
    custom::this_thread::sleep_for(custom::chrono::Seconds(1));
}

void _Employee::assign_lunch_partner(_Employee &e1, _Employee &e2)
{
    static custom::Mutex io_mutex;

    {
        custom::LockGuard<custom::Mutex> lk(io_mutex);
        std::cout << e1._ID << " and " << e2._ID << " are waiting for locks" << std::endl;
    }
 
    // use custom::lock to acquire two locks without worrying about 
    // other calls to assign_lunch_partner deadlocking us
    {
        // ========= Classical solution =========
        // custom::lock(e1._Mutex, e2._Mutex);
        // custom::LockGuard<custom::Mutex> lk1(e1._Mutex, custom::AdoptLock);
        // custom::LockGuard<custom::Mutex> lk2(e2._Mutex, custom::AdoptLock);
        
        // ========= Equivalent code (if UniqueLocks are needed, e.g. for condition variables) =========
        // custom::UniqueLock<custom::Mutex> lk1(e1._Mutex, custom::DeferLock);
		// custom::UniqueLock<custom::Mutex> lk2(e2._Mutex, custom::DeferLock);
		// custom::lock(lk1, lk2);

        // ========= Superior solution available in C++17 =========
        custom::ScopedLock slk(e1._Mutex, e2._Mutex);

        {
            custom::LockGuard<custom::Mutex> lk(io_mutex);
            std::cout << e1._ID << " and " << e2._ID << " got locks" << std::endl;
        }

        e1._LunchPartners.push_back(e2._ID);
        e2._LunchPartners.push_back(e1._ID);
    }

    _Employee::send_mail(e1, e2);
    _Employee::send_mail(e2, e1);
}

// actual thread test functions
void lock_locks_test() {
    _Employee alice("alice"), bob("bob"), christina("christina"), dave("dave");
 
    // assign in parallel threads because mailing users about lunch assignments takes a long time
    custom::vector<custom::thread> threads;
    threads.emplace_back(_Employee::assign_lunch_partner, custom::ref(alice), custom::ref(bob));
    threads.emplace_back(_Employee::assign_lunch_partner, custom::ref(christina), custom::ref(bob));
    threads.emplace_back(_Employee::assign_lunch_partner, custom::ref(christina), custom::ref(alice));
    threads.emplace_back(_Employee::assign_lunch_partner, custom::ref(dave), custom::ref(bob));
 
    for (auto &thread : threads)
        thread.join();

    std::cout   << alice.output()       << '\n'
                << bob.output()         << '\n'
                << christina.output()   << '\n'
                << dave.output()        << '\n';
}

void thread_test() {
	std::cout << custom::this_thread::get_id();
	custom::thread t(test::deque_test);
	t.join();
}

void semaphore_test() {
    auto semaphore_test_task = [](int id, custom::CountingSemaphore<>& sem) {
        // custom::chrono::system_clock::now() + custom::chrono::Milliseconds(2000);
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
    };  // semaphore task

	const int numTasks = 3;
    custom::CountingSemaphore<> semaphore(2); // Initialize semaphore with a count of 2
    custom::thread tasks[numTasks];
    
    // Start multiple tasks that attempt to acquire the semaphore
    for (int i = 0; i < numTasks; ++i)
        tasks[i] = custom::thread(semaphore_test_task, i + 1, custom::ref(semaphore));

    // Join all threads to the main thread
    for (int i = 0; i < numTasks; ++i)
        tasks[i].join();
}

void timed_mutex_test() {
    auto timed_mutex_test_task = [](int id, custom::TimedMutex& mtx) {
        // Try to acquire the timed mutex for 3 seconds
        if (mtx.try_lock_for(custom::chrono::Seconds(3)))
        {
            std::cout << "thread " << id << " acquired the timed mutex." << std::endl;
            // Simulating some work
            custom::this_thread::sleep_for(custom::chrono::Seconds(2));
            // Release the mutex
            mtx.unlock();
            std::cout << "thread " << id << " released the timed mutex." << std::endl;
        }
        else
            std::cout << "thread " << id << " couldn't acquire the timed mutex within 3 seconds." << std::endl;
    };  // task for timed mutex

    custom::TimedMutex mtx;
    custom::thread t1(timed_mutex_test_task, 1, custom::ref(mtx));
    custom::thread t2(timed_mutex_test_task, 2, custom::ref(mtx));

    t1.join();
    t2.join();
}

void recursive_timed_mutex_test() {
    auto recursive_timed_mutex_test_task = [] ( int id, int depth,
                                                custom::RecursiveTimedMutex& rmtx,
                                                auto&& recursive_timed_mutex_test_task
                                                /* for recursive lambda */) {

        if (depth <= 0)
            return;

        // Try to acquire the recursive timed mutex for 3 seconds
        if (rmtx.try_lock_for(custom::chrono::Seconds(3)))
        {
            std::cout << "thread " << id << " acquired the recursive timed mutex." << std::endl;

            // Simulating some work
            custom::this_thread::sleep_for(custom::chrono::Seconds(1));

            // Call the function recursively with a decreased depth
            recursive_timed_mutex_test_task(id, depth - 1, rmtx, recursive_timed_mutex_test_task);
            
            // Release the mutex
            rmtx.unlock();
            std::cout << "thread " << id << " released the recursive timed mutex." << std::endl;
        }
        else
            std::cout << "thread " << id << " couldn't acquire the recursive timed mutex within 3 seconds." << std::endl;
    };  // task for recursive timed mutex

	custom::RecursiveTimedMutex rmtx;
    custom::thread t1(recursive_timed_mutex_test_task, 1, 3, custom::ref(rmtx), recursive_timed_mutex_test_task);
    custom::thread t2(recursive_timed_mutex_test_task, 2, 2, custom::ref(rmtx), recursive_timed_mutex_test_task);

    t1.join();
    t2.join();
}

void condition_variable_any_test() {
    auto cva_producer = [](custom::ConditionVariableAny& cva, custom::Mutex& mtx, bool& dataReady) {
        std::cout << "Producer is working..." << std::endl;
        custom::this_thread::sleep_for(custom::chrono::Seconds(2)); // Simulating some work
        
        {
            custom::LockGuard<custom::Mutex> lock(mtx);
            dataReady = true;
        }

        std::cout << "Producer: Data is ready!" << std::endl;
        cva.notify_one(); // Notify the consumer that data is ready
    };  // producer task

    auto cva_consumer = [](custom::ConditionVariableAny& cva, custom::Mutex& mtx, bool& dataReady) {
        std::cout << "Consumer is waiting..." << std::endl;
        custom::UniqueLock<custom::Mutex> lock(mtx);

#if 1   // switch 0/1 for different example
        cva.wait(lock, [&dataReady] { return dataReady; });
        std::cout << "Consumer: Data can be used!" << std::endl;    // Process the data
#else
        if (cva.wait_for(lock, custom::chrono::Seconds(3), [&dataReady] { return dataReady; })) // Data is available within 3 seconds
            std::cout << "Consumer: Data can be used!" << std::endl;
        else    // Timeout occurred
            std::cout << "Consumer: Timeout! Data is not ready within 3 seconds." << std::endl;
#endif  // 0/1
    };  // consumer task

    custom::ConditionVariableAny cva;
    custom::Mutex mtx;
    bool dataReady = false;

    custom::thread producerThread(cva_producer, custom::ref(cva), custom::ref(mtx), custom::ref(dataReady));
    custom::thread consumerThread(cva_consumer, custom::ref(cva), custom::ref(mtx), custom::ref(dataReady));

    producerThread.join();
    consumerThread.join();
}

void shared_mutex_test() {
    int sharedData      = 0;        // critical resource
    int storedData[40]  = {-1};     // output ("safe" to share)

    auto writer = [&](custom::SharedMutex& smtx) {
        custom::LockGuard<custom::SharedMutex> lock(smtx);  // exclusive
        ++sharedData;                                       // modify data
    };

    auto reader = [&](custom::SharedMutex& smtx, int current) {
        custom::SharedLock<custom::SharedMutex> lock(smtx); // shared
        storedData[current] = sharedData;                   // read data
        //custom::this_thread::sleep_for(custom::chrono::Milliseconds(100));
    };

    custom::vector<custom::thread> threads;
    custom::SharedMutex smtx;
    int index = 0;

    threads.emplace_back(custom::thread(writer, custom::ref(smtx)));                // start 1 writer

    for (/*Empty*/; index < 20; ++index)
        threads.emplace_back(custom::thread(reader, custom::ref(smtx), index));     // start 20 readers

    threads.emplace_back(custom::thread(writer, custom::ref(smtx)));                // start another writer

    for (/*Empty*/; index < 40; ++index)
        threads.emplace_back(custom::thread(reader, custom::ref(smtx), index));     // start onother 20 readers

    for (auto& thr : threads)
        thr.join();                                                                 // join all

    std::cout << "Stored Data=\n";
    for (index = 0; index < 40; ++index)                                            // print results
        std::cout << storedData[index] << ' ';
    
    std::cout << "\nShared Data= " << sharedData << '\n';
}

void shared_timed_mutex_test() {
    int sharedData      = 0;        // critical resource
    int storedData[40]  = {-1};     // output ("safe" to share)

    auto writer = [&](custom::SharedTimedMutex& stmtx) {
        custom::UniqueLock<custom::SharedTimedMutex> lock(stmtx, custom::chrono::Seconds(2));  // exclusive (try_lock_for)
        ++sharedData;                                       // modify data
    };

    auto reader = [&](custom::SharedTimedMutex& stmtx, int current) {
        custom::SharedLock<custom::SharedTimedMutex> lock(stmtx, custom::chrono::Seconds(1)); // shared (try_lock_shared_for)
        storedData[current] = sharedData;                   // read data
        custom::this_thread::sleep_for(custom::chrono::Milliseconds(2000));
    };

    custom::vector<custom::thread> threads;
    custom::SharedTimedMutex stmtx;
    int index = 0;

    threads.emplace_back(custom::thread(writer, custom::ref(stmtx)));               // start 1 writer

    for (/*Empty*/; index < 20; ++index)
        threads.emplace_back(custom::thread(reader, custom::ref(stmtx), index));    // start 20 readers

    threads.emplace_back(custom::thread(writer, custom::ref(stmtx)));               // start another writer

    for (/*Empty*/; index < 40; ++index)
        threads.emplace_back(custom::thread(reader, custom::ref(stmtx), index));    // start onother 20 readers

    for (auto& thr : threads)
        thr.join();                                                                 // join all

    std::cout << "Stored Data=\n";
    for (index = 0; index < 40; ++index)                                            // print results
        std::cout << storedData[index] << ' ';
    
    std::cout << "\nShared Data= " << sharedData << '\n';
}

TEST_BOOST_END

#endif		// __GNUG__