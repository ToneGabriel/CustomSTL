#include "unity.h"
#include "custom/c_pair.h"


// Setup ==========================


DEFINE_GENERIC_PAIR(
    PairINTAndDOUBLE,
    int,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_MOVE,
    DEFAULT_TYPE_REF_DESTROY,
    double,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_MOVE,
    DEFAULT_TYPE_REF_DESTROY
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
    TEST_ASSERT_EQUAL_MESSAGE(1, g_customPairINTAndDoubleInstance.first, "First value should be equal to the first constructor argument");
    TEST_ASSERT_EQUAL_MESSAGE(1.0, g_customPairINTAndDoubleInstance.second, "Second value should be equal to the second constructor argument");
}

void test_copy()
{
    int val1 = 0;
    double val2 = 0.0;
    PairINTAndDOUBLE other = PairINTAndDOUBLE_create(&val1, &val2);
    TEST_ASSERT_FALSE_MESSAGE(PairINTAndDOUBLE_equals(&other, &g_customPairINTAndDoubleInstance), "Other pair should NOT be equal to original");

    PairINTAndDOUBLE_copy(&other, &g_customPairINTAndDoubleInstance);
    TEST_ASSERT_TRUE_MESSAGE(PairINTAndDOUBLE_equals(&other, &g_customPairINTAndDoubleInstance), "Other pair should be equal to original");

    PairINTAndDOUBLE_destroy(&other);
}

void test_move()
{
    int val1 = 0;
    double val2 = 0.0;
    PairINTAndDOUBLE original_copy = PairINTAndDOUBLE_create(&val1, &val2);
    PairINTAndDOUBLE_copy(&original_copy, &g_customPairINTAndDoubleInstance);

    PairINTAndDOUBLE other = PairINTAndDOUBLE_create(&val1, &val2);
    TEST_ASSERT_FALSE_MESSAGE(PairINTAndDOUBLE_equals(&other, &g_customPairINTAndDoubleInstance), "Other pair should NOT be equal to original");

    PairINTAndDOUBLE_move(&other, &g_customPairINTAndDoubleInstance);
    TEST_ASSERT_FALSE_MESSAGE(PairINTAndDOUBLE_equals(&other, &g_customPairINTAndDoubleInstance), "Other pair should STILL NOT be equal to original");
    TEST_ASSERT_TRUE_MESSAGE(PairINTAndDOUBLE_equals(&other, &original_copy), "Other pair should be equal to original copy");

    PairINTAndDOUBLE_destroy(&other);
    PairINTAndDOUBLE_destroy(&original_copy);
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
