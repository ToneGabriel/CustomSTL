#include "unity.h"
#include "custom/c_list.h"


// Setup ==========================


DEFINE_DEFAULT_TYPE_OPERATIONS(unsigned int, myuint_t)
DEFINE_GENERIC_LIST(ListUINT, myuint_t)


static ListUINT g_customListUINTInstance;


void setUp()
{
    g_customListUINTInstance = ListUINT_create();
}

void tearDown()
{
    ListUINT_destroy(&g_customListUINTInstance);
}


// Tests ==========================


void test_default_create()
{
    TEST_ASSERT_TRUE_MESSAGE(ListUINT_empty(&g_customListUINTInstance), "List should be empty");
    TEST_ASSERT_EQUAL_MESSAGE(0, ListUINT_size(&g_customListUINTInstance), "Initial size should be 0");
}

void test_copy()
{
    myuint_t val = 0;
    ListUINT_push_back(&g_customListUINTInstance, &val);
    ListUINT_push_back(&g_customListUINTInstance, &val);
    ListUINT_push_back(&g_customListUINTInstance, &val);

    ListUINT other = ListUINT_create();
    TEST_ASSERT_FALSE_MESSAGE(ListUINT_equals(&other, &g_customListUINTInstance), "Other list should NOT be equal to original");

    ListUINT_copy(&other, &g_customListUINTInstance);
    TEST_ASSERT_TRUE_MESSAGE(ListUINT_equals(&other, &g_customListUINTInstance), "Other list should be equal to original");

    ListUINT_destroy(&other);
}

void test_move()
{
    myuint_t val = 0;
    ListUINT_push_back(&g_customListUINTInstance, &val);
    ListUINT_push_back(&g_customListUINTInstance, &val);
    ListUINT_push_back(&g_customListUINTInstance, &val);

    ListUINT original_copy = ListUINT_create();
    ListUINT_copy(&original_copy, &g_customListUINTInstance);

    ListUINT other = ListUINT_create();
    TEST_ASSERT_FALSE_MESSAGE(ListUINT_equals(&other, &g_customListUINTInstance), "Other list should NOT be equal to original");

    ListUINT_move(&other, &g_customListUINTInstance);
    TEST_ASSERT_FALSE_MESSAGE(ListUINT_equals(&other, &g_customListUINTInstance), "Other list should STILL NOT be equal to original");
    TEST_ASSERT_TRUE_MESSAGE(ListUINT_equals(&other, &original_copy), "Other list should be equal to original copy");

    ListUINT_destroy(&other);
    ListUINT_destroy(&original_copy);
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
