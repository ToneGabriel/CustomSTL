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
    IntPQ_initialize(&g_customIntPQInstance);
}

void tearDown()
{
    IntPQ_destroy(&g_customIntPQInstance);
}


// Tests ==========================


void test_default_create()
{
    TEST_ASSERT_EQUAL_UINT(0, IntPQ_size(&g_customIntPQInstance));
    TEST_ASSERT_TRUE(IntPQ_empty(&g_customIntPQInstance));
}

void test_copy()
{
    int val = 0;
    IntPQ_insert(&g_customIntPQInstance, &val);
    IntPQ_insert(&g_customIntPQInstance, &val);
    IntPQ_insert(&g_customIntPQInstance, &val);

    IntPQ other = IntPQ_create();
    TEST_ASSERT_FALSE(IntPQ_equals(&other, &g_customIntPQInstance));

    IntPQ_copy(&other, &g_customIntPQInstance);
    TEST_ASSERT_TRUE(IntPQ_equals(&other, &g_customIntPQInstance));

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
    IntPQ_initialize(&other);
    TEST_ASSERT_FALSE(IntPQ_equals(&other, &g_customIntPQInstance));

    IntPQ_move(&other, &g_customIntPQInstance);
    TEST_ASSERT_FALSE(IntPQ_equals(&other, &g_customIntPQInstance));
    TEST_ASSERT_TRUE(IntPQ_equals(&other, &original_copy));

    IntPQ_destroy(&other);
    IntPQ_destroy(&original_copy);
}


// RUN ==========================

int main()
{
    g_customIntPQInstance = IntPQ_create();

    UNITY_BEGIN();
    
    RUN_TEST(test_default_create);
    RUN_TEST(test_copy);
    RUN_TEST(test_move);

    return UNITY_END();
}
