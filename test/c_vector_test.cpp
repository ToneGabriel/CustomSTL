#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <utility>

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
    this->_custom_vector_instance = {TypeParam(), TypeParam(), TypeParam()};

    EXPECT_EQ(this->_custom_vector_instance.size(), 3);
    EXPECT_EQ(this->_custom_vector_instance.front(), TypeParam());
}

// Register test cases to suite
REGISTER_TYPED_TEST_SUITE_P(
                                VectorTypeTestFixture,
                                construct_default_values_from_initializer_list
                            );

// Create test cases based on types
using VectorTestTypes = ::testing::Types<int, double, std::string>;
INSTANTIATE_TYPED_TEST_SUITE_P(
                                VectorTypeTestSuite,
                                VectorTypeTestFixture,
                                VectorTestTypes
                            );

// Value dependent tests
// =====================================================================================================

class VectorValueTestFixture : public ::testing::TestWithParam<std::tuple<int, double, std::string>>
{
protected:
    custom::vector<ParamType> _custom_vector_instance;

protected:

    void SetUp() override
    {
        _custom_vector_instance = {ParamType(), ParamType(), ParamType()};
    }

    void TearDown() override
    {
        _custom_vector_instance.clear();  // Clear the vector after each test
    }
};  // END VectorValueTestFixture

TEST_P(VectorValueTestFixture, emplace_back)
{
    auto tuple_param = GetParam();
    auto prev_vector_size = this->_custom_vector_instance.size();

    this->_custom_vector_instance.emplace_back( std::get<0>(tuple_param),
                                                std::get<1>(tuple_param),
                                                std::get<2>(tuple_param));

    EXPECT_EQ(this->_custom_vector_instance.size(), prev_vector_size + 1);
    EXPECT_EQ(this->_custom_vector_instance.back(), tuple_param);
}   // END emplace_back

TEST_P(VectorValueTestFixture, clear)
{
    this->_custom_vector_instance.clear();

    EXPECT_EQ(this->_custom_vector_instance.size(), 0);
    EXPECT_TRUE(this->_custom_vector_instance.empty());
}

INSTANTIATE_TEST_SUITE_P(
                            VectorValueTestSuite,
                            VectorValueTestFixture,
                            ::testing::Values   (
                                                    std::make_tuple(3, 3.5, "hello")
                                                )   // can add more values
                        );
