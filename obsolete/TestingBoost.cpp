#include <TestingBoost.h>


#if defined __GNUG__    // thread tests available only on __GNUG__

TEST_BOOST_BEGIN

// !!! OPTIONAL !!!
// mutexes and semaphores can be declared above lambda,
// can be put into capture clause [&mtx] (...)
// if so, they can be removed from lambda parameters


custom::string _Employee::output() const
{
    custom::string ret = "Employee " + _ID + " has lunch partners: ";

    for(const auto& partner : _LunchPartners)
        ret += partner + " ";

    return ret;
}

void _Employee::send_mail(_Employee &, _Employee &)
{
    // simulate a time-consuming messaging operation
    custom::this_thread::sleep_for(custom::chrono::seconds(1));
}

void _Employee::assign_lunch_partner(_Employee &e1, _Employee &e2)
{
    static custom::mutex io_mutex;

    {
        custom::lock_guard<custom::mutex> lk(io_mutex);
        std::cout << e1._ID << " and " << e2._ID << " are waiting for locks" << std::endl;
    }
 
    // use custom::lock to acquire two locks without worrying about 
    // other calls to assign_lunch_partner deadlocking us
    {
        // ========= Classical solution =========
        // custom::lock(e1._Mutex, e2._Mutex);
        // custom::lock_guard<custom::mutex> lk1(e1._Mutex, custom::adopt_lock);
        // custom::lock_guard<custom::mutex> lk2(e2._Mutex, custom::adopt_lock);
        
        // ========= Equivalent code (if UniqueLocks are needed, e.g. for condition variables) =========
        // custom::unique_lock<custom::mutex> lk1(e1._Mutex, custom::defer_lock);
		// custom::unique_lock<custom::mutex> lk2(e2._Mutex, custom::defer_lock);
		// custom::lock(lk1, lk2);

        // ========= Superior solution available in C++17 =========
        custom::scoped_lock slk(e1._Mutex, e2._Mutex);

        {
            custom::lock_guard<custom::mutex> lk(io_mutex);
            std::cout << e1._ID << " and " << e2._ID << " got locks" << std::endl;
        }

        e1._LunchPartners.push_back(e2._ID);
        e2._LunchPartners.push_back(e1._ID);
    }

    _Employee::send_mail(e1, e2);
    _Employee::send_mail(e2, e1);
}

