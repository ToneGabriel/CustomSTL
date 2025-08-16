#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "custom/string.h"
#include "custom/tuple.h"
#include "custom/vector.h"   // unit to be tested



TEST(VectorInitTests, default_init)
{
    custom::vector<int> custom_vector_instance;
    EXPECT_EQ(custom_vector_instance.size(), 0);
    EXPECT_EQ(custom_vector_instance.capacity(), 8);    // default capacity
}


TEST(VectorInitTests, custom_capacity_default_value_init)
{
    int custom_capacity = 5;
    auto custom_vector_instance = custom::vector<int>(custom_capacity);

    EXPECT_EQ(custom_vector_instance.size(), custom_capacity);
    EXPECT_EQ(custom_vector_instance.capacity(), custom_capacity);
    EXPECT_EQ(custom_vector_instance.back(), int());    // has default values
}


TEST(VectorInitTests, custom_capacity_given_value_init)
{
    int custom_capacity = 5;
    int custom_value = 10;
    auto custom_vector_instance = custom::vector<int>(custom_capacity, custom_value);

    EXPECT_EQ(custom_vector_instance.size(), custom_capacity);
    EXPECT_EQ(custom_vector_instance.capacity(), custom_capacity);
    EXPECT_EQ(custom_vector_instance.back(), custom_value); // has given values
}


TEST(VectorInitTests, copy_init)
{
    custom::vector<int> custom_vector_instance {0, 1, 2, 3};
    custom::vector<int> custom_vector_instance_copy = custom_vector_instance;
    EXPECT_TRUE(custom_vector_instance == custom_vector_instance_copy);
}


TEST(VectorInitTests, move_init)
{
    custom::vector<int> custom_vector_instance {0, 1, 2, 3};
    custom::vector<int> custom_vector_instance_copy = custom_vector_instance;
    custom::vector<int> other = custom::move(custom_vector_instance);

    EXPECT_FALSE(other == custom_vector_instance);      // the original was moved
    EXPECT_TRUE(other == custom_vector_instance_copy);  // equal to original copy
}


// ===========================================================================================


class VectorTestFixture : public ::testing::Test
{
protected:
    custom::vector<int> _custom_vector_instance;

protected:

    void SetUp() override
    {
        _custom_vector_instance.push_back(1);
        _custom_vector_instance.push_back(2);
        _custom_vector_instance.push_back(3);
    }

    void TearDown() override
    {
        // empty
    }
};  // END VectorTestFixture


TEST_F(VectorTestFixture, copy_assign_operator)
{
    custom::vector<int> other;

    EXPECT_FALSE(this->_custom_vector_instance == other);
    other = this->_custom_vector_instance;
    EXPECT_TRUE(this->_custom_vector_instance == other);
}


TEST_F(VectorTestFixture, move_assign_operator)
{
    custom::vector<int> other = {0, 0, 0, 0};
    custom::vector<int> other_copy = other;

    EXPECT_FALSE(this->_custom_vector_instance == other);
    this->_custom_vector_instance = custom::move(other);
    EXPECT_FALSE(this->_custom_vector_instance == other);       // still different
    EXPECT_TRUE(this->_custom_vector_instance == other_copy);   // equal to original
}


TEST_F(VectorTestFixture, reserve_more_than_size)
{
    int custom_capacity = 5;
    int back_value = this->_custom_vector_instance.back();

    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 8); // default

    this->_custom_vector_instance.reserve(custom_capacity);

    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), custom_capacity);
    EXPECT_EQ(this->_custom_vector_instance.back(), back_value);
}


TEST_F(VectorTestFixture, reserve_less_than_size)
{
    int custom_capacity = 2;
    int back_value = this->_custom_vector_instance.back();

    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 8); // default

    this->_custom_vector_instance.reserve(custom_capacity);

    EXPECT_EQ(this->_custom_vector_instance.size(), custom_capacity);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), custom_capacity);
    EXPECT_NE(this->_custom_vector_instance.back(), back_value);
}


