#include "unity.h"
#include "custom/c_priority_queue.h"


// Setup ==========================


DEFINE_GENERIC_PRIORITY_QUEUE(
    IntPQ,
    int,
    DEFAULT_TYPE_REF_LESS,
    DEFAULT_TYPE_REF_EQUALS,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_DELETE
)


static IntPQ _custom_pq_instance;


void setUp(void)
{
    _custom_pq_instance = IntPQ_create();
}

void tearDown(void)
{
    IntPQ_destroy(&_custom_pq_instance);
}


// Tests ==========================


void test_default_create(void)
{
    TEST_ASSERT_EQUAL_UINT(0, IntPQ_size(&_custom_pq_instance));
    TEST_ASSERT_TRUE(IntPQ_empty(&_custom_pq_instance));
}

void test_copy(void)
{
    int val = 0;
    IntPQ_insert(&_custom_pq_instance, &val);
    IntPQ_insert(&_custom_pq_instance, &val);
    IntPQ_insert(&_custom_pq_instance, &val);

    IntPQ other = IntPQ_create();
    TEST_ASSERT_FALSE(IntPQ_equals(&other, &_custom_pq_instance));

    IntPQ_copy(&other, &_custom_pq_instance);
    TEST_ASSERT_TRUE(IntPQ_equals(&other, &_custom_pq_instance));

    IntPQ_destroy(&other);
}

void test_move(void)
{
    int val = 0;
    IntPQ_insert(&_custom_pq_instance, &val);
    IntPQ_insert(&_custom_pq_instance, &val);
    IntPQ_insert(&_custom_pq_instance, &val);

    IntPQ original_copy = IntPQ_create();
    IntPQ_copy(&original_copy, &_custom_pq_instance);

    IntPQ other = IntPQ_create();
    TEST_ASSERT_FALSE(IntPQ_equals(&other, &_custom_pq_instance));

    IntPQ_move(&other, &_custom_pq_instance);
    TEST_ASSERT_FALSE(IntPQ_equals(&other, &_custom_pq_instance));
    TEST_ASSERT_TRUE(IntPQ_equals(&other, &original_copy));

    IntPQ_destroy(&other);
    IntPQ_destroy(&original_copy);
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
