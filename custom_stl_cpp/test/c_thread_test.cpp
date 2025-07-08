#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "stl_cpp/c_algorithm.h"
#include "stl_cpp/c_chrono.h"
#include "stl_cpp/c_thread.h"   // unit to be tested

// custom::thread and custom::this_thread tests
// =============================================================================

class ThreadTestFixture : public ::testing::Test
{
protected:
    custom::thread _custom_thread_instance_1;
    custom::thread _custom_thread_instance_2;

protected:

    void SetUp() override
    {
        _custom_thread_instance_1 = custom::move(custom::thread(_test_thread_func));
        _custom_thread_instance_2 = custom::move(custom::thread(_test_thread_func));
    }

    void TearDown() override
    {
        if (_custom_thread_instance_1.joinable())
            _custom_thread_instance_1.join();

        if (_custom_thread_instance_2.joinable())
            _custom_thread_instance_2.join();
    }

private:

    static void _test_thread_func() { /*Empty*/ }
};  // ThreadTestFixture

TEST_F(ThreadTestFixture, different_thread_id)
{
    EXPECT_NE(  this->_custom_thread_instance_1.get_id(),
                this->_custom_thread_instance_2.get_id());
}

TEST_F(ThreadTestFixture, joinable_after_init)
{
    EXPECT_TRUE(this->_custom_thread_instance_1.joinable());
    EXPECT_TRUE(this->_custom_thread_instance_2.joinable());
}

TEST_F(ThreadTestFixture, throw_on_join_after_detach)
{
    this->_custom_thread_instance_1.detach();
    this->_custom_thread_instance_2.detach();

    EXPECT_FALSE(this->_custom_thread_instance_1.joinable());
    EXPECT_FALSE(this->_custom_thread_instance_2.joinable());

    EXPECT_THROW(this->_custom_thread_instance_1.join(), std::system_error);
    EXPECT_THROW(this->_custom_thread_instance_2.join(), std::system_error);
}

TEST_F(ThreadTestFixture, throw_on_join_after_join)
{
    this->_custom_thread_instance_1.join();
    this->_custom_thread_instance_2.join();

    EXPECT_FALSE(this->_custom_thread_instance_1.joinable());
    EXPECT_FALSE(this->_custom_thread_instance_2.joinable());

    EXPECT_THROW(this->_custom_thread_instance_1.join(), std::system_error);
    EXPECT_THROW(this->_custom_thread_instance_2.join(), std::system_error);
}

TEST_F(ThreadTestFixture, throw_on_detach_after_detach)
{
    this->_custom_thread_instance_1.detach();
    this->_custom_thread_instance_2.detach();

    EXPECT_FALSE(this->_custom_thread_instance_1.joinable());
    EXPECT_FALSE(this->_custom_thread_instance_2.joinable());

    EXPECT_THROW(this->_custom_thread_instance_1.detach(), std::system_error);
    EXPECT_THROW(this->_custom_thread_instance_2.detach(), std::system_error);
}

TEST_F(ThreadTestFixture, throw_on_detach_after_join)
{
    this->_custom_thread_instance_1.join();
    this->_custom_thread_instance_2.join();

    EXPECT_FALSE(this->_custom_thread_instance_1.joinable());
    EXPECT_FALSE(this->_custom_thread_instance_2.joinable());

    EXPECT_THROW(this->_custom_thread_instance_1.detach(), std::system_error);
    EXPECT_THROW(this->_custom_thread_instance_2.detach(), std::system_error);
}

TEST_F(ThreadTestFixture, no_throw_on_self_assignment)
{
    // nothing happens
    EXPECT_NO_THROW(this->_custom_thread_instance_1 =
                    custom::move(this->_custom_thread_instance_1));
}

TEST_F(ThreadTestFixture, no_throw_ownership_transfer_on_move_assignment)
{
    custom::thread::id initial_t2_id = this->_custom_thread_instance_2.get_id();
    this->_custom_thread_instance_1.join();

    EXPECT_FALSE(this->_custom_thread_instance_1.joinable());

    EXPECT_NO_THROW(this->_custom_thread_instance_1 =
                    custom::move(this->_custom_thread_instance_2));

    EXPECT_TRUE(this->_custom_thread_instance_1.joinable());
    EXPECT_FALSE(this->_custom_thread_instance_2.joinable());

    EXPECT_EQ(this->_custom_thread_instance_1.get_id(), initial_t2_id);
    EXPECT_EQ(this->_custom_thread_instance_2.get_id(), custom::thread::id());
}

TEST_F(ThreadTestFixture, death_on_move_assignment_when_joinable)
{
    EXPECT_TRUE(this->_custom_thread_instance_1.joinable());
    EXPECT_TRUE(this->_custom_thread_instance_2.joinable());

    EXPECT_DEATH_IF_SUPPORTED(  this->_custom_thread_instance_1 =
                                custom::move(this->_custom_thread_instance_2),
                                "");
}

TEST_F(ThreadTestFixture, get_id)
{
    this->_custom_thread_instance_1.join();

    EXPECT_EQ(this->_custom_thread_instance_1.get_id(), custom::thread::id());
    EXPECT_NE(this->_custom_thread_instance_2.get_id(), custom::thread::id());
}

TEST_F(ThreadTestFixture, this_thread_get_id)
{
    EXPECT_NE(custom::this_thread::get_id(), custom::thread::id());
    EXPECT_NE(custom::this_thread::get_id(), this->_custom_thread_instance_1.get_id());
    EXPECT_NE(custom::this_thread::get_id(), this->_custom_thread_instance_2.get_id());
}

// Thread time tests
// =============================================================================

class ThisThreadTimeTestFixture : public ::testing::TestWithParam<int>
{
protected:
    using _clock = typename custom::chrono::steady_clock;
    using _milli = typename custom::chrono::milliseconds;

    static constexpr int _MS_TOLERANCE = 20;
};  // END ThisThreadTimeTestFixture

TEST_P(ThisThreadTimeTestFixture, sleep_for_duration_ms)
{
    // if time is <= 0 then it sleeps for 0
    int ms_to_sleep = GetParam();
    int ms_to_check = (custom::max)(0, ms_to_sleep);

    auto t1 = _clock::now();
    custom::this_thread::sleep_for(_milli(ms_to_sleep));
    auto t2 = _clock::now();

    auto duration = custom::chrono::duration_cast<_milli>(t2 - t1);
    EXPECT_GE(duration.count(), ms_to_check - _MS_TOLERANCE);
    EXPECT_LE(duration.count(), ms_to_check + _MS_TOLERANCE);
}

TEST_P(ThisThreadTimeTestFixture, sleep_until_duration_ms)
{
    // if time is <= 0 then it sleeps for 0
    int ms_to_sleep = GetParam();
    int ms_to_check = (custom::max)(0, ms_to_sleep);

    auto t1 = _clock::now();
    custom::this_thread::sleep_until(t1 + _milli(ms_to_sleep));
    auto t2 = _clock::now();

    auto duration = custom::chrono::duration_cast<_milli>(t2 - t1);
    EXPECT_GE(duration.count(), ms_to_check - _MS_TOLERANCE);
    EXPECT_LE(duration.count(), ms_to_check + _MS_TOLERANCE);
}

INSTANTIATE_TEST_SUITE_P(
                            ThisThreadTimeTestSuite,
                            ThisThreadTimeTestFixture,
                            ::testing::Values(
                                                300,
                                                0,
                                                -300
                                            )
                        );
