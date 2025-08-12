#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "custom/c_vector.h"


DEFINE_GENERIC_VECTOR(
    VectorINT,
    int,
    DEFAULT_TYPE_REF_EQUALS,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_DELETE
)


class CVectorValueTestFixture : public ::testing::TestWithParam<int>
{
protected:
    VectorINT _custom_vector_instance;

    static constexpr size_t _DEFAULT_INIT_CAPACITY = 10;

protected:

    void SetUp() override
    {
        _custom_vector_instance = VectorINT_create(_DEFAULT_INIT_CAPACITY);
    }

    void TearDown() override
    {
        VectorINT_destroy(&_custom_vector_instance);
    }
};  // END CVectorValueTestFixture


TEST_F(CVectorValueTestFixture, default_create)
{
    EXPECT_EQ(VectorINT_size(&this->_custom_vector_instance), 0);
    EXPECT_EQ(VectorINT_capacity(&this->_custom_vector_instance), _DEFAULT_INIT_CAPACITY);
    EXPECT_TRUE(VectorINT_empty(&this->_custom_vector_instance));
}

TEST_F(CVectorValueTestFixture, copy)
{
    int val = 0;
    VectorINT_push_back(&this->_custom_vector_instance, &val);
    VectorINT_push_back(&this->_custom_vector_instance, &val);
    VectorINT_push_back(&this->_custom_vector_instance, &val);

    VectorINT other = VectorINT_create(_DEFAULT_INIT_CAPACITY);
    EXPECT_FALSE(VectorINT_equals(&other, &this->_custom_vector_instance));

    VectorINT_copy(&other, &this->_custom_vector_instance);
    EXPECT_TRUE(VectorINT_equals(&other, &this->_custom_vector_instance));
}

TEST_F(CVectorValueTestFixture, move)
{
    int val = 0;
    VectorINT_push_back(&this->_custom_vector_instance, &val);
    VectorINT_push_back(&this->_custom_vector_instance, &val);
    VectorINT_push_back(&this->_custom_vector_instance, &val);

    VectorINT original_copy = VectorINT_create(1);
    VectorINT_copy(&original_copy, &this->_custom_vector_instance);

    VectorINT other = VectorINT_create(_DEFAULT_INIT_CAPACITY);
    EXPECT_FALSE(VectorINT_equals(&other, &this->_custom_vector_instance));

    VectorINT_move(&other, &this->_custom_vector_instance);
    EXPECT_FALSE(VectorINT_equals(&other, &this->_custom_vector_instance));
    EXPECT_TRUE(VectorINT_equals(&other, &original_copy));
}