TEST_F(VectorTestFixture, realloc_default)
{
    int custom_capacity = 5;

    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 8); // default

    this->_custom_vector_instance.realloc(custom_capacity);

    EXPECT_EQ(this->_custom_vector_instance.size(), custom_capacity);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), custom_capacity);
    EXPECT_EQ(this->_custom_vector_instance.back(), int());
}


TEST_F(VectorTestFixture, realloc_copy)
{
    int custom_capacity = 5;
    int custom_value = 10;

    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 8); // default

    this->_custom_vector_instance.realloc(custom_capacity, custom_value);

    EXPECT_EQ(this->_custom_vector_instance.size(), custom_capacity);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), custom_capacity);
    EXPECT_EQ(this->_custom_vector_instance.back(), custom_value);
}


TEST_F(VectorTestFixture, resize_less_than_size)
{
    int custom_size = 2;

    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 8); // default

    this->_custom_vector_instance.resize(custom_size);

    EXPECT_EQ(this->_custom_vector_instance.size(), custom_size);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 8); // default
}


TEST_F(VectorTestFixture, resize_more_than_size_default)
{
    int custom_size = 4;

    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 8); // default

    this->_custom_vector_instance.resize(custom_size);

    EXPECT_EQ(this->_custom_vector_instance.size(), custom_size);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 8); // default
    EXPECT_EQ(this->_custom_vector_instance.back(), int());
}


TEST_F(VectorTestFixture, resize_more_than_size_copy)
{
    int custom_size = 4;
    int custom_value = 10;

    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 8); // default

    this->_custom_vector_instance.resize(custom_size, custom_value);

    EXPECT_EQ(this->_custom_vector_instance.size(), custom_size);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 8); // default
    EXPECT_EQ(this->_custom_vector_instance.back(), custom_value);
}


TEST_F(VectorTestFixture, shrink_to_fit)
{
    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 8); // default

    this->_custom_vector_instance.shrink_to_fit();

    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 3);
}


TEST_F(VectorTestFixture, push_back_copy)
{
    auto custom_value = 111;
    auto prev_vector_size = this->_custom_vector_instance.size();

    this->_custom_vector_instance.emplace_back(custom_value);

    EXPECT_EQ(this->_custom_vector_instance.size(), prev_vector_size + 1);
    EXPECT_EQ(this->_custom_vector_instance.back(), custom_value);
}


TEST_F(VectorTestFixture, pop_back)
{
    EXPECT_EQ(this->_custom_vector_instance.size(), 3);

    this->_custom_vector_instance.pop_back();

    EXPECT_EQ(this->_custom_vector_instance.size(), 2);
}


TEST_F(VectorTestFixture, clear)
{
    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 8);
    EXPECT_FALSE(this->_custom_vector_instance.empty());

    this->_custom_vector_instance.clear();

    EXPECT_EQ(this->_custom_vector_instance.size(), 0);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 8);
    EXPECT_TRUE(this->_custom_vector_instance.empty());
}


TEST_F(VectorTestFixture, at_index)
{
    EXPECT_EQ(this->_custom_vector_instance.at(0), 1);
}


TEST_F(VectorTestFixture, at_index_throw)
{
    this->_custom_vector_instance.clear();
    EXPECT_TRUE(this->_custom_vector_instance.empty());
    EXPECT_THROW(this->_custom_vector_instance.at(0), std::out_of_range);
}


TEST_F(VectorTestFixture, operator_index)
{
    EXPECT_EQ(this->_custom_vector_instance[0], 1);
}


TEST_F(VectorTestFixture, operator_index_death)
{
    this->_custom_vector_instance.clear();
    EXPECT_TRUE(this->_custom_vector_instance.empty());
    EXPECT_DEATH_IF_SUPPORTED(this->_custom_vector_instance[0], "");
}
