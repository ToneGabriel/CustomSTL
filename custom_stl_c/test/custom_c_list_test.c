#include "unity.h"
#include "custom/c_list.h"


// Setup ==========================


DEFINE_GENERIC_LIST(
    ListINT,
    int,
    DEFAULT_TYPE_REF_EQUALS,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_DESTROY
)


static ListINT g_customListINTInstance;


void setUp()
{
    g_customListINTInstance = ListINT_create();
}

void tearDown()
{
    ListINT_destroy(&g_customListINTInstance);
}


// Tests ==========================


void test_default_create()
{
    TEST_ASSERT_TRUE_MESSAGE(ListINT_empty(&g_customListINTInstance), "List should be empty");
    TEST_ASSERT_EQUAL_MESSAGE(0, ListINT_size(&g_customListINTInstance), "Initial size should be 0");
}

void test_copy()
{
    int val = 0;
    ListINT_push_back(&g_customListINTInstance, &val);
    ListINT_push_back(&g_customListINTInstance, &val);
    ListINT_push_back(&g_customListINTInstance, &val);

    ListINT other = ListINT_create();
    TEST_ASSERT_FALSE_MESSAGE(ListINT_equals(&other, &g_customListINTInstance), "Other list should NOT be equal to original");

    ListINT_copy(&other, &g_customListINTInstance);
    TEST_ASSERT_TRUE_MESSAGE(ListINT_equals(&other, &g_customListINTInstance), "Other list should be equal to original");

    ListINT_destroy(&other);
}

void test_move()
{
    int val = 0;
    ListINT_push_back(&g_customListINTInstance, &val);
    ListINT_push_back(&g_customListINTInstance, &val);
    ListINT_push_back(&g_customListINTInstance, &val);

    ListINT original_copy = ListINT_create();
    ListINT_copy(&original_copy, &g_customListINTInstance);

    ListINT other = ListINT_create();
    TEST_ASSERT_FALSE_MESSAGE(ListINT_equals(&other, &g_customListINTInstance), "Other list should NOT be equal to original");

    ListINT_move(&other, &g_customListINTInstance);
    TEST_ASSERT_FALSE_MESSAGE(ListINT_equals(&other, &g_customListINTInstance), "Other list should STILL NOT be equal to original");
    TEST_ASSERT_TRUE_MESSAGE(ListINT_equals(&other, &original_copy), "Other list should be equal to original copy");

    ListINT_destroy(&other);
    ListINT_destroy(&original_copy);
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