// actual thread test functions
void lock_locks_test()
{
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

void thread_test()
{
	std::cout << custom::this_thread::get_id();
	custom::thread t(test::deque_test);
	t.join();
}

void semaphore_test()
{
    auto semaphore_test_task = [](int id, custom::counting_semaphore<>& sem)
    {
        // custom::chrono::system_clock::now() + custom::chrono::Milliseconds(2000);
        // custom::chrono::Milliseconds(2000);

        if (sem.try_acquire_for(custom::chrono::milliseconds(2000)))
        {
            // If the semaphore is acquired within the timeout
            std::cout << "Task " << id << " acquired the semaphore." << std::endl;
            custom::this_thread::sleep_for(custom::chrono::milliseconds(5000)); // Simulating work
            sem.release(); // Release the semaphore after work is done
            std::cout << "Task " << id << " released the semaphore." << std::endl;
        }
        else
        {
            // If unable to acquire the semaphore within the timeout
            std::cout << "Task " << id << " couldn't acquire the semaphore within the timeout." << std::endl;
        }
    };  // semaphore_test_task

	const int numTasks = 3;
    custom::counting_semaphore<> semaphore(2); // Initialize semaphore with a count of 2
    custom::thread tasks[numTasks];
    
    // Start multiple tasks that attempt to acquire the semaphore
    for (int i = 0; i < numTasks; ++i)
        tasks[i] = custom::thread(semaphore_test_task, i + 1, custom::ref(semaphore));

    // Join all threads to the main thread
    for (int i = 0; i < numTasks; ++i)
        tasks[i].join();
}

void timed_mutex_test()
{
    auto timed_mutex_test_task = [](int id, custom::timed_mutex& mtx)
    {
        // Try to acquire the timed mutex for 3 seconds
        if (mtx.try_lock_for(custom::chrono::seconds(3)))
        {
            std::cout << "thread " << id << " acquired the timed mutex." << std::endl;
            // Simulating some work
            custom::this_thread::sleep_for(custom::chrono::seconds(2));
            // Release the mutex
            mtx.unlock();
            std::cout << "thread " << id << " released the timed mutex." << std::endl;
        }
        else
            std::cout << "thread " << id << " couldn't acquire the timed mutex within 3 seconds." << std::endl;
    };  // timed_mutex_test_task

    custom::timed_mutex mtx;
    custom::thread t1(timed_mutex_test_task, 1, custom::ref(mtx));
    custom::thread t2(timed_mutex_test_task, 2, custom::ref(mtx));

    t1.join();
    t2.join();
}

void recursive_timed_mutex_test()
{
    auto recursive_timed_mutex_test_task = [] ( int id, int depth,
                                                custom::recursive_timed_mutex& rmtx,
                                                auto&& recursive_timed_mutex_test_task
                                                /* for recursive lambda */)
    {
        if (depth <= 0)
            return;

        // Try to acquire the recursive timed mutex for 3 seconds
        if (rmtx.try_lock_for(custom::chrono::seconds(3)))
        {
            std::cout << "thread " << id << " acquired the recursive timed mutex." << std::endl;

            // Simulating some work
            custom::this_thread::sleep_for(custom::chrono::seconds(1));

            // Call the function recursively with a decreased depth
            recursive_timed_mutex_test_task(id, depth - 1, rmtx, recursive_timed_mutex_test_task);
            
            // Release the mutex
            rmtx.unlock();
            std::cout << "thread " << id << " released the recursive timed mutex." << std::endl;
        }
        else
            std::cout << "thread " << id << " couldn't acquire the recursive timed mutex within 3 seconds." << std::endl;
    };  // recursive_timed_mutex_test_task

	custom::recursive_timed_mutex rmtx;
    custom::thread t1(recursive_timed_mutex_test_task, 1, 3, custom::ref(rmtx), recursive_timed_mutex_test_task);
    custom::thread t2(recursive_timed_mutex_test_task, 2, 2, custom::ref(rmtx), recursive_timed_mutex_test_task);

    t1.join();
    t2.join();
}

void condition_variable_any_test()
{
    auto cva_producer = [](custom::condition_variable_any& cva, custom::mutex& mtx, bool& dataReady)
    {
        std::cout << "Producer is working..." << std::endl;
        custom::this_thread::sleep_for(custom::chrono::seconds(2)); // Simulating some work
        
        {
            custom::lock_guard<custom::mutex> lock(mtx);
            dataReady = true;
        }

        std::cout << "Producer: Data is ready!" << std::endl;
        cva.notify_one(); // Notify the consumer that data is ready
    };  // cva_producer

    auto cva_consumer = [](custom::condition_variable_any& cva, custom::mutex& mtx, bool& dataReady)
    {
        std::cout << "Consumer is waiting..." << std::endl;
        custom::unique_lock<custom::mutex> lock(mtx);

#if 1   // switch 0/1 for different example
        cva.wait(lock, [&dataReady] { return dataReady; });
        std::cout << "Consumer: Data can be used!" << std::endl;    // Process the data
#else
        if (cva.wait_for(lock, custom::chrono::Seconds(3), [&dataReady] { return dataReady; })) // Data is available within 3 seconds
            std::cout << "Consumer: Data can be used!" << std::endl;
        else    // Timeout occurred
            std::cout << "Consumer: Timeout! Data is not ready within 3 seconds." << std::endl;
#endif  // 0/1
    };  // cva_consumer

    custom::condition_variable_any cva;
    custom::mutex mtx;
    bool dataReady = false;

    custom::thread producerThread(cva_producer, custom::ref(cva), custom::ref(mtx), custom::ref(dataReady));
    custom::thread consumerThread(cva_consumer, custom::ref(cva), custom::ref(mtx), custom::ref(dataReady));

    producerThread.join();
    consumerThread.join();
}

void shared_mutex_test()
{
    int sharedData      = 0;        // critical resource
    int storedData[40]  = {-1};     // output ("safe" to share)

    auto writer = [&](custom::shared_mutex& smtx)
    {
        custom::lock_guard<custom::shared_mutex> lock(smtx);  // exclusive
        ++sharedData;                                       // modify data
    };  // writer

    auto reader = [&](custom::shared_mutex& smtx, int current)
    {
        custom::shared_lock<custom::shared_mutex> lock(smtx); // shared
        storedData[current] = sharedData;                   // read data
        //custom::this_thread::sleep_for(custom::chrono::Milliseconds(100));
    };  // reader

    custom::vector<custom::thread> threads;
    custom::shared_mutex smtx;
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

void shared_timed_mutex_test()
{
    int sharedData      = 0;        // critical resource
    int storedData[40]  = {-1};     // output ("safe" to share)

    auto writer = [&](custom::shared_timed_mutex& stmtx)
    {
        custom::unique_lock<custom::shared_timed_mutex> lock(stmtx, custom::chrono::seconds(2));  // exclusive (try_lock_for)
        ++sharedData;                                       // modify data
    };  // writer

    auto reader = [&](custom::shared_timed_mutex& stmtx, int current)
    {
        custom::shared_lock<custom::shared_timed_mutex> lock(stmtx, custom::chrono::seconds(1)); // shared (try_lock_shared_for)
        storedData[current] = sharedData;                   // read data
        custom::this_thread::sleep_for(custom::chrono::milliseconds(2000));
    };  // reader

    custom::vector<custom::thread> threads;
    custom::shared_timed_mutex stmtx;
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