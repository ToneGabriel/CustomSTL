#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "custom/string.h"
#include "custom/tuple.h"
#include "custom/vector.h"   // unit to be tested

// Type dependent tests
// =====================================================================================================

// Define the test fixture class
template<class Type>
class VectorTypeTestFixture : public ::testing::Test
{
protected:
    custom::vector<Type> _custom_vector_instance;
};  // END VectorTypeTestFixture

// Declare test suite for template type
TYPED_TEST_SUITE_P(VectorTypeTestFixture);

// Create test cases
TYPED_TEST_P(VectorTypeTestFixture, construct_default_values_from_initializer_list)
{
    TypeParam val;
    this->_custom_vector_instance = {val};

    EXPECT_EQ(this->_custom_vector_instance.size(), 1);
    EXPECT_EQ(this->_custom_vector_instance.back(), val);
}

// Register test cases to suite
REGISTER_TYPED_TEST_SUITE_P(
                                VectorTypeTestFixture,
                                construct_default_values_from_initializer_list
                            );

// Create test cases based on types
using VectorTestTypes = ::testing::Types<int, custom::string>;
INSTANTIATE_TYPED_TEST_SUITE_P(
                                VectorTypeTestSuite,
                                VectorTypeTestFixture,
                                VectorTestTypes
                            );

// Value dependent tests
// =====================================================================================================

class VectorValueTestFixture : public ::testing::TestWithParam<custom::tuple<int, double, custom::string>>
{
protected:
    custom::vector<ParamType> _custom_vector_instance;
};  // END VectorValueTestFixture

TEST_P(VectorValueTestFixture, default_init)
{
    EXPECT_EQ(this->_custom_vector_instance.size(), 0);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 8); // default capacity
}

TEST_P(VectorValueTestFixture, custom_capacity_default_value_init)
{
    this->_custom_vector_instance = custom::vector<ParamType>(1);

    EXPECT_EQ(this->_custom_vector_instance.size(), 1);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 1); // custom capacity
    EXPECT_EQ(this->_custom_vector_instance.back(), ParamType());   // has default values
}

TEST_P(VectorValueTestFixture, custom_capacity_given_value_init)
{
    this->_custom_vector_instance = custom::vector<ParamType>(1, GetParam());

    EXPECT_EQ(this->_custom_vector_instance.size(), 1);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 1); // custom capacity
    EXPECT_EQ(this->_custom_vector_instance.back(), GetParam());   // has given values
}

TEST_P(VectorValueTestFixture, copy_assign_operator)
{
    custom::vector<ParamType> other = {GetParam(), GetParam(), GetParam()};

    EXPECT_FALSE(this->_custom_vector_instance == other);
    this->_custom_vector_instance = other;
    EXPECT_TRUE(this->_custom_vector_instance == other);
}

TEST_P(VectorValueTestFixture, move_assign_operator)
{
    custom::vector<ParamType> other = {GetParam(), GetParam(), GetParam()};
    custom::vector<ParamType> other_copy = other;

    EXPECT_FALSE(this->_custom_vector_instance == other);
    this->_custom_vector_instance = custom::move(other);
    EXPECT_FALSE(this->_custom_vector_instance == other);   // still different
    EXPECT_TRUE(this->_custom_vector_instance == other_copy);   // equal to original
}

TEST_P(VectorValueTestFixture, shrink_to_fit)
{
    this->_custom_vector_instance.emplace_back();
    this->_custom_vector_instance.emplace_back();
    this->_custom_vector_instance.emplace_back();

    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 8); // default

    this->_custom_vector_instance.shrink_to_fit();

    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 3);
}

TEST_P(VectorValueTestFixture, emplace_back)
{
    auto tuple_param = GetParam();
    auto prev_vector_size = this->_custom_vector_instance.size();

    this->_custom_vector_instance.emplace_back( custom::get<0>(tuple_param),
                                                custom::get<1>(tuple_param),
                                                custom::get<2>(tuple_param));

    EXPECT_EQ(this->_custom_vector_instance.size(), prev_vector_size + 1);
    EXPECT_EQ(this->_custom_vector_instance.back(), tuple_param);
}

TEST_P(VectorValueTestFixture, push_back_copy)
{
    auto tuple_param = GetParam();
    auto prev_vector_size = this->_custom_vector_instance.size();

    this->_custom_vector_instance.push_back(tuple_param);

    EXPECT_EQ(this->_custom_vector_instance.size(), prev_vector_size + 1);
    EXPECT_EQ(this->_custom_vector_instance.back(), tuple_param);
}

TEST_P(VectorValueTestFixture, push_back_move)
{
    auto tuple_param = GetParam();
    auto moved_tuple_param = GetParam();
    auto prev_vector_size = this->_custom_vector_instance.size();

    this->_custom_vector_instance.push_back(custom::move(moved_tuple_param));

    EXPECT_EQ(this->_custom_vector_instance.size(), prev_vector_size + 1);
    EXPECT_EQ(this->_custom_vector_instance.back(), tuple_param);
    EXPECT_NE(this->_custom_vector_instance.back(), moved_tuple_param);
}

TEST_P(VectorValueTestFixture, pop_back)
{
    this->_custom_vector_instance.emplace_back();
    this->_custom_vector_instance.emplace_back();

    EXPECT_EQ(this->_custom_vector_instance.size(), 2);

    this->_custom_vector_instance.pop_back();

    EXPECT_EQ(this->_custom_vector_instance.size(), 1);
}

TEST_P(VectorValueTestFixture, clear)
{
    this->_custom_vector_instance = {ParamType(), ParamType(), ParamType()};

    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 3);
    EXPECT_FALSE(this->_custom_vector_instance.empty());

    this->_custom_vector_instance.clear();

    EXPECT_EQ(this->_custom_vector_instance.size(), 0);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 3);
    EXPECT_TRUE(this->_custom_vector_instance.empty());
}

TEST_P(VectorValueTestFixture, at_index)
{
    this->_custom_vector_instance.push_back(GetParam());

    EXPECT_EQ(this->_custom_vector_instance.at(0), GetParam());
}

TEST_P(VectorValueTestFixture, at_index_throw)
{
    EXPECT_TRUE(this->_custom_vector_instance.empty());
    EXPECT_THROW(this->_custom_vector_instance.at(0), std::out_of_range);
}

TEST_P(VectorValueTestFixture, operator_index)
{
    this->_custom_vector_instance.push_back(GetParam());

    EXPECT_EQ(this->_custom_vector_instance[0], GetParam());
}

TEST_P(VectorValueTestFixture, operator_index_death)
{
    EXPECT_TRUE(this->_custom_vector_instance.empty());
    EXPECT_DEATH_IF_SUPPORTED(this->_custom_vector_instance[0], "");
}

INSTANTIATE_TEST_SUITE_P(
                            VectorValueTestSuite,
                            VectorValueTestFixture,
                            ::testing::Values   (
                                                    custom::make_tuple(3, 3.5, "hello")
                                                )   // can add more values
                        );
