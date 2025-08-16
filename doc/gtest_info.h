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