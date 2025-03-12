#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <utility>

#include <c_thread.h>   // unit to be tested

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

TEST_F(ThreadTestFixture, default_thread)
{
    EXPECT_TRUE(this->_custom_thread_instance_1.joinable() && this->_custom_thread_instance_2.joinable());
    EXPECT_TRUE(this->_custom_thread_instance_1 != this->_custom_thread_instance_2);
}

TEST_F(ThreadTestFixture, throw_on_join_after_detach_thread)
{
    this->_custom_thread_instance_1.detach();
    this->_custom_thread_instance_2.detach();

    EXPECT_THROW(this->_custom_thread_instance_1.join(), std::runtime_error);
    EXPECT_THROW(this->_custom_thread_instance_2.join(), std::runtime_error);
}