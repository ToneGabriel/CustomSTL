#include "unity.h"
#include "custom/c_stack.h"


// Setup ==========================


DEFINE_DEFAULT_TYPE_OPERATIONS(unsigned int, myuint_t)
DEFINE_GENERIC_STACK(StackUINT, myuint_t)


static StackUINT g_customStackUINTInstance;


void setUp()
{
    g_customStackUINTInstance = StackUINT_create();
}

void tearDown()
{
    StackUINT_destroy(&g_customStackUINTInstance);
}


// Tests ==========================


void test_default_create()
{
    TEST_ASSERT_TRUE_MESSAGE(StackUINT_empty(&g_customStackUINTInstance), "Stack should be empty");
    TEST_ASSERT_EQUAL_MESSAGE(0, StackUINT_size(&g_customStackUINTInstance), "Initial size should be 0");
}

void test_copy()
{
    myuint_t val = 0;
    StackUINT_insert(&g_customStackUINTInstance, &val);
    StackUINT_insert(&g_customStackUINTInstance, &val);
    StackUINT_insert(&g_customStackUINTInstance, &val);

    StackUINT other = StackUINT_create();
    TEST_ASSERT_FALSE_MESSAGE(StackUINT_equals(&other, &g_customStackUINTInstance), "Other stack should NOT be equal to original");

    StackUINT_copy(&other, &g_customStackUINTInstance);
    TEST_ASSERT_TRUE_MESSAGE(StackUINT_equals(&other, &g_customStackUINTInstance), "Other stack should be equal to original");

    StackUINT_destroy(&other);
}

void test_move()
{
    myuint_t val = 0;
    StackUINT_insert(&g_customStackUINTInstance, &val);
    StackUINT_insert(&g_customStackUINTInstance, &val);
    StackUINT_insert(&g_customStackUINTInstance, &val);

    StackUINT original_copy = StackUINT_create();
    StackUINT_copy(&original_copy, &g_customStackUINTInstance);

    StackUINT other = StackUINT_create();
    TEST_ASSERT_FALSE_MESSAGE(StackUINT_equals(&other, &g_customStackUINTInstance), "Other stack should NOT be equal to original");

    StackUINT_move(&other, &g_customStackUINTInstance);
    TEST_ASSERT_FALSE_MESSAGE(StackUINT_equals(&other, &g_customStackUINTInstance), "Other stack should STILL NOT be equal to original");
    TEST_ASSERT_TRUE_MESSAGE(StackUINT_equals(&other, &original_copy), "Other stack should be equal to original copy");

    StackUINT_destroy(&other);
    StackUINT_destroy(&original_copy);
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
