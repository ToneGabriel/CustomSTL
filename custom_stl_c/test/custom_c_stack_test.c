#include "unity.h"
#include "custom/c_stack.h"


// Setup ==========================


DEFINE_GENERIC_STACK(
    StackINT,
    int,
    DEFAULT_TYPE_REF_EQUALS,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_DELETE
)


static StackINT g_customStackINTInstance;


void setUp()
{
    g_customStackINTInstance = StackINT_create();
}

void tearDown()
{
    StackINT_destroy(&g_customStackINTInstance);
}


// Tests ==========================


void test_default_create()
{
    TEST_ASSERT_TRUE_MESSAGE(StackINT_empty(&g_customStackINTInstance), "Stack should be empty");
    TEST_ASSERT_EQUAL_MESSAGE(0, StackINT_size(&g_customStackINTInstance), "Initial size should be 0");
}

void test_copy()
{
    int val = 0;
    StackINT_insert(&g_customStackINTInstance, &val);
    StackINT_insert(&g_customStackINTInstance, &val);
    StackINT_insert(&g_customStackINTInstance, &val);

    StackINT other = StackINT_create();
    TEST_ASSERT_FALSE_MESSAGE(StackINT_equals(&other, &g_customStackINTInstance), "Other stack should NOT be equal to original");

    StackINT_copy(&other, &g_customStackINTInstance);
    TEST_ASSERT_TRUE_MESSAGE(StackINT_equals(&other, &g_customStackINTInstance), "Other stack should be equal to original");

    StackINT_destroy(&other);
}

void test_move()
{
    int val = 0;
    StackINT_insert(&g_customStackINTInstance, &val);
    StackINT_insert(&g_customStackINTInstance, &val);
    StackINT_insert(&g_customStackINTInstance, &val);

    StackINT original_copy = StackINT_create();
    StackINT_copy(&original_copy, &g_customStackINTInstance);

    StackINT other = StackINT_create();
    TEST_ASSERT_FALSE_MESSAGE(StackINT_equals(&other, &g_customStackINTInstance), "Other stack should NOT be equal to original");

    StackINT_move(&other, &g_customStackINTInstance);
    TEST_ASSERT_FALSE_MESSAGE(StackINT_equals(&other, &g_customStackINTInstance), "Other stack should STILL NOT be equal to original");
    TEST_ASSERT_TRUE_MESSAGE(StackINT_equals(&other, &original_copy), "Other stack should be equal to original copy");

    StackINT_destroy(&other);
    StackINT_destroy(&original_copy);
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
