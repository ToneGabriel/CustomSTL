#include "unity.h"
#include "custom/c_vector.h"


// Setup ==========================


DEFINE_GENERIC_VECTOR(
    VectorINT,
    int,
    DEFAULT_TYPE_REF_EQUALS,
    DEFAULT_TYPE_REF_COPY,
    DEFAULT_TYPE_REF_DELETE
)


static VectorINT g_customVectorINTInstance;


void setUp()
{
    g_customVectorINTInstance = VectorINT_create(GENERIC_VECTOR_DEFAULT_CAPACITY);
}

void tearDown()
{
    VectorINT_destroy(&g_customVectorINTInstance);
}


// Tests ==========================


void test_default_create()
{
    TEST_ASSERT_TRUE_MESSAGE(VectorINT_empty(&g_customVectorINTInstance), "Vector should be empty");
    TEST_ASSERT_EQUAL_MESSAGE(0, VectorINT_size(&g_customVectorINTInstance), "Initial size should be 0");
    TEST_ASSERT_EQUAL_MESSAGE(GENERIC_VECTOR_DEFAULT_CAPACITY, VectorINT_capacity(&g_customVectorINTInstance), "Initial capacity should be GENERIC_VECTOR_DEFAULT_CAPACITY");
}

void test_copy()
{
    int val = 0;
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);

    VectorINT other = VectorINT_create(GENERIC_VECTOR_DEFAULT_CAPACITY);
    TEST_ASSERT_FALSE_MESSAGE(VectorINT_equals(&other, &g_customVectorINTInstance), "Other vector should NOT be equal to original");

    VectorINT_copy(&other, &g_customVectorINTInstance);
    TEST_ASSERT_TRUE_MESSAGE(VectorINT_equals(&other, &g_customVectorINTInstance), "Other vector should be equal to original");

    VectorINT_destroy(&other);
}

void test_move()
{
    int val = 0;
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);

    VectorINT original_copy = VectorINT_create(GENERIC_VECTOR_DEFAULT_CAPACITY);
    VectorINT_copy(&original_copy, &g_customVectorINTInstance);

    VectorINT other = VectorINT_create(GENERIC_VECTOR_DEFAULT_CAPACITY);
    TEST_ASSERT_FALSE_MESSAGE(VectorINT_equals(&other, &g_customVectorINTInstance), "Other vector should NOT be equal to original");

    VectorINT_move(&other, &g_customVectorINTInstance);
    TEST_ASSERT_FALSE_MESSAGE(VectorINT_equals(&other, &g_customVectorINTInstance), "Other vector should STILL NOT be equal to original");
    TEST_ASSERT_TRUE_MESSAGE(VectorINT_equals(&other, &original_copy), "Other vector should be equal to original copy");

    VectorINT_destroy(&other);
    VectorINT_destroy(&original_copy);
}

void test_clear()
{
    int val = 0;
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    TEST_ASSERT_FALSE_MESSAGE(VectorINT_empty(&g_customVectorINTInstance), "Vector should NOT be empty");

    VectorINT_clear(&g_customVectorINTInstance);
    TEST_ASSERT_TRUE_MESSAGE(VectorINT_empty(&g_customVectorINTInstance), "Vector should be empty");
}

void test_push_back_without_realloc()
{
    int val = 0;
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    TEST_ASSERT_EQUAL_MESSAGE(GENERIC_VECTOR_DEFAULT_CAPACITY, VectorINT_capacity(&g_customVectorINTInstance), "Vector capacity should NOT increase");
}

void test_push_back_with_realloc()
{
    int val = 0;
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    VectorINT_push_back(&g_customVectorINTInstance, &val);  // add 1 more
    TEST_ASSERT_TRUE_MESSAGE(GENERIC_VECTOR_DEFAULT_CAPACITY < VectorINT_capacity(&g_customVectorINTInstance), "Vector capacity should increase");
}

void test_pop_back()
{
    int val = 0;
    VectorINT_push_back(&g_customVectorINTInstance, &val);
    TEST_ASSERT_EQUAL_MESSAGE(1, VectorINT_size(&g_customVectorINTInstance), "Vector size should increase after push_back");

    VectorINT_pop_back(&g_customVectorINTInstance);
    TEST_ASSERT_EQUAL_MESSAGE(0, VectorINT_size(&g_customVectorINTInstance), "Vector size should decrease after pop_back");
}

void test_element_check()
{
    int val1 = 1;
    int val2 = 2;
    int val3 = 3;
    VectorINT_push_back(&g_customVectorINTInstance, &val1);
    VectorINT_push_back(&g_customVectorINTInstance, &val2);
    VectorINT_push_back(&g_customVectorINTInstance, &val3);

    TEST_ASSERT_TRUE_MESSAGE(val1 == *VectorINT_element_front(&g_customVectorINTInstance), "Vector front element should be equal to the first push_back");
    TEST_ASSERT_TRUE_MESSAGE(val2 == *VectorINT_element_at(&g_customVectorINTInstance, 1), "Vector element at index 1 should be equal to the second push_back");
    TEST_ASSERT_TRUE_MESSAGE(val3 == *VectorINT_element_back(&g_customVectorINTInstance), "Vector back element should be equal to the last push_back");
}

// RUN ==========================

int main()
{
    UNITY_BEGIN();

    RUN_TEST(test_default_create);
    RUN_TEST(test_copy);
    RUN_TEST(test_move);
    RUN_TEST(test_clear);
    RUN_TEST(test_push_back_without_realloc);
    RUN_TEST(test_push_back_with_realloc);
    RUN_TEST(test_pop_back);
    RUN_TEST(test_element_check);

    return UNITY_END();
}
