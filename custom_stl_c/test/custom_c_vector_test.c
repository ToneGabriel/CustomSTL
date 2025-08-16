#include "unity.h"
#include "custom/c_vector.h"


// Setup ==========================


DEFINE_GENERIC_VECTOR(
    VectorINT,
    int,
    DEFAULT_TYPE_REF_EQUALS,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_DELETE
)


static VectorINT _custom_vector_instance;
static const size_t _DEFAULT_INIT_CAPACITY = 10;


void setUp()
{
    _custom_vector_instance = VectorINT_create(_DEFAULT_INIT_CAPACITY);
}

void tearDown()
{
    VectorINT_destroy(&_custom_vector_instance);
}


// Tests ==========================


void test_default_create()
{
    TEST_ASSERT_EQUAL_UINT(0, VectorINT_size(&_custom_vector_instance));
    TEST_ASSERT_EQUAL_UINT(_DEFAULT_INIT_CAPACITY, VectorINT_capacity(&_custom_vector_instance));
    TEST_ASSERT_TRUE(VectorINT_empty(&_custom_vector_instance));
}

void test_copy()
{
    int val = 0;
    VectorINT_push_back(&_custom_vector_instance, &val);
    VectorINT_push_back(&_custom_vector_instance, &val);
    VectorINT_push_back(&_custom_vector_instance, &val);

    VectorINT other = VectorINT_create(_DEFAULT_INIT_CAPACITY);
    TEST_ASSERT_FALSE(VectorINT_equals(&other, &_custom_vector_instance));

    VectorINT_copy(&other, &_custom_vector_instance);
    TEST_ASSERT_TRUE(VectorINT_equals(&other, &_custom_vector_instance));

    VectorINT_destroy(&other);
}

void test_move()
{
    int val = 0;
    VectorINT_push_back(&_custom_vector_instance, &val);
    VectorINT_push_back(&_custom_vector_instance, &val);
    VectorINT_push_back(&_custom_vector_instance, &val);

    VectorINT original_copy = VectorINT_create(1);
    VectorINT_copy(&original_copy, &_custom_vector_instance);

    VectorINT other = VectorINT_create(_DEFAULT_INIT_CAPACITY);
    TEST_ASSERT_FALSE(VectorINT_equals(&other, &_custom_vector_instance));

    VectorINT_move(&other, &_custom_vector_instance);
    TEST_ASSERT_FALSE(VectorINT_equals(&other, &_custom_vector_instance));
    TEST_ASSERT_TRUE(VectorINT_equals(&other, &original_copy));

    VectorINT_destroy(&other);
    VectorINT_destroy(&original_copy);
}


// RUN ==========================

int main()
{
    UNITY_BEGIN();
    
    RUN_TEST(test_default_create);
    RUN_TEST(test_copy);
    RUN_TEST(test_move);

    return UNITY_END();
}
