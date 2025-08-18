#include "unity.h"
#include "custom/c_list.h"


// Setup ==========================


DEFINE_GENERIC_LIST(
    ListINT,
    int,
    DEFAULT_TYPE_REF_EQUALS,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_DELETE
)


static ListINT g_customListINTInstance;


void setUp()
{
    ListINT_initialize(&g_customListINTInstance);
}

void tearDown()
{
    ListINT_destroy(&g_customListINTInstance);
}


// Tests ==========================


void test_default_create()
{
    TEST_ASSERT_EQUAL_UINT(0, ListINT_size(&g_customListINTInstance));
    TEST_ASSERT_TRUE(ListINT_empty(&g_customListINTInstance));
}

void test_copy()
{
    int val = 0;
    ListINT_push_back(&g_customListINTInstance, &val);
    ListINT_push_back(&g_customListINTInstance, &val);
    ListINT_push_back(&g_customListINTInstance, &val);

    ListINT other = ListINT_create();
    TEST_ASSERT_FALSE(ListINT_equals(&other, &g_customListINTInstance));

    ListINT_copy(&other, &g_customListINTInstance);
    TEST_ASSERT_TRUE(ListINT_equals(&other, &g_customListINTInstance));

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
    ListINT_initialize(&other);
    TEST_ASSERT_FALSE(ListINT_equals(&other, &g_customListINTInstance));

    ListINT_move(&other, &g_customListINTInstance);
    TEST_ASSERT_FALSE(ListINT_equals(&other, &g_customListINTInstance));
    TEST_ASSERT_TRUE(ListINT_equals(&other, &original_copy));

    ListINT_destroy(&other);
    ListINT_destroy(&original_copy);
}


// RUN ==========================

int main()
{
    g_customListINTInstance = ListINT_create();

    UNITY_BEGIN();
    
    RUN_TEST(test_default_create);
    RUN_TEST(test_copy);
    RUN_TEST(test_move);

    return UNITY_END();
}
