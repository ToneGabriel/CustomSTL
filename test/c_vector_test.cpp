#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <c_string.h>
#include <c_tuple.h>
#include <c_vector.h>   // unit to be tested

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
    this->_custom_vector_instance = {val, val, val};

    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance[0], val);
    EXPECT_EQ(this->_custom_vector_instance[1], val);
    EXPECT_EQ(this->_custom_vector_instance[2], val);
}

// Register test cases to suite
REGISTER_TYPED_TEST_SUITE_P(
                                VectorTypeTestFixture,
                                construct_default_values_from_initializer_list
                            );

// Create test cases based on types
using VectorTestTypes = ::testing::Types<int, double, custom::string>;
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

protected:

    void TearDown() override
    {
        _custom_vector_instance.clear();  // Clear the vector after each test
    }
};  // END VectorValueTestFixture

TEST_P(VectorValueTestFixture, default_init)
{
    EXPECT_EQ(this->_custom_vector_instance.size(), 0);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 8); // default capacity
}

TEST_P(VectorValueTestFixture, custom_capacity_default_value_init)
{
    this->_custom_vector_instance = custom::vector<ParamType>(3);

    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 3); // custom capacity

    EXPECT_EQ(this->_custom_vector_instance.back(), ParamType());   // has default values
}

TEST_P(VectorValueTestFixture, custom_capacity_given_value_init)
{
    this->_custom_vector_instance = custom::vector<ParamType>(3, GetParam());

    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 3); // custom capacity

    EXPECT_EQ(this->_custom_vector_instance.back(), GetParam());   // has given values
}

TEST_P(VectorValueTestFixture, initializer_list_init)
{
    this->_custom_vector_instance = {GetParam(), GetParam(), GetParam()};

    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 3); // custom capacity
    EXPECT_EQ(this->_custom_vector_instance.back(), GetParam());   // has given values
}

TEST_P(VectorValueTestFixture, copy_assign_operator)
{
    custom::vector<ParamType> other = {GetParam(), GetParam(), GetParam()};

    EXPECT_FALSE(this->_custom_vector_instance.size() == other.size());
    EXPECT_FALSE(this->_custom_vector_instance.capacity() == other.capacity());
    EXPECT_FALSE(this->_custom_vector_instance == other);

    this->_custom_vector_instance = other;

    EXPECT_TRUE(this->_custom_vector_instance.size() == other.size());
    EXPECT_TRUE(this->_custom_vector_instance.capacity() == other.capacity());
    EXPECT_TRUE(this->_custom_vector_instance == other);
}

TEST_P(VectorValueTestFixture, move_assign_operator)
{
    custom::vector<ParamType> other = {GetParam(), GetParam(), GetParam()};

    EXPECT_EQ(this->_custom_vector_instance.size(), 0);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 8); // default capacity
    
    EXPECT_EQ(other.size(), 3);
    EXPECT_EQ(other.capacity(), 3); // custom capacity
    EXPECT_EQ(other.back(), GetParam());

    this->_custom_vector_instance = custom::move(other);

    EXPECT_EQ(other.size(), 0);
    EXPECT_EQ(other.capacity(), 0);
    
    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance.capacity(), 3);
    EXPECT_EQ(this->_custom_vector_instance.back(), GetParam());
}

TEST_P(VectorValueTestFixture, reserve_less)
{
    // TODO
}

TEST_P(VectorValueTestFixture, reserve_more)
{
    // TODO
}

TEST_P(VectorValueTestFixture, shrink_to_fit)
{
    // TODO
}

TEST_P(VectorValueTestFixture, realloc_default)
{
    // TODO
}

TEST_P(VectorValueTestFixture, realloc_value)
{
    // TODO
}

TEST_P(VectorValueTestFixture, resize_less)
{
    // TODO
}

TEST_P(VectorValueTestFixture, resize_more_default)
{
    // TODO
}

TEST_P(VectorValueTestFixture, resize_more_value)
{
    // TODO
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
}   // END emplace_back

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
    // TODO
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
    // TODO
}

TEST_P(VectorValueTestFixture, at_index_throw)
{
    // TODO
}

TEST_P(VectorValueTestFixture, operator_index)
{
    // TODO
}

TEST_P(VectorValueTestFixture, operator_index_death)
{
    // TODO
}

INSTANTIATE_TEST_SUITE_P(
                            VectorValueTestSuite,
                            VectorValueTestFixture,
                            ::testing::Values   (
                                                    custom::make_tuple(3, 3.5, "hello")
                                                )   // can add more values
                        );
