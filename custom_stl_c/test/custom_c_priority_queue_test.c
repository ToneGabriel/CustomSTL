#include "unity.h"
#include "custom/c_priority_queue.h"


// Setup ==========================


DEFINE_DEFAULT_TYPE_OPERATIONS(unsigned int, myuint_t)
DEFINE_GENERIC_PRIORITY_QUEUE(UIntPQ, myuint_t, myuint_t_less)


static UIntPQ g_customUIntPQInstance;


void setUp()
{
    g_customUIntPQInstance = UIntPQ_create();
}

void tearDown()
{
    UIntPQ_destroy(&g_customUIntPQInstance);
}


// Tests ==========================


void test_default_create()
{
    TEST_ASSERT_TRUE_MESSAGE(UIntPQ_empty(&g_customUIntPQInstance), "PQ should be empty");
    TEST_ASSERT_EQUAL_MESSAGE(0, UIntPQ_size(&g_customUIntPQInstance), "PQ initial size should be 0");
}

void test_copy()
{
    myuint_t val = 0;
    UIntPQ_insert(&g_customUIntPQInstance, &val);
    UIntPQ_insert(&g_customUIntPQInstance, &val);
    UIntPQ_insert(&g_customUIntPQInstance, &val);

    UIntPQ other = UIntPQ_create();
    TEST_ASSERT_FALSE_MESSAGE(UIntPQ_equals(&other, &g_customUIntPQInstance), "Other PQ should NOT be equal to original");

    UIntPQ_copy(&other, &g_customUIntPQInstance);
    TEST_ASSERT_TRUE_MESSAGE(UIntPQ_equals(&other, &g_customUIntPQInstance), "Other PQ should be equal to original");

    UIntPQ_destroy(&other);
}

void test_move()
{
    myuint_t val = 0;
    UIntPQ_insert(&g_customUIntPQInstance, &val);
    UIntPQ_insert(&g_customUIntPQInstance, &val);
    UIntPQ_insert(&g_customUIntPQInstance, &val);

    UIntPQ original_copy = UIntPQ_create();
    UIntPQ_copy(&original_copy, &g_customUIntPQInstance);

    UIntPQ other = UIntPQ_create();
    TEST_ASSERT_FALSE_MESSAGE(UIntPQ_equals(&other, &g_customUIntPQInstance), "Other PQ should NOT be equal to original");

    UIntPQ_move(&other, &g_customUIntPQInstance);
    TEST_ASSERT_FALSE_MESSAGE(UIntPQ_equals(&other, &g_customUIntPQInstance), "Other PQ should STILL NOT be equal to original");
    TEST_ASSERT_TRUE_MESSAGE(UIntPQ_equals(&other, &original_copy), "Other PQ should be equal to original copy");

    UIntPQ_destroy(&other);
    UIntPQ_destroy(&original_copy);
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
