#include "unity.h"
#include "custom/c_pair.h"


// Setup ==========================


DEFINE_DEFAULT_TYPE_OPERATIONS(unsigned int, myuint_t)
DEFINE_DEFAULT_TYPE_OPERATIONS(double, mydouble_t)
DEFINE_GENERIC_PAIR(PairUINTAndDOUBLE, myuint_t, mydouble_t)


static PairUINTAndDOUBLE g_customPairUINTAndDOUBLEInstance;


void setUp()
{
    myuint_t a = 1;
    mydouble_t b = 1.0;
    g_customPairUINTAndDOUBLEInstance = PairUINTAndDOUBLE_create(&a, &b);
}

void tearDown()
{
    PairUINTAndDOUBLE_destroy(&g_customPairUINTAndDOUBLEInstance);
}


// Tests ==========================


void test_default_create()
{
    TEST_ASSERT_EQUAL_MESSAGE(1, g_customPairUINTAndDOUBLEInstance.first, "First value should be equal to the first constructor argument");
    TEST_ASSERT_EQUAL_MESSAGE(1.0, g_customPairUINTAndDOUBLEInstance.second, "Second value should be equal to the second constructor argument");
}

void test_copy()
{
    myuint_t val1 = 0;
    mydouble_t val2 = 0.0;
    PairUINTAndDOUBLE other = PairUINTAndDOUBLE_create(&val1, &val2);
    TEST_ASSERT_FALSE_MESSAGE(PairUINTAndDOUBLE_equals(&other, &g_customPairUINTAndDOUBLEInstance), "Other pair should NOT be equal to original");

    PairUINTAndDOUBLE_copy(&other, &g_customPairUINTAndDOUBLEInstance);
    TEST_ASSERT_TRUE_MESSAGE(PairUINTAndDOUBLE_equals(&other, &g_customPairUINTAndDOUBLEInstance), "Other pair should be equal to original");

    PairUINTAndDOUBLE_destroy(&other);
}

void test_move()
{
    myuint_t val1 = 0;
    mydouble_t val2 = 0.0;
    PairUINTAndDOUBLE original_copy = PairUINTAndDOUBLE_create(&val1, &val2);
    PairUINTAndDOUBLE_copy(&original_copy, &g_customPairUINTAndDOUBLEInstance);

    PairUINTAndDOUBLE other = PairUINTAndDOUBLE_create(&val1, &val2);
    TEST_ASSERT_FALSE_MESSAGE(PairUINTAndDOUBLE_equals(&other, &g_customPairUINTAndDOUBLEInstance), "Other pair should NOT be equal to original");

    PairUINTAndDOUBLE_move(&other, &g_customPairUINTAndDOUBLEInstance);
    // TEST_ASSERT_FALSE_MESSAGE(PairUINTAndDOUBLE_equals(&other, &g_customPairUINTAndDOUBLEInstance), "Other pair should STILL NOT be equal to original");
    TEST_ASSERT_TRUE_MESSAGE(PairUINTAndDOUBLE_equals(&other, &original_copy), "Other pair should be equal to original copy");

    PairUINTAndDOUBLE_destroy(&other);
    PairUINTAndDOUBLE_destroy(&original_copy);
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
