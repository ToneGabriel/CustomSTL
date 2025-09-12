#include "unity.h"
#include "custom/c_pair.h"


// Setup ==========================


DEFINE_GENERIC_PAIR(
    PairINTAndDOUBLE,
    int,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_MOVE,
    DEFAULT_TYPE_REF_DELETE,
    double,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_MOVE,
    DEFAULT_TYPE_REF_DELETE
)


static PairINTAndDOUBLE g_customPairINTAndDoubleInstance;


void setUp()
{
    int a = 1;
    double b = 1.0;
    g_customPairINTAndDoubleInstance = PairINTAndDOUBLE_create(&a, &b);
}

void tearDown()
{
    PairINTAndDOUBLE_destroy(&g_customPairINTAndDoubleInstance);
}


// Tests ==========================


void test_default_create()
{
    TEST_ASSERT_EQUAL(1, g_customPairINTAndDoubleInstance.first);
    TEST_ASSERT_EQUAL(1.0, g_customPairINTAndDoubleInstance.second);
}

void test_copy()
{
    int val = 0;
    ListINT_push_back(&g_customPairINTAndDoubleInstance, &val);
    ListINT_push_back(&g_customPairINTAndDoubleInstance, &val);
    ListINT_push_back(&g_customPairINTAndDoubleInstance, &val);

    PairINTAndDOUBLE other = PairINTAndDOUBLE_create();
    TEST_ASSERT_FALSE(ListINT_equals(&other, &g_customPairINTAndDoubleInstance));

    ListINT_copy(&other, &g_customPairINTAndDoubleInstance);
    TEST_ASSERT_TRUE(ListINT_equals(&other, &g_customPairINTAndDoubleInstance));

    ListINT_destroy(&other);
}

void test_move()
{
    int val = 0;
    ListINT_push_back(&g_customPairINTAndDoubleInstance, &val);
    ListINT_push_back(&g_customPairINTAndDoubleInstance, &val);
    ListINT_push_back(&g_customPairINTAndDoubleInstance, &val);

    ListINT original_copy = ListINT_create();
    ListINT_copy(&original_copy, &g_customPairINTAndDoubleInstance);

    ListINT other = ListINT_create();
    TEST_ASSERT_FALSE(ListINT_equals(&other, &g_customPairINTAndDoubleInstance));

    ListINT_move(&other, &g_customPairINTAndDoubleInstance);
    TEST_ASSERT_FALSE(ListINT_equals(&other, &g_customPairINTAndDoubleInstance));
    TEST_ASSERT_TRUE(ListINT_equals(&other, &original_copy));

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
