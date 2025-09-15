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


static IntPQ g_customIntPQInstance;


void setUp()
{
    g_customIntPQInstance = IntPQ_create();
}

void tearDown()
{
    IntPQ_destroy(&g_customIntPQInstance);
}


// Tests ==========================


void test_default_create()
{
    TEST_ASSERT_TRUE_MESSAGE(IntPQ_empty(&g_customIntPQInstance), "PQ should be empty");
    TEST_ASSERT_EQUAL_MESSAGE(0, IntPQ_size(&g_customIntPQInstance), "PQ initial size should be 0");
}

void test_copy()
{
    int val = 0;
    IntPQ_insert(&g_customIntPQInstance, &val);
    IntPQ_insert(&g_customIntPQInstance, &val);
    IntPQ_insert(&g_customIntPQInstance, &val);

    IntPQ other = IntPQ_create();
    TEST_ASSERT_FALSE_MESSAGE(IntPQ_equals(&other, &g_customIntPQInstance), "Other PQ should NOT be equal to original");

    IntPQ_copy(&other, &g_customIntPQInstance);
    TEST_ASSERT_TRUE_MESSAGE(IntPQ_equals(&other, &g_customIntPQInstance), "Other PQ should be equal to original");

    IntPQ_destroy(&other);
}

void test_move()
{
    int val = 0;
    IntPQ_insert(&g_customIntPQInstance, &val);
    IntPQ_insert(&g_customIntPQInstance, &val);
    IntPQ_insert(&g_customIntPQInstance, &val);

    IntPQ original_copy = IntPQ_create();
    IntPQ_copy(&original_copy, &g_customIntPQInstance);

    IntPQ other = IntPQ_create();
    TEST_ASSERT_FALSE_MESSAGE(IntPQ_equals(&other, &g_customIntPQInstance), "Other PQ should NOT be equal to original");

    IntPQ_move(&other, &g_customIntPQInstance);
    TEST_ASSERT_FALSE_MESSAGE(IntPQ_equals(&other, &g_customIntPQInstance), "Other PQ should STILL NOT be equal to original");
    TEST_ASSERT_TRUE_MESSAGE(IntPQ_equals(&other, &original_copy), "Other PQ should be equal to original copy");

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